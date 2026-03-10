#include "connectionmanager.h"
#include <QDebug>
#include <QThread>
#include <QTimer>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#include <unistd.h>
#endif
/**
 * @brief 构造函数
 * @param parent 父对象
 *
 * 初始化定时器并设置默认参数：
 * - 策略：FixedLimit
 * - 全局最大连接数：1000
 * - 每IP最大连接数：10
 * - 最大连接速率：50个/秒
 * - CPU阈值：80%
 * - 内存阈值：90%
 */
ConnectionManager::ConnectionManager(QObject *parent)
    : QObject(parent)
    , m_strategy(FixedLimit)
    , m_globalMax(1000)
    , m_perIpMax(10)
    , m_maxRatePerSecond(50)
    , m_cpuThreshold(80)
    , m_memThreshold(90)
{
    // 创建清理定时器，每分钟触发一次，用于清理过期的黑名单和速率记录
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &ConnectionManager::onCleanupTimer);
    m_cleanupTimer->start(60000); // 每分钟清理一次

    // 创建统计定时器，每5秒触发一次，用于输出当前状态并发射信号
    m_statsTimer = new QTimer(this);
    connect(m_statsTimer, &QTimer::timeout, this, &ConnectionManager::onStatsTimer);
    m_statsTimer->start(5000); // 每5秒统计一次

    // 动态更新定时器，仅当策略为 DynamicLimit 时启动，此处先创建但不启动
    m_dynamicUpdateTimer = new QTimer(this);
    connect(m_dynamicUpdateTimer, &QTimer::timeout, this, &ConnectionManager::updateDynamicLimit);

    // qDebug() << "[ConnectionManager] 初始化完成，全局最大连接数:" << m_globalMax;
}
/**
 * @brief 析构函数，停止所有定时器
 */
ConnectionManager::~ConnectionManager()
{
    m_cleanupTimer->stop();
    m_statsTimer->stop();
    m_dynamicUpdateTimer->stop();
}

/**
 * @brief 设置连接限制策略
 * @param strategy 新策略
 *
 * 如果新策略为 DynamicLimit，则启动动态更新定时器（每10秒）；
 * 否则停止该定时器。
 */
void ConnectionManager::setStrategy(LimitStrategy strategy)
{
    QWriteLocker locker(&m_rwLock);
    m_strategy = strategy;

    if (strategy == DynamicLimit) {
        m_dynamicUpdateTimer->start(10000); // 每10秒更新一次
    } else {
        m_dynamicUpdateTimer->stop();
    }

    // qDebug() << "[ConnectionManager] 策略已切换为:" << strategy;
}
/**
 * @brief 检查是否可以接受新连接
 * @param clientIp 客户端 IP
 * @param socketDescriptor 套接字描述符（未使用，保留参数）
 * @return true 接受，false 拒绝
 *
 * 执行顺序：
 * 1. 检查黑名单，若在黑名单中则直接拒绝。
 * 2. 根据当前策略调用相应的检查函数。
 * 3. 若拒绝，发射 connectionRejected 信号并附带原因。
 */
bool ConnectionManager::canAcceptConnection(const QHostAddress& clientIp, qintptr socketDescriptor)
{
    QReadLocker locker(&m_rwLock);// 读锁，允许多个线程同时检查

    // 1. 检查黑名单
    if (!checkBlacklist(clientIp)) {
        emit connectionRejected(clientIp, "IP在黑名单中");
        return false;
    }

    // 2. 根据策略检查
    bool canAccept = false;
    QString rejectReason;

    switch (m_strategy) {
    case FixedLimit:
        canAccept = checkFixedLimit();
        if (!canAccept) rejectReason = "已达到全局连接上限";
        break;

    case PerIPLimit:
        canAccept = checkPerIpLimit(clientIp);
        if (!canAccept) rejectReason = "该IP连接数已达上限";
        break;

    case DynamicLimit:
        canAccept = checkDynamicLimit();
        if (!canAccept) rejectReason = "系统负载过高，暂时拒绝新连接";
        break;

    case RateLimit:
        canAccept = checkRateLimit(clientIp);
        if (!canAccept) rejectReason = "连接频率过高，请稍后再试";
        break;
    }

    if (!canAccept) {
        emit connectionRejected(clientIp, rejectReason);
    }

    return canAccept;
}
/**
 * @brief 记录一个新连接
 * @param clientIp 客户端 IP
 * @param port 客户端端口
 * @param socketDescriptor 套接字描述符
 *
 * 更新所有数据结构，并发射 connectionAccepted 信号。
 */
void ConnectionManager::addConnection(const QHostAddress& clientIp, quint16 port, qintptr socketDescriptor)
{
    QWriteLocker locker(&m_rwLock);

    ConnectionInfo info;
    info.ip = clientIp;
    info.port = port;
    info.connectTime = QDateTime::currentDateTime();
    info.socketDescriptor = socketDescriptor;

    // 更新各个映射表
    m_ipConnectionCount[clientIp] = m_ipConnectionCount.value(clientIp, 0) + 1;
    m_connections[clientIp].append(info);
    m_descriptorMap[socketDescriptor] = info;

    // 更新速率统计
    RateInfo& rate = m_rateMap[clientIp];
    QDateTime now = QDateTime::currentDateTime();
    if (!rate.windowStart.isValid() || rate.windowStart.secsTo(now) >= 1) {
        // 新窗口
        rate.windowStart = now;
        rate.count = 1;
    } else {
        // 仍在同一秒内
        rate.count++;
    }

    qDebug() << "[ConnectionManager] 添加连接" << clientIp.toString()
             << "当前总连接数:" << m_ipConnectionCount.size()
             << "该IP连接数:" << m_ipConnectionCount[clientIp];

    QString msg= "[ConnectionManager] 添加连接" + clientIp.toString() + "当前总连接数:" +
                  QString::number(m_ipConnectionCount.size()) + "该IP连接数:" + QString::number(m_ipConnectionCount[clientIp]);
    emit connectionAccepted(clientIp, msg);
}
/**
 * @brief 根据 IP 移除一个连接（简化版本，移除该IP的最新连接）
 * @param clientIp 客户端 IP
 *
 * 注意：此方法可能不精确，建议使用 removeConnectionByDescriptor。
 */
void ConnectionManager::removeConnection(const QHostAddress& clientIp)
{
    QWriteLocker locker(&m_rwLock);

    int count = m_ipConnectionCount.value(clientIp, 0);
    if (count > 1) {
        m_ipConnectionCount[clientIp] = count - 1;
        // 从连接列表中移除最新的那个
        if (m_connections.contains(clientIp) && !m_connections[clientIp].isEmpty()) {
            auto& list = m_connections[clientIp];
            qintptr desc = list.last().socketDescriptor;
            m_descriptorMap.remove(desc);
            list.removeLast();
        }
    } else {
        // 移除该IP的所有记录
        if (m_connections.contains(clientIp)) {
            for (const auto& info : m_connections[clientIp]) {
                m_descriptorMap.remove(info.socketDescriptor);
            }
            m_connections.remove(clientIp);
        }
        m_ipConnectionCount.remove(clientIp);
        m_rateMap.remove(clientIp);
    }
}
/**
 * @brief 根据套接字描述符移除连接（精确移除）
 * @param socketDescriptor 要移除的连接描述符
 *
 * 通过描述符找到对应的连接信息，然后从所有映射表中删除该连接。
 * 这是推荐使用的断开连接清理方法。
 */
void ConnectionManager::removeConnectionByDescriptor(qintptr socketDescriptor)
{
    QWriteLocker locker(&m_rwLock);

    if (m_descriptorMap.contains(socketDescriptor)) {
        ConnectionInfo info = m_descriptorMap[socketDescriptor];
        m_descriptorMap.remove(socketDescriptor);

        // 从IP列表中移除
        if (m_connections.contains(info.ip)) {
            auto& list = m_connections[info.ip];
            for (int i = 0; i < list.size(); ++i) {
                if (list[i].socketDescriptor == socketDescriptor) {
                    list.removeAt(i);
                    break;
                }
            }

            if (list.isEmpty()) {
                m_connections.remove(info.ip);
            }
        }

        // 更新IP连接计数
        int count = m_ipConnectionCount.value(info.ip, 0);
        if (count > 1) {
            m_ipConnectionCount[info.ip] = count - 1;
        } else {
            m_ipConnectionCount.remove(info.ip);
        }

        qDebug() << "[ConnectionManager] 移除连接描述符:" << socketDescriptor;
    }
}
/**
 * @brief 获取指定 IP 的当前连接数
 * @param ip 要查询的 IP
 * @return 该 IP 的连接数
 */
int ConnectionManager::getConnectionsByIp(const QHostAddress& ip) const
{
    QReadLocker locker(&m_rwLock);
    return m_ipConnectionCount.value(ip, 0);
}
/**
 * @brief 将 IP 加入黑名单
 * @param ip 要封禁的 IP
 * @param durationSeconds 封禁时长（秒）
 *
 * 黑名单中的 IP 在有效期内无法建立新连接。
 */
void ConnectionManager::addToBlacklist(const QHostAddress& ip, int durationSeconds)
{
    QWriteLocker locker(&m_rwLock);
    QDateTime expireTime = QDateTime::currentDateTime().addSecs(durationSeconds);
    m_blacklist[ip] = expireTime;

    qDebug() << "[ConnectionManager] IP已加入黑名单:" << ip.toString()
             << "有效期:" << durationSeconds << "秒";

    emit ipBlacklisted(ip, "手动加入黑名单");
}
/**
 * @brief 将 IP 从黑名单移除
 * @param ip 要解封的 IP
 */
void ConnectionManager::removeFromBlacklist(const QHostAddress& ip)
{
    QWriteLocker locker(&m_rwLock);
    m_blacklist.remove(ip);
    qDebug() << "[ConnectionManager] IP已从黑名单移除:" << ip.toString();
}
/**
 * @brief 检查 IP 是否在黑名单中
 * @param ip 要检查的 IP
 * @return true 如果在黑名单且未过期，false 否则
 */
bool ConnectionManager::isBlacklisted(const QHostAddress& ip) const
{
    QReadLocker locker(&m_rwLock);
    return m_blacklist.contains(ip) && m_blacklist[ip] > QDateTime::currentDateTime();
}

// 私有方法实现

bool ConnectionManager::checkFixedLimit() const
{
    // 注意：m_ipConnectionCount.size() 是不同IP的数量，不是总连接数
    // 这里逻辑上应该是总连接数（所有IP的连接数之和），但为了与 setGlobalMax 保持一致，
    // 我们暂时保留原样。实际使用中应根据需求修改为总和。
    return m_ipConnectionCount.size() < m_globalMax;
}

bool ConnectionManager::checkPerIpLimit(const QHostAddress& ip) const
{
    int ipCount = m_ipConnectionCount.value(ip, 0);
    return ipCount < m_perIpMax;
}

bool ConnectionManager::checkRateLimit(const QHostAddress& clientIp)
{
    auto it = m_rateMap.find(clientIp);
    if (it == m_rateMap.end()) return true; // 新 IP 允许

    RateInfo& rate = it.value();
    QDateTime now = QDateTime::currentDateTime();
    if (rate.windowStart.secsTo(now) >= 1) {
        // 窗口已过，重置
        rate.windowStart = now;
        rate.count = 1;
        return true;
    }
    return rate.count < m_maxRatePerSecond;
}

bool ConnectionManager::checkDynamicLimit()
{
    double cpuUsage = getSystemCpuUsage();
    double memUsage = getSystemMemoryUsage();

    return (cpuUsage < m_cpuThreshold && memUsage < m_memThreshold);
}

bool ConnectionManager::checkBlacklist(const QHostAddress& ip) const
{
    if (m_blacklist.contains(ip)) {
        QDateTime expireTime = m_blacklist[ip];
        if (expireTime > QDateTime::currentDateTime()) {
            return false; // 还在黑名单中
        } else {
            // 过期了，可以移除
            const_cast<ConnectionManager*>(this)->m_blacklist.remove(ip);
        }
    }
    return true;
}

double ConnectionManager::getSystemCpuUsage()
{
#ifdef Q_OS_LINUX
    static long long prevIdle = 0, prevTotal = 0;

    FILE* file = fopen("/proc/stat", "r");
    if (!file) return 0.0;

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file)) {
        long long user, nice, system, idle, iowait, irq, softirq, steal;
        sscanf(buffer, "cpu %lld %lld %lld %lld %lld %lld %lld %lld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal);
        fclose(file);

        long long idleAll = idle + iowait;
        long long total = user + nice + system + idleAll + irq + softirq + steal;

        if (prevTotal > 0) {
            long long diffIdle = idleAll - prevIdle;
            long long diffTotal = total - prevTotal;
            double usage = (100.0 * (diffTotal - diffIdle)) / diffTotal;

            prevIdle = idleAll;
            prevTotal = total;
            return usage;
        }

        prevIdle = idleAll;
        prevTotal = total;
    }
    fclose(file);
#endif
    return 50.0; // 默认返回50%
}

double ConnectionManager::getSystemMemoryUsage()
{
#ifdef Q_OS_LINUX
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        unsigned long totalMem = info.totalram;
        unsigned long freeMem = info.freeram;
        return 100.0 * (totalMem - freeMem) / totalMem;
    }
#endif
    return 50.0; // 默认返回50%
}
/**
 * @brief 定时清理任务
 *
 * 1. 移除过期的黑名单条目（当前时间超过解禁时间）。
 * 2. 移除超过1分钟没有活动的速率统计记录（简化：只要窗口开始时间早于1分钟前就移除）。
 */
void ConnectionManager::onCleanupTimer()
{
    QWriteLocker locker(&m_rwLock);

    // 清理过期的黑名单
    QDateTime now = QDateTime::currentDateTime();
    for (auto it = m_blacklist.begin(); it != m_blacklist.end();) {
        if (it.value() < now) {
            qDebug() << "[ConnectionManager] 黑名单过期，移除IP:" << it.key().toString();
            it = m_blacklist.erase(it);
        } else {
            ++it;
        }
    }

    // 清理超过1分钟无活动的速率记录（基于最后一次窗口开始时间）
    for (auto it = m_rateMap.begin(); it != m_rateMap.end();) {
        if (it.value().windowStart.secsTo(now) > 60) {
            it = m_rateMap.erase(it);
        } else {
            ++it;
        }
    }
}
/**
 * @brief 定时统计任务
 *
 * 发射 statsUpdated 信号，并打印当前统计信息。
 */
void ConnectionManager::onStatsTimer()
{
    QReadLocker locker(&m_rwLock);
    emit statsUpdated(m_ipConnectionCount.size(), m_connections.size());

    qDebug() << "[ConnectionManager] 统计信息 - 总连接数:"
             << m_ipConnectionCount.size()
             << "独立IP数:" << m_connections.size()
             << "黑名单数:" << m_blacklist.size();
    QString msg = "[ConnectionManager]   统计信息 - 总连接数:  "
                  + QString::number(m_ipConnectionCount.size())
                  + "  独立IP数:   " + QString::number(m_connections.size())
                  + "  黑名单数:  " + QString::number(m_blacklist.size());
    emit connectionAccepted(QHostAddress("0.0.0.0"), msg);
}
/**
 * @brief 动态更新最大连接数（仅在 DynamicLimit 策略下被定时调用）
 *
 * 根据当前系统负载调整 m_globalMax：
 * - 如果 CPU 或内存使用率超过阈值，则减少最大连接数（每次减50，不低于100）
 * - 否则增加最大连接数（每次加10，不超过1000）
 */
void ConnectionManager::updateDynamicLimit()
{
    if (m_strategy == DynamicLimit) {
        double cpuUsage = getSystemCpuUsage();
        double memUsage = getSystemMemoryUsage();

        // 根据负载动态调整最大连接数
        if (cpuUsage > m_cpuThreshold || memUsage > m_memThreshold) {
            m_globalMax = qMax(100, m_globalMax - 50); // 负载高时减少连接数
            qDebug() << "[ConnectionManager] 系统负载过高，降低最大连接数为:" << m_globalMax;
        } else {
            m_globalMax = qMin(1000, m_globalMax + 10); // 负载低时增加连接数
        }
    }
}
