#include <QMenu>
#include <QDesktopServices>
#include <QProcess>
#include <QMessageBox>
#include "csproject.h"
#include "./ui_csproject.h"
#include "xmlmanager.h"
void loadGlobalQss(const QString &qssPath);
CSproject::CSproject(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CSproject)
    ,m_originalWidth(320)    // 原窗口宽度（主区域宽度）
    ,m_targetWidth(820)      // 扩展后总宽度
    ,m_isExpanded(false)   // 扩展后的宽度
    ,m_serverWidget(NULL)
    ,loadCompleted(false)
    ,m_lastQQ("")
{
     ui->setupUi(this);

    // 2. 创建中心部件（必须！QMainWindow 需要中心部件
     // 1. 基础窗口设置（固定高度，初始固定宽度）
    QWidget *centralWidget = new QWidget(this);
    //setCentralWidget(centralWidget);
    // 2. 创建主布局（水平布局：主区域 + 扩展区域）
    m_mainLayout = new QHBoxLayout(centralWidget);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    init();

    // 3. 创建固定主区域
    createMainArea();

    // 4. 创建右侧扩展区域（默认隐藏）
    createExpandArea();
    // 5. 创建动画（控制窗口宽度渐变）
    m_animation = new QPropertyAnimation(this, "width", this);
    m_animation->setDuration(500); // 动画时长300ms，越久越慢
    m_animation->setEasingCurve(QEasingCurve::Linear);
    // 缓动曲线，更自然
    //控制起始位子
    m_expandAnim = new QPropertyAnimation(m_expandWidget, "geometry", this);
    m_expandAnim->setDuration(300); // 动画时长300ms，越久越慢
    m_expandAnim->setEasingCurve(QEasingCurve::Linear);
    this->loadCompleted=true;
}
void CSproject::init()
{

    setObjectName("loginWindow");
    setFixedSize(m_originalWidth, 460);    // 窗口高度固定，仅调整宽度
    setWindowFlags(Qt::FramelessWindowHint);
    ui->serverModel->setObjectName("CircleBtn");



    ui->leQQNum->setPlaceholderText("QQ号/手机号/邮箱");
    ui->leQQPwd->setPlaceholderText("密码");
    ui->leQQNum->setFocusPolicy(Qt::StrongFocus);
    //ui->leQQNum->setAlignment(Qt::AlignCenter);
    // 在 CSproject 构造函数中
    ui->leQQNum->setEditable(true);
    if (ui->leQQNum->lineEdit()) {
        ui->leQQNum->lineEdit()->setAlignment(Qt::AlignCenter);
        ui->leQQNum->lineEdit()->setPlaceholderText("QQ号/手机号/邮箱");
        ui->leQQNum->lineEdit()->setTextMargins(25, 0, 2, 0); // 左、上、右、下
        ui->leQQNum->lineEdit()->setMaxLength(7);       // 限制输入最多7位
    }
    QIntValidator *validator = new QIntValidator(this);
    validator->setRange(0, 9999999);  // 设置允许的范围，例如 0 到 999999
    ui->leQQNum->lineEdit()->setValidator(validator);
    ui->leQQPwd->setAlignment(Qt::AlignCenter);
    ui->leQQPwd->setEchoMode(QLineEdit::Password);
    ui->leQQPwd->setMaxLength(7);

    ui->lblService->setEnabled(true);
    ui->lblService->installEventFilter(this);
    ui->lblRegister->installEventFilter(this);
    ui->autoLogin->setAutoExclusive(false);
    ui->forgetPwd->setAutoExclusive(false);

    connect(ui->autoLogin, &QRadioButton::toggled, this, [=](bool checked) {
        if (checked) {
            ui->forgetPwd->setChecked(true);
        }
        // 取消时什么也不做
    });
    connect(this->ui->serverModel,&QPushButton::clicked, this, &CSproject::toggleExpand);


    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QMap<QString, userInfo> users = xml_mgr.getUserInfoMap();
    QMutableMapIterator<QString, userInfo> it(users);
    while (it.hasNext()) {
        it.next();
        ui->leQQNum->addItem(QString::number(it.value().QQNumber));
    }
    // 第二步：设置所有项居中对齐
    for (int i = 0; i < ui->leQQNum->count(); ++i) {
        ui->leQQNum->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole);
    }
    // 第三步：如果有用户，设置当前显示的为第一个用户（或按需选择）
    if (ui->leQQNum->count() > 0) {
        // 获取第一个用户的 QQ 号（即列表第一项的文本）
        QString firstQQ = ui->leQQNum->itemText(0);
        ui->leQQNum->setCurrentText(firstQQ);
        m_lastQQ = firstQQ;
        ui->leQQNum->removeItem(0);
        // 查找该用户的信息并填充密码和复选框
        QMap<QString, userInfo>::const_iterator userIt = users.constBegin();
        if (userIt != users.constEnd()) {
            const userInfo& info = userIt.value();
            ui->leQQPwd->setText(info.PassWord);
            ui->autoLogin->setChecked(info.AutoLogin);
            ui->forgetPwd->setChecked(info.RememberPassword);
        }
    }
}
// 创建固定主区域（包含按钮和基础内容）
void CSproject::createMainArea()
{
    // ========== 初始化系统托盘 ==========
    // 1. 检查系统是否支持托盘图标
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        // 如果不支持，可以输出警告或直接忽略此功能
        qWarning() << "System tray is not available on this system.";
    } else {
        // 2. 创建托盘图标对象，并设置图标（请确保图标文件存在于你的资源中）
        m_trayIcon = new QSystemTrayIcon(this);
        m_trayIcon->setIcon(QIcon("pic/icon.ico"));
        //m_trayIcon->setIcon(QIcon::fromTheme("application-exit", QIcon(":/fallback.png")));
        // 或使用样式表图标（较麻烦）
        m_trayIcon->setToolTip("CSProject"); // 鼠标悬停时显示的提示

        // 3. 创建托盘右键菜单
        m_trayMenu = new QMenu(this);
        QAction *restoreAction = m_trayMenu->addAction("恢复窗口");
        QAction *quitAction = m_trayMenu->addAction("退出程序");

        // 连接菜单项的触发信号
        connect(restoreAction, &QAction::triggered, this, &CSproject::restoreWindow);
        connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);

        m_trayIcon->setContextMenu(m_trayMenu);

        // 4. 连接托盘图标的激活信号（例如双击）
        connect(m_trayIcon, &QSystemTrayIcon::activated, this, &CSproject::onTrayIconActivated);

        // 注意：此时不调用 m_trayIcon->show()，因为窗口默认是可见的，托盘图标先不显示
    }

    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "LoginQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        loadGlobalQss(qssPath);
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
}

// 创建右侧扩展区域
void CSproject::createExpandArea()
{
    //m_expandWidget = new QWidget(this);
    m_expandWidget = new ServerSetting(this);
    // 关键：设置扩展区域的尺寸策略——宽度填充剩余空间，高度固定
    m_expandWidget->setFixedSize(m_targetWidth - m_originalWidth, 460);
    m_expandWidget->setVisible(false);
}

// 切换展开/收起状态
void CSproject::toggleExpand()
{
    if (!m_isExpanded) {
        // 更新按钮文字
        m_isExpanded = true;
        /*
        QPushButton *btn = qobject_cast<QPushButton*>(sender());
        if (btn)
            btn->setText("隐藏右侧界面");
         */
        this->ui->serverModel->setText("服务器模式");
        m_animation->stop();
        m_expandAnim->stop();
        // 显示：扩展界面从右侧缓缓移入（原界面位置不变）
        m_expandWidget->setVisible(true);
        // 动画：扩展界面X坐标从 300（主界面宽度）→ 300（固定），窗口宽度扩展到800
        // 起始位置：X=300（主界面右边缘），但宽度0（完全隐藏）
        m_animation->setStartValue(m_originalWidth);
        // 结束位置：X=300（主界面右边缘），宽度500（完全显示）
        m_animation->setEndValue(m_targetWidth);
        // 补充：若要“滑入”效果，可将startValue设为QRect(m_mainWidth + m_expandWidth, 0, m_expandWidth, 400)

        // 起始状态：扩展区左边缘=300px（主窗口最右侧）、宽度=0（完全隐藏）
        m_expandAnim->setStartValue(QRect(m_originalWidth, 0, 0, 460));
        // 结束状态：扩展区左边缘=300px、宽度=500px（完全显示）
        m_expandAnim->setEndValue(QRect(m_originalWidth, 0, m_targetWidth-m_originalWidth, 460));

        m_animation->start();
        m_expandAnim->start();

        ui->lblService->setText("客户端模式");
    } else {
        // 第二步：回收 - 窗口从800px缓慢变到300px
        this->ui->serverModel->setText("客户端模式");

        // 动画：宽度从800→300px，匀速渐变
        m_animation->stop();
        m_expandAnim->stop();
        m_animation->setStartValue(m_targetWidth);
        m_animation->setEndValue(m_originalWidth);

        // 起始状态：扩展区左边缘=300px（主窗口最右侧）、宽度=0（完全隐藏）
        m_expandAnim->setStartValue(QRect(m_originalWidth, 0, m_targetWidth-m_originalWidth, 460));
        // 结束状态：扩展区左边缘=300px、宽度=500px（完全显示）
        m_expandAnim->setEndValue(QRect(m_originalWidth, 0, 0, 460));
        // 动画结束后隐藏扩展区域（避免回收后残留）
        connect(m_animation, &QPropertyAnimation::finished, this, [=]() {
            m_expandWidget->setVisible(false);
            disconnect(m_animation, &QPropertyAnimation::finished, this, nullptr);
        });

        m_animation->start();
        m_expandAnim->start();
        m_isExpanded = false;
        ui->lblService->setText("服务器模式");
    }
    m_animation->start();
}
void CSproject::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void CSproject::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void CSproject::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}

CSproject::~CSproject()
{
    delete ui;
}


// 封装加载QSS的函数（简单易用）
void loadGlobalQss(const QString &qssPath) {
    QFile file(qssPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "[ERROR] 打开QSS文件失败：" << file.errorString();
        return;
    }

    // 读取文件（强制UTF-8编码，避免中文/特殊字符问题）
    QTextStream stream(&file);

    QString qss = stream.readAll();
    file.close();

    // 尝试应用样式，并捕获解析错误
    qApp->setStyleSheet(qss);

    // 验证解析结果
    QString appliedQss = qApp->styleSheet();
    if (appliedQss.isEmpty() && !qss.isEmpty()) {
        qDebug() << "[ERROR] QSS解析失败！";

        // 逐段测试（定位错误位置）
        QStringList sections = qss.split("\n\n"); // 按空行分割样式段
        for (int i = 0; i < sections.size(); i++) {
            QString section = sections[i].trimmed();
            if (section.isEmpty()) continue;

            qApp->setStyleSheet(section);
            if (qApp->styleSheet().isEmpty()) {
                qDebug() << "[ERROR] 第" << i+1 << "段样式解析失败：\n" << section;
                break; // 找到错误段后停止
            }
            qApp->setStyleSheet(""); // 清空，继续测试下一段
        }
    } else {
        qDebug() << "[SUCCESS] QSS解析并应用成功！";
    }
}

void CSproject::on_loadButton_clicked()
{
    this->hide();
}

void CSproject::on_btnLogin_clicked()
{
    if(validateBothEmpty())
    {
        this->m_serverWidget = new serverwidget(m_expandWidget->get_ServerParam());
        m_serverWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
        this->hide();

        m_serverWidget->show();
    }
}


void CSproject::on_closeBtn_clicked()
{
    this->hide();               // 隐藏窗口
    if (m_trayIcon) {
        m_trayIcon->show();     // 显示托盘图标
        // 可选：弹出气泡提示
        //m_trayIcon->showMessage("提示", "程序已最小化到系统托盘", QSystemTrayIcon::Information, 3000);
    }
}

void CSproject::on_minBtn_clicked()
{
    QCoreApplication::exit(0);
}

void CSproject::changeEvent(QEvent *event)
{
    // 当窗口状态改变时（如最小化、恢复、最大化）
    if (event->type() == QEvent::WindowStateChange) {
        // 检查窗口是否被最小化
        if (this->isMinimized()) {
            // 延迟一小段时间后隐藏窗口，避免动画卡顿或窗口闪烁
            // 这是常见技巧，因为直接 hide() 可能会导致最小化动画不完整
            QTimer::singleShot(0, this, [this]() {
                this->hide(); // 隐藏窗口
                if (m_trayIcon) {
                    m_trayIcon->show(); // 显示托盘图标
                }
            });
        }
    }

    // 调用基类的 changeEvent，确保其他行为正常
    QMainWindow::changeEvent(event);
}

void CSproject::closeEvent(QCloseEvent *event)
{
    // 如果托盘图标有效，说明我们想要最小化到托盘而不是退出
    if (m_trayIcon && m_trayIcon->isSystemTrayAvailable()) {
        // 忽略关闭事件，不退出程序
        event->ignore();
        // 隐藏窗口并显示托盘图标
        this->hide();
        m_trayIcon->show();
        // 可选：弹出一个气泡提示，告知用户程序已最小化到托盘
        //m_trayIcon->showMessage("CSProject", "程序已最小化到系统托盘，双击图标恢复。", QSystemTrayIcon::Information, 3000);
    } else {
        // 如果没有托盘图标（比如系统不支持），则正常关闭程序
        event->accept();
    }
}

void CSproject::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    // 如果是双击托盘图标，则恢复窗口
    if (reason == QSystemTrayIcon::DoubleClick) {
        restoreWindow();
    }
}

void CSproject::restoreWindow()
{
    // 显示窗口
    this->show();
    // 如果之前是最小化状态，恢复正常大小
    this->setWindowState(this->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    this->raise();           // 将窗口提升到最前
    this->activateWindow();  // 激活窗口，使其获得焦点

    // 隐藏托盘图标（当窗口显示时，通常可以隐藏托盘图标，让界面更干净）
    if (m_trayIcon) {
        m_trayIcon->hide();
    }
}

bool CSproject::eventFilter(QObject *obj, QEvent *event)
{
    //qDebug() << "eventFilter called for object:" << obj << "event type:" << event->type();
    if (obj == ui->lblService && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            toggleExpand();
            return true; // 表示事件已处理，不再传递
        }
    }
    else if(obj == ui->lblRegister && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            openUrlWithRegistry("www.baidu.com");
        }
    }
    return QMainWindow::eventFilter(obj, event);
}


bool CSproject::validateBothEmpty() const
{
    QString account = ui->leQQNum->currentText().trimmed();
    QString password = ui->leQQPwd->text().trimmed();
    return !account.isEmpty() && !password.isEmpty();  // 两者都为空返回 true
}

bool CSproject::openUrlWithRegistry(const QString &url)
{
    // 尝试从 HKEY_CURRENT_USER 获取用户选择的浏览器 ProgId
    QSettings userChoice("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice", QSettings::NativeFormat);
    QString progId = userChoice.value("Progid").toString();

    // 如果未获取到，从 HKEY_CLASSES_ROOT 的 http 关联读取
    if (progId.isEmpty()) {
        QSettings httpAssoc("HKEY_CLASSES_ROOT\\http\\shell\\open\\command", QSettings::NativeFormat);
        QString command = httpAssoc.value(".").toString();
        if (!command.isEmpty()) {
            command.replace("%1", url);
            // 简单解析命令（处理带引号的路径）
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
    // 同上解析 command
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

void CSproject::on_leQQNum_currentTextChanged(const QString &arg1)
{
    if (!this->loadCompleted) return;
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QMap<QString, userInfo> users = xml_mgr.getUserInfoMap();
    bool found = false;

    userInfo currentInfo;

    // 查找新文本是否在用户映射中
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
        // 1. 如果新文本在列表中（可能因为之前被删了所以不在），则从列表中删除它
        int index = ui->leQQNum->findText(arg1);
        if (index != -1) {
            ui->leQQNum->removeItem(index);
        }

        // 2. 如果上一个用户存在且不是新用户，且是有效用户，则将其添加回列表
        if (!m_lastQQ.isEmpty() && m_lastQQ != arg1) {
            // 检查上一个用户是否在映射中
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
                // 检查上一个用户是否已在列表中（避免重复添加）
                if (ui->leQQNum->findText(m_lastQQ) == -1) {
                    ui->leQQNum->addItem(m_lastQQ);
                    // 设置居中对齐
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
        // 手动输入新号码：可根据需求决定是否添加为历史记录
        // 这里不做处理，保持列表不变
    }

    // 更新上一次的QQ号
    m_lastQQ = arg1;
}

