#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QMap>
#include <QPointer>
#include <QTimer>
#include <QDateTime>
#include "common.h"   // 包含通用定义：TransportType, NetData, ProcessedData, ThreadSafeQueue 等
class ServerManager;   // 前置声明
/**
 * @brief 客户端处理器类，负责管理与单个客户端的 TCP 连接。
 *
 * 每个 ClientHandler 对象对应一个已连接的客户端，封装了对应的 QTcpSocket，
 * 处理数据的接收、发送、连接断开和错误事件。接收到的数据会转换为 NetData 结构
 * 并推入线程安全队列，供消费者线程处理。
 */
class ClientHandler : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数，接受已建立的客户端套接字描述符并初始化。
     * @param socketDescriptor 客户端套接字描述符（由 QTcpServer 提供）
     * @param queue 指向线程安全队列的指针，用于存放接收到的 NetData
     * @param parent Qt 父对象
     *
     * 内部创建 QTcpSocket 并关联描述符，设置 socket 选项，连接信号槽。
     * 如果绑定失败，则立即销毁自身。
     */
    explicit ClientHandler(qintptr socketDescriptor, ThreadSafeQueue<NetData>* queue, QObject* parent = nullptr);

    /**
     * @brief 向客户端发送数据。
     * @param data 要发送的字节数组
     *
     * 检查套接字状态，只有处于已连接状态时才执行写入和立即发送。
     */
    void sendData(const QByteArray& data);

    /**
     * @brief 获取客户端 IP 地址（字符串形式）。
     * @return 客户端 IP 字符串
     */
    QString clientIp() const { return m_clientIp; }

    /**
     * @brief 获取客户端端口号。
     * @return 客户端端口
     */
    quint16 clientPort() const { return m_clientPort; }

    /**
     * @brief 获取套接字描述符。
     * @return 套接字描述符
     */
    qintptr socketDescriptor() const { return m_socketDescriptor; }

private slots:
    /**
     * @brief 当套接字有数据可读时被调用（由 readyRead 信号触发）。
     *
     * 读取所有数据，封装成 NetData 结构（包含 TCP 类型、客户端信息），
     * 然后推入线程安全队列。
     */
    void onReadyRead();

    /**
     * @brief 当客户端断开连接时被调用（由 disconnected 信号触发）。
     *
     * 记录日志，关闭套接字，发射 disconnected 信号，并安排自身销毁。
     */
    void onDisconnected();

    /**
     * @brief 当套接字发生错误时被调用（由 errorOccurred 信号触发）。
     * @param error 错误类型
     *
     * 记录错误信息，关闭套接字，发射 disconnected 信号，并安排自身销毁。
     */
    void onError(QAbstractSocket::SocketError error);

    void onHeartbeatTimeout();

signals:
    /**
     * @brief 客户端断开连接时发射的信号。
     * @param socketDescriptor 断开客户端的套接字描述符，用于服务器清理映射表。
     */
    void disconnected(qintptr socketDescriptor);

    void dataReceived();   // 新增信号，在 onReadyRead 中发射

private:
    qintptr m_socketDescriptor;              // 客户端的套接字描述符
    QTcpSocket* m_socket;                    // 与该客户端通信的 TCP 套接字
    ThreadSafeQueue<NetData>* m_queue;       // 指向共享线程安全队列的指针
    QString m_clientIp;                      // 客户端 IP 字符串
    quint16 m_clientPort;                    // 客户端端口号


    QTimer* m_heartbeatTimer;                   // 心跳定时器
    static const int HEARTBEAT_TIMEOUT = 30000; // 30秒超时
};


/**
 * @brief 数据处理任务类，作为消费者运行在线程池中。
 *
 * 每个 ServerProcessTask 对象从共享队列中取出 NetData 数据，
 * 调用 processData 进行实际业务处理，处理结果通过跨线程调用传递给 ServerManager。
 */
class ServerProcessTask : public QRunnable
{
public:
    /**
     * @brief 构造函数。
     * @param queue 指向共享线程安全队列的指针
     * @param manager 指向 ServerManager 的指针，用于将处理结果传回主线程
     *
     * 设置任务为自动删除（执行完后自动销毁）。
     */
    explicit ServerProcessTask(ThreadSafeQueue<NetData>* queue,
                               ServerManager* manager);

    /**
     * @brief 任务入口函数，由线程池调用。
     *
     * 循环从队列中取数据，调用 processData 处理，直到线程被请求中断。
     */
    void run() override;

private:
    /**
     * @brief 处理单个 NetData 数据的业务逻辑。
     * @param data 从队列取出的原始数据
     *
     * 根据 data.type 进行不同处理，构建 ProcessedData 结果，
     * 然后通过 QMetaObject::invokeMethod 跨线程调用 ServerManager::onDataProcessed。
     */
    void processData(const NetData& data);

    ThreadSafeQueue<NetData>* m_queue;   // 共享队列指针
    ServerManager *m_manager;            // ServerManager 指针（使用 QPointer 防止野指针）
};


class RunTimeMarkable
{
public:

    virtual ~RunTimeMarkable() = default;

    // 更新最后活动时间（子类需实现并发射自己的信号）
    virtual void updateActivityTime() = 0;

    // 获取最后活动时间
    virtual QDateTime lastActivityTime() const = 0;
};


#endif // SERVER_H
