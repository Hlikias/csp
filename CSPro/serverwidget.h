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
#include <QQuickView>
#include "dashboardwidget.h"
#include "XmlManager/xmlmanager.h"
#include "NetManager/servermanager.h"
#include "usermanagerwidget.h"
#include "sqlmanager.h"
#include "floatingwindow.h"
namespace Ui {
class serverwidget;
}

class serverwidget : public QWidget
{
    Q_OBJECT

public:
    explicit serverwidget(const serverParam &param,QWidget *parent = nullptr);
    ~serverwidget();

private:
    Ui::serverwidget *ui;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    void init();
    void initUI();
    void initTray();  //初始化最小托盘
    bool initNet();   //初始化网络
private slots:
    void on_closeBtn_clicked();       // 关闭按钮
    void onMinimizeToTray();          // 最小化到托盘的槽函数
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason); // 托盘图标激活
    void onShowWindow();  // 显示窗口
    void onQuitApp();   // 退出程序

    void on_mainBtn_clicked();

    void on_userBtn_clicked();

    void onFloatingViewStatusChanged(QQuickView::Status status);

    void on_minBtn_clicked();

    void onToggleFloatingWindow();      // 切换悬浮窗显示/隐藏

    void onFloatingWindowDoubleClicked(); // 悬浮窗双击处理
protected:
    void keyPressEvent(QKeyEvent *event) override;
public slots:
    void slot_Server_Start();                    //启动服务器
    void slot_Server_Info_Update(const QHostAddress &host, QString msg);

signals:
    void toDashBoard(const QHostAddress &host, QString msg);

private:
    bool m_dragging = false;  // 是否正在拖动
    QPoint m_dragPosition;    // 记录鼠标按下时的位置

    QSystemTrayIcon *m_trayIcon; // 托盘图标对象
    QMenu *m_trayMenu; // 托盘菜单
    QAction *m_showAction; // 显示窗口动作
    QAction *m_quitAction; // 退出程序动作

    DashboardWidget *m_dashBoarWidget;
    UserManagerWidget      *m_userWidget;         // 用户界面指针，初始为 nullptr
    SqlManager *m_sqlManager;          // 数据库管理对象

    ServerManager *m_socketList;
    serverParam m_param;

    FloatingWindow *m_floatingWin;      // 悬浮窗对象


};

#endif // SERVERWIDGET_H
