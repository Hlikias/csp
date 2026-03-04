#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QPainter>
#include <QStyleOptionTab>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "dashboardwidget.h"
#include "xmlmanager.h"
namespace Ui {
class serverwidget;
}

class serverwidget : public QWidget
{
    Q_OBJECT

public:
    explicit serverwidget(const serverParam &param,QWidget *parent = nullptr);
    ~serverwidget();

    void loadLocalQss(const QString &qssPath);
private:
    Ui::serverwidget *ui;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    void init();
    void initTray();  //初始化最小托盘
private slots:
    void on_closeBtn_clicked();
    void onMinimizeToTray();          // 最小化到托盘的槽函数
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason); // 托盘图标激活
    void onShowWindow();  // 显示窗口
    void onQuitApp();   // 退出程序

    void on_mainBtn_clicked();
public slots:


private:
    bool m_dragging = false;  // 是否正在拖动
    QPoint m_dragPosition;    // 记录鼠标按下时的位置

    QSystemTrayIcon *m_trayIcon; // 托盘图标对象
    QMenu *m_trayMenu; // 托盘菜单
    QAction *m_showAction; // 显示窗口动作
    QAction *m_quitAction; // 退出程序动作

    DashboardWidget *m_dashBoarWidget;
    serverParam param;
};

#endif // SERVERWIDGET_H
