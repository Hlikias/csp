#include "client.h"

/**
 * @brief 客户端发送线程的运行函数
 *
 * 该函数在 ClientSender 线程中执行，持续从发送队列中取出数据并发送到服务端。
 * 当队列为空时，pop() 可能返回空数据，此时跳过本次循环。
 * 每次发送后短暂休眠（100ms），避免发送过快导致队列耗尽或占用过多 CPU。
 * 该循环由 m_running 标志控制，可安全退出。
 */
void ClientSender::run(){
    while (m_running) {
        QByteArray data = m_queue->pop();   // 阻塞直到有数据或队列被销毁
        if (data.isEmpty()) continue;       // 空数据视为无效，继续等待

        // 检查套接字状态，仅当已连接时发送
        if (m_socket->state() == QTcpSocket::ConnectedState) {
            m_socket->write(data);           // 写入发送缓冲区
            m_socket->flush();                // 立即发送（非必须，但确保数据不滞留）
            qDebug() << "[客户端] 发送数据:" << QString(data);
        } else {
            // 未连接时，数据被丢弃（可根据业务需求改为缓存）
            qDebug() << "[客户端] 未连接，暂存数据（实际项目可缓存）";
        }

        QThread::msleep(100); // 控制发送频率，避免空转
    }
}

/**
 * @brief 连接到服务端
 *
 * 检查当前套接字状态，如果未连接则发起连接请求。
 * 连接是异步的，成功或失败会通过信号通知。
 */
void TCPClient::connectToServer() {
    if (m_socket->state() != QTcpSocket::ConnectedState) {
        m_socket->connectToHost(m_serverIp, m_serverPort);
        qDebug() << "[客户端] 正在连接" << m_serverIp << ":" << m_serverPort;
    }
}

/**
 * @brief 对外发送数据接口
 * @param data 要发送的字符串数据（UTF-8编码）
 *
 * 将数据转换为 QByteArray 并放入发送队列，由发送线程异步处理。
 * 此函数是线程安全的，可在任何线程调用。
 */
void TCPClient::sendData(const QString& data) {
    m_sendQueue->push(data.toUtf8());
}

/**
 * @brief 连接成功时的槽函数（由 QTcpSocket::connected 信号触发）
 *
 * 连接成功后停止重连定时器，启动心跳定时器，并输出日志。
 */
void TCPClient::onConnected() {
    qDebug() << "[客户端] 连接服务端成功:" << m_serverIp << ":" << m_serverPort;
    m_reconnectTimer->stop();       // 停止自动重连
    m_heartbeatTimer->start();      // 开始发送心跳包
}

/**
 * @brief 断开连接时的槽函数（由 QTcpSocket::disconnected 信号触发）
 *
 * 连接断开后停止心跳，启动重连定时器尝试重新连接，并记录日志。
 */
void TCPClient::onDisconnected() {
    qDebug() << "[客户端] 与服务端断开连接";
    m_heartbeatTimer->stop();       // 停止心跳
    m_reconnectTimer->start();      // 开始尝试重连
}

/**
 * @brief 收到服务端数据时的槽函数（由 QTcpSocket::readyRead 信号触发）
 *
 * 读取所有可用数据并打印日志。可在此处扩展业务逻辑，如解析协议、处理回复等。
 */
void TCPClient::onReadyRead() {
    QByteArray data = m_socket->readAll();  // 一次性读取所有待处理数据
    if (!data.isEmpty()) {
        qDebug() << "[客户端] 收到服务端回复:" << QString(data);
        // TODO: 解析数据、触发业务处理等
    }
}

/**
 * @brief 错误处理槽函数（由 QTcpSocket::errorOccurred 信号触发）
 * @param error 错误类型（未直接使用，可通过 m_socket->errorString() 获取详情）
 *
 * 发生错误时关闭套接字，并记录错误信息。注意：关闭套接字会触发 disconnected 信号，
 * 因此重连逻辑将在 onDisconnected 中处理。
 */
void TCPClient::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    qDebug() << "[客户端] 错误:" << m_socket->errorString();
    m_socket->close();  // 关闭套接字，触发 disconnected 信号
}

/**
 * @brief 发送心跳包
 *
 * 由心跳定时器定时调用，向服务端发送一个简单的心跳消息，以维持连接或检测断线。
 * 实际应用中可根据协议定义心跳包内容。
 */
void TCPClient::sendHeartbeat() {
    sendData("心跳包");
}
