#include "serverwidget.h"
#include "ui_serverwidget.h"
#include <QMessageBox>
#include <QFileInfo>
#include <QObject>
#include <QQuickItem>
#include "qsshelper.h"
/**
 * @brief serverwidget 构造函数
 * @param param 服务器配置参数，用于传递给仪表盘等子界面
 * @param parent 父窗口指针
 *
 * 初始化 UI，保存参数，设置窗口大小，加载 QSS 样式，并调用 init() 和 initTray()
 * 完成界面初始化和系统托盘设置。
 */
serverwidget::serverwidget(const serverParam &param, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::serverwidget)
    , m_userWidget(nullptr)   // 初始化 nullptr
{
    ui->setupUi(this);
    this->m_param = param;               // 保存服务器配置参数
    this->resize(1920, 1080);          // 设置窗口默认大小
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "ServerMainQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径：如果是相对路径，则拼接应用程序目录
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        QSSHelper::loadLocalQss(this, qssPath);
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
    initUI();
    init();          // 初始化界面元素（按钮文字、图片、仪表盘等）
    initTray();      // 初始化系统托盘

}

/**
 * @brief 析构函数，释放 UI 资源
 */
serverwidget::~serverwidget()
{
    delete ui;
}

/**
 * @brief 初始化系统托盘
 *
 * 检查系统是否支持托盘图标，如果支持则创建托盘对象、设置图标、创建上下文菜单，
 * 并连接相关信号（激活、最小化按钮等）。托盘图标初始时不显示，在窗口最小化时显示。
 */
void serverwidget::initTray()
{
    // 1. 检查系统是否支持托盘图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // 如果不支持，可以选择不创建托盘或给出提示
        return;
    }

    // 2. 创建托盘图标对象，并设置图标和提示
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon("pic/icon.ico"));   // 请确保图标文件存在
    m_trayIcon->setToolTip("我的应用程序");

    // 3. 创建托盘菜单和动作
    m_trayMenu = new QMenu(this);
    m_showAction = new QAction("显示窗口", this);
    m_quitAction = new QAction("退出程序", this);

    connect(m_showAction, &QAction::triggered, this, &serverwidget::onShowWindow);
    connect(m_quitAction, &QAction::triggered, this, &serverwidget::onQuitApp);

    m_trayMenu->addAction(m_showAction);
    m_trayMenu->addSeparator(); // 添加分隔线
    m_trayMenu->addAction(m_quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    // 4. 连接托盘图标的激活信号（单击、双击等）
    connect(m_trayIcon, &QSystemTrayIcon::activated,
            this, &serverwidget::onTrayIconActivated);

    // 5. 连接最小化按钮的点击信号到自定义槽函数
    /*
    connect(ui->minBtn, &QPushButton::clicked,
            this, &serverwidget::onMinimizeToTray);
    */
}

bool serverwidget::initNet()
{

    if(m_socketList==NULL)
        return false;
    m_socketList = new ServerManager();
    // 这里只开一个tcp和udp单播服务，后面在写界面设置吧
    m_socketList->addTcpServer("Tcp测试用",
                               m_param.m_host,
                               m_param.m_port,
                               m_param.m_maxUsers,
                               m_param.m_maxConnectionsOptions,
                               m_param.m_memoryLimitMB);

    m_socketList->addUdpServer("Ucp测试用",
                               m_param.m_host,
                               m_param.m_port,
                               m_param.m_maxUsers,
                               m_param.m_memoryLimitMB);
    //connect(m_socketList, &ServerManager::toWinQList,this,&serverwidget::slot_Server_Info_Update);

    m_socketList->startAll();
    connect(m_socketList, &ServerManager::toWinQList,
            m_dashBoarWidget,&DashboardWidget::updateTCPLog);
    return false;
}



/**
 * @brief 鼠标按下事件，实现窗口拖动
 * @param event 鼠标事件
 *
 * 当鼠标左键按下时，记录拖动起点，并接受事件。
 */
void serverwidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
 * @brief 鼠标移动事件，实现窗口拖动
 * @param event 鼠标事件
 *
 * 如果正在拖动，根据鼠标移动偏移量移动窗口位置。
 */
void serverwidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

/**
 * @brief 鼠标释放事件，结束拖动
 * @param event 鼠标事件
 *
 * 当左键释放时，停止拖动。
 */
void serverwidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

/**
 * @brief 窗口关闭事件处理
 * @param event 关闭事件
 *
 * 重写 closeEvent 以实现最小化到托盘而不是退出。
 * 默认行为是忽略关闭事件并调用 onMinimizeToTray() 隐藏窗口。
 * 如果希望询问用户，可以取消注释下方的询问框代码。
 */
void serverwidget::closeEvent(QCloseEvent *event)
{
    // 忽略关闭事件，并调用最小化到托盘的函数
    event->ignore();
    onMinimizeToTray();

    // 如果需要弹出询问框，可以参考以下注释代码
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

/**
 * @brief 初始化界面元素
 *
 * 设置标题标签的图片、各按钮的文字、创建仪表盘（DashboardWidget）并添加到 stackedWidget，
 * 并默认选中主界面按钮。
 */
void serverwidget::init()
{






    QString dbPath = QCoreApplication::applicationDirPath() + "/data/server.db";
    // 初始化数据库管理器
    m_sqlManager = new SqlManager(dbPath, this);  // 数据库文件名
    if (!m_sqlManager->initialize()) {
        qCritical() << "Failed to initialize database!";
        // 可根据需要处理错误，例如禁用相关功能
    } else {
        qDebug() << "Database initialized successfully.";
    }


    // 创建仪表盘界面并添加到 stackedWidget
    m_dashBoarWidget = new DashboardWidget(this->m_param, this);
    ui->stackedWidget->addWidget(m_dashBoarWidget);

    connect(m_dashBoarWidget,&DashboardWidget::emit_Start_Server,this,&serverwidget::slot_Server_Start);
    // 默认选中主界面按钮
    ui->mainBtn->setChecked(true);
}

void serverwidget::initUI()
{
    // 设置标题标签大小并加载 Logo 图片
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

    // 设置按钮文字
    ui->mainBtn->setText("主界面");
    ui->userBtn->setText("用户管理");
    ui->settingBtn->setText("系统设置");
    ui->serviceBtn->setText("服务/进程管理");
    ui->logBtn->setText("日志管理");
    ui->monitoringBtn->setText("监控与告警");

    /*
    // 初始化悬浮窗口（但不显示）
    m_floatingView = new QQuickView();
    m_floatingView->setSource(QUrl("qrc:/new/prefix1/FloatingWindow.qml")); // 确保 qml 已加入资源
    m_floatingView->setResizeMode(QQuickView::SizeRootObjectToView);
    m_floatingView->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    m_floatingView->setColor(QColor(Qt::transparent)); // 背景透明
    // 连接状态变化信号到专门槽函数
    connect(m_floatingView, &QQuickView::statusChanged,
            this, &serverwidget::onFloatingViewStatusChanged);/
    */

    // 创建悬浮窗对象（父对象为 this，自动管理）
    m_floatingWin = new FloatingWindow(this);
    // 连接双击信号到恢复窗口的槽
    connect(m_floatingWin, &FloatingWindow::doubleClicked,
            this, &serverwidget::onFloatingWindowDoubleClicked);
    // 初始隐藏
    m_floatingWin->setVisible(false);
}

/**
 * @brief 关闭按钮点击槽（“X”按钮）
 *
 * 直接退出应用程序。注意：不会经过 closeEvent，因为按钮点击会直接触发此槽。
 * 如需最小化到托盘，应连接其他按钮（如最小化按钮）。
 */
void serverwidget::on_closeBtn_clicked()
{
    QMessageBox::StandardButton btn = QMessageBox::question(this, "确认退出", "确定要退出程序吗？");
    if (btn == QMessageBox::Yes) {
        qApp->quit();
        FreeConsole();  // 释放控制台
    }
    /*
    this->hide();

    // 将悬浮窗口定位到屏幕右下角（或其他合适位置）
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeo = screen->availableGeometry();
    m_floatingView->setPosition(screenGeo.right() - m_floatingView->width(),
                                screenGeo.bottom() - m_floatingView->height());
    m_floatingView->show();
    //QCoreApplication::exit(0);
*/
}

/**
 * @brief 最小化到托盘
 *
 * 隐藏主窗口，并显示托盘图标（如果尚未显示）。
 * 通常由最小化按钮点击或 closeEvent 调用。
 */
void serverwidget::onMinimizeToTray()
{
    this->hide();   // 隐藏窗口
    if (m_trayIcon && !m_trayIcon->isVisible()) {
        m_trayIcon->show();   // 显示托盘图标
        // 可选：显示气泡提示
        // m_trayIcon->showMessage("提示","应用程序已最小化到系统托盘", QSystemTrayIcon::Information, 2000);
    }
    // 显示悬浮窗口
    if (m_floatingWin) {
        m_floatingWin->setVisible(true);
    }
}

/**
 * @brief 托盘图标激活事件处理
 * @param reason 激活原因（单击、双击等）
 *
 * 当用户单击或双击托盘图标时，调用 onShowWindow() 恢复窗口显示。
 */
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

/**
 * @brief 显示主窗口并隐藏托盘图标
 *
 * 将窗口显示出来，激活并提升至最前，同时隐藏托盘图标。
 */
void serverwidget::onShowWindow()
{
    // 隐藏悬浮窗口
    if (m_floatingWin) {
        m_floatingWin->setVisible(false);
    }
    this->show();
    this->activateWindow(); // 确保窗口获得焦点并显示在最前
    this->raise();

    // 隐藏托盘图标（可选，也可以选择保留图标）
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

/**
 * @brief 退出应用程序
 *
 * 清理托盘图标资源，然后退出应用程序。
 */
void serverwidget::onQuitApp()
{
    if (m_trayIcon) {
        m_trayIcon->hide();
        m_trayIcon->deleteLater();   // 安全删除
    }
    QCoreApplication::exit(0);
}

/**
 * @brief 主界面按钮点击槽
 *
 * 切换 stackedWidget 到主界面页面。
 * 注意：当前代码使用 setCurrentWidget(0) 是不正确的，因为 setCurrentWidget
 * 需要 QWidget* 参数。正确的做法是使用 setCurrentIndex(0) 或
 * setCurrentWidget(m_dashBoarWidget)。此处保持原样，但建议修改。
 */
void serverwidget::on_mainBtn_clicked()
{
    ui->stackedWidget->setCurrentWidget(m_dashBoarWidget);
}

void serverwidget::slot_Server_Start()
{
    initNet();
    if (m_dashBoarWidget && m_socketList) {
        connect(m_socketList, &ServerManager::serverStatusUpdated,
                m_dashBoarWidget, &DashboardWidget::onServerStatusUpdated);
        // 可选：立即获取一次初始状态（因为 startAll 内部会触发一次 updateServerStatus）
    }
}

void serverwidget::slot_Server_Info_Update(const QHostAddress &host, QString msg)
{
    emit toDashBoard(host,msg);
}

void serverwidget::on_userBtn_clicked()
{
    // 如果尚未创建用户界面
    if (!m_userWidget) {
        m_userWidget = new UserManagerWidget(this);   // 根据你的构造函数传参调整
        ui->stackedWidget->addWidget(m_userWidget);
    }
    // 从数据库获取数据
    if (m_sqlManager) {
        // 获取连接日志（例如最近100条）
        QList<ConnectionLog*> connLogs = m_sqlManager->selectConnectionLogs("", QDateTime(), QDateTime(), 100);
        // 获取处理数据（最近100条）
        QList<ProcessedDataEntity*> procData = m_sqlManager->selectProcessedData("", QDateTime(), QDateTime(), 100);

        // 传递给用户界面
        m_userWidget->setConnectionLogs(connLogs);
        m_userWidget->setProcessedData(procData);

        // 注意：ConnectionLog 和 ProcessedDataEntity 是通过 new 创建的，需要管理内存
        // 如果 UserManagerWidget 内部不再需要，应负责删除。或者此处使用智能指针。
        // 简单起见，可以在 set 方法中接收后直接使用，但要注意内存泄漏。
        // 建议将 QList<ConnectionLog*> 改为 QList<ConnectionLog> 值类型，避免手动内存管理。
        // 但原代码返回的是指针列表，所以需要处理内存。一个简单的方式是在 UserManagerWidget 中接收后自行删除，但数据需要拷贝。
        // 这里我们假设 set 方法内部会拷贝数据并自行管理，调用方可以删除原指针。
        // 为简化，可以修改 select 函数返回 QList<ConnectionLog> 值类型，或者使用 shared_ptr。
        // 这里为了演示，采用值传递，需要修改 SqlManager 返回类型。如果不改，则需要循环 delete。
        // 建议修改 SqlManager 返回 QList<ConnectionLog> 和 QList<ProcessedDataEntity> 值类型。
    }

    // 切换到用户界面

    // 切换到用户界面
    ui->stackedWidget->setCurrentWidget(m_userWidget);
    // 或者用索引：ui->stackedWidget->setCurrentIndex(ui->stackedWidget->indexOf(m_userWidget));
}

void serverwidget::onFloatingViewStatusChanged(QQuickView::Status status)
{

}


void serverwidget::on_minBtn_clicked()
{
    onMinimizeToTray();  // 最小化到托盘并显示悬浮窗

}

void serverwidget::onToggleFloatingWindow()
{

}

void serverwidget::onFloatingWindowDoubleClicked()
{
    onShowWindow();   // 直接调用显示窗口
}

void serverwidget::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == (Qt::ControlModifier | Qt::AltModifier) &&
        event->key() == Qt::Key_C) {
        // 显示/隐藏控制台
        HWND consoleWnd = GetConsoleWindow();
        if (IsWindowVisible(consoleWnd)) {
            ShowWindow(consoleWnd, SW_HIDE);
        } else {
            ShowWindow(consoleWnd, SW_SHOW);
        }
    }
    QWidget::keyPressEvent(event);
}

