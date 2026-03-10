#include "serversetting.h"
#include "ui_serversetting.h"
#include "qsshelper.h"
#include <QDebug>
#include <QFileDialog>

/**
 * @brief ServerSetting 构造函数
 * @param parent 父窗口指针
 *
 * 初始化 UI，调用 initUI() 和 init() 分别完成界面初始化和样式加载。
 */
ServerSetting::ServerSetting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerSetting)
{
    ui->setupUi(this);
    initUI();   // 初始化控件值、验证器、信号连接等
    init();     // 加载 QSS 样式
}

/**
 * @brief 析构函数，释放 UI 资源
 */
ServerSetting::~ServerSetting()
{
    delete ui;
}

/**
 * @brief 获取服务器配置映射表
 * @return QMap<QString, serverConfig> 存储了所有服务器配置的映射
 *
 * 该函数返回成员变量 serversMap，该映射在 initUI 中从 XML 加载。
 */
QMap<QString, serverConfig> ServerSetting::getServerConfig()
{
    return this->serversMap;
}

/**
 * @brief 获取当前 UI 中设置的服务器参数
 * @return serverParam 结构体，包含所有配置项
 *
 * 从各个输入控件读取当前值，组装成 serverParam 返回。
 * 注意：部分字段（如 m_messageRetentionDays）被硬编码为 30，可根据需要调整。
 */
serverParam ServerSetting::get_ServerParam()
{
    param.m_host = ui->host->text();
    param.m_port = ui->port->text().toUInt();
    param.m_maxConnectionsOptions = ui->maxConnections->currentText().toUInt();
    param.m_workerThreadsOptions = ui->workerThreads->currentText().toUInt();
    param.m_timeoutOptions = ui->connectionTimeout->currentText().toUInt();
    param.m_maxUsers = ui->maxUsers->text().toUInt();
    param.m_bufferSize = ui->bufferSize->text().toUInt();
    param.m_sessionTimeout = ui->sessionTimeout->text().toUInt();
    param.m_memoryLimitMB = ui->memoryLimitMB->text().toUInt();
    param.m_logLevelOptions = ui->logLevel->currentText();
    param.m_logFile = ui->logPath->text();
    param.m_logMaxSize = ui->filemaxSize->text().toUInt();
    param.m_logMaxBackups = ui->maxBackups->text().toUInt();
    param.m_allowGuestOptions = ui->allowGuest->currentText();
    param.m_encryptData = ui->encryptData->currentText();
    param.m_uploadPath = ui->uploadPath->text();
    param.m_downloadPath = ui->downloadPath->text();
    param.m_messageRetentionDays = 30;   // 固定值，可根据需要改为可配置
    return this->param;
}

/**
 * @brief 初始化界面控件
 *
 * 从 XML 加载服务器配置（serversMap["main"]），并设置：
 * - IP 输入框的验证器（自定义 IPValidator）
 * - 端口输入框的整数验证器（0-65535）
 * - 下拉框选项（连接数、工作线程、超时等）
 * - 整数输入框的验证器
 * - 复选框的联动逻辑（日志相关）
 * - 日志路径选择按钮的槽函数
 * - 其他文本输入框的默认值
 *
 * 注意：serversMap 中存储了多套配置，此处默认使用 "main" 键对应的配置。
 */
void ServerSetting::initUI(){
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    serversMap = xml_mgr.getServerInfoMap();   // 获取所有服务器配置
    serverConfig server = serversMap["main"];  // 使用主配置

    // ---- 网络设置 ----
    validator = new IPValidator(this);
    ui->host->setValidator(validator);
    ui->host->setText(server.host);

    QIntValidator *validator = new QIntValidator(0, 65535, this);
    ui->port->setValidator(validator);
    ui->port->setMaxLength(5); // 端口最大 5 位
    ui->port->setText(QString::number(server.port));

    // ---- 性能选项（下拉框）----
    foreach (int item, server.maxConnectionsOptions) {
        ui->maxConnections->addItem(QString::number(item));
    }
    foreach (int item, server.workerThreadsOptions) {
        ui->workerThreads->addItem(QString::number(item));
    }
    foreach (int item, server.timeoutOptions) {
        ui->connectionTimeout->addItem(QString::number(item));
    }

    // ---- 用户和资源限制 ----
    validator = new QIntValidator(0, 99999, this);
    ui->maxUsers->setValidator(validator);
    ui->maxUsers->setText(QString::number(server.maxUsers));

    validator = new QIntValidator(0, 9999999, this);
    // ui->bufferSize->installEventFilter(this);   // 原计划安装事件过滤器处理单位，但被注释掉
    ui->bufferSize->setValidator(validator);
    ui->bufferSize->setText(QString::number(server.bufferSize));

    ui->sessionTimeout->setText(QString::number(server.sessionTimeout));
    ui->memoryLimitMB->setValidator(validator);
    ui->memoryLimitMB->setText(QString::number(server.memoryLimitMB));

    // ---- 日志设置 ----
    ui->localLog->setChecked(true);    // 默认启用本地日志
    ui->enableLog->setChecked(true);   // 默认启用日志

    // 日志复选框联动：当 localLog 勾选时，根据 enableLog 状态控制日志路径等是否可用
    connect(ui->localLog, &QCheckBox::toggled, this, [=](bool checked) {
        if(ui->enableLog->isChecked()){
            ui->logBtn->setEnabled(checked);
            ui->logPath->setEnabled(checked);
            ui->localLog->setEnabled(checked);  // 注意：这里会导致递归？但无信号循环，因为 toggled 触发时 checked 已变
        }
    });

    // 总日志开关：控制所有日志相关控件的可用性
    connect(ui->enableLog, &QCheckBox::toggled, this, [=](bool checked) {
        ui->logLevel->setEnabled(checked);
        ui->logBtn->setEnabled(checked);
        ui->logPath->setEnabled(checked);
        ui->filemaxSize->setEnabled(checked);
        ui->maxBackups->setEnabled(checked);
    });

    foreach (QString item, server.logLevelOptions) {
        ui->logLevel->addItem(item);
    }
    ui->logPath->setText(server.logFile);

    // 日志路径选择按钮：打开目录选择对话框
    connect(ui->logBtn, &QPushButton::clicked, this, [this]() {
        QString dirPath = QFileDialog::getExistingDirectory(this, "选择目录");
        if (!dirPath.isEmpty()) {
            ui->logPath->setText(dirPath);
        }
    });

    ui->filemaxSize->setText(QString::number(server.logMaxSize));
    ui->maxBackups->setRange(1, 99);
    ui->maxBackups->setValue(30);   // 默认保留 30 个备份

    // ---- 其他选项 ----
    foreach (QString item, server.allowGuestOptions) {
        ui->allowGuest->addItem(item);
    }
    ui->encryptData->addItem(server.encryptData);
    ui->downloadPath->setText(server.downloadPath);
    ui->uploadPath->setText(server.uploadPath);

    ui->disable->setEnabled(false);   // 可能用于禁用某个功能，当前未使用
}

/**
 * @brief 初始化样式
 *
 * 从 XML 配置中读取 QSS 文件路径，加载并应用到当前窗口。
 * 支持相对路径（相对于应用程序目录）。
 */
void ServerSetting::init(){
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "SettingQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        QSSHelper::loadLocalQss(this, qssPath);
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
}


/**
 * @brief 事件过滤器，用于处理 bufferSize 输入框的焦点事件
 * @param obj 事件源对象
 * @param event 事件
 * @return true 表示事件已处理，否则返回基类处理
 *
 * 当前代码中虽未安装事件过滤器（对应行被注释），但保留此函数供后续启用。
 * 功能：
 * - 获得焦点时，若文本以 "MB" 结尾，则移除单位，便于编辑。
 * - 失去焦点时，若文本为有效数字，则附加 " MB" 单位；否则清空输入。
 */
bool ServerSetting::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->bufferSize) {
        if (event->type() == QEvent::FocusIn) {
            QString text = ui->bufferSize->text();
            if (text.endsWith("MB", Qt::CaseInsensitive)) {
                text.chop(2); // 去掉 "MB"
                text = text.trimmed();
                ui->bufferSize->setText(text);
            }
        } else if (event->type() == QEvent::FocusOut) {
            QString text = ui->bufferSize->text().trimmed();
            if (!text.isEmpty()) {
                bool ok;
                double val = text.toDouble(&ok);
                if (ok) {
                    ui->bufferSize->setText(QString::number(val) + " MB");
                } else {
                    ui->bufferSize->clear();
                }
            }
        }
    }
    return ServerSetting::eventFilter(obj, event);
}

// ---------- IPValidator 实现 ----------

/**
 * @brief 自定义 IP 地址输入验证器，用于实时验证用户输入
 */
IPValidator::IPValidator(QObject *parent)
    : QValidator(parent)
{
}

/**
 * @brief 验证 IP 输入字符串的当前状态
 * @param input 当前输入的字符串（可修改）
 * @param pos 光标位置（可修改，此处未使用）
 * @return 状态：Acceptable（完整有效）、Intermediate（部分有效）、Invalid（无效）
 *
 * 验证规则：
 * - 允许空字符串（Intermediate）
 * - 整体格式必须符合 "数字.数字.数字.数字" 的雏形，每个数字 0-255
 * - 不允许前导零（如 "01"），但 "0" 本身允许
 * - 点号个数不能超过 3，总段数不能超过 4
 */
QValidator::State IPValidator::validate(QString &input, int &pos) const
{
    if (input.isEmpty()) {
        return Intermediate;   // 空字符串暂时接受
    }

    // 正则表达式：允许数字和点，最多 4 段，每段最多 3 位数字
    QRegularExpression rx("^(\\d{0,3}\\.?){0,4}$");
    if (!rx.match(input).hasMatch()) {
        return Invalid;
    }

    QStringList parts = input.split('.');
    if (parts.size() > 4) {
        return Invalid;   // 超过四段
    }

    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts.at(i);
        if (part.isEmpty()) {
            continue;     // 空段允许，属于中间状态
        }

        bool ok;
        int val = part.toInt(&ok);
        if (!ok) {
            return Invalid;   // 非数字（但正则已过滤，此处为安全）
        }

        if (val < 0 || val > 255) {
            return Invalid;   // 数值超出范围
        }

        // 检查前导零：如果长度大于1且以0开头（且不是单个0），视为中间状态（可提示修正）
        if (part.length() > 1 && part.startsWith('0')) {
            return Intermediate;
        }
    }

    // 如果已经有四段且最后一段非空，则完整有效
    if (parts.size() == 4 && !parts.last().isEmpty()) {
        return Acceptable;
    }

    return Intermediate;   // 不足四段或最后一段为空
}

/**
 * @brief 修正输入字符串，使其尽可能符合有效 IP 格式
 * @param input 待修正的字符串（引用）
 *
 * 在失去焦点或接受输入时调用，自动：
 * - 去除所有空格
 * - 去除空段（如 "192..168" 变为 "192.168"）
 * - 去除每个数字段的前导零（如 "01" -> "1"）
 * - 补足四段（不足时补充空段，如 "192.168" -> "192.168.."）
 */
void IPValidator::fixup(QString &input) const
{
    input = input.simplified();   // 去除前后空格和中间多余空格

    if (input.isEmpty()) {
        return;
    }

    QStringList parts = input.split('.');
    parts.removeAll("");   // 去掉空段，例如 "192..168" -> ["192","168"]

    // 去除每个段的前导零
    for (int i = 0; i < parts.size(); ++i) {
        QString &part = parts[i];
        if (part.length() > 1 && part.startsWith('0')) {
            bool ok;
            int val = part.toInt(&ok);
            if (ok) {
                part = QString::number(val);   // 转回数字去除前导零
            }
        }
    }

    // 补足到四段，不足的用空字符串填充
    while (parts.size() < 4) {
        parts.append("");
    }

    input = parts.join('.');
}
