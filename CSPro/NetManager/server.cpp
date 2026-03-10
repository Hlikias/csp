#include "server.h"
#include "servermanager.h"
// ---------- ClientHandler 类实现 ----------
// server.cpp 中
ClientHandler::ClientHandler(qintptr socketDescriptor, ThreadSafeQueue<NetData>* queue, QObject* parent)
    : QObject(parent), m_socketDescriptor(socketDescriptor), m_queue(queue)
{
    m_socket = new QTcpSocket(this);
    m_socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(m_socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &ClientHandler::onError);

    if (!m_socket->setSocketDescriptor(socketDescriptor)) {
        qDebug() << "[客户端] 绑定失败:" << m_socket->errorString();
        deleteLater();
        return;
    }
    m_clientIp = m_socket->peerAddress().toString();
    m_clientPort = m_socket->peerPort();

    // 创建心跳定时器
    m_heartbeatTimer = new QTimer(this);
    m_heartbeatTimer->setInterval(HEARTBEAT_TIMEOUT);
    m_heartbeatTimer->setSingleShot(true);  // 单次触发，每次收到数据后重启
    connect(m_heartbeatTimer, &QTimer::timeout, this, &ClientHandler::onHeartbeatTimeout);
    m_heartbeatTimer->start();  // 启动
}
/**
 * @brief 向客户端发送数据
 * @param data 要发送的字节数组
 *
 * 检查套接字状态，仅当处于已连接状态时才发送数据。
 * 调用 write 将数据写入发送缓冲区，flush 确保数据立即发送（非必须，但有助于实时性）。
 */
void ClientHandler::sendData(const QByteArray& data) {
    if (m_socket->state() == QTcpSocket::ConnectedState) {
        m_socket->write(data);
        m_socket->flush();
    }
}

/**
 * @brief 客户端断开连接时的槽函数（由 QTcpSocket::disconnected 信号触发）
 *
 * 当客户端主动断开或网络异常导致连接断开时，该函数被调用。
 * 记录断开日志，关闭套接字，发射 disconnected 信号以便服务器移除该客户端的映射，
 * 最后调用 deleteLater 安全地销毁本对象（延迟删除，避免在信号处理中直接删除导致问题）。
 */
void ClientHandler::onDisconnected() {
    qDebug() << "[客户端] 断开连接:" << m_clientIp << ":" << m_clientPort;
    m_socket->close();
    emit disconnected(this->m_socketDescriptor);
    deleteLater();
}

/**
 * @brief 错误处理槽函数（由 QTcpSocket::errorOccurred 信号触发）
 * @param error 错误类型（未直接使用，可通过 m_socket->errorString() 获取详情）
 *
 * 当套接字发生错误时调用，记录错误信息，关闭套接字，并发射 disconnected 信号，
 * 最后销毁自身。注意：关闭套接字会触发 disconnected 信号，因此实际断开流程会走 onDisconnected，
 * 此处主动发射 disconnected 信号是为了确保服务器能及时清理资源。
 */
void ClientHandler::onError(QAbstractSocket::SocketError error) {
    Q_UNUSED(error);
    qDebug() << "[客户端] 错误:" << m_clientIp << ":" << m_clientPort << "-" << this->m_socket->errorString();
    this->m_socket->close();
    emit disconnected(this->m_socketDescriptor);
    deleteLater();
}

/**
 * @brief 接收客户端数据时的槽函数（由 QTcpSocket::readyRead 信号触发）
 *
 * 从套接字读取所有可用数据，封装成 NetData 结构（包含套接字描述符、客户端IP、端口、数据），
 * 推入共享队列 m_queue，供工作线程（ServerProcessTask）处理。
 * 同时可选地回复客户端一个确认消息（可注释掉或根据业务需求修改）。
 */
void ClientHandler::onReadyRead() {
    m_heartbeatTimer->start();  // 重置心跳定时器

    QByteArray data = this->m_socket->readAll();
    if (!data.isEmpty()) {
        NetData netData;
        netData.type = TransportType::TCP;          // 设置为 TCP 类型
        netData.socketDescriptor = this->m_socketDescriptor;
        netData.data = data;
        netData.clientIp = m_clientIp;
        netData.clientPort = m_clientPort;
        // 使用非阻塞 push，第二个参数 false
        if (!m_queue->push(netData, false)) {
            qWarning() << "[客户端] 队列已满，断开客户端:" << m_clientIp << ":" << m_clientPort;
            sendData("服务器负载过高，连接将被关闭");
            m_socket->disconnectFromHost();  // 触发 onDisconnected，最终清理资源
        } else {
            sendData(QString("服务端已收到: %1").arg(QString(data)).toUtf8());
        }
    }
    emit dataReceived();
}


void ClientHandler::onHeartbeatTimeout() {
    qDebug() << "[客户端] 心跳超时，断开连接:" << m_clientIp << ":" << m_clientPort;
    m_socket->disconnectFromHost();  // 会触发 onDisconnected，最终清理资源
}

// ---------- ServerProcessTask 类实现 ----------


// ServerProcessTask 构造函数定义
ServerProcessTask::ServerProcessTask(ThreadSafeQueue<NetData>* queue, ServerManager* manager)
    : m_queue(queue), m_manager(manager)
{
    setAutoDelete(true);   // 任务执行完毕后自动销毁
}
/**
 * @brief 工作线程的入口函数（继承自 QThread）
 *
 * 在线程中循环从共享队列中取出 NetData 数据，调用 processData 进行业务处理。
 * 循环由线程的 interruptionRequested 标志控制，可安全退出。
 * 每次处理后短暂休眠（5ms），避免 CPU 空转。
 */
void ServerProcessTask::run() {
    while (!QThread::currentThread()->isInterruptionRequested()) {
        NetData data;
        if (m_queue->tryPop(data)) {
            if (!data.data.isEmpty()) {
                processData(data);
            }
        } else {
            // 队列空，短暂休眠避免忙等
            QThread::msleep(5);
        }
    }
}

/**
 * @brief 处理客户端数据的具体业务逻辑
 * @param data 包含客户端信息和数据的结构体
 *
 * 该函数在消费者线程中被调用，负责对接收到的数据进行实际处理。
 * 目前仅打印日志，可扩展为：
 *   - 解析协议（JSON/Protobuf）
 *   - 数据库操作
 *   - 触发其他业务逻辑
 *   - 广播给其他客户端
 */
void ServerProcessTask::processData(const NetData& data) {
    // 构建处理结果
    ProcessedData result;
    result.type = data.type;
    result.originalData = data.data;
    result.senderAddress = data.senderAddress;
    result.senderPort = data.senderPort;
    result.socketDescriptor = data.socketDescriptor;
    result.decodedMessage = QString::fromUtf8(data.data);

    // 跨线程发送给 ServerManager
    // 元对象系统
    if (m_manager) {
        QMetaObject::invokeMethod(m_manager, "onDataProcessed",// 目标对象// 要调用的槽函数名
                                  Qt::QueuedConnection, // 连接类型：队列连接
                                  Q_ARG(ProcessedData, result)); // 参数：将 result 传递给槽函数
    }
}
