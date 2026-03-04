#include "systemmonitor.h"
#include <QDebug>
#include <QThread>
#include <windows.h>
#include <pdhmsg.h>
#include <winternl.h>  // 用于磁盘信息

#pragma comment(lib, "pdh.lib")

SystemMonitor::SystemMonitor(QObject *parent)
    : QObject{parent}, m_running(false)
{
    qDebug() << "[INFO] SystemMonitor constructor, thread:" << QThread::currentThread();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &SystemMonitor::collectData);

    m_memoryStatus.dwLength = sizeof(m_memoryStatus);
    m_lastNetSent = 0;
    m_lastNetRecv = 0;
    m_lastNetTime = QTime::currentTime();

    // 初始化性能计数器
    initCounters();
}

SystemMonitor::~SystemMonitor()
{
    stop();
    cleanupCounters();
}

void SystemMonitor::initCounters()
{
    PDH_STATUS status;

    // 打开查询
    status = PdhOpenQuery(NULL, 0, &m_cpuQuery);
    if (status != ERROR_SUCCESS) {
        // qDebug() << "[ERROR] PdhOpenQuery failed with code:" << status;
        m_cpuQuery = NULL;
        return;
    }

    // 添加CPU计数器 - 推荐使用英文计数器名称
    status = PdhAddEnglishCounter(m_cpuQuery,
                                  L"\\Processor(_Total)\\% Processor Time",
                                  0, &m_cpuTotal);
    if (status != ERROR_SUCCESS) {
        // qDebug() << "[ERROR] PdhAddEnglishCounter failed with code:" << status;
        // 如果英文计数器失败，可以尝试本地化名称作为备选
        status = PdhAddCounter(m_cpuQuery,
                               L"\\Processor(_Total)\\% Processor Time",
                               0, &m_cpuTotal);
        if (status != ERROR_SUCCESS) {
            // qDebug() << "[ERROR] PdhAddCounter also failed with code:" << status;
        }
    } else {
        // qDebug() << "[INFO] CPU counter added successfully";
    }

    // 首次采集数据建立基线
    status = PdhCollectQueryData(m_cpuQuery);
    // qDebug() << "[INFO] First PdhCollectQueryData returned:" << status;
}

void SystemMonitor::cleanupCounters()
{
    if (m_cpuQuery) {
        PdhCloseQuery(m_cpuQuery);
        m_cpuQuery = NULL;
    }
}

void SystemMonitor::start()
{
    // qDebug() << "[INFO] SystemMonitor::start() called";
    if (m_running) return;
    m_running = true;
    m_timer->start(1000); // 每秒采集一次
    // qDebug() << "[INFO] Timer is active:" << m_timer->isActive();
}

void SystemMonitor::stop()
{
    m_timer->stop();
    m_running = false;
}

void SystemMonitor::collectData()
{
    // qDebug() << "[INFO] collectData() triggered, thread:" << QThread::currentThread();
    SystemData data = {0};

    // --- CPU ---
    PDH_STATUS status = PdhCollectQueryData(m_cpuQuery);
    if (status == ERROR_SUCCESS) {
        PDH_FMT_COUNTERVALUE cpuVal;
        status = PdhGetFormattedCounterValue(m_cpuTotal, PDH_FMT_DOUBLE, NULL, &cpuVal);
        if (status == ERROR_SUCCESS) {
            data.cpuUsage = static_cast<float>(cpuVal.doubleValue);
            // qDebug() << "[INFO] CPU Usage:" << data.cpuUsage << "%";
        }
    } else {
        // qDebug() << "[ERROR] PdhCollectQueryData failed:" << status;
        return;  // 如果CPU采集失败，本次跳过（可根据需要调整）
    }

    // --- 内存 ---
    if (GlobalMemoryStatusEx(&m_memoryStatus)) {
        data.memoryUsage = static_cast<float>(m_memoryStatus.dwMemoryLoad);
        // qDebug() << "[INFO] Memory Usage:" << data.memoryUsage << "%";
    } else {
        // qDebug() << "[ERROR] GlobalMemoryStatusEx failed";
    }

    // --- 磁盘 ---
    // data.diskUsage = getDiskUsage("F:");  // 指定F盘
    // 获取 F 盘剩余空间和总空间
    ULARGE_INTEGER freeBytes, totalBytes;
    if (GetDiskFreeSpaceExW(L"F:\\", &freeBytes, &totalBytes, NULL)) {
        data.freeDiskSpace = freeBytes.QuadPart;
        // 计算磁盘使用率（已用百分比），替换之前可能获取的值
        if (totalBytes.QuadPart > 0) {
            data.diskUsage = static_cast<float>((totalBytes.QuadPart - freeBytes.QuadPart) * 100.0 / totalBytes.QuadPart);
        }
    } else {
        data.freeDiskSpace = 0;
        qDebug() << "[ERROR] Failed to get disk space for F:";
    }

    // ... 网络采集或模拟 ...

    // --- 网络（临时模拟）---
    //data.netSent = QRandomGenerator::global()->bounded(100, 500);
    //data.netRecv = QRandomGenerator::global()->bounded(50, 300);

    // 发射信号
    emit dataUpdated(data);
}

float SystemMonitor::getDiskUsage(const QString &drive)
{
    // 转换 QString 到宽字符或ASCII，这里简化使用本地8位字符
    std::wstring drivePath = drive.toStdWString() + L"\\";
    ULARGE_INTEGER freeBytes, totalBytes;
    if (GetDiskFreeSpaceExW(drivePath.c_str(), &freeBytes, &totalBytes, NULL)) {
        if (totalBytes.QuadPart == 0) return 0.0f;
        float used = static_cast<float>(totalBytes.QuadPart - freeBytes.QuadPart);
        return (used / static_cast<float>(totalBytes.QuadPart)) * 100.0f;
    } else {
        // qDebug() << "[ERROR] Failed to get disk space for" << drive;
        return 0.0f;
    }
}

float SystemMonitor::getNetworkSpeed(quint64 &outSent, quint64 &outRecv)
{
    // 使用 GetIfTable 或 GetAdaptersInfo 获取网络接口流量
    // 简化实现：使用性能计数器（需要添加网络计数器），这里返回0并给出注释
    // 实际项目中可参考 MSDN 示例使用 PDH 计数器 "\Network Interface(*)\Bytes Sent/sec" 等
    // 为保持示例简洁，此处返回模拟值（可自行扩展）
    outSent = 0;
    outRecv = 0;
    return 0.0f; // 请根据实际需求实现
}
