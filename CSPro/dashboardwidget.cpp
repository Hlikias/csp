#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include "qsshelper.h"
/**
 * @brief DashboardWidget 构造函数
 * @param param 服务器配置参数，用于显示静态配置信息
 * @param parent 父窗口
 */
DashboardWidget::DashboardWidget(serverParam param, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DashboardWidget),
    m_timeCounter(0)   // 时间计数器，用于图表X轴
{
    ui->setupUi(this);
    this->param = param;                // 保存服务器配置参数
    setupCharts();                       // 初始化图表
    init();                              // 初始化样式和静态配置标签

    // --- 创建系统监控线程，用于实时获取CPU、内存、磁盘等数据 ---
    m_monitor = new SystemMonitor();      // 监控对象
    m_monitorThread = new QThread(this);  // 工作线程
    m_monitor->moveToThread(m_monitorThread); // 将监控对象移至工作线程

    // 线程结束时自动销毁监控对象，防止内存泄漏
    connect(m_monitorThread, &QThread::finished, m_monitor, &QObject::deleteLater);
    // 连接监控数据更新信号到主线程的槽函数，用于刷新UI
    connect(m_monitor, &SystemMonitor::dataUpdated, this, &DashboardWidget::onSystemDataUpdated);
    // 窗口销毁时，确保线程安全退出并等待
    connect(this, &DashboardWidget::destroyed, this, [this]() {
        m_monitorThread->quit();
        m_monitorThread->wait();
    });

    m_monitorThread->start(); // 启动线程（事件循环开始）
    // 在线程中调用监控对象的 start() 方法，开始定时采集数据
    QMetaObject::invokeMethod(m_monitor, "start");

    // 连接刷新按钮，点击时清空图表并重置（具体实现在槽函数中）
    connect(ui->btnRefresh, &QPushButton::clicked, this, &DashboardWidget::on_btnRefresh_clicked);
}

/**
 * @brief 析构函数，释放UI资源
 */
DashboardWidget::~DashboardWidget()
{
    delete ui;
}

/**
 * @brief 处理系统监控数据更新（由 SystemMonitor 通过信号触发）
 * @param data 包含CPU、内存、磁盘、网络等实时数据的结构体
 *
 * 该函数在主线程执行，负责将实时数据显示在卡片、进度条和图表上，
 * 并定时添加活动日志。
 */
void DashboardWidget::onSystemDataUpdated(const SystemData &data)
{
    m_timeCounter++;  // 递增时间戳，用于图表X轴

    // 更新四个状态卡片（显示数值，保留一位小数）
    ui->cpuCard->setText(QString("CPU: %1%").arg(data.cpuUsage, 0, 'f', 1));
    ui->memoryCard->setText(QString("内存: %1%").arg(data.memoryUsage, 0, 'f', 1));
    ui->diskCard->setText(QString("磁盘: %1%").arg(data.diskUsage, 0, 'f', 1));
    ui->netCard->setText(QString("网络: ↑%1 KB/s  ↓%2 KB/s")
                             .arg(data.netSent, 0, 'f', 1)
                             .arg(data.netRecv, 0, 'f', 1));

    // 根据使用率设置卡片文字颜色（绿色/橙色/红色）
    setCardColor(ui->cpuCard, data.cpuUsage);
    setCardColor(ui->memoryCard, data.memoryUsage);
    setCardColor(ui->diskCard, data.diskUsage);

    // --- 更新磁盘剩余空间进度条 ---
    float usedPercent = data.diskUsage;   // 已用百分比
    ui->diskFreeBar->setValue(static_cast<int>(usedPercent)); // 进度条显示已用百分比

    // 根据已用百分比设置进度条颜色：<80% 绿色，80%-95% 橙色，>95% 红色
    QString barStyle;
    if (usedPercent > 95.0f) {
        barStyle = "QProgressBar::chunk { background-color: red; }";
    } else if (usedPercent > 80.0f) {
        barStyle = "QProgressBar::chunk { background-color: orange; }";
    } else {
        barStyle = "QProgressBar::chunk { background-color: green; }";
    }
    // 设置进度条背景为浅灰色
    barStyle = "QProgressBar { background-color: lightgray; }" + barStyle;
    ui->diskFreeBar->setStyleSheet(barStyle);

    // 将剩余字节转换为GB，显示在标签上（保留两位小数）
    double freeGB = data.freeDiskSpace / (1024.0 * 1024.0 * 1024.0);
    ui->diskFreeLabel->setText(QString("剩余 %1 GB").arg(freeGB, 0, 'f', 2));
    // ---------------------------------

    // 更新CPU折线图和内存柱状图
    updateCpuChart(data.cpuUsage, m_timeCounter);
    updateMemoryChart(data.memoryUsage);

    // 活动日志：每5秒记录一次系统状态（包含网络速度）
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

/**
 * @brief 初始化函数，在构造函数中调用
 * 负责加载QSS样式、设置进度条初始范围，并用富文本显示服务器配置参数
 */
void DashboardWidget::init()
{
    xmlmanager &xml = xmlmanager::getinstance();
    QString qssPath = xml.get_string("style", "ServerMainInfoQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径：如果路径是相对的，则加上应用程序目录
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        QSSHelper::loadLocalQss(this, qssPath);  // 加载样式到当前窗口
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }

    // 初始化进度条范围（0-100）
    ui->diskFreeBar->setRange(0, 100);
    ui->diskFreeBar->setValue(0);

    // --- 以下使用富文本在标签中显示服务器配置参数 ---
    // 标题部分为默认颜色（黑色），值部分显示为灰色，并使用 toHtmlEscaped() 转义特殊字符

    ui->host->setText(QString("Host: <span style='color: gray;'>%1</span>")
                          .arg(param.m_host.toHtmlEscaped()));

    ui->port->setText(QString("Port: <span style='color: gray;'>%1</span>")
                          .arg(param.m_port));

    ui->thread->setText(QString("Work Threads: <span style='color: gray;'>%1</span>")
                            .arg(param.m_workerThreadsOptions));

    // 注意原代码中冒号为中文“：”，保留原样
    ui->connect->setText(QString("MaxConnections： <span style='color: gray;'>%1</span>")
                             .arg(param.m_maxConnectionsOptions));

    ui->maxUser->setText(QString("Max User: <span style='color: gray;'>%1</span>")
                             .arg(param.m_maxUsers));

    ui->buffSize->setText(QString("Buffer Size: <span style='color: gray;'>%1</span>")
                              .arg(param.m_bufferSize));

    ui->timeOut->setText(QString("Time Out Options: <span style='color: gray;'>%1</span>")
                             .arg(param.m_sessionTimeout));

    ui->maxMemery->setText(QString("Memory Limit: <span style='color: gray;'>%1</span> MB")
                               .arg(param.m_memoryLimitMB));

    ui->logLevel->setText(QString("Log Level: <span style='color: gray;'>%1</span>")
                              .arg(param.m_logLevelOptions.toHtmlEscaped()));

    ui->logPath->setText(QString("Log Path: <span style='color: gray;'>%1</span>")
                             .arg(param.m_logFile.toHtmlEscaped()));

    ui->maxLog->setText(QString("Max Log: <span style='color: gray;'>%1</span>")
                            .arg(param.m_logMaxSize));

    ui->refalshLog->setText(QString("Log Max Backups: <span style='color: gray;'>%1</span>")
                                .arg(param.m_logMaxBackups));

    ui->Guest->setText(QString("Allow Guest: <span style='color: gray;'>%1</span>")
                           .arg(param.m_allowGuestOptions.toHtmlEscaped()));

    ui->ency->setText(QString("Encrypt Data: <span style='color: gray;'>%1</span>")
                          .arg(param.m_encryptData.toHtmlEscaped()));

    ui->upPath->setText(QString("Up Load Path: <span style='color: gray;'>%1</span>")
                            .arg(param.m_uploadPath.toHtmlEscaped()));

    ui->downLoad->setText(QString("Down Load Path: <span style='color: gray;'>%1</span>")
                              .arg(param.m_downloadPath.toHtmlEscaped()));
}

/**
 * @brief 根据数值设置卡片文字颜色
 * @param card 要设置颜色的QLabel指针
 * @param value 当前数值（百分比）
 *
 * 颜色规则：
 *   - <80%   : 绿色
 *   - 80%-95%: 橙色
 *   - >=95%  : 红色并加粗
 */
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

/**
 * @brief 初始化所有图表（CPU折线图、内存柱状图、流量双轴图）
 *
 * 注意：此函数假设UI中已放置了三个 QChartView 控件，分别命名为
 * cpuChartView, memoryChartView, trafficChartView，并且已提升为 QChartView。
 */
void DashboardWidget::setupCharts()
{
    // ---------- CPU 折线图 ----------
    m_cpuSeries = new QLineSeries();
    m_cpuSeries->setName("CPU 使用率 (%)");
    QChart *cpuChart = new QChart();
    cpuChart->addSeries(m_cpuSeries);
    cpuChart->setTitle("CPU 使用率趋势 (最近60秒)");
    cpuChart->setAnimationOptions(QChart::SeriesAnimations);  // 开启动画

    // X轴（时间）
    QValueAxis *axisX = new QValueAxis();
    axisX->setRange(0, 60);
    axisX->setLabelFormat("%d s");
    axisX->setTitleText("时间 (秒)");
    cpuChart->addAxis(axisX, Qt::AlignBottom);
    m_cpuSeries->attachAxis(axisX);

    // Y轴（百分比）
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 100);
    axisY->setTitleText("使用率 (%)");
    cpuChart->addAxis(axisY, Qt::AlignLeft);
    m_cpuSeries->attachAxis(axisY);

    ui->cpuChartView->setChart(cpuChart);
    ui->cpuChartView->setRenderHint(QPainter::Antialiasing);  // 抗锯齿

    // ---------- 内存柱状图 ----------
    m_memorySet = new QBarSet("内存使用率");
    *m_memorySet << 0;
    m_memorySeries = new QBarSeries();
    m_memorySeries->append(m_memorySet);
    QChart *memoryChart = new QChart();
    memoryChart->addSeries(m_memorySeries);
    memoryChart->setTitle("当前内存使用率");
    memoryChart->setAnimationOptions(QChart::SeriesAnimations);

    // Y轴（百分比）
    QValueAxis *memoryAxisY = new QValueAxis();
    memoryAxisY->setRange(0, 100);
    memoryChart->addAxis(memoryAxisY, Qt::AlignLeft);
    m_memorySeries->attachAxis(memoryAxisY);

    // X轴（只有一个柱，标签为“内存”）
    QCategoryAxis *memoryAxisX = new QCategoryAxis();
    memoryAxisX->append("内存", 0);
    memoryChart->addAxis(memoryAxisX, Qt::AlignBottom);
    m_memorySeries->attachAxis(memoryAxisX);

    ui->memoryChartView->setChart(memoryChart);
    ui->memoryChartView->setRenderHint(QPainter::Antialiasing);

    // ---------- 服务器流量图表 (连接数 & 吞吐量) ----------
    m_connSeries = new QLineSeries();
    m_connSeries->setName("连接数");
    m_throughputSeries = new QLineSeries();
    m_throughputSeries->setName("吞吐量 (MB/s)");

    QChart *trafficChart = new QChart();
    trafficChart->addSeries(m_connSeries);
    trafficChart->addSeries(m_throughputSeries);
    trafficChart->setTitle("服务器连接数与吞吐量 (最近60秒)");
    trafficChart->setAnimationOptions(QChart::SeriesAnimations);

    // X轴（时间）
    QValueAxis *trafficAxisX = new QValueAxis();
    trafficAxisX->setRange(0, 60);
    trafficAxisX->setLabelFormat("%d s");
    trafficAxisX->setTitleText("时间 (秒)");
    trafficChart->addAxis(trafficAxisX, Qt::AlignBottom);
    m_connSeries->attachAxis(trafficAxisX);
    m_throughputSeries->attachAxis(trafficAxisX);

    // 左侧Y轴（连接数）
    QValueAxis *trafficAxisYLeft = new QValueAxis();
    trafficAxisYLeft->setRange(0, 2000);   // 假设连接数范围 0-2000
    trafficAxisYLeft->setTitleText("连接数");
    trafficChart->addAxis(trafficAxisYLeft, Qt::AlignLeft);
    m_connSeries->attachAxis(trafficAxisYLeft);

    // 右侧Y轴（吞吐量）
    QValueAxis *trafficAxisYRight = new QValueAxis();
    trafficAxisYRight->setRange(0, 100);   // 吞吐量范围 0-100 MB/s
    trafficAxisYRight->setTitleText("吞吐量 (MB/s)");
    trafficChart->addAxis(trafficAxisYRight, Qt::AlignRight);
    m_throughputSeries->attachAxis(trafficAxisYRight);

    ui->trafficChartView->setChart(trafficChart);
    ui->trafficChartView->setRenderHint(QPainter::Antialiasing);
}

/**
 * @brief 更新仪表盘（使用模拟数据，可能用于测试或备用）
 *
 * 注意：此函数目前未被使用（被 SystemMonitor 替代），但保留以供参考。
 */
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
    updateTrafficChart(connections, throughput, m_timeCounter);

    // 活动日志（随机添加）
    if (QRandomGenerator::global()->bounded(10) > 7) {
        addActivityLog(QString("[%1] 系统状态：CPU=%2%, 内存=%3%, 连接数=%4, 吞吐量=%5 MB/s")
                           .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                           .arg(cpuUsage, 0, 'f', 1)
                           .arg(memoryUsage, 0, 'f', 1)
                           .arg(connections)
                           .arg(throughput, 0, 'f', 1));
    }

    // 告警检测（模拟）
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

/**
 * @brief 更新CPU折线图，添加新数据点并保持最近60个点
 * @param cpuUsage 当前CPU使用率
 * @param time 当前时间戳（递增）
 */
void DashboardWidget::updateCpuChart(float cpuUsage, qint64 time)
{
    m_cpuSeries->append(time, cpuUsage);
    if (m_cpuSeries->count() > 60) m_cpuSeries->remove(0);  // 只保留最近60个点
    QChart *chart = ui->cpuChartView->chart();
    QValueAxis *axisX = qobject_cast<QValueAxis*>(chart->axes(Qt::Horizontal).first());
    if (axisX) {
        // 移动X轴范围，使最新数据始终可见
        axisX->setRange(qMax(0LL, time - 60), time);
    }
}

/**
 * @brief 更新内存柱状图（只显示当前值）
 * @param memoryUsage 当前内存使用率
 */
void DashboardWidget::updateMemoryChart(float memoryUsage)
{
    m_memorySet->replace(0, memoryUsage);
}

/**
 * @brief 更新流量图表（连接数和吞吐量）
 * @param connections 当前连接数
 * @param throughput 当前吞吐量
 * @param time 当前时间戳
 */
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
    // Y轴范围保持固定，可根据需要动态调整
}

/**
 * @brief 在活动列表顶部添加一条日志，并限制条目数量
 * @param msg 日志信息
 */
void DashboardWidget::addActivityLog(const QString &msg)
{
    ui->recentActivityList->insertItem(0, msg);   // 插入到顶部
    // 限制列表项最多50条，防止内存无限增长
    while (ui->recentActivityList->count() > 50) {
        delete ui->recentActivityList->takeItem(50);
    }
}

/**
 * @brief 设置服务器配置参数（用于外部更新）
 * @param param 新的配置参数
 */
void DashboardWidget::setServerConfig(const serverParam &param)
{
    this->param = param;
}

/**
 * @brief 刷新按钮点击的槽函数
 *
 * 当前实现注释掉了清空图表的代码，改为发送启动服务器信号。
 * 可根据实际需求调整。
 */
void DashboardWidget::on_btnRefresh_clicked()
{
    /*
    m_cpuSeries->clear();
    m_connSeries->clear();
    m_throughputSeries->clear();
    m_timeCounter = 0;
    ui->recentActivityList->clear();
    updateDashboard();
    */
    emit_Restart_Server();  // 发射启动服务器信号
}

/**
 * @brief 启动按钮点击的槽函数
 *
 * 发射重启服务器信号（注意函数名可能拼写有误，但保持一致）
 */
void DashboardWidget::on_btnStart_clicked()
{
    emit_Start_Server();
}

void DashboardWidget::updateTCPLog(const QHostAddress &host, const QString &msg)
{
    QString Massage = "[ "+host.toString()+" ] " + msg;
    ui->recentActivityList->insertItem(0, Massage);   // 插入到顶部
    // 限制列表项最多50条，防止内存无限增长
    while (ui->recentActivityList->count() > 50) {
        delete ui->recentActivityList->takeItem(50);
    }
}

void DashboardWidget::onServerStatusUpdated(const QList<ServerStatus> &statusList)
{
    for (const ServerStatus &status : statusList) {
        QString key = QString("名称：%1.IP:%2").arg(status.Name).arg(status.port);
        if (m_statusMap.contains(key)) {
            // 更新已有服务器的图标颜色
            m_statusMap[key].first->setStyleSheet(
                QString("background-color: %1; border-radius: 8px;").arg(status.color));
        } else {
            // 新服务器，创建水平布局（图标 + 文字）
            QHBoxLayout *hLayout = new QHBoxLayout();
            hLayout->setSpacing(5);

            QLabel *icon = new QLabel(this);
            icon->setFixedSize(16, 16);
            icon->setStyleSheet(QString("background-color: %1; border-radius: 8px;").arg(status.color));

            QLabel *text = new QLabel(key, this);
            hLayout->addWidget(icon);
            hLayout->addWidget(text);
            hLayout->addStretch(); // 让布局左对齐

            // 计算新项在网格中的位置：行 = 当前总数 / 5，列 = 当前总数 % 5
            int total = m_statusMap.size(); // 插入前的数量
            int row = total / 5;
            int col = total % 5;

            // 将布局添加到网格的指定位置
            ui->netStatus->addLayout(hLayout, row, col, Qt::AlignLeft);

            // 保存到映射表中
            m_statusMap[key] = qMakePair(icon, text);
        }
    }
}
