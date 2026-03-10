#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include "NetManager/common.h"

// 客户端数据发送线程（生产者）
class ClientSender : public QThread
{
    Q_OBJECT
public:
    explicit ClientSender(ThreadSafeQueue<QByteArray>* queue, QTcpSocket* socket, QObject* parent = nullptr)
        : QThread(parent), m_queue(queue), m_socket(socket), m_running(true) {}

    //解构发送数据
    ~ClientSender() {
        //判断是否运行
        m_running = false;
        m_queue->push(QByteArray()); // 唤醒等待的线程
        wait();
    }

protected:
    void run() override ;

private:
    //线程安全
    ThreadSafeQueue<QByteArray>* m_queue;
    QTcpSocket* m_socket;
    bool m_running;
};

// TCP客户端（带自动重连、心跳、收发分离）
class TCPClient : public QObject
{
    Q_OBJECT
public:
    explicit TCPClient(const QString& ip = SERVER_IP, quint16 port = SERVER_PORT, QObject* parent = nullptr)
        : QObject(parent), m_serverIp(ip), m_serverPort(port) {
        // 初始化Socket
        m_socket = new QTcpSocket(this);
        //处理Nagle 算法
        m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);

        // 初始化信号连接
        connect(m_socket, &QTcpSocket::connected, this, &TCPClient::onConnected);
        connect(m_socket, &QTcpSocket::disconnected, this, &TCPClient::onDisconnected);
        connect(m_socket, &QTcpSocket::readyRead, this, &TCPClient::onReadyRead);
        connect(m_socket, &QTcpSocket::errorOccurred, this, &TCPClient::onError);

        // 初始化发送队列和发送线程（生产者）
        m_sendQueue = new ThreadSafeQueue<QByteArray>();
        m_sender = new ClientSender(m_sendQueue, m_socket, this);
        m_sender->start();

        // 初始化重连定时器
        m_reconnectTimer = new QTimer(this);
        m_reconnectTimer->setInterval(RECONNECT_INTERVAL);
        connect(m_reconnectTimer, &QTimer::timeout, this, &TCPClient::connectToServer);

        // 初始化心跳定时器
        m_heartbeatTimer = new QTimer(this);
        m_heartbeatTimer->setInterval(HEARTBEAT_INTERVAL);
        connect(m_heartbeatTimer, &QTimer::timeout, this, &TCPClient::sendHeartbeat);

        // 主动连接服务端
        connectToServer();
    }

    // 连接服务端
    void connectToServer();

    // 发送数据（外部调用接口）
    void sendData(const QString& data);
private slots:
    // 连接成功
    void onConnected();

    // 断开连接
    void onDisconnected();

    // 接收服务端数据（消费者）
    void onReadyRead();

    // 错误处理
    void onError(QAbstractSocket::SocketError error);

    // 发送心跳包
    void sendHeartbeat();

private:
    QString m_serverIp;
    quint16 m_serverPort;
    QTcpSocket* m_socket;

    // 发送相关
    ThreadSafeQueue<QByteArray>* m_sendQueue;
    // 客户端发送
    ClientSender* m_sender;

    // 重连定时器
    QTimer* m_reconnectTimer;
    // 心跳定时器
    QTimer* m_heartbeatTimer;
};

#endif // CLIENT_H
