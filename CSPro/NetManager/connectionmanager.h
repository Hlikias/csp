#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QHostAddress>
#include <QMap>
#include <QDateTime>
#include <QTimer>
#include <QMutex>
#include <QReadWriteLock>

/**
 * @brief 连接管理器类
 *
 * 负责管理 TCP 服务器的客户端连接，提供多种连接限制策略，
 * 防止资源耗尽或恶意攻击。主要功能包括：
 * - 全局连接数上限限制
 * - 单个 IP 的连接数限制
 * - 基于系统负载的动态连接数调整
 * - 连接速率限制（防洪水攻击）
 * - IP 黑名单管理
 * - 连接统计与监控
 *
 * 该类是线程安全的，内部使用 QReadWriteLock 保护共享数据。
 */
class ConnectionManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 连接限制策略枚举
     */
    enum LimitStrategy {
        FixedLimit,      ///< 固定全局上限策略，限制同时连接的独立 IP 数量
        PerIPLimit,      ///< 每 IP 限制策略，每个 IP 允许的最大连接数
        DynamicLimit,    ///< 动态调整策略，根据系统 CPU/内存负载调整全局上限
        RateLimit        ///< 连接速率限制策略，每个 IP 每秒允许的最大新连接数
    };

    /**
     * @brief 连接信息结构体，用于记录每个连接的基本信息
     */
    struct ConnectionInfo {
        QHostAddress ip;          ///< 客户端 IP 地址
        quint16 port;             ///< 客户端端口号
        QDateTime connectTime;    ///< 连接建立时间
        qintptr socketDescriptor; ///< 套接字描述符，唯一标识一个连接
    };

    /**
     * @brief 构造函数
     * @param parent QObject 父对象，用于 Qt 内存管理
     *
     * 初始化定时器并设置默认参数：
     * - 全局最大连接数：1000
     * - 每 IP 最大连接数：10
     * - 最大连接速率：50 个/秒
     * - 动态阈值：CPU 80%，内存 90%
     */
    explicit ConnectionManager(QObject *parent = nullptr);
    /**
     * @brief 析构函数，停止所有定时器
     */
    ~ConnectionManager();

    // ==================== 策略设置接口 ====================
    /**
     * @brief 设置连接限制策略
     * @param strategy 要使用的策略
     *
     * 如果设置为 DynamicLimit，则会启动动态更新定时器；
     * 否则停止该定时器。
     */
    void setStrategy(LimitStrategy strategy);
    /**
     * @brief 设置全局最大连接数（适用于 FixedLimit 和 DynamicLimit）
     * @param max 最大连接数
     */
    void setGlobalMax(int max) { m_globalMax = max; }
    /**
     * @brief 设置每个 IP 的最大连接数（适用于 PerIPLimit）
     * @param max 每个 IP 允许的最大连接数
     */
    void setPerIpMax(int max) { m_perIpMax = max; }
    /**
     * @brief 设置最大连接速率（适用于 RateLimit）
     * @param maxPerSecond 每秒允许的最大新连接数
     */
    void setRateLimit(int maxPerSecond) { m_maxRatePerSecond = maxPerSecond; }
    /**
     * @brief 设置动态调整的阈值（适用于 DynamicLimit）
     * @param cpuThreshold CPU 使用率阈值（百分比）
     * @param memThreshold 内存使用率阈值（百分比）
     *
     * 当任一指标超过对应阈值时，会减少最大连接数；
     * 当两个指标都低于阈值时，会增加最大连接数。
     */
    void setDynamicThreshold(int cpuThreshold, int memThreshold) {
        m_cpuThreshold = cpuThreshold;
        m_memThreshold = memThreshold;
    }

    // ==================== 核心检查与连接管理 ====================

    /**
     * @brief 检查是否可以接受一个新的连接
     * @param clientIp 客户端 IP 地址
     * @param socketDescriptor 套接字描述符（用于记录，默认0表示忽略）
     * @return true 表示可以接受，false 表示拒绝
     *
     * 该函数按照当前设置的策略进行检查：
     * 1. 先检查 IP 是否在黑名单中
     * 2. 根据策略调用对应的检查函数
     * 如果拒绝，会发射 connectionRejected 信号。
     * 注意：此函数是只读操作，使用读锁。
     */
    bool canAcceptConnection(const QHostAddress& clientIp, qintptr socketDescriptor = 0);
    /**
     * @brief 记录一个新连接的建立
     * @param clientIp 客户端 IP
     * @param port 客户端端口
     * @param socketDescriptor 套接字描述符
     *
     * 更新内部统计数据结构，包括：
     * - IP 连接计数
     * - 连接详情列表
     * - 描述符映射
     * - 速率统计
     * 发射 connectionAccepted 信号。
     * 此函数会加写锁。
     */
    void addConnection(const QHostAddress& clientIp, quint16 port, qintptr socketDescriptor);
    /**
     * @brief 根据 IP 移除一个连接（通常用于客户端主动断开）
     * @param clientIp 要移除的客户端 IP
     *
     * 注意：如果该 IP 有多个连接，只会移除最新建立的那个。
     * 这只是一个简化的实现，实际场景可能需要根据具体描述符移除。
     */
    void removeConnection(const QHostAddress& clientIp);
    /**
     * @brief 根据套接字描述符移除连接（推荐使用，更精确）
     * @param socketDescriptor 要移除的连接描述符
     *
     * 通过描述符找到对应的连接信息，并从各个映射表中移除。
     * 这是断开连接时的标准清理方式。
     */
    void removeConnectionByDescriptor(qintptr socketDescriptor);

    // ==================== 统计信息查询 ====================
    /**
     * @brief 获取当前总连接数（不同 IP 的连接数总和）
     * @return 当前连接总数
     *
     * 注意：这里的“连接数”指的是不同 IP 的数量，
     * 但 m_ipConnectionCount 存储的是每个 IP 的连接计数值，
     * 因此总连接数应该是所有 IP 的连接数之和。
     * 当前实现返回 m_ipConnectionCount.size()，这实际上是不同 IP 的数量，
     * 可能不符合命名含义。建议改为计算总和。
     * 此处保留原代码逻辑，但需注意。
     */
    int getCurrentConnections() const {
        QReadLocker locker(&m_rwLock);
        return m_ipConnectionCount.size();
    }
    /**
     * @brief 获取指定 IP 的当前连接数
     * @param ip 要查询的 IP
     * @return 该 IP 的连接数
     */
    int getConnectionsByIp(const QHostAddress& ip) const;
    /**
     * @brief 获取当前所有活跃的 IP 地址列表
     * @return QList<QHostAddress> 包含所有活跃 IP
     */
    QList<QHostAddress> getActiveIps() const { return m_ipConnectionCount.keys(); }

    // ==================== 黑名单管理 ====================

    /**
     * @brief 将指定 IP 加入黑名单
     * @param ip 要加入黑名单的 IP
     * @param durationSeconds 黑名单有效期（秒），默认 3600 秒（1小时）
     *
     * 黑名单中的 IP 在有效期内无法建立新连接。
     * 发射 ipBlacklisted 信号。
     */
    void addToBlacklist(const QHostAddress& ip, int durationSeconds = 3600);
    /**
     * @brief 将指定 IP 从黑名单移除
     * @param ip 要移除的 IP
     */
    void removeFromBlacklist(const QHostAddress& ip);
    /**
     * @brief 检查指定 IP 是否在黑名单中
     * @param ip 要检查的 IP
     * @return true 表示在黑名单中（且未过期），false 表示不在或已过期
     */
    bool isBlacklisted(const QHostAddress& ip) const;

signals:
    /**
     * @brief 连接被接受时发送
     * @param ip 客户端的 IP
     * @param currentCount 当前总连接数（不同IP数量）
     */
    void connectionAccepted(const QHostAddress& ip, QString msg);
    /**
     * @brief 连接被拒绝时发送
     * @param ip 客户端的 IP
     * @param reason 拒绝原因描述
     */
    void connectionRejected(const QHostAddress& ip, const QString& reason);
    /**
     * @brief IP 被加入黑名单时发射
     * @param ip 被加入的 IP
     * @param reason 原因（例如“手动加入”）
     */
    void ipBlacklisted(const QHostAddress& ip, const QString& reason);
    /**
     * @brief 定期统计信息更新信号
     * @param totalConnections 总连接数（不同IP数量）
     * @param uniqueIps 独立IP数（实际上与 totalConnections 相同，这里可能冗余）
     */
    void statsUpdated(int totalConnections, int uniqueIps);

private slots:
    /**
     * @brief 定时清理任务，每分钟执行一次
     * 清理过期的黑名单条目和过期的速率统计记录。
     */
    void onCleanupTimer();
    /**
     * @brief 定时统计任务，每5秒执行一次
     * 发射 statsUpdated 信号，并打印当前统计信息。
     */
    void onStatsTimer();
    /**
     * @brief 动态更新任务，每10秒执行一次（仅在 DynamicLimit 策略下启用）
     * 根据系统负载调整全局最大连接数。
     */
    void updateDynamicLimit();

private:
    // 策略检查辅助函数（非速率检查，速率检查已内联到 canAcceptConnection 中）
    /**
     * @brief 检查固定上限策略
     * @return true 如果当前连接数小于 m_globalMax
     */
    bool checkFixedLimit() const;
    /**
     * @brief 检查每 IP 限制策略
     * @param ip 客户端 IP
     * @return true 如果该 IP 的连接数小于 m_perIpMax
     */
    bool checkPerIpLimit(const QHostAddress& ip) const;
    /**
     * @brief 检查速率限制策略（简化实现，全局速率）
     * @return true 如果当前秒内的新连接数小于 m_maxRatePerSecond
     *
     * 注意：这是一个简化的全局速率限制，不是按 IP 的。
     * 实际可根据需要修改为按 IP 统计。
     */
    bool checkRateLimit(const QHostAddress& clientIp);
    /**
     * @brief 检查动态限制策略
     * @return true 如果系统 CPU 和内存使用率均低于阈值
     */
    bool checkDynamicLimit();
    /**
     * @brief 检查 IP 是否在黑名单中（且未过期）
     * @param ip 客户端 IP
     * @return true 表示不在黑名单或已过期，false 表示在黑名单中
     */
    bool checkBlacklist(const QHostAddress& ip) const;

    // 系统负载获取
    /**
     * @brief 获取当前 CPU 使用率（Linux 专用）
     * @return CPU 使用率百分比（0-100），非 Linux 系统返回 50.0
     */
    double getSystemCpuUsage();
    /**
     * @brief 获取当前内存使用率（Linux 专用）
     * @return 内存使用率百分比（0-100），非 Linux 系统返回 50.0
     */
    double getSystemMemoryUsage();

    // 速率限制相关结构
    struct RateInfo {
        int count;              // 当前时间窗口内的连接数
        QDateTime windowStart;  // 当前窗口的开始时间（秒级精度）
    };
    // 读写锁，保护所有共享数据
    mutable QReadWriteLock m_rwLock;

    // 核心数据结构
    QHash<QHostAddress, int> m_ipConnectionCount;           // IP -> 该IP的连接数
    QHash<QHostAddress, QList<ConnectionInfo>> m_connections; // IP -> 该IP的所有连接列表
    QHash<qintptr, ConnectionInfo> m_descriptorMap;         // 描述符 -> 连接信息
    QHash<QHostAddress, QDateTime> m_blacklist;             // 黑名单IP及解禁时间
    QHash<QHostAddress, RateInfo> m_rateMap;                // 每个IP的连接速率统计

    // 策略参数
    LimitStrategy m_strategy;
    int m_globalMax;               // 全局最大独立IP数
    int m_perIpMax;                // 每个IP的最大连接数
    int m_maxRatePerSecond;        // 每个IP每秒允许的最大新连接数
    int m_cpuThreshold;            // CPU使用率阈值（%）
    int m_memThreshold;            // 内存使用率阈值（%）

    // 定时器
    QTimer* m_cleanupTimer;       // 清理定时器
    QTimer* m_statsTimer;
    QTimer* m_dynamicUpdateTimer;
};

#endif // CONNECTIONMANAGER_H
