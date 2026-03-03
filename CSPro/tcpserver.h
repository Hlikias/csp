#ifndef TCPSERVER_H
#define TCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadPool>
#include <QRunnable>
#include <QMap>
#include <QObject>
#include"server.h"

class TCPserver:public QObject
{
    Q_OBJECT
public:
    explicit TCPserver(int port = SERVER_PORT, int consumerCount = 8, QObject* parent = nullptr)
        : QObject(parent), m_port(port), m_consumerCount(consumerCount) {
        // 初始化线程安全队列
        m_queue = new ThreadSafeQueue<NetData>();

        // 初始化TCP服务端
        m_server = new QTcpServer(this);
        connect(m_server, &QTcpServer::newConnection, this, &TCPserver::onNewConnection);

        // 启动消费者线程池
        startConsumers();
    }

    // 启动服务端
    bool start();
    // 停止服务端
    void stop() ;
private slots:
    // 新客户端连接
    void onNewConnection();

private:
    // 启动消费者线程池
    void startConsumers();
    //端口
    int m_port;
    int m_consumerCount;
    QTcpServer* m_server;
    ThreadSafeQueue<NetData>* m_queue;
    QMap<qintptr, ClientHandler*> m_clientHandlers; // 管理所有客户端
};

#endif // TCPSERVER_H
