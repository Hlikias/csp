#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QList>
#include <QHostAddress>
#include "tcpserver.h"
#include "udpserver.h"
#include "common.h"   // 包含 ProcessedData 定义
#include "../XmlManager/xmlmanager.h"


/**
 * @brief 服务器管理器类，统一管理多个 TCP 和 UDP 服务器。
 *
 * 可以添加任意数量的 TCP 和 UDP 服务器（每个 UDP 服务器可独立配置广播和组播），
 * 并提供统一的启动、停止接口。同时，它接收消费者线程处理后的数据（ProcessedData），
 * 并通过 dataReceived 信号转发给主窗口。
 */
class ServerManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数。
     * @param parent Qt 父对象
     *
     * 注册 ProcessedData 元类型以便跨线程传递。
     */
    explicit ServerManager(QObject *parent = nullptr);
    ServerManager(const serverParam &param, QObject *parent = nullptr);

    /**
     * @brief 析构函数，停止所有服务器并清理资源。
     */
    ~ServerManager();

    /**
     * @brief 添加一个 TCP 服务器。
     * @param port 监听端口
     * @param consumerCount 消费者线程数
     */
    void addTcpServer(const QString serverName,
                      const QString &ip, quint16 port,
                      const int &consumerCount = 8,
                      const int &maxConnections = 100,
                      const uint &buffsize = QUEUE_MAX_SIZE);

    /**
     * @brief 添加一个 UDP 服务器。
     * @param port 监听端口
     * @param consumerCount 消费者线程数
     * @param enableBroadcast 是否启用广播接收
     * @param multicastGroups 要加入的组播组地址列表
     */
    void addUdpServer(const QString serverName,
                      const QString &ip, quint16 port,
                      const int &consumerCount = 8,
                      const uint &buffsize = QUEUE_MAX_SIZE,
                      bool enableBroadcast = false,
                      const QList<QHostAddress>& multicastGroups = {}
                      );

    /**
     * @brief 启动所有已添加的服务器。
     * @return 如果所有服务器都成功启动返回 true，否则返回 false。
     *
     * 依次调用每个服务器的 start() 方法。
     */
    bool startAll();

    /**
     * @brief 停止所有服务器。
     *
     * 依次调用每个服务器的 stop() 方法。
     */
    void stopAll();

    /**
     * @brief 获取所有 TCP 服务器列表。
     * @return TCP 服务器列表
     */
    QList<TCPserver*> tcpServers() const { return m_tcpServers; }

    /**
     * @brief 获取所有 UDP 服务器列表。
     * @return UDP 服务器列表
     */
    QList<UDPserver*> udpServers() const { return m_udpServers; }

signals:
    /**
     * @brief 当任一服务器发生错误时发射的信号。
     * @param serverId 服务器标识（如 "TCP:8888" 或 "UDP:8889"）
     * @param message 错误信息
     */
    void error(const QString& serverId, const QString& message);

    /**
     * @brief 当消费者线程处理完数据后发射的信号。
     * @param data 处理后的数据
     *
     * 该信号在主线程中发射，主窗口可连接此信号以更新 UI。
     */
    void dataReceived(const ProcessedData& data);

    /**
     * @brief 客户端离线后发射的信号。
     * @param ip 客户端IP地址
     * @param ip 客户端端口
     *
     * 该信号在主线程中发射，主窗口可连接此信号以更新 UI。
     */
    void udpClientOffline(const QHostAddress& ip, quint16 port);

    /**
     * @brief 发送给窗口。
     * @param ip 客户端IP地址
     * @param msg 链接的信息
     *
     * 该信号在主线程中发射，主窗口可连接此信号以更新 UI。
     */
    void toWinQList(const QHostAddress& ip, QString msg);

    void serverStatusUpdated(const QList<ServerStatus> &statusList);

public slots:
    /**
     * @brief 接收消费者线程处理后的数据（由 ServerProcessTask 跨线程调用）。
     * @param data 处理后的数据
     *
     * 该槽运行在主线程，直接将数据以 dataReceived 信号转发出去。
     */
    void onDataProcessed(const ProcessedData& data);

private slots:

    /**
     * @brief 处理单个服务器发出的 error 信号，将其转换为带有服务器标识的 error 信号。
     * @param message 原始错误信息
     */
    void onServerError(const QString& message);

    /**
     * @brief 处理来自服务器的小部件消息。
     * @param host 主机地址
     * @param msg 消息内容
     */
    void onWidgetMsg(const QHostAddress& host,QString msg);



    void updateServerStatus();  // 定时器槽函数，计算状态并发射信号

private:
    QList<TCPserver*> m_tcpServers;   ///< 所有 TCP 服务器对象
    QList<UDPserver*> m_udpServers;   ///< 所有 UDP 服务器对象

    serverParam param;                ///< 服务器参数

    QTimer *m_statusTimer;            ///< 状态更新定时器
    static const int GREEN_THRESHOLD = 5;   ///< 5秒内为绿
    static const int ORANGE_THRESHOLD = 60; ///< 60秒内为橙，否则红
};


#endif // SERVERMANAGER_H
