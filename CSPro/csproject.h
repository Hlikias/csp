#ifndef CSPROJECT_H
#define CSPROJECT_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QLabel>
#include <QMouseEvent>
#include <QFile>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QTimer> // 用于延时处理
#include <QSettings>

#include "serverwidget.h"
#include "serversetting.h"
#include <QFileInfo>
QT_BEGIN_NAMESPACE
namespace Ui {
class CSproject;
}
QT_END_NAMESPACE


class CSproject : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(int width READ width WRITE setFixedWidth)
public:
    CSproject(QWidget *parent = nullptr);

    ~CSproject();


private:
    // 创建固定主区域（包含按钮和基础内容）
    void createMainArea();
    void createExpandArea();
    bool validateBothEmpty() const;
    void init();

    bool openUrlWithRegistry(const QString &url);

private slots:
    // 切换展开/收起状态
    void toggleExpand();

    void on_loadButton_clicked();

    void on_btnLogin_clicked();

    void on_closeBtn_clicked();

    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void restoreWindow(); // 用于恢复窗口的槽函数

    void on_minBtn_clicked();

    void on_leQQNum_currentTextChanged(const QString &arg1);
signals:
    void emit_serverConfig(const serverParam &param);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

    // 重写 closeEvent 来处理窗口关闭按钮（叉号）的行为
    void closeEvent(QCloseEvent *event) override;
    // 重写 changeEvent 来检测窗口最小化事件
    void changeEvent(QEvent *event) override;
    // 标签
    bool eventFilter(QObject *obj, QEvent *event) override;


private:
    Ui::CSproject *ui;
    int m_originalWidth;      // 原窗口/主区域宽度（300px）
    int m_targetWidth;        // 扩展后总宽度（800px）
    bool m_isExpanded;         // 是否展开
    QHBoxLayout *m_mainLayout; // 主布局
    //QWidget *m_expandWidget;   // 扩展区域Widget
    ServerSetting *m_expandWidget;
    QPropertyAnimation *m_animation; // 宽度动画
    QPropertyAnimation *m_expandAnim;   // 控制起始位子
    QPushButton *m_expandBtn = nullptr;   // 触发按钮（全局可访问）

    serverwidget *m_serverWidget; //服务器界面
    QPoint m_dragPosition;    // 记录鼠标按下时的位置
    bool m_dragging = false;  // 是否正在拖动

    QSystemTrayIcon *m_trayIcon; // 托盘图标对象
    QMenu *m_trayMenu;            // 托盘右键菜单
    bool loadCompleted;
    QString m_lastQQ;  // 上一次选中的QQ号

};
#endif // CSPROJECT_H
