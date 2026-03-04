#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>
#include <QtCharts/QCategoryAxis>
#include <QTimer>
#include <QDateTime>
#include <QLabel>
#include "systemmonitor.h"
#include "xmlmanager.h"
namespace Ui {
class DashboardWidget;
}

class DashboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardWidget(serverParam param,QWidget *parent = nullptr);
    ~DashboardWidget();
private slots:
    void updateDashboard();
    void on_btnRefresh_clicked();
    void onSystemDataUpdated(const SystemData &data);  // 新增：接收系统数据

private:
    void init();
    void loadLocalQss(const QString &qssPath);
    void setCardColor(QLabel *card, float value);
    void setupCharts();
    void updateCpuChart(float cpuUsage, qint64 time);
    void updateMemoryChart(float memoryUsage);
    void updateTrafficChart(int connections, float throughput, qint64 time);  // NEW
    void addActivityLog(const QString &msg);
public:
    void setServerConfig(const serverParam &param);
private:
    Ui::DashboardWidget *ui;
    QTimer *m_timer;
    QLineSeries *m_cpuSeries;
    QLineSeries *m_connSeries;          // 连接数系列
    QLineSeries *m_throughputSeries;    // 吞吐量系列
    QBarSeries *m_memorySeries;
    QBarSet *m_memorySet;
    qint64 m_timeCounter;
    SystemMonitor *m_monitor;
    QThread *m_monitorThread;
    serverParam param;
};

#endif // DASHBOARDWIDGET_H
