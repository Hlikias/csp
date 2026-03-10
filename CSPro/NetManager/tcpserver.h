#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QMap>
#include "server.h"   // 包含 ClientHandler 和 ServerProcessTask
#include "connectionmanager.h"  // 添加连接管理器头文件
// 前置声明 ServerManager
class ServerManager;
struct NetData;
class ClientHandler;
class ServerProcessTask;
/**
 * @brief TCP 服务器类，封装 QTcpServer，管理客户端连接和数据队列。
 * 管理整个 TCP 服务器的生命周期
 * 管理消费者线程池
 * 管理客户端连接
 * 协调数据处理
 * 负责监听端口、接受客户端连接、创建 ClientHandler 并管理它们的生命周期。
 * 内部拥有一个线程安全队列，所有接收到的数据经 ClientHandler 推入该队列，
 * 并启动消费者线程池（ServerProcessTask）处理数据。
 */
class TCPserver :public QObject,public RunTimeMarkable
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param port 监听端口
     * @param consumerCount 消费者线程数
     * @param manager 指向 ServerManager 的指针，用于创建 ServerProcessTask 时传递
     * @param parent Qt 父对象
     */
    explicit TCPserver(quint16 port, int consumerCount, ServerManager* manager,
                       int maxConnections = 1000,  const uint &buffsize = QUEUE_MAX_SIZE, QObject* parent = nullptr);

    TCPserver(QHostAddress address, quint16 port, int consumerCount, ServerManager* manager,
              int maxConnections = 1000, const uint &buffsize = QUEUE_MAX_SIZE, QObject* parent = nullptr);

    /**
     * @brief 析构函数，停止服务器并释放资源。
     */
    ~TCPserver();

    /**
     * @brief 启动服务器，开始监听指定端口。
     * @return 成功返回 true，否则 false
     */
    bool start();

    /**
     * @brief 停止服务器，断开所有客户端连接并关闭监听。
     */
    void stop();

    /**
     * @brief 获取监听端口。
     * @return 端口号
     */
    quint16 port() const { return m_port; }
    /**
     * @brief 管理链接。
     * @return 管理器
     */
    ConnectionManager* getConnectionManager() { return m_connManager; }
    // 设置连接限制策略（转发给ConnectionManager）

    // 实现 RunTimeMarkable 接口
    void updateActivityTime() override;
    QDateTime lastActivityTime() const override;

    void setConnectionStrategy(ConnectionManager::LimitStrategy strategy) {
        m_connManager->setStrategy(strategy);
    }
    void setServeName(const QString &name){this->ServerName = name;}

    void setPerIpMax(const int &max) { m_connManager->setPerIpMax(max); }
    void setRateLimit(const int &maxPerSecond) { m_connManager->setRateLimit(maxPerSecond); }
    void setDynamicThreshold(const int &cpuThreshold, int memThreshold) {
        m_connManager->setDynamicThreshold(cpuThreshold, memThreshold);
    }

    // 黑名单管理
    void addToBlacklist(const QHostAddress& ip, int durationSeconds = 3600) {
        m_connManager->addToBlacklist(ip, durationSeconds);
    }
    void removeFromBlacklist(const QHostAddress& ip) {
        m_connManager->removeFromBlacklist(ip);
    }

    // 统计信息
    const int getCurrentConnections() const { return m_connManager->getCurrentConnections(); }
    const int getConnectionsByIp(const QHostAddress& ip) const {
        return m_connManager->getConnectionsByIp(ip);
    }
    const QString getServeName(){return this->ServerName;}

signals:
    /**
     * @brief 服务器发生错误时发射的信号。
     * @param message 错误信息
     */
    void error(const QString& message);
    void servermanagerMsg(const QHostAddress& host,QString msg);
    void connectionRejected(QHostAddress clientIp, quint16 clientPort, const QString& reason);
    void statsUpdated(const int &totalConnections, const int &uniqueIps);
    void activityUpdated();// 自己的信号，当活动时间更新时发射

private slots:
    /**
     * @brief 当有新客户端连接时被调用（由 QTcpServer::newConnection 触发）。
     *
     * 循环接受所有待处理的连接，为每个客户端创建 ClientHandler 对象，
     * 并将其加入 m_clientHandlers 映射表，同时连接其 disconnected 信号。
     */
    void onNewConnection();

    /**
     * @brief 当某个客户端断开连接时被调用（由 ClientHandler::disconnected 触发）。
     * @param socketDescriptor 断开客户端的套接字描述符
     *
     * 从映射表中移除该客户端处理器，并安排其销毁。
     */
    void onClientDisconnected(const qintptr &socketDescriptor);
    /**
     * @brief 当某个客户端被拒绝时被调用（由 ClientHandler::disconnected 触发）。
     * @param socketDescriptor 断开客户端的套接字描述符
     * @param reason 拒绝原因
     *
     */
    void onConnectionRejected(const QHostAddress& ip, const QString& reason);
    /**
     * @brief 查询链接状态。
     * @param totalConnections 总链接数
     * @param uniqueIps 总IP数
     *
     */
    void onConnManagerStats(const int &totalConnections, const int &uniqueIps);

private:
    /**
     * @brief 启动消费者线程池，创建指定数量的 ServerProcessTask 任务。
     *
     * 每个任务都持有 m_queue 和 m_manager 指针。
     */
    void startConsumers();

    QString ServerName = "";
    quint16 m_port;                                   // 监听端口
    int m_consumerCount;                              // 消费者线程数
    QTcpServer* m_server;                              // 实际的 TCP 服务器对象
    ThreadSafeQueue<NetData>* m_queue;                 // 线程安全队列，用于存放原始数据
    QMap<qintptr, ClientHandler*> m_clientHandlers;    // 管理所有客户端处理器，键为套接字描述符
    ServerManager* m_manager;                           // 指向 ServerManager 的指针
    QHostAddress addr = QHostAddress::Any;

    ConnectionManager* m_connManager;                  // 连接管理器

    QDateTime m_lastActivityTime;
    QTimer *m_keepAliveTimer;  // 用于定时刷新的计时器

};

#endif // TCPSERVER_H
