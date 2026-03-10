#include <QMenu>
#include <QDesktopServices>
#include <QProcess>
#include <QMessageBox>
#include "csproject.h"
#include "./ui_csproject.h"
#include "XmlManager/xmlmanager.h"
#include "qsshelper.h"


/**
 * @brief CSproject 构造函数
 * @param parent 父窗口指针
 *
 * 初始化主窗口的基本属性：
 * - 固定初始宽度 320，高度 460
 * - 无边框窗口
 * - 创建主区域和可扩展的右侧区域
 * - 设置动画用于平滑展开/收起
 * - 加载用户列表并填充到账号下拉框
 */
CSproject::CSproject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CSproject)
    , m_originalWidth(320)      // 主区域宽度（未展开时）
    , m_targetWidth(820)        // 展开后的总宽度（主区域+扩展区域）
    , m_isExpanded(false)       // 当前是否处于展开状态
    , m_serverWidget(nullptr)
    , loadCompleted(false)
    , m_lastQQ("")              // 上一次选中的QQ号，用于列表管理
{
    ui->setupUi(this);

    // 创建中心部件（QMainWindow 必须有一个中心部件）
    QWidget *centralWidget = new QWidget(this);
    // 注意：这里没有 setCentralWidget，后续可能会添加，但当前布局直接放到 centralWidget 中

    // 主布局：水平排列，包含主区域和扩展区域
    m_mainLayout = new QHBoxLayout(centralWidget);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    // 初始化样式和界面元素（先调用 init，但 init 中使用了 ui 指针，必须在 setupUI 之后）
    init();

    // 创建固定的左侧主区域（登录表单等）
    createMainArea();

    // 创建右侧可扩展区域（服务器设置界面），默认隐藏
    createExpandArea();

    // 创建窗口宽度动画（用于主窗口整体宽度变化）
    m_animation = new QPropertyAnimation(this, "width", this);
    m_animation->setDuration(500);                     // 动画时长500ms
    m_animation->setEasingCurve(QEasingCurve::Linear); // 匀速动画

    // 创建扩展区域几何动画（位置和宽度变化，用于滑入滑出效果）
    m_expandAnim = new QPropertyAnimation(m_expandWidget, "geometry", this);
    m_expandAnim->setDuration(300);
    m_expandAnim->setEasingCurve(QEasingCurve::Linear);

    this->loadCompleted = true; // 标记初始化完成，用于后续信号处理中避免过早响应
}

/**
 * @brief 初始化界面控件的属性
 *
 * 设置窗口固定大小、无边框、对象名；
 * 设置输入框的占位符、对齐方式、输入验证器；
 * 加载已保存的用户信息到下拉框，并填充第一个用户的密码和选项。
 */
void CSproject::init()
{
    setObjectName("loginWindow");
    setFixedSize(m_originalWidth, 460);    // 窗口高度固定，宽度由动画控制
    setWindowFlags(Qt::FramelessWindowHint); // 无边框，用于自定义拖动

    ui->serverModel->setObjectName("CircleBtn"); // 为按钮设置对象名，便于QSS样式

    // 账号下拉框设置
    ui->leQQNum->setPlaceholderText("QQ号/手机号/邮箱");
    ui->leQQPwd->setPlaceholderText("密码");
    ui->leQQNum->setFocusPolicy(Qt::StrongFocus);

    // 使下拉框可编辑（允许手动输入新号码）
    ui->leQQNum->setEditable(true);
    if (ui->leQQNum->lineEdit()) {
        ui->leQQNum->lineEdit()->setAlignment(Qt::AlignCenter); // 文字居中
        ui->leQQNum->lineEdit()->setPlaceholderText("QQ号/手机号/邮箱");
        ui->leQQNum->lineEdit()->setTextMargins(25, 0, 2, 0); // 左、上、右、下边距
        ui->leQQNum->lineEdit()->setMaxLength(7); // 限制输入最多7位
    }

    // 设置整数验证器，只允许输入数字
    QIntValidator *validator = new QIntValidator(this);
    validator->setRange(0, 9999999);
    ui->leQQNum->lineEdit()->setValidator(validator);

    // 密码框设置
    ui->leQQPwd->setAlignment(Qt::AlignCenter);
    ui->leQQPwd->setEchoMode(QLineEdit::Password);
    ui->leQQPwd->setMaxLength(7);

    // 为两个标签安装事件过滤器，以响应点击事件（实现展开/收起和打开链接）
    ui->lblService->setEnabled(true);
    ui->lblService->installEventFilter(this);
    ui->lblRegister->installEventFilter(this);

    // 使两个单选按钮不互斥，允许同时选中（或都不选）
    ui->autoLogin->setAutoExclusive(false);
    ui->forgetPwd->setAutoExclusive(false);

    // 自动登录与记住密码的联动逻辑：当自动登录勾选时，强制记住密码也被勾选
    connect(ui->autoLogin, &QRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            ui->forgetPwd->setChecked(true);
        }
    });

    // 服务器模式按钮点击时切换展开/收起
    connect(this->ui->serverModel, &QPushButton::clicked, this, &CSproject::toggleExpand);

    // 从XML加载已保存的用户信息
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QMap<QString, userInfo> users = xml_mgr.getUserInfoMap();

    // 将所有用户的QQ号添加到下拉框
    QMutableMapIterator<QString, userInfo> it(users);
    while (it.hasNext()) {
        it.next();
        ui->leQQNum->addItem(QString::number(it.value().QQNumber));
    }

    // 设置下拉框所有项居中对齐
    for (int i = 0; i < ui->leQQNum->count(); ++i) {
        ui->leQQNum->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }

    // 如果有用户，默认选中第一个，并填充密码和复选框状态
    if (ui->leQQNum->count() > 0) {
        QString firstQQ = ui->leQQNum->itemText(0);
        ui->leQQNum->setCurrentText(firstQQ);
        m_lastQQ = firstQQ;
        // 移除第一项（因为当前选中项不再出现在下拉列表中，以避免重复）
        ui->leQQNum->removeItem(0);
        // 获取第一个用户的信息
        QMap<QString, userInfo>::const_iterator userIt = users.constBegin();
        if (userIt != users.constEnd()) {
            const userInfo& info = userIt.value();
            ui->leQQPwd->setText(info.PassWord);
            ui->autoLogin->setChecked(info.AutoLogin);
            ui->forgetPwd->setChecked(info.RememberPassword);
        }
    }
}

/**
 * @brief 创建主区域（左侧登录界面）
 *
 * 初始化系统托盘，加载QSS样式。
 * 注意：实际界面的布局由UI文件定义，本函数只负责与托盘相关的初始化和样式加载。
 */
void CSproject::createMainArea()
{
    // ---------- 初始化系统托盘 ----------
    // 检查系统是否支持托盘图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        qWarning() << "System tray is not available on this system.";
    } else {
        // 创建托盘图标对象，设置图标和提示
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon("pic/icon.ico"));
        m_trayIcon->setToolTip("CSProject");

        // 创建托盘右键菜单
        m_trayMenu = new QMenu(this);
        QAction *restoreAction = m_trayMenu->addAction("恢复窗口");
        QAction *quitAction = m_trayMenu->addAction("退出程序");

        connect(restoreAction, &QAction::triggered, this, &CSproject::restoreWindow);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

        m_trayIcon->setContextMenu(m_trayMenu);

        // 连接托盘图标的激活信号（例如双击）
        connect(m_trayIcon, &QSystemTrayIcon::activated, this, &CSproject::onTrayIconActivated);
    }

    // 加载登录界面的QSS样式
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "LoginQssPath");
    if (!qssPath.isEmpty()) {
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        QSSHelper::loadLocalQss(this, qssPath);  // 全局加载，会影响整个应用程序
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
}

/**
 * @brief 创建右侧扩展区域（服务器设置界面）
 *
 * 该区域是一个 ServerSetting 窗口，默认隐藏。
 * 其宽度为 m_targetWidth - m_originalWidth，高度与主窗口一致（460）。
 */
void CSproject::createExpandArea()
{
    m_expandWidget = new ServerSetting(this);  // 注意：ServerSetting 是一个 QWidget
    m_expandWidget->setFixedSize(m_targetWidth - m_originalWidth, 460);
    m_expandWidget->setVisible(false);
}

/**
 * @brief 切换右侧扩展区域的展开/收起状态
 *
 * 通过两个属性动画实现：
 * - m_animation：控制主窗口的整体宽度变化
 * - m_expandAnim：控制扩展区域的几何变化（从右侧滑入/滑出）
 * 同时更新按钮文字和状态标志。
 */
void CSproject::toggleExpand()
{
    if (!m_isExpanded) {
        // 展开
        m_isExpanded = true;
        this->ui->serverModel->setText("服务器模式");  // 按钮文字改为“服务器模式”
        m_animation->stop();
        m_expandAnim->stop();

        // 先显示扩展区域（虽然宽度为0，但可见后动画才会生效）
        m_expandWidget->setVisible(true);

        // 设置窗口宽度动画：从原始宽度到目标宽度
        m_animation->setStartValue(m_originalWidth);
        m_animation->setEndValue(m_targetWidth);

        // 设置扩展区域几何动画：从右侧边缘（左边缘在 m_originalWidth）宽度0开始，到完全展开
        m_expandAnim->setStartValue(QRect(m_originalWidth, 0, 0, 460));
        m_expandAnim->setEndValue(QRect(m_originalWidth, 0, m_targetWidth - m_originalWidth, 460));

        m_animation->start();
        m_expandAnim->start();

        ui->lblService->setText("客户端模式");  // 左下角标签提示
    } else {
        // 收起
        this->ui->serverModel->setText("客户端模式");
        m_animation->stop();
        m_expandAnim->stop();

        // 设置窗口宽度动画：从目标宽度回到原始宽度
        m_animation->setStartValue(m_targetWidth);
        m_animation->setEndValue(m_originalWidth);

        // 设置扩展区域几何动画：从完全展开收缩到右侧宽度0
        m_expandAnim->setStartValue(QRect(m_originalWidth, 0, m_targetWidth - m_originalWidth, 460));
        m_expandAnim->setEndValue(QRect(m_originalWidth, 0, 0, 460));

        // 动画结束后隐藏扩展区域，避免残留
        connect(m_animation, &QPropertyAnimation::finished, this, [=]() {
            m_expandWidget->setVisible(false);
            disconnect(m_animation, &QPropertyAnimation::finished, this, nullptr);
        });

        m_animation->start();
        m_expandAnim->start();
        m_isExpanded = false;
        ui->lblService->setText("服务器模式");
    }
    m_animation->start(); // 注意：这一行可能多余，因为上面已经 start 了，但无妨
}

/**
 * @brief 鼠标按下事件，实现窗口拖动
 *
 * 当鼠标左键按下时，记录拖动起点。
 */
void CSproject::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

/**
 * @brief 鼠标移动事件，实现窗口拖动
 *
 * 根据记录的偏移量移动窗口位置。
 */
void CSproject::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

/**
 * @brief 鼠标释放事件，结束拖动
 */
void CSproject::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

/**
 * @brief 析构函数，释放UI资源
 */
CSproject::~CSproject()
{
    delete ui;
}

/**
 * @brief “加载”按钮点击槽（目前仅隐藏窗口，未实现加载功能）
 */
void CSproject::on_loadButton_clicked()
{
    this->hide();
}

/**
 * @brief “登录”按钮点击槽
 *
 * 验证账号密码非空后，创建 serverwidget 窗口并显示，隐藏当前登录窗口。
 * 使用 m_expandWidget 获取服务器参数（ServerSetting 提供 get_ServerParam 接口）。
 */
void CSproject::on_btnLogin_clicked()
{
    if (validateBothEmpty()) {
        this->m_serverWidget = new serverwidget(m_expandWidget->get_ServerParam());
        m_serverWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        this->hide();
        m_serverWidget->show();
    }
}

/**
 * @brief 关闭按钮点击槽（“X”按钮）
 *
 * 隐藏主窗口，并显示系统托盘图标（如果可用）。
 */
void CSproject::on_closeBtn_clicked()
{
    this->hide();
    if (m_trayIcon) {
        m_trayIcon->show();
    }
}

/**
 * @brief 最小化按钮点击槽（实际实现为退出程序，可能与预期不符）
 *
 * 注意：此函数名为 on_minBtn_clicked，但内部调用 QCoreApplication::exit(0)，
 * 即退出整个应用程序。若需最小化到托盘，应改为调用 showMinimized() 并配合 changeEvent。
 */
void CSproject::on_minBtn_clicked()
{
    QCoreApplication::exit(0);
}

/**
 * @brief 窗口状态改变事件处理
 *
 * 当窗口被最小化时，隐藏窗口并显示托盘图标（实现最小化到托盘）。
 */
void CSproject::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (this->isMinimized()) {
            QTimer::singleShot(0, this, [this]() {
                this->hide();
                if (m_trayIcon) {
                    m_trayIcon->show();
                }
            });
        }
    }
    QMainWindow::changeEvent(event);
}

/**
 * @brief 窗口关闭事件处理
 *
 * 如果系统支持托盘，则忽略关闭事件并隐藏窗口（最小化到托盘），
 * 否则接受事件并退出。
 */
void CSproject::closeEvent(QCloseEvent *event)
{
    if (m_trayIcon && m_trayIcon->isSystemTrayAvailable()) {
        event->ignore();
        this->hide();
        m_trayIcon->show();
    } else {
        event->accept();
    }
}

/**
 * @brief 托盘图标激活事件处理（例如双击）
 * @param reason 激活原因
 *
 * 如果是双击，则恢复窗口。
 */
void CSproject::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        restoreWindow();
    }
}

/**
 * @brief 从托盘恢复窗口显示
 *
 * 显示窗口，取消最小化状态，提升到最前并激活，然后隐藏托盘图标。
 */
void CSproject::restoreWindow()
{
    this->show();
    this->setWindowState(this->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    this->raise();
    this->activateWindow();

    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

/**
 * @brief 事件过滤器，用于捕获标签的点击事件
 * @param obj 事件源对象
 * @param event 事件
 * @return true 表示事件已处理
 *
 * - 点击 lblService（服务模式标签）时，触发 toggleExpand() 展开/收起右侧区域。
 * - 点击 lblRegister（注册标签）时，调用 openUrlWithRegistry 打开注册链接。
 */
bool CSproject::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->lblService && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleExpand();
            return true;
        }
    }
    else if (obj == ui->lblRegister && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            openUrlWithRegistry("www.baidu.com"); // 固定打开百度，可改为配置文件中的注册链接
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

/**
 * @brief 验证账号和密码是否都非空
 * @return true 如果两者都不为空
 */
bool CSproject::validateBothEmpty() const
{
    QString account = ui->leQQNum->currentText().trimmed();
    QString password = ui->leQQPwd->text().trimmed();
    return !account.isEmpty() && !password.isEmpty();
}

/**
 * @brief 使用系统注册表获取默认浏览器并打开URL
 * @param url 要打开的网址
 * @return 是否成功启动浏览器进程
 *
 * 通过读取 Windows 注册表中 HTTP 协议关联的 ProgId 或直接命令，启动默认浏览器。
 * 这是一个跨浏览器兼容的打开方法，不依赖 QDesktopServices。
 */
bool CSproject::openUrlWithRegistry(const QString &url)
{
    // 尝试从 HKEY_CURRENT_USER 获取用户选择的浏览器 ProgId
    QSettings userChoice("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice", QSettings::NativeFormat);
    QString progId = userChoice.value("Progid").toString();

    // 如果未获取到，从 HKEY_CLASSES_ROOT 的 http 关联读取默认命令
    if (progId.isEmpty()) {
        QSettings httpAssoc("HKEY_CLASSES_ROOT\\http\\shell\\open\\command", QSettings::NativeFormat);
        QString command = httpAssoc.value(".").toString();
        if (!command.isEmpty()) {
            command.replace("%1", url);
            // 解析命令：处理带引号的路径和参数
            QString program;
            QStringList args;
            if (command.startsWith('"')) {
                int end = command.indexOf('"', 1);
                if (end != -1) {
                    program = command.mid(1, end - 1);
                    QString rest = command.mid(end + 1).trimmed();
                    if (!rest.isEmpty())
                        args = rest.split(' ', Qt::SkipEmptyParts);
                }
            } else {
                QStringList parts = command.split(' ', Qt::SkipEmptyParts);
                if (!parts.isEmpty()) {
                    program = parts.first();
                    args = parts.mid(1);
                }
            }
            if (!program.isEmpty())
                return QProcess::startDetached(program, args);
        }
        return false;
    }

    // 通过 ProgId 获取打开命令
    QString key = "HKEY_CLASSES_ROOT\\" + progId + "\\shell\\open\\command";
    QSettings cmdSettings(key, QSettings::NativeFormat);
    QString command = cmdSettings.value(".").toString();
    if (command.isEmpty()) return false;

    command.replace("%1", url);
    // 同样解析 command
    QString program;
    QStringList args;
    if (command.startsWith('"')) {
        int end = command.indexOf('"', 1);
        if (end != -1) {
            program = command.mid(1, end - 1);
            QString rest = command.mid(end + 1).trimmed();
            if (!rest.isEmpty())
                args = rest.split(' ', Qt::SkipEmptyParts);
        }
    } else {
        QStringList parts = command.split(' ', Qt::SkipEmptyParts);
        if (!parts.isEmpty()) {
            program = parts.first();
            args = parts.mid(1);
        }
    }

    if (program.isEmpty()) return false;
    return QProcess::startDetached(program, args);
}

/**
 * @brief 账号下拉框当前文本改变时的槽函数
 * @param arg1 新的文本（可能是从列表选择或手动输入）
 *
 * 当用户切换账号时，自动填充对应的密码和复选框状态。
 * 同时维护下拉列表中的项：当前选中的账号从列表中移除（避免重复），上一个账号重新加入列表（如果有效）。
 */
void CSproject::on_leQQNum_currentTextChanged(const QString &arg1)
{
    if (!this->loadCompleted) return; // 初始化未完成时不处理

    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QMap<QString, userInfo> users = xml_mgr.getUserInfoMap();
    bool found = false;
    userInfo currentInfo;

    // 查找新文本是否在已保存的用户映射中
    QMapIterator<QString, userInfo> it(users);
    while (it.hasNext()) {
        it.next();
        if (arg1 == QString::number(it.value().QQNumber)) {
            found = true;
            currentInfo = it.value();
            break;
        }
    }

    if (found) {
        // 1. 如果新文本已经在列表中，则从列表中移除它（因为当前选中的项不应显示在下拉菜单中）
        int index = ui->leQQNum->findText(arg1);
        if (index != -1) {
            ui->leQQNum->removeItem(index);
        }

        // 2. 如果上一个用户存在且不是新用户，且是有效用户，则将其添加回列表
        if (!m_lastQQ.isEmpty() && m_lastQQ != arg1) {
            // 检查上一个用户是否在映射中（有效）
            bool lastValid = false;
            QMapIterator<QString, userInfo> it2(users);
            while (it2.hasNext()) {
                it2.next();
                if (m_lastQQ == QString::number(it2.value().QQNumber)) {
                    lastValid = true;
                    break;
                }
            }
            if (lastValid) {
                // 检查是否已在列表中，避免重复添加
                if (ui->leQQNum->findText(m_lastQQ) == -1) {
                    ui->leQQNum->addItem(m_lastQQ);
                    int newIndex = ui->leQQNum->count() - 1;
                    ui->leQQNum->setItemData(newIndex, Qt::AlignCenter, Qt::TextAlignmentRole);
                }
            }
        }

        // 3. 填充密码和复选框
        ui->leQQPwd->setText(currentInfo.PassWord);
        ui->autoLogin->setChecked(currentInfo.AutoLogin);
        ui->forgetPwd->setChecked(currentInfo.RememberPassword);

    } else {
        // 手动输入新号码：可根据需求决定是否添加为历史记录，此处不处理
    }

    // 更新上一次的QQ号
    m_lastQQ = arg1;
}
