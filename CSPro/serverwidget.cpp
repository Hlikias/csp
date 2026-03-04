#include "serverwidget.h"
#include "ui_serverwidget.h"

#include <QFileInfo>
serverwidget::serverwidget(const serverParam &param,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serverwidget)
{
    ui->setupUi(this);
    this->param = param;
    this->resize(1920,1080);
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "ServerMainQssPath");
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
    init();
    initTray();
}

serverwidget::~serverwidget()
{
    delete ui;
}

void serverwidget::initTray()
{
    // 1. 检查系统是否支持托盘图标 [citation:5][citation:9]
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // 如果不支持，可以选择不创建托盘或给出提示
        return;
    }

    // 2. 创建托盘图标对象，并设置图标和提示 [citation:2][citation:5]
    m_trayIcon = new QSystemTrayIcon(this);
    // 请确保你的资源文件中有这个图标，或者使用其他有效路径
    //m_trayIcon->setIcon(QIcon::fromTheme("application-exit", QIcon(":/fallback.png")));
    m_trayIcon->setIcon(QIcon("pic/icon.ico"));
    m_trayIcon->setToolTip("我的应用程序");

    // 3. 创建托盘菜单和动作 [citation:1][citation:3]
    m_trayMenu = new QMenu(this);
    m_showAction = new QAction("显示窗口", this);
    m_quitAction = new QAction("退出程序", this);

    // 连接菜单动作的信号
    connect(m_showAction, &QAction::triggered, this, &serverwidget::onShowWindow);
    connect(m_quitAction, &QAction::triggered, this, &serverwidget::onQuitApp);

    // 将动作添加到菜单
    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addSeparator(); // 添加分隔线
    m_trayMenu->addAction(m_quitAction);

    // 将菜单设置为托盘图标的上下文菜单 [citation:2]
    m_trayIcon->setContextMenu(m_trayMenu);

    // 4. 连接托盘图标的激活信号（单击、双击等） [citation:3][citation:9]
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &serverwidget::onTrayIconActivated);

    // 5. 连接最小化按钮的点击信号到自定义槽函数
    connect(ui->minBtn, &QPushButton::clicked,
            this, &serverwidget::onMinimizeToTray);
}

// 封装加载QSS的函数（简单易用）
void serverwidget::loadLocalQss(const QString &qssPath) {
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

void serverwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void serverwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void serverwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

void serverwidget::closeEvent(QCloseEvent *event)
{
    // 如果不是真的想退出，而是最小化到托盘
    // 可以忽略关闭事件，并调用最小化到托盘的函数
    event->ignore();
    onMinimizeToTray();

    // 如果希望弹出询问框，可以参考以下注释代码
    /*
    QMessageBox::StandardButton button = QMessageBox::question(this, "提示",
                                                                "是否最小化到系统托盘？",
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
    if (button == QMessageBox::Yes) {
        event->ignore();
        onMinimizeToTray();
    } else {
        event->accept();
        onQuitApp(); // 直接退出
    }
    */
}

void serverwidget::init()
{
    ui->titleLabel->resize(200, 100);
    QPixmap pixmap("pic/servermain/logo.png");
    if (!pixmap.isNull()) {
        // 缩放图片以适应 label 大小，并保持比例
        QSize labelSize = ui->titleLabel->size();
        QPixmap scaled = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->titleLabel->setPixmap(scaled);
    } else {
        ui->titleLabel->setText("图片加载失败");
    }
    ui->mainBtn->setText("主界面");
    ui->userBtn->setText("用户管理");
    ui->settingBtn->setText("系统设置");
    ui->serviceBtn->setText("服务/进程管理");
    ui->logBtn->setText("日志管理");
    ui->monitoringBtn->setText("监控与告警");
    m_dashBoarWidget = new DashboardWidget(this->param,this);
    ui->stackedWidget->addWidget(m_dashBoarWidget);

}

void serverwidget::on_closeBtn_clicked()
{
    QCoreApplication::exit(0);
}

void serverwidget::onMinimizeToTray()
{
    // 隐藏主窗口
    this->hide();
    // 显示托盘图标 [citation:4][citation:10]
    if (m_trayIcon && !m_trayIcon->isVisible()) {
        m_trayIcon->show();
        // 可选：显示一个提示消息，告诉用户程序已最小化到托盘 [citation:5][citation:9]
        //m_trayIcon->showMessage("提示","应用程序已最小化到系统托盘",QSystemTrayIcon::Information,2000);
    }
}

void serverwidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:      // 单击
    case QSystemTrayIcon::DoubleClick:  // 双击
        onShowWindow();                 // 显示窗口
        break;
    default:
        break;
    }
}

void serverwidget::onShowWindow()
{
    // 显示窗口 [citation:4][citation:10]
    this->show();
    this->activateWindow(); // 确保窗口获得焦点并显示在最前 [citation:4]
    this->raise();

    // 隐藏托盘图标（可选，也可以选择保留图标）
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

void serverwidget::onQuitApp()
{
    // 清理托盘图标并退出 [citation:10]
    if (m_trayIcon) {
        m_trayIcon->hide();
        m_trayIcon->deleteLater();
    }
    QCoreApplication::exit(0);
}



void serverwidget::on_mainBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(0);
}

