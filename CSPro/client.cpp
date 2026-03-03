#include "client.h"

void ClientSender::run(){
    while (m_running) {
        QByteArray data = m_queue->pop();
        if (data.isEmpty()) continue;

        // 发送数据到服务端
        if (m_socket->state() == QTcpSocket::ConnectedState) {
            m_socket->write(data);
            m_socket->flush();
            qDebug() << "[客户端] 发送数据:" << QString(data);
        } else {
            qDebug() << "[客户端] 未连接，暂存数据（实际项目可缓存）";
        }

        QThread::msleep(100); // 模拟发送间隔
    }
}
// 连接服务端
void TCPClient::connectToServer() {
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        m_socket->connectToHost(m_serverIp, m_serverPort);
        qDebug() << "[客户端] 正在连接" << m_serverIp << ":" << m_serverPort;
    }
}

// 发送数据（外部调用接口）
void TCPClient::sendData(const QString& data) {
    m_sendQueue->push(data.toUtf8());
}

// 连接成功
void TCPClient::onConnected() {
    qDebug() << "[客户端] 连接服务端成功:" << m_serverIp << ":" << m_serverPort;
    m_reconnectTimer->stop();       // 停止重连
    m_heartbeatTimer->start();      // 启动心跳
}

// 断开连接
void TCPClient::onDisconnected() {
    qDebug() << "[客户端] 与服务端断开连接";
    m_heartbeatTimer->stop();       // 停止心跳
    m_reconnectTimer->start();      // 启动重连
}

// 接收服务端数据（消费者）
void TCPClient::onReadyRead() {
    QByteArray data = m_socket->readAll();
    if (!data.isEmpty()) {
        qDebug() << "[客户端] 收到服务端回复:" << QString(data);
        // 这里可扩展：解析服务端数据、业务处理等
    }
}

// 错误处理
void TCPClient::onError(QAbstractSocket::SocketError error) {
    qDebug() << "[客户端] 错误:" << m_socket->errorString();
    m_socket->close();
}

// 发送心跳包
void TCPClient::sendHeartbeat() {
    sendData("心跳包");
}
