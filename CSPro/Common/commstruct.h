#ifndef COMMSTRUCT_H
#define COMMSTRUCT_H
#include <QObject>
#include <QByteArray>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QThread>
#include <QDebug>
#include <QHostAddress>
// 常量定义
const quint16 SERVER_PORT = 8888;          // 服务端端口
const QString SERVER_IP = "127.0.0.1";     // 服务端IP
const int QUEUE_MAX_SIZE = 10000;          // 缓冲区最大容量
const int HEARTBEAT_INTERVAL = 5000;       // 心跳间隔（ms）
const int RECONNECT_INTERVAL = 3000;       // 重连间隔（ms）

// 数据结构体（用于C/S通信和线程间传递）
enum class TransportType { TCP, UDP_Unicast, UDP_Multicast, UDP_Broadcast };
//这个是传过来的协议
struct NetData {
    TransportType type;               // 协议类型，用于区分
    // 通用信息
    QByteArray data;                 // 数据内容
    QHostAddress senderAddress;       // 发送者IP（对UDP有效）
    quint16 senderPort;               // 发送者端口（对UDP有效）
    // TCP 专用：socket 描述符（用于定位客户端）
    qintptr socketDescriptor = 0;
    // 便捷日志字段
    QString clientIp;
    quint16 clientPort = 0;
};
//这个是要处理的数据
struct ProcessedData {
    TransportType type;
    QByteArray originalData;
    QString decodedMessage;
    QHostAddress senderAddress;
    quint16 senderPort;
    qintptr socketDescriptor;
};

// 服务器状态结构体
struct ServerStatus {
    QString type;      // "TCP" 或 "UDP"
    quint16 port;      // 端口号
    QString color;     // "green", "orange", "red"
    QString Name="";
};

#endif // COMMSTRUCT_H
