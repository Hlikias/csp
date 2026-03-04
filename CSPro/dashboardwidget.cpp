#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QThread>
#include <QFileInfo>
DashboardWidget::DashboardWidget(serverParam param,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardWidget),
    m_timeCounter(0)
{
    ui->setupUi(this);
    this->param = param;
    setupCharts();
    init();
    // 创建系统监控线程
    m_monitor = new SystemMonitor();
    m_monitorThread = new QThread(this);
    m_monitor->moveToThread(m_monitorThread);

    connect(m_monitorThread, &QThread::finished, m_monitor, &QObject::deleteLater);
    connect(m_monitor, &SystemMonitor::dataUpdated, this, &DashboardWidget::onSystemDataUpdated);
    connect(this, &DashboardWidget::destroyed, this, [this]() {
        m_monitorThread->quit();
        m_monitorThread->wait();
    });

    m_monitorThread->start();
    QMetaObject::invokeMethod(m_monitor, "start"); // 启动监控

    // 连接刷新按钮（仍保留，用于清空图表）
    connect(ui->btnRefresh, &QPushButton::clicked, this, &DashboardWidget::on_btnRefresh_clicked);

    // 移除原来的定时器，不再需要
    // m_timer->start(1000);
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}


// 新增：处理系统数据更新
void DashboardWidget::onSystemDataUpdated(const SystemData &data)
{
    m_timeCounter++;  // 可以用实际时间戳替代，这里简单递增

    // 更新卡片
    ui->cpuCard->setText(QString("CPU: %1%").arg(data.cpuUsage, 0, 'f', 1));
    ui->memoryCard->setText(QString("内存: %1%").arg(data.memoryUsage, 0, 'f', 1));
    ui->diskCard->setText(QString("磁盘: %1%").arg(data.diskUsage, 0, 'f', 1));
    ui->netCard->setText(QString("网络: ↑%1 KB/s  ↓%2 KB/s")
                             .arg(data.netSent, 0, 'f', 1)
                             .arg(data.netRecv, 0, 'f', 1));

    // 根据使用率设置颜色
    setCardColor(ui->cpuCard, data.cpuUsage);
    setCardColor(ui->memoryCard, data.memoryUsage);
    setCardColor(ui->diskCard, data.diskUsage);

    // --- 新增：更新剩余空间进度条和标签 ---
    // 计算剩余百分比（100% - 已用百分比）
    float freePercent = 100.0f - data.diskUsage;
    ui->diskFreeBar->setValue(static_cast<int>(freePercent));

    // 将剩余字节转换为 GB，保留两位小数
    double freeGB = data.freeDiskSpace / (1024.0 * 1024.0 * 1024.0);
    ui->diskFreeLabel->setText(QString("剩余 %1 GB").arg(freeGB, 0, 'f', 2));
    // --------------------------------------

    // 更新图表
    updateCpuChart(data.cpuUsage, m_timeCounter);
    updateMemoryChart(data.memoryUsage);
    // 如果有连接数和吞吐量图表，可以保留模拟或集成实际数据（如果服务器提供）

    // 活动日志：每5秒记录一次，包含网络速度
    if (m_timeCounter % 5 == 0) {
        addActivityLog(QString("[%1] 系统状态：CPU=%2%, 内存=%3%, 磁盘=%4%, 网络↑%5 KB/s ↓%6 KB/s")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                           .arg(data.cpuUsage, 0, 'f', 1)
                           .arg(data.memoryUsage, 0, 'f', 1)
                           .arg(data.diskUsage, 0, 'f', 1)
                           .arg(data.netSent, 0, 'f', 1)
                           .arg(data.netRecv, 0, 'f', 1));
    }
}

void DashboardWidget::init()
{
    xmlmanager &xml = xmlmanager::getinstance();
    QString qssPath = xml.get_string("style","ServerMainInfoQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        loadLocalQss(qssPath);
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
    ui->diskFreeBar->setRange(0, 100);
    ui->diskFreeBar->setValue(0);

    ui->host->setText("Host: "+param.m_host);
    ui->port->setText("Port: "+QString::number(param.m_port));
    ui->thread->setText("Work Threads: "+QString::number(param.m_workerThreadsOptions));
    ui->connect->setText("MaxConnections： "+QString::number(param.m_maxConnectionsOptions));

    ui->maxUser->setText("Max User: "+QString::number(param.m_maxUsers));
    ui->buffSize->setText("Buffer Size: "+QString::number(param.m_bufferSize));
    ui->timeOut->setText("Time Out Options: "+QString::number(param.m_sessionTimeout));
    ui->maxMemery->setText("Memory Limit: "+QString::number(param.m_memoryLimitMB));

    ui->logLevel->setText("Log Level: "+param.m_logLevelOptions);
    ui->logPath->setText("Log Path: "+param.m_logFile);
    ui->maxLog->setText("Max Log: "+QString::number(param.m_logMaxSize));
    ui->refalshLog->setText("Log Max Backups: "+QString::number(param.m_logMaxBackups));

    ui->Guest->setText("Allow Guest: "+param.m_allowGuestOptions);
    ui->ency->setText("Encrypt Data"+param.m_encryptData);
    ui->upPath->setText("Up Load Path: "+param.m_uploadPath);
    ui->downLoad->setText("Down Load Path: "+param.m_downloadPath);


}

// 封装加载QSS的函数（简单易用）
void DashboardWidget::loadLocalQss(const QString &qssPath) {
    QFile file(qssPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "[ERROR] 打开QSS文件失败：" << file.errorString();
        return;
    }
    QTextStream stream(&file);
    QString qss = stream.readAll();
    file.close();

    // 仅设置到当前窗口，而非全局
    this->setStyleSheet(qss);

    // 可选：检查样式是否被接受（this->styleSheet() 会返回设置的内容）
    if (this->styleSheet().isEmpty() && !qss.isEmpty()) {
        qDebug() << "[ERROR] QSS解析失败！";
        // 进一步的错误定位可按需实现（例如逐段测试）
    } else {
        qDebug() << "[SUCCESS] QSS解析并应用成功！";
    }
}

void DashboardWidget::setCardColor(QLabel *card, float value)
{
    if (value < 80.0f) {
        card->setStyleSheet("color: green;");
    } else if (value < 95.0f) {
        card->setStyleSheet("color: orange;");
    } else {
        card->setStyleSheet("color: red; font-weight: bold;"); // 超过95%加粗显示
    }
}


void DashboardWidget::setupCharts()
{
    // ========== CPU 折线图 ==========
    m_cpuSeries = new QLineSeries();
    m_cpuSeries->setName("CPU 使用率 (%)");
    QChart *cpuChart = new QChart();
    cpuChart->addSeries(m_cpuSeries);
    cpuChart->setTitle("CPU 使用率趋势 (最近60秒)");
    cpuChart->setAnimationOptions(QChart::SeriesAnimations);
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 60);
    axisX->setLabelFormat("%d s");
    axisX->setTitleText("时间 (秒)");
    cpuChart->addAxis(axisX, Qt::AlignBottom);
    m_cpuSeries->attachAxis(axisX);
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("使用率 (%)");
    cpuChart->addAxis(axisY, Qt::AlignLeft);
    m_cpuSeries->attachAxis(axisY);
    ui->cpuChartView->setChart(cpuChart);
    ui->cpuChartView->setRenderHint(QPainter::Antialiasing);

    // ========== 内存柱状图 ==========
    m_memorySet = new QBarSet("内存使用率");
    *m_memorySet << 0;
    m_memorySeries = new QBarSeries();
    m_memorySeries->append(m_memorySet);
    QChart *memoryChart = new QChart();
    memoryChart->addSeries(m_memorySeries);
    memoryChart->setTitle("当前内存使用率");
    memoryChart->setAnimationOptions(QChart::SeriesAnimations);
    QValueAxis *memoryAxisY = new QValueAxis();
    memoryAxisY->setRange(0, 100);
    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);
    m_memorySeries->attachAxis(memoryAxisY);
    QCategoryAxis *memoryAxisX = new QCategoryAxis();
    memoryAxisX->append("内存", 0);
    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    m_memorySeries->attachAxis(memoryAxisX);
    ui->memoryChartView->setChart(memoryChart);
    ui->memoryChartView->setRenderHint(QPainter::Antialiasing);

    // ========== NEW: 服务器流量图表 (连接数 & 吞吐量) ==========
    m_connSeries = new QLineSeries();
    m_connSeries->setName("连接数");
    m_throughputSeries = new QLineSeries();
    m_throughputSeries->setName("吞吐量 (MB/s)");

    QChart *trafficChart = new QChart();
    trafficChart->addSeries(m_connSeries);
    trafficChart->addSeries(m_throughputSeries);
    trafficChart->setTitle("服务器连接数与吞吐量 (最近60秒)");
    trafficChart->setAnimationOptions(QChart::SeriesAnimations);

    // X轴 (时间)
    QValueAxis *trafficAxisX = new QValueAxis();
    trafficAxisX->setRange(0, 60);
    trafficAxisX->setLabelFormat("%d s");
    trafficAxisX->setTitleText("时间 (秒)");
    trafficChart->addAxis(trafficAxisX, Qt::AlignBottom);
    m_connSeries->attachAxis(trafficAxisX);
    m_throughputSeries->attachAxis(trafficAxisX);

    // 左侧Y轴 (连接数)
    QValueAxis *trafficAxisYLeft = new QValueAxis();
    trafficAxisYLeft->setRange(0, 2000);   // 假设连接数范围 0-2000
    trafficAxisYLeft->setTitleText("连接数");
    trafficChart->addAxis(trafficAxisYLeft, Qt::AlignLeft);
    m_connSeries->attachAxis(trafficAxisYLeft);

    // 右侧Y轴 (吞吐量)
    QValueAxis *trafficAxisYRight = new QValueAxis();
    trafficAxisYRight->setRange(0, 100);   // 吞吐量范围 0-100 MB/s
    trafficAxisYRight->setTitleText("吞吐量 (MB/s)");
    trafficChart->addAxis(trafficAxisYRight, Qt::AlignRight);
    m_throughputSeries->attachAxis(trafficAxisYRight);

    ui->trafficChartView->setChart(trafficChart);
    ui->trafficChartView->setRenderHint(QPainter::Antialiasing);
}

void DashboardWidget::updateDashboard()
{
    m_timeCounter++;   // 统一时间戳

    // 模拟系统指标
    float cpuUsage = QRandomGenerator::global()->bounded(30, 95);
    float memoryUsage = QRandomGenerator::global()->bounded(20, 80);
    float diskUsage = QRandomGenerator::global()->bounded(10, 70);

    // 模拟服务器性能参数
    int connections = QRandomGenerator::global()->bounded(500, 1800);
    float throughput = QRandomGenerator::global()->bounded(20, 90);

    // 更新卡片
    ui->cpuCard->setText(QString("CPU: %1%").arg(cpuUsage, 0, 'f', 1));
    ui->memoryCard->setText(QString("内存: %1%").arg(memoryUsage, 0, 'f', 1));
    ui->diskCard->setText(QString("磁盘: %1%").arg(diskUsage, 0, 'f', 1));
    float netIn = QRandomGenerator::global()->bounded(100, 500);
    float netOut = QRandomGenerator::global()->bounded(50, 300);
    ui->netCard->setText(QString("网络: ↑%1 KB/s  ↓%2 KB/s").arg(netOut, 0, 'f', 0).arg(netIn, 0, 'f', 0));

    // 更新所有图表
    updateCpuChart(cpuUsage, m_timeCounter);
    updateMemoryChart(memoryUsage);
    updateTrafficChart(connections, throughput, m_timeCounter);   //

    // 活动日志
    if (QRandomGenerator::global()->bounded(10) > 7) {
        addActivityLog(QString("[%1] 系统状态：CPU=%2%, 内存=%3%, 连接数=%4, 吞吐量=%5 MB/s")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                           .arg(cpuUsage, 0, 'f', 1)
                           .arg(memoryUsage, 0, 'f', 1)
                           .arg(connections)
                           .arg(throughput, 0, 'f', 1));
    }

    // 告警检测
    if (connections > 1500) {
        addActivityLog(QString("[%1] 告警：连接数过高 (%2)")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                           .arg(connections));
    } else if (throughput > 80) {
        addActivityLog(QString("[%1] 告警：吞吐量过高 (%2 MB/s)")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                           .arg(throughput, 0, 'f', 1));
    }
}

void DashboardWidget::updateCpuChart(float cpuUsage, qint64 time)
{
    m_cpuSeries->append(time, cpuUsage);
    if (m_cpuSeries->count() > 60) m_cpuSeries->remove(0);
    QChart *chart = ui->cpuChartView->chart();
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        axisX->setRange(qMax(0LL, time - 60), time);
    }
}

void DashboardWidget::updateMemoryChart(float memoryUsage)
{
    m_memorySet->replace(0, memoryUsage);
}

// 更新服务器流量图表
void DashboardWidget::updateTrafficChart(int connections, float throughput, qint64 time)
{
    m_connSeries->append(time, connections);
    m_throughputSeries->append(time, throughput);
    if (m_connSeries->count() > 60) m_connSeries->remove(0);
    if (m_throughputSeries->count() > 60) m_throughputSeries->remove(0);

    QChart *chart = ui->trafficChartView->chart();
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        axisX->setRange(qMax(0LL, time - 60), time);
    }
    // Y轴范围保持固定（可根据需要动态调整）
}

void DashboardWidget::addActivityLog(const QString &msg)
{
    ui->recentActivityList->insertItem(0, msg);
    while (ui->recentActivityList->count() > 50) {
        delete ui->recentActivityList->takeItem(50);
    }
}

void DashboardWidget::setServerConfig(const serverParam &param)
{
    this->param = param;
}

void DashboardWidget::on_btnRefresh_clicked()
{
    m_cpuSeries->clear();
    m_connSeries->clear();
    m_throughputSeries->clear();
    m_timeCounter = 0;
    ui->recentActivityList->clear();
    updateDashboard();
}
