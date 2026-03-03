#include "server.h"
// 发送数据给客户端

void ClientHandler::sendData(const QByteArray& data) {
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->write(data);
        m_socket->flush();
    }
}
// 客户端断开连接
void ClientHandler::onDisconnected() {
    qDebug() << "[客户端] 断开连接:" << m_clientIp << ":" << m_clientPort;
    m_socket->close();
    emit disconnected(this->m_socketDescriptor);
    deleteLater();
}

// 错误处理
void ClientHandler::onError(QAbstractSocket::SocketError error) {
    qDebug() << "[客户端] 错误:" << m_clientIp << ":" << m_clientPort << "-" << this->m_socket->errorString();
    this->m_socket->close();
    emit disconnected(this->m_socketDescriptor);
    deleteLater();
}

// 接收客户端数据（生产者逻辑）
void ClientHandler::onReadyRead() {
    QByteArray data = this->m_socket->readAll();
    if (!data.isEmpty()) {
        NetData netData;
        netData.socketDescriptor = this->m_socketDescriptor;
        netData.data = data;
        netData.clientIp = m_clientIp;
        netData.clientPort = m_clientPort;
        m_queue->push(netData);

        // 回复客户端（可选）
        sendData(QString("服务端已收到: %1").arg(QString(data)).toUtf8());
    }
}


void ServerProcessTask :: run() {
    //
    while (!QThread::currentThread()->isInterruptionRequested()) {
        NetData data = m_queue->pop();
        if (data.socketDescriptor != 0 && !data.data.isEmpty()) {
            processData(data);
            QThread::msleep(5); // 模拟处理耗时
        }
    }
}

void ServerProcessTask :: processData(const NetData& data) {
    qDebug() << QString("[服务端-消费者%1] 处理%2:%3的数据: %4 (长度:%5)")
                    .arg((quint64)QThread::currentThreadId())
                    .arg(data.clientIp)
                    .arg(data.clientPort)
                    .arg(QString(data.data))
                    .arg(data.data.size());

    // 这里可扩展：
    // 1. 解析协议（JSON/Protobuf）
    // 2. 数据库操作
    // 3. 业务逻辑计算
    // 4. 广播给其他客户端
}

