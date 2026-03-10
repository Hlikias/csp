#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QObject>
#include <QTimer>
#include <windows.h>
#include <pdh.h>
#include <QTime>

// 系统性能数据结构
struct SystemData {
    float cpuUsage;       // CPU使用率 (%)
    float memoryUsage;    // 内存使用率 (%)
    float diskUsage;      // 磁盘使用率 (%)
    quint64 freeDiskSpace; // 新增：剩余磁盘空间（字节）
    float netSent;        // 上传速度 (KB/s)
    float netRecv;        // 下载速度 (KB/s)
};

class SystemMonitor : public QObject
{
    Q_OBJECT
public:
    explicit SystemMonitor(QObject *parent = nullptr);
    ~SystemMonitor();
public slots:
    void start();          // 启动采集
    void stop();           // 停止采集

signals:
    void dataUpdated(const SystemData &data);  // 数据更新信号

private slots:
    void collectData();    // 定时采集数据

private:
    void initCounters();   // 初始化性能计数器
    void cleanupCounters(); // 清理计数器
    float getDiskUsage(const QString &drive = "F:");  // 默认F盘，可根据需要修改
    float getNetworkSpeed(quint64 &outSent, quint64 &outRecv); // 获取网络速度

    QTimer *m_timer;
    bool m_running;

    // CPU 计数器
    PDH_HQUERY m_cpuQuery;
    PDH_HCOUNTER m_cpuTotal;

    // 内存信息
    MEMORYSTATUSEX m_memoryStatus;

    // 网络统计（用于计算速度）
    quint64 m_lastNetSent, m_lastNetRecv;
    QTime m_lastNetTime;

signals:
};

#endif // SYSTEMMONITOR_H
