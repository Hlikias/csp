#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QThreadPool>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QPair>
#include <QHash>
#include "common.h"   // 包含 NetData, TransportType 等
#include "server.h"   // 包含 ServerProcessTask

inline uint qHash(const QPair<QHostAddress, quint16> &key, uint seed = 0) {
    // 组合哈希：QHostAddress 的哈希值异或端口号的哈希值
    return qHash(key.first) ^ qHash(key.second, seed);
}

// 前置声明 ServerManager
class ServerManager;

/**
 * @brief UDP 服务器类，封装 QUdpSocket，支持单播、组播和广播接收。
 *
 * 内部拥有一个线程安全队列，接收到的数据报转换为 NetData 结构后推入队列，
 * 并启动消费者线程池（ServerProcessTask）处理数据。可通过 addMulticastGroup()
 * 加入多个组播组，通过 setEnableBroadcast() 开启广播接收。
 */
class UDPserver : public QObject, public RunTimeMarkable
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
    explicit UDPserver(quint16 port,
                       int consumerCount,
                       ServerManager* manager,
                       const uint &buffsize= QUEUE_MAX_SIZE,
                       QObject* parent = nullptr);

    /**
     * @brief 构造函数。
     * @param address 监听端口
     * @param port 监听端口
     * @param consumerCount 消费者线程数
     * @param manager 指向 ServerManager 的指针，用于创建 ServerProcessTask 时传递
     * @param parent Qt 父对象
     */
    explicit UDPserver(const QHostAddress& address,
                       quint16 port,
                       int consumerCount,
                       ServerManager* manager,
                       const uint &buffsize = QUEUE_MAX_SIZE,
                       QObject* parent = nullptr);
    /**
     * @brief 析构函数，停止服务器并释放资源。
     */
    ~UDPserver();

    /**
     * @brief 添加一个组播组地址，使服务器加入该组。
     * @param groupAddress 组播 IP 地址
     */
    void addMulticastGroup(const QHostAddress& groupAddress);

    /**
     * @brief 清空所有已设置的组播组地址。
     */
    void clearMulticastGroups();

    /**
     * @brief 设置是否启用广播接收。
     * @param enable true 启用，false 禁用
     */
    void setEnableBroadcast(bool enable);

    /**
     * @brief 启动服务器，绑定端口并进行相关设置（加入组播组、广播选项）。
     * @return 成功返回 true，否则 false
     */
    bool start();

    /**
     * @brief 停止服务器，关闭套接字。
     */
    void stop();

    /**
     * @brief 获取监听端口。
     * @return 端口号
     */
    quint16 port() const { return m_port; }

    void setServeName(const QString &name){this->ServerName = name;}
    const QString getServeName(){return this->ServerName;}
    void updateActivityTime() override;
    QDateTime lastActivityTime() const override;


signals:
    /**
     * @brief 服务器发生错误时发射的信号。
     * @param message 错误信息
     */
    void error(const QString& message);
    /**
     * @brief 当客户端被判定离线时发送信号。
     * @param clientIp 客户端的IP
     * @param clientPort 客户端的Port
     */
    void clientOffline(const QHostAddress& clientIp, quint16 clientPort);

    void activityUpdated();


private slots:
    /**
     * @brief 当有数据报到达时被调用（由 QUdpSocket::readyRead 触发）。
     *
     * 循环接收所有待处理的数据报，构造 NetData 结构（根据目的地址判断类型），
     * 然后推入队列。
     */
    void onReadyRead();
    /**
     * @brief 定时器触发
     *
     * 定时发送心跳包
     */
    void onHeartbeatSend();
    /**
     * @brief 客户端超时检测
     */
    void onClientTimeout(const QHostAddress& clientIp, quint16 clientPort);

private:
    /**
     * @brief 启动消费者线程池，创建指定数量的 ServerProcessTask 任务。
     *
     * 每个任务都持有 m_queue 和 m_manager 指针。
     */
    void startConsumers();

    QString ServerName = "";
    QHostAddress m_address;                // 绑定的 IP 地址
    quint16 m_port;                        // 监听端口
    int m_consumerCount;                   // 消费者线程数
    QUdpSocket* m_socket;                  // 实际的 UDP 套接字
    ThreadSafeQueue<NetData>* m_queue;     // 线程安全队列，用于存放原始数据
    QList<QHostAddress> m_multicastGroups; // 要加入的组播组地址列表
    bool m_enableBroadcast;                // 是否启用广播接收
    ServerManager* m_manager;              // 指向 ServerManager 的指针
    // 心跳管理相关
    struct ClientInfo {
        int missedPongs;          // 连续未收到回复的次数
        QTimer* timeoutTimer;     // 超时定时器（用于单次心跳等待）
    };

    using ClientKey = QPair<QHostAddress, quint16>;
    QHash<ClientKey, ClientInfo> m_clients;   // 记录所有活跃客户端

    QDateTime m_lastActivityTime;
    QTimer *m_keepAliveTimer;


    QTimer* m_heartbeatSendTimer; // 发送心跳的全局定时器
    static const int HEARTBEAT_INTERVAL = 10000;   // 10秒发送一次心跳
    static const int HEARTBEAT_TIMEOUT = 5000;     // 发送后5秒内无回复视为一次丢失
    static const int MAX_MISSED_PONGS = 3;          // 连续3次未回复判定离线
};

#endif // UDPSERVER_H
