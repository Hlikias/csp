#include "udpserver.h"
#include <QNetworkDatagram>
#include "server.h"
#include "servermanager.h"

/**
 * @brief UDPserver 构造函数
 * @param port 监听端口
 * @param consumerCount 消费者线程数
 * @param manager 指向 ServerManager 的指针，用于传递给消费者任务
 * @param parent Qt 父对象
 *
 * 初始化成员变量，创建线程安全队列，但网络套接字和消费者线程池的启动在 start() 中完成。
 */
UDPserver::UDPserver(quint16 port, int consumerCount, ServerManager* manager, const uint &buffsize, QObject* parent)
    : QObject(parent)
    , m_address(QHostAddress::Any)
    , m_port(port)
    , m_consumerCount(consumerCount)
    , m_manager(manager)
    , m_socket(nullptr)                     // 套接字初始为空，start 时创建
    , m_queue(new ThreadSafeQueue<NetData>()) // 创建线程安全队列
    , m_enableBroadcast(false)               // 默认不启用广播接收
    , m_lastActivityTime(QDateTime::currentDateTime())
    , m_keepAliveTimer(new QTimer(this))
{
    // 创建心跳发送定时器
    m_heartbeatSendTimer = new QTimer(this);
    connect(m_heartbeatSendTimer, &QTimer::timeout, this, &UDPserver::onHeartbeatSend);
    m_heartbeatSendTimer->setInterval(HEARTBEAT_INTERVAL);

    m_keepAliveTimer = new QTimer(this);
    connect(m_keepAliveTimer, &QTimer::timeout, this, &UDPserver::updateActivityTime);
}

UDPserver::UDPserver(const QHostAddress &address, quint16 port, int consumerCount,
                     ServerManager *manager,  const uint &buffsize, QObject *parent)
    : QObject(parent)
    , m_address(address)                 // 保存指定地址
    , m_port(port)
    , m_consumerCount(consumerCount)
    , m_manager(manager)
    , m_socket(nullptr)
    , m_queue(new ThreadSafeQueue<NetData>())
    , m_enableBroadcast(false)
{
    // 心跳定时器初始化（与原构造函数相同）
    m_heartbeatSendTimer = new QTimer(this);
    connect(m_heartbeatSendTimer, &QTimer::timeout, this, &UDPserver::onHeartbeatSend);
    m_heartbeatSendTimer->setInterval(HEARTBEAT_INTERVAL);

    m_keepAliveTimer = new QTimer(this);
    connect(m_keepAliveTimer, &QTimer::timeout, this, &UDPserver::updateActivityTime);
}

/**
 * @brief UDPserver 析构函数
 *
 * 调用 stop() 停止服务器，并释放队列内存。
 */
UDPserver::~UDPserver()
{
    stop();                 // 停止服务器，关闭套接字并等待消费者线程结束
    delete m_queue;          // 释放内部队列（确保此时没有线程再使用队列）
}

/**
 * @brief 添加一个组播组地址
 * @param groupAddress 组播 IP 地址
 *
 * 将地址添加到内部列表，在 start() 中会加入这些组。
 * 避免重复添加。
 */
void UDPserver::addMulticastGroup(const QHostAddress& groupAddress)
{
    if (!m_multicastGroups.contains(groupAddress))
        m_multicastGroups.append(groupAddress);
}

/**
 * @brief 清空所有已设置的组播组地址
 */
void UDPserver::clearMulticastGroups()
{
    m_multicastGroups.clear();
}

/**
 * @brief 设置是否启用广播接收
 * @param enable true 启用，false 禁用
 *
 * 注意：广播接收只需要绑定端口时使用 ShareAddress 即可，无需特殊选项。
 * 此标志主要用于日志和用户配置记录。
 */
void UDPserver::setEnableBroadcast(bool enable)
{
    m_enableBroadcast = enable;
}

/**
 * @brief 启动 UDP 服务器
 * @return 成功返回 true，失败返回 false
 *
 * 创建 QUdpSocket 并绑定端口，加入组播组，连接 readyRead 信号，
 * 然后启动消费者线程池。
 */
bool UDPserver::start()
{
    if (m_socket) return true; // 已经启动，直接返回

    // 创建 UDP 套接字
    m_socket = new QUdpSocket(this);
    // 绑定端口，使用 ShareAddress 选项以支持组播和广播（允许多个程序共享同一端口）
    if (!m_socket->bind(m_address, m_port, QUdpSocket::ShareAddress)) {
        emit error("绑定端口失败: " + m_socket->errorString());
        m_socket->deleteLater();   // 安全销毁
        m_socket = nullptr;
        return false;
    }

    // 加入所有预设的组播组
    for (const QHostAddress &group : m_multicastGroups) {
        if (!m_socket->joinMulticastGroup(group)) {
            emit error("加入组播组 " + group.toString() + " 失败: " + m_socket->errorString());
            // 即使加入某个组失败，仍继续尝试其他组，不中断启动
        }
    }

    // 注意：接收广播不需要显式设置 BroadcastSocketOption，绑定 ShareAddress 即可。
    // 如果需要发送广播，则需设置，但本类仅接收数据，故省略。

    // 连接数据可读信号
    connect(m_socket, &QUdpSocket::readyRead, this, &UDPserver::onReadyRead);

    // 启动心跳发送定时器
    m_heartbeatSendTimer->start();

    // 启动消费者线程池
    startConsumers();

    qDebug() << "[UDPserver] 启动成功，端口:" << m_port
             << "，组播组:" << m_multicastGroups.size()
             << "，广播:" << (m_enableBroadcast ? "是" : "否");

    m_keepAliveTimer->start(5000);
    return true;
}

/**
 * @brief 停止 UDP 服务器
 *
 * 关闭套接字，离开所有组播组，并等待消费者线程池结束。
 */
void UDPserver::stop()
{
    if (m_socket) {
        // 离开所有之前加入的组播组（清理操作，非必须但推荐）
        for (const QHostAddress& group : m_multicastGroups) {
            m_socket->leaveMulticastGroup(group);
        }
        m_socket->close();               // 关闭套接字
        m_socket->deleteLater();         // 延迟删除套接字对象
        m_socket = nullptr;
    }

    // 停止心跳定时器
    m_heartbeatSendTimer->stop();

    // 清理所有客户端定时器
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        delete it.value().timeoutTimer;
    }
    m_clients.clear();


    // 停止消费者线程池：清除所有等待任务，并等待正在运行的任务结束（最多 3 秒）
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone(3000);
    qDebug() << "[UDPserver] 已停止，端口:" << m_port;

    m_keepAliveTimer->stop();
    disconnect(m_keepAliveTimer, &QTimer::timeout, this, &UDPserver::updateActivityTime);
}

void UDPserver::updateActivityTime()
{
    m_lastActivityTime = QDateTime::currentDateTime();
    emit activityUpdated();
}

QDateTime UDPserver::lastActivityTime() const
{
    return m_lastActivityTime;
}

/**
 * @brief 启动消费者线程池
 *
 * 设置全局线程池的最大线程数，并创建指定数量的 ServerProcessTask 任务提交给线程池。
 * 每个任务持有队列指针和 ServerManager 指针，以便处理数据并回传结果。
 */
void UDPserver::startConsumers()
{
    QThreadPool* pool = QThreadPool::globalInstance();
    pool->setMaxThreadCount(m_consumerCount);   // 设置最大线程数
    for (int i = 0; i < m_consumerCount; ++i) {
        // 创建消费者任务，传入队列和 manager 指针
        ServerProcessTask* task = new ServerProcessTask(m_queue, m_manager);
        pool->start(task);   // 线程池自动调度并删除任务（autoDelete 为 true）
    }
}

/**
 * @brief 处理接收到的数据报（槽函数，由 readyRead 触发）
 *
 * 循环接收所有待处理的数据报，构造 NetData 结构，根据目的地址判断数据报类型
 * （单播/组播/广播），然后推入线程安全队列供消费者处理。
 */
void UDPserver::onReadyRead()
{
    // 循环直到没有更多数据报
    while (m_socket->hasPendingDatagrams()) {
        // 接收一个数据报
        QNetworkDatagram datagram = m_socket->receiveDatagram();
        if (!datagram.isValid()) continue;   // 跳过无效数据报

        QHostAddress senderIp = datagram.senderAddress();
        quint16 senderPort = datagram.senderPort();
        QByteArray data = datagram.data();
        ClientKey key(senderIp, senderPort);

        updateActivityTime();   // 收到数据时也刷新
        // 1. 检查是否是心跳回复
        if (data == "PONG") {
            // 找到对应的客户端
            auto it = m_clients.find(key);
            if (it != m_clients.end()) {
                // 重置未回复计数
                it.value().missedPongs = 0;
                // 停止超时定时器（如果有）
                if (it.value().timeoutTimer && it.value().timeoutTimer->isActive()) {
                    it.value().timeoutTimer->stop();
                }
            }
            // 心跳回复不进入业务队列，直接丢弃
            continue;
        }

        NetData netData;
        netData.data = datagram.data();                          // 原始数据
        netData.senderAddress = datagram.senderAddress();        // 发送者 IP
        netData.senderPort = datagram.senderPort();              // 发送者端口
        netData.clientIp = netData.senderAddress.toString();     // 字符串形式的 IP（日志用）
        netData.clientPort = netData.senderPort;                 // 端口（日志用）

        // 根据目的地址判断数据报类型
        QHostAddress destAddr = datagram.destinationAddress();
        if (destAddr.isMulticast()) {
            netData.type = TransportType::UDP_Multicast;   // 组播
        } else if (destAddr.isBroadcast()) {
            netData.type = TransportType::UDP_Broadcast;   // 广播
        } else {
            netData.type = TransportType::UDP_Unicast;     // 单播
        }

        // 非阻塞入队
        if (!m_queue->push(netData, false)) {
            qWarning() << "队列满，丢弃数据报来自" << senderIp.toString() << ":" << senderPort;
        }

        // 3. 更新客户端活跃状态（如果是新客户端则添加）
        auto it = m_clients.find(key);
        if (it == m_clients.end()) {
            // 新客户端，创建 ClientInfo
            ClientInfo info;
            info.missedPongs = 0;
            info.timeoutTimer = new QTimer(this);
            info.timeoutTimer->setSingleShot(true);
            info.timeoutTimer->setInterval(HEARTBEAT_TIMEOUT);
            connect(info.timeoutTimer, &QTimer::timeout, this, [this, senderIp, senderPort]() {
                onClientTimeout(senderIp, senderPort);
            });
            m_clients.insert(key, info);
            qDebug() << "新客户端上线:" << senderIp.toString() << ":" << senderPort;
        } else {
            // 已有客户端，重置未回复计数，并停止超时定时器
            ClientInfo& info = it.value();
            info.missedPongs = 0;
            if (info.timeoutTimer && info.timeoutTimer->isActive()) {
                info.timeoutTimer->stop();
            }
        }
    }
}

void UDPserver::onHeartbeatSend()
{

    QByteArray pingData = "PING";  // 心跳请求内容，可根据协议自定义

    // 遍历所有活跃客户端
    for (auto it = m_clients.begin(); it != m_clients.end(); ) {
        const ClientKey& key = it.key();          // key 是 (QHostAddress, quint16)
        ClientInfo& info = it.value();

        // 通过 UDP socket 发送数据报到客户端的 IP 和端口
        m_socket->writeDatagram(pingData, key.first, key.second);

        // 增加未回复计数
        info.missedPongs++;
        if (info.missedPongs >= MAX_MISSED_PONGS) {
            // 连续多次未收到回复，判定离线
            qDebug() << "客户端离线:" << key.first.toString() << ":" << key.second;
            emit clientOffline(key.first, key.second);   // 通知上层

            // 清理该客户端的定时器并移除
            if (info.timeoutTimer) {
                info.timeoutTimer->stop();
                delete info.timeoutTimer;
            }
            it = m_clients.erase(it);  // 从活跃列表中移除
            continue;
        }

        // 启动超时定时器（如果尚未激活或已停止），用于在超时前等待回复
        if (!info.timeoutTimer->isActive()) {
            info.timeoutTimer->start();
        }
        ++it;
    }
}

void UDPserver::onClientTimeout(const QHostAddress &clientIp, quint16 clientPort)
{
    ClientKey key(clientIp, clientPort);
    auto it = m_clients.find(key);
    if (it == m_clients.end()) return;

    // 如果已经连续多次未回复，可能已经在 onHeartbeatSend 中被移除
    // 这里可以根据需要处理，比如直接判定离线
    if (it.value().missedPongs >= MAX_MISSED_PONGS) {
        emit clientOffline(clientIp, clientPort);
        delete it.value().timeoutTimer;
        m_clients.erase(it);
    }
}
