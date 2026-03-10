#include "servermanager.h"
#include <QThreadPool>
#include <QDebug>
#include <QMetaType>
#include "connectionmanager.h"
/**
 * @brief ServerManager 构造函数
 * @param parent 父对象指针
 *
 * 初始化时注册 ProcessedData 元类型，以便该类型能通过 Qt 的信号槽跨线程传递。
 */
ServerManager::ServerManager(QObject *parent)
    : QObject(parent)
{
    // 注册元类型，以便跨线程传递 ProcessedData
    qRegisterMetaType<ProcessedData>("ProcessedData");

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &ServerManager::updateServerStatus);
    m_statusTimer->setInterval(3000); // 每2秒更新一次状态
}

/**
 * @brief ServerManager 构造函数
 * @param parent 父对象指针
 *
 * 初始化时注册 ProcessedData 元类型，以便该类型能通过 Qt 的信号槽跨线程传递。
 */
ServerManager::ServerManager(const serverParam &param, QObject *parent)
    : QObject(parent)
    ,param(param)
{
    // 注册元类型，以便跨线程传递 ProcessedData
    qRegisterMetaType<ProcessedData>("ProcessedData");

    m_statusTimer = new QTimer(this);
    connect(m_statusTimer, &QTimer::timeout, this, &ServerManager::updateServerStatus);
    m_statusTimer->setInterval(3000); // 每2秒更新一次状态
}

/**
 * @brief ServerManager 析构函数
 *
 * 首先调用 stopAll() 停止所有服务器并等待消费者线程结束，
 * 然后手动删除所有 TCP 和 UDP 服务器对象（虽然它们设置了父对象，但显式删除更安全）。
 */
ServerManager::~ServerManager()
{
    stopAll();                        // 停止所有服务器，等待线程池结束
    qDeleteAll(m_tcpServers);         // 删除所有 TCP 服务器对象
    qDeleteAll(m_udpServers);         // 删除所有 UDP 服务器对象
}

/**
 * @brief 添加一个 TCP 服务器
 * @param ip 监听 IP
 * @param port 监听端口
 * @param consumerCount 消费者线程数
 * @param maxConnections 最大连接数
 * @param buffsize 缓冲区大小
 *
 * 创建 TCPserver 对象，连接其 error 信号到本类的 onServerError 槽，
 * 并将其加入内部列表 m_tcpServers 以便统一管理。
 */
void ServerManager::addTcpServer(const QString serverName,
                                 const QString &ip, quint16 port,
                                 const int &consumerCount,
                                 const int &maxConnections,
                                 const uint &buffsize)
{
    // 创建 TCPserver 对象，注意构造函数参数顺序可能需调整：当前传递了 (port, consumerCount, this)
    QHostAddress addr = QHostAddress(ip);
    TCPserver* server = new TCPserver(addr, port, consumerCount, this, maxConnections, buffsize);
    server->setServeName(serverName);
    // 连接服务器的错误信号到本类的处理槽
    // 设置限制策略
    server->setConnectionStrategy(ConnectionManager::PerIPLimit);  // 每IP限制
    server->setPerIpMax(5);  // 每个IP最多5个连接

    // 或者使用组合策略（动态调整）
    server->setConnectionStrategy(ConnectionManager::DynamicLimit);
    server->setDynamicThreshold(70, 80);  // CPU超过70%或内存超过80%时降低连接数

    // 或者使用速率限制
    server->setConnectionStrategy(ConnectionManager::RateLimit);
    server->setRateLimit(100);  // 每秒最多100个新连接

    // 黑名单管理
    server->addToBlacklist(QHostAddress("192.168.1.100"), 3600);  // 拉黑1小时

    // 连接信号
    connect(server, &TCPserver::connectionRejected,
            [](QHostAddress ip, quint16 port, QString reason) {
                qWarning() << "连接被拒绝:" << ip.toString() << reason;
            });

    connect(server, &TCPserver::statsUpdated,
            [](int total, int uniqueIps) {
                qDebug() << "当前统计 - 总连接:" << total << "独立IP:" << uniqueIps;
            });
    connect(server, &TCPserver::error, this, &ServerManager::onServerError);
    connect(server, &TCPserver::servermanagerMsg,this,&ServerManager::toWinQList);

    m_tcpServers.append(server);
    qDebug() << "[ServerManager] 已添加 TCP 服务器，端口:" << port;
}

/**
 * @brief 添加一个 UDP 服务器
 * @param ip 监听 IP
 * @param port 监听端口
 * @param consumerCount 消费者线程数
 * @param buffsize 缓冲区大小
 * @param enableBroadcast 是否启用广播接收
 * @param multicastGroups 要加入的组播组地址列表
 *
 * 创建 UDPserver 对象，配置广播和组播选项，连接其 error 信号，
 * 并将对象加入内部列表 m_udpServers。
 */
void ServerManager::addUdpServer(const QString serverName,
                                 const QString &ip, quint16 port,
                                 const int &consumerCount,
                                 const uint &buffsize,
                                 bool enableBroadcast,
                                 const QList<QHostAddress>& multicastGroups)
{
    // 创建 UDPserver 对象，同样需注意构造函数参数顺序

    QHostAddress addr = QHostAddress(ip);
    UDPserver* server = new UDPserver(addr, port, consumerCount, this, buffsize);
    server->setServeName(serverName);
    // 设置是否接收广播
    server->setEnableBroadcast(enableBroadcast);
    // 添加所有组播组地址
    for (const QHostAddress& group : multicastGroups) {
        server->addMulticastGroup(group);
    }
    // 连接错误信号
    connect(server, &UDPserver::error, this, &ServerManager::onServerError);

    connect(server, &UDPserver::clientOffline, this, [this](const QHostAddress& ip, quint16 port) {
        qDebug() << "[ServerManager] UDP客户端离线:" << ip.toString() << ":" << port;
        emit udpClientOffline(ip, port);  // 可转发给上层
    });
    m_udpServers.append(server);
    qDebug() << "[ServerManager] 已添加 UDP 服务器，端口:" << port
             << "，广播:" << (enableBroadcast ? "是" : "否")
             << "，组播组数:" << multicastGroups.size();
}

/**
 * @brief 启动所有已添加的服务器
 * @return 如果所有服务器都成功启动返回 true，否则返回 false
 *
 * 遍历所有 TCP 和 UDP 服务器，依次调用它们的 start() 方法。
 * 如果任一服务器启动失败，allSuccess 标记为 false，但会继续尝试启动其他服务器。
 */
bool ServerManager::startAll()
{
    bool allSuccess = true;
    // 启动所有 TCP 服务器
    for (TCPserver* server : m_tcpServers) {
        if (!server->start()) allSuccess = false;
    }
    // 启动所有 UDP 服务器
    for (UDPserver* server : m_udpServers) {
        if (!server->start()) allSuccess = false;
    }
    m_statusTimer->start();
    return allSuccess;
}

/**
 * @brief 停止所有服务器
 *
 * 遍历所有 TCP 和 UDP 服务器，依次调用它们的 stop() 方法。
 * stop() 内部会关闭监听、断开连接并等待消费者线程池结束。
 */
void ServerManager::stopAll()
{
    for (TCPserver* server : m_tcpServers)
        server->stop();
    for (UDPserver* server : m_udpServers)
        server->stop();
    m_statusTimer->stop();
    qDebug() << "[ServerManager] 所有服务器已停止";
}

/**
 * @brief 接收消费者线程处理后的数据（槽函数，在主线程中执行）
 * @param data 处理后的数据
 *
 * 此槽由 ServerProcessTask 通过跨线程调用触发（使用 QMetaObject::invokeMethod）。
 * 它简单地转发数据，发射 dataReceived 信号，以便主窗口或其他组件可以连接并更新 UI。
 */
void ServerManager::onDataProcessed(const ProcessedData& data)
{
    emit dataReceived(data);
}

/**
 * @brief 处理单个服务器发出的错误信号
 * @param message 原始错误信息
 *
 * 通过 sender() 获取发出信号的服务器对象，判断其类型（TCPserver 或 UDPserver），
 * 构造包含服务器标识（如 "TCP:8888"）的错误信息，然后发射统一的 error 信号。
 * 这样上层只需连接一个信号即可处理所有服务器的错误。
 */
void ServerManager::onServerError(const QString& message)
{
    QObject* senderObj = sender();   // 获取发出信号的服务器对象
    QString serverId;
    // 尝试转换为 TCPserver 指针
    if (TCPserver* tcpServer = qobject_cast<TCPserver*>(senderObj)) {
        serverId = QString("TCP:%1").arg(tcpServer->port());
    }
    // 尝试转换为 UDPserver 指针
    else if (UDPserver* udpServer = qobject_cast<UDPserver*>(senderObj)) {
        serverId = QString("UDP:%1").arg(udpServer->port());
    } else {
        serverId = "未知服务器";
    }
    // 发射带有服务器标识的错误信号
    emit error(serverId, message);
}

void ServerManager::onWidgetMsg(const QHostAddress &host, QString msg)
{
    emit toWinQList(host, msg);

}

void ServerManager::updateServerStatus()
{
    QList<ServerStatus> statusList;
    QDateTime now = QDateTime::currentDateTime();

    // 处理 TCP 服务器
    for (TCPserver* server : m_tcpServers) {
        int secs = server->lastActivityTime().secsTo(now);
        QString color;
        if (secs <= GREEN_THRESHOLD)
            color = "green";
        else if (secs <= ORANGE_THRESHOLD)
            color = "orange";
        else
            color = "red";
        statusList.append({"TCP", server->port(), color, server->getServeName()});
    }

    // 处理 UDP 服务器
    for (UDPserver* server : m_udpServers) {
        int secs = server->lastActivityTime().secsTo(now);
        QString color;
        if (secs <= GREEN_THRESHOLD)
            color = "green";
        else if (secs <= ORANGE_THRESHOLD)
            color = "orange";
        else
            color = "red";
        statusList.append({"UDP", server->port(), color, server->getServeName()});
    }

    emit serverStatusUpdated(statusList);
}
