#include "tcpserver.h"
#include <QDebug>
#include "server.h"
#include "servermanager.h"

/**
 * @brief TCPserver 构造函数
 * @param port 监听端口
 * @param consumerCount 消费者线程数
 * @param manager 指向 ServerManager 的指针，用于传递给消费者任务
 * @param maxConnections 最大的连接数
 * @param parent Qt 父对象
 *
 * 初始化成员变量，创建 QTcpServer 对象并连接新连接信号。
 * 注意：消费者线程池的启动在 start() 中完成，此处仅创建队列和服务器。
 */
TCPserver::TCPserver(quint16 port,
                     int consumerCount,
                     ServerManager* manager,
                     int maxConnections,
                     const uint &buffsize,
                     QObject* parent)
    : QObject(parent)
    , m_port(port)
    , m_consumerCount(consumerCount)
    , m_manager(manager)
    , m_server(nullptr)
    , m_queue(new ThreadSafeQueue<NetData>(buffsize))   // 创建线程安全队列
    ,m_connManager(new ConnectionManager(this))
    , m_lastActivityTime(QDateTime::currentDateTime())
    , m_keepAliveTimer(new QTimer(this))

{
    // 设置连接管理器参数
    m_connManager->setGlobalMax(maxConnections);
    m_connManager->setStrategy(ConnectionManager::FixedLimit);

    // 连接管理器的信号
    connect(m_connManager, &ConnectionManager::connectionRejected,
            this, &TCPserver::onConnectionRejected);
    connect(m_connManager, &ConnectionManager::statsUpdated,
            this, &TCPserver::onConnManagerStats);
    // 创建 TCP 服务器对象，并设置父对象为 this 以便自动管理生命周期
    m_server = new QTcpServer(this);
    // 连接新连接信号到处理槽
    connect(m_server, &QTcpServer::newConnection, this, &TCPserver::onNewConnection);
    //m_keepAliveTimer->start(5000); // 每5秒刷新一次

    m_keepAliveTimer = new QTimer(this);
    connect(m_keepAliveTimer, &QTimer::timeout,this,&TCPserver::updateActivityTime);
}
/**
 * @brief TCPserver 构造函数
 * @param address 监听网卡
 * @param port 监听端口
 * @param consumerCount 消费者线程数
 * @param manager 指向 ServerManager 的指针，用于传递给消费者任务
 * @param maxConnections 最大的连接数
 * @param parent Qt 父对象
 *
 * 初始化成员变量，创建 QTcpServer 对象并连接新连接信号。
 * 注意：消费者线程池的启动在 start() 中完成，此处仅创建队列和服务器。
 */
TCPserver::TCPserver(QHostAddress address,
                     quint16 port,
                     int consumerCount,
                     ServerManager *manager,
                     int maxConnections,
                     const uint &buffsize,QObject *parent)
    : QObject(parent)
    ,addr(address)
    , m_port(port)
    , m_consumerCount(consumerCount)
    , m_manager(manager)
    , m_server(nullptr)
    , m_queue(new ThreadSafeQueue<NetData>(buffsize))   // 创建线程安全队列
    ,m_connManager(new ConnectionManager(this))
{
    // 设置连接管理器参数
    m_connManager->setGlobalMax(maxConnections);
    m_connManager->setStrategy(ConnectionManager::FixedLimit);

    connect(m_connManager, &ConnectionManager::connectionRejected,
            this, &TCPserver::onConnectionRejected);
    connect(m_connManager, &ConnectionManager::statsUpdated,
            this, &TCPserver::onConnManagerStats);
    // 创建 TCP 服务器对象，并设置父对象为 this 以便自动管理生命周期
    m_server = new QTcpServer(this);
    // 连接新连接信号到处理槽
    connect(m_server, &QTcpServer::newConnection, this, &TCPserver::onNewConnection);
    connect(m_connManager, &ConnectionManager::connectionAccepted,[=](const QHostAddress& host,QString msg){
        emit servermanagerMsg(host, msg);
    });


    m_keepAliveTimer = new QTimer(this);
    connect(m_keepAliveTimer, &QTimer::timeout,this,&TCPserver::updateActivityTime);
}

/**
 * @brief TCPserver 析构函数
 *
 * 调用 stop() 停止服务器，并释放队列内存。
 */
TCPserver::~TCPserver()
{
    stop();             // 停止服务器并清理资源
    delete m_queue;      // 释放队列（注意：确保此时没有线程再使用队列）
}

/**
 * @brief 启动 TCP 服务器
 * @return 成功返回 true，失败返回 false
 *
 * 如果服务器尚未启动，则创建新的 QTcpServer（实际上构造函数已创建，此处逻辑可能冗余），
 * 开始监听指定端口。若监听成功，启动消费者线程池，否则发射错误信号。
 */
bool TCPserver::start()
{
    if (m_server && m_server->isListening()) {
        return true; // 已经在监听
    }

    // 如果 m_server 存在但没在监听（比如之前 stop 了）
    if (!m_server) {
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection,
                this, &TCPserver::onNewConnection);
    }
    // 监听所有网络接口的指定端口
    if (!m_server->listen(this->addr, m_port)) {
        // 监听失败，发射错误信号并清理
        emit error("TCP服务器监听失败: " + m_server->errorString());
        delete m_server;
        m_server = nullptr;
        return false;
    }

    // 连接新连接信号（可能之前已连接，但重新创建后需要重新连接）
    connect(m_server, &QTcpServer::newConnection, this, &TCPserver::onNewConnection);

    // 启动消费者线程池
    startConsumers();

    qDebug() << "[TCPserver] 启动成功，监听端口:" << m_port;
    // 启动定时器（每秒？这里用5秒）
    m_keepAliveTimer->start(5000);   // 开始自动刷新活动时间
    updateActivityTime();             // 立即更新一次，使状态立即变绿
    return true;
}

/**
 * @brief 停止 TCP 服务器
 *
 * 关闭所有客户端连接，停止监听，并等待消费者线程池结束。
 */
void TCPserver::stop()
{
    if (m_server) {
        // 关闭所有客户端连接：向每个客户端发送通知并安排销毁
        for (const auto &handler : m_clientHandlers) {
            handler->sendData("服务端即将关闭");
            handler->deleteLater();   // 延迟删除，确保当前事件处理完毕
        }
        m_clientHandlers.clear();     // 清空映射表

        // 关闭服务器监听
        m_server->close();
        delete m_server;               // 删除服务器对象
        m_server = nullptr;
    }

    // 停止消费者线程池：清除所有等待任务，并等待正在运行的任务结束（最多 3 秒）
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone(3000);
    qDebug() << "[TCPserver] 已停止，端口:" << m_port;
    m_keepAliveTimer->stop();   // 停止定时器，不再自动刷新
    disconnect(m_keepAliveTimer, &QTimer::timeout,this,&TCPserver::updateActivityTime);
}

void TCPserver::updateActivityTime()
{

    this->m_lastActivityTime = QDateTime::currentDateTime();
    // emit activityUpdated();  // 通知监听者
}

QDateTime TCPserver::lastActivityTime() const
{
    return m_lastActivityTime;
}

/**
 * @brief 启动消费者线程池
 *
 * 设置全局线程池的最大线程数，并创建指定数量的 ServerProcessTask 任务提交给线程池。
 * 每个任务持有队列指针和 ServerManager 指针，以便处理数据并回传结果。
 */
void TCPserver::startConsumers()
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
 * @brief 处理新客户端连接的槽函数
 *
 * 当有客户端连接时，循环接受所有待处理的连接，为每个客户端创建 ClientHandler 对象，
 * 并保存到映射表中，同时连接其断开信号。
 */
void TCPserver::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        // 获取下一个等待的连接套接字
        QTcpSocket* socket = m_server->nextPendingConnection();
        qintptr socketDescriptor = socket->socketDescriptor();   // 获取套接字描述符

        QHostAddress clientIp = socket->peerAddress();
        quint16 clientPort = socket->peerPort();

        // 1. 询问 ConnectionManager 是否可以接受此连接
        if (!m_connManager->canAcceptConnection(clientIp, socketDescriptor)) {
            // 拒绝连接：发送友好提示并关闭
            QString rejectMsg = QString("服务器繁忙，无法处理新连接。");
            socket->write(rejectMsg.toUtf8());
            socket->waitForBytesWritten(500);
            socket->disconnectFromHost();
            socket->deleteLater();
            continue; // 跳过后续处理，继续下一个待处理连接
        }

        socket->setParent(nullptr);   // 将套接字所有权转移，避免被 QTcpServer 自动删除


        // 创建客户端处理器，传入描述符、队列和父对象（父对象为 this，以便自动管理）
        ClientHandler* handler = new ClientHandler(socketDescriptor, m_queue, this);
        // 保存到映射表，键为描述符，方便后续根据描述符查找
        m_clientHandlers[socketDescriptor] = handler;

        // 监听客户端断开信号，以便从映射表中移除
        connect(handler, &ClientHandler::disconnected,
                this, &TCPserver::onClientDisconnected);
        // 通知连接管理器记录该连接
        m_connManager->addConnection(clientIp, clientPort, socketDescriptor);
    }
}

/**
 * @brief 客户端断开连接的处理槽
 * @param socketDescriptor 断开客户端的套接字描述符
 *
 * 从映射表中查找对应的 ClientHandler，安排其销毁并从映射表中移除。
 */
void TCPserver::onClientDisconnected(const qintptr &socketDescriptor)
{
    auto it = m_clientHandlers.find(socketDescriptor);
    if (it != m_clientHandlers.end()) {
        // 调用 deleteLater 安全销毁对象（避免在信号处理中直接删除）
        // 从连接管理器中移除（通过描述符）
        m_connManager->removeConnectionByDescriptor(socketDescriptor);
        it.value()->deleteLater();
        m_clientHandlers.erase(it);   // 从映射表中移除
    }
}
/**
 * @brief 客户端断开连接的处理槽
 * @param ip 客户端的ip
 * @param reason 被服务器拒绝的原因
 *
 * 输出被拒绝的原因。
 */
void TCPserver::onConnectionRejected(const QHostAddress& ip, const QString& reason)
{
    emit connectionRejected(ip, 0, reason);
    qWarning() << "[TCPserver] 连接被拒绝 - IP:" << ip.toString() << "原因:" << reason;
}
/**
 * @brief 客户端断开连接的处理槽
 * @param socketDescriptor 断开客户端的套接字描述符
 *
 * 从映射表中查找对应的 ClientHandler，安排其销毁并从映射表中移除。
 */
void TCPserver::onConnManagerStats(const int &totalConnections, const int &uniqueIps)
{
    emit statsUpdated(totalConnections, uniqueIps);

    // 可以根据统计信息动态调整
    if (totalConnections > m_connManager->getCurrentConnections() * 0.8) {
        qDebug() << "[TCPserver] 接近连接上限，当前:" << totalConnections;
    }
}
