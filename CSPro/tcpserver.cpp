#include "tcpserver.h"
// 启动服务端
bool TCPserver::start() {
    if (m_server->listen(QHostAddress::Any, m_port)) {
        qDebug() << "[服务端] 启动成功，监听端口:" << m_port;
        return true;
    } else {
        qDebug() << "[服务端] 启动失败:" << m_server->errorString();
        return false;
    }
}
// 停止服务端
void TCPserver::stop() {
    // 停止线程池
    QThreadPool::globalInstance()->clear();
    QThreadPool::globalInstance()->waitForDone(3000);

    // 关闭所有客户端连接
    for (auto handler : m_clientHandlers) {
        handler->sendData("服务端即将关闭");
        handler->deleteLater();
    }
    m_clientHandlers.clear();

    // 关闭服务端
    m_server->close();
    qDebug() << "[服务端] 已停止";
}

// 新客户端连接
void TCPserver::onNewConnection() {
    while (m_server->hasPendingConnections()) {
        QTcpSocket* socket = m_server->nextPendingConnection();
        qintptr socketDescriptor = socket->socketDescriptor();
        socket->setParent(nullptr);

        // 创建客户端处理器
        ClientHandler* handler = new ClientHandler(socketDescriptor, m_queue);
        m_clientHandlers[socketDescriptor] = handler;

        // 监听客户端断开
        connect(handler, &ClientHandler::disconnected, this, [this](qintptr fd) {
            m_clientHandlers.remove(fd);
        });
    }
}
// 启动消费者线程池
void TCPserver::startConsumers() {
    QThreadPool* pool = QThreadPool::globalInstance();
    pool->setMaxThreadCount(m_consumerCount); // 根据CPU核心数调整

    for (int i = 0; i < m_consumerCount; ++i) {
        ServerProcessTask* task = new ServerProcessTask(m_queue);
        pool->start(task);
    }

    qDebug() << "[服务端] 启动" << m_consumerCount << "个消费者线程";
}
