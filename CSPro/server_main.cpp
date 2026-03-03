#include "tcpserver.h"
#include "client.h"
#include <QCoreApplication>
#include <QSignalMapper>
#include <QTimer>
int server_execu(const quint16 server_port){
    // 创建并启动服务端（8个消费者线程）
    TCPserver server(SERVER_PORT, 8);
    if (!server.start()) {
        return -1;
    }
    return 0;
}

int client_execu(const QString server_ip,const quint16 server_port, const QString ){
    // 创建客户端
    TCPClient client(SERVER_IP, SERVER_PORT);

    // 定时发送测试数据（模拟业务数据）
    QTimer sendTimer;
    int dataCount = 0;
    QObject::connect(&sendTimer, &QTimer::timeout, [&]() {
        client.sendData(QString("测试数据%1").arg(++dataCount));
        if (dataCount >= 20) { // 发送20条后停止
            sendTimer.stop();
            qDebug() << "[客户端] 测试数据发送完成";
        }
    });
    sendTimer.start(1000); // 每秒发送1条
    return 0;
}
