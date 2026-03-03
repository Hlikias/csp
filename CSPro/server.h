#ifndef SERVER_H
#define SERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QMap>
#include "common.h"
/*
 * 客户端处理函数，记录客户端的信息

*/
class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, ThreadSafeQueue<NetData>* queue, QObject* parent = nullptr)
        : QObject(parent), m_socketDescriptor(socketDescriptor), m_queue(queue) {
        //获取到当前的tcp通信
        m_socket = new QTcpSocket(this);
        // 禁用Nagle算法，提升小数据传输效率
        m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        //链接读取函数
        connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
        //断开函数
        connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
        //错误处理
        connect(m_socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onError);

        if (!m_socket->setSocketDescriptor(socketDescriptor)) {
            qDebug() << "[客户端] 绑定失败:" << m_socket->errorString();
            deleteLater();
            return;
        }
        // 记录客户端信息
        m_clientIp = m_socket->peerAddress().toString();
        m_clientPort = m_socket->peerPort();
        qDebug() << "[客户端] 新连接:" << m_clientIp << ":" << m_clientPort << "(fd:" << socketDescriptor << ")";
    }

    // 发送数据
    void sendData(const QByteArray& data);
    //客户端IP
    QString clientIp() const{ return m_clientIp; }
    //客户端端口
    quint16 clientPort() const { return m_clientPort; }
    //套接字描述
    qintptr socketDescriptor() const { return m_socketDescriptor; }

private slots:

    // 接收客户端数据（生产者逻辑）
    void onReadyRead();
    // 客户端断开连接
    void onDisconnected();
    // 错误处理
    void onError(QAbstractSocket::SocketError error);
signals:
    void disconnected(qintptr socketDescriptor); // 客户端断开信号
private:
    //socket信息
    qintptr m_socketDescriptor;
    //tcp套接字
    QTcpSocket* m_socket;
    //线程安全队列,解决生产者-消费者问题
    ThreadSafeQueue<NetData>* m_queue;
    QString m_clientIp;
    quint16 m_clientPort;
};

// 数据处理任务（消费者，运行在线程池）
class ServerProcessTask : public QRunnable
{
public:
    explicit ServerProcessTask(ThreadSafeQueue<NetData>* queue, QObject* parent = nullptr)
        : m_queue(queue) {
        setAutoDelete(true);
    }
    void run() override ;

private:
    void processData(const NetData& data) ;

    ThreadSafeQueue<NetData>* m_queue;
};


#endif // SERVER_H
