#include "serversetting.h"
#include "ui_serversetting.h"

#include <QDebug>
#include <QFileDialog>
ServerSetting::ServerSetting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerSetting)
{
    ui->setupUi(this);
    initUI();
    init();
}

ServerSetting::~ServerSetting()
{
    delete ui;

}

QMap<QString, serverConfig> ServerSetting::getServerConfig()
{
    return this->serversMap;
}

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
    param.m_messageRetentionDays = 30;
    return this->param;
}
void ServerSetting::initUI(){
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    serversMap = xml_mgr.getServerInfoMap();
    serverConfig server = serversMap["main"];
    validator = new IPValidator(this);
    ui->host->setValidator(validator);
    ui->host->setText(server.host);
    QIntValidator *validator = new QIntValidator(0, 65535, this);
    ui->port->setValidator(validator);
    ui->port->setMaxLength(5); // 最多三位数
    ui->port->setText(QString::number(server.port));

    foreach (int item, server.maxConnectionsOptions) {
        ui->maxConnections->addItem(QString::number(item));
    }
    foreach (int item, server.workerThreadsOptions) {
        ui->workerThreads->addItem(QString::number(item));
    }
    foreach (int item, server.timeoutOptions) {
        ui->connectionTimeout->addItem(QString::number(item));
    }
    validator = new QIntValidator(0, 99999, this);
    ui->maxUsers->setValidator(validator);
    ui->maxUsers->setText(QString::number(server.maxUsers));
    validator = new QIntValidator(0, 9999999, this);
    //ui->bufferSize->installEventFilter(this);
    ui->bufferSize->setValidator(validator);
    ui->bufferSize->setText(QString::number(server.bufferSize));
    ui->sessionTimeout->setText(QString::number(server.sessionTimeout));
    ui->memoryLimitMB->setValidator(validator);
    ui->memoryLimitMB->setText(QString::number(server.memoryLimitMB));
    ui->localLog->setChecked(true);
    ui->enableLog->setChecked(true);
    connect(ui->localLog, &QCheckBox::toggled, this, [=](bool checked) {

        if(ui->enableLog->isChecked()){
            ui->logBtn->setEnabled(checked);
            ui->logPath->setEnabled(checked);
            ui->localLog->setEnabled(checked);
        }
    });
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
    // 连接按钮点击信号

    connect(ui->logBtn, &QPushButton::clicked, this, [this]() {
        QString dirPath = QFileDialog::getExistingDirectory(this, "选择目录");
        if (!dirPath.isEmpty()) {
            ui->logPath->setText(dirPath);
        }
    });
    ui->filemaxSize->setText(QString::number(server.logMaxSize));
    ui->maxBackups->setRange(1, 99);
    ui->maxBackups->setValue(30);         // 当前值设为 30
    foreach (QString item, server.allowGuestOptions) {
        ui->allowGuest->addItem(item);
    }
    ui->encryptData->addItem(server.encryptData);
    ui->downloadPath->setText(server.downloadPath);
    ui->uploadPath->setText(server.uploadPath);
    ui->disable->setEnabled(false);
}

void ServerSetting::init(){
    xmlmanager& xml_mgr = xmlmanager::getinstance();
    QString qssPath = xml_mgr.get_string("Style", "SettingQssPath");
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
}
// 封装加载QSS的函数（简单易用）
void ServerSetting::loadLocalQss(const QString &qssPath) {
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

bool ServerSetting::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->bufferSize) {
        if (event->type() == QEvent::FocusIn) {
            QString text = ui->bufferSize->text();
            // 如果文本以"MB"结尾，去除它
            if (text.endsWith("MB", Qt::CaseInsensitive)) {
                text.chop(2); // 去掉"MB"
                text = text.trimmed();
                ui->bufferSize->setText(text);
            }
        } else if (event->type() == QEvent::FocusOut) {
            QString text = ui->bufferSize->text().trimmed();
            if (!text.isEmpty()) {
                // 可以验证是否为数字，如果不是则恢复上次有效值或清空
                bool ok;
                double val = text.toDouble(&ok);
                if (ok) {
                    // 如果是数字，重新设置为带单位的文本
                    ui->bufferSize->setText(QString::number(val) + " MB");
                } else {
                    // 非法输入，可以清空或保留原样，但为了友好，可以提示
                    // 这里简单清空
                    ui->bufferSize->clear();
                }
            }
        }
    }
    return ServerSetting::eventFilter(obj, event);
}

IPValidator::IPValidator(QObject *parent)
    : QValidator(parent)
{
}

QValidator::State IPValidator::validate(QString &input, int &pos) const
{
    // 允许空字符串
    if (input.isEmpty()) {
        return Intermediate;
    }

    // 正则表达式：允许输入数字和点，格式大致为 "数字.数字.数字.数字"
    // 每个数字部分最多三位，点可选，总长度最多15位（xxx.xxx.xxx.xxx）
    QRegularExpression rx("^(\\d{0,3}\\.?){0,4}$");
    if (!rx.match(input).hasMatch()) {
        return Invalid;
    }

    // 分割成段
    QStringList parts = input.split('.');
    if (parts.size() > 4) {
        return Invalid; // 超过四段
    }

    // 检查每个段的数值范围
    for (int i = 0; i < parts.size(); ++i) {
        const QString &part = parts.at(i);
        if (part.isEmpty()) {
            // 空的段（例如 "192..168"）允许中间状态，但最终必须填满
            continue;
        }

        bool ok;
        int val = part.toInt(&ok);
        if (!ok) {
            return Invalid; // 非数字字符（虽然正则已经过滤，但保险）
        }

        // 检查数值范围 0-255
        if (val < 0 || val > 255) {
            return Invalid;
        }

        // 检查是否有前导零（例如 "01"），且数值不为零本身
        if (part.length() > 1 && part.startsWith('0')) {
            // 允许 "0" 本身，但不允许 "01"
            return Intermediate; // 可以提示用户修正，但暂时接受
        }
    }

    // 如果已经有四段且最后一段非空，则为可接受状态
    if (parts.size() == 4 && !parts.last().isEmpty()) {
        return Acceptable;
    }

    // 其他情况（如只有三段，或最后一段空）为中间状态
    return Intermediate;
}

void IPValidator::fixup(QString &input) const
{
    // 去除所有空格
    input = input.simplified();

    // 如果输入为空，无需修正
    if (input.isEmpty()) {
        return;
    }

    // 分割成段
    QStringList parts = input.split('.');
    // 去除空段（例如 "192..168" -> ["192","168"]）
    parts.removeAll("");

    // 修正每个段的前导零，例如 "01" -> "1"，但保留 "0"
    for (int i = 0; i < parts.size(); ++i) {
        QString &part = parts[i];
        if (part.length() > 1 && part.startsWith('0')) {
            // 转换为整数再转回字符串，自动去除前导零
            bool ok;
            int val = part.toInt(&ok);
            if (ok) {
                part = QString::number(val);
            }
        }
    }

    // 如果段数不足4，用空字符串补足，例如 ["192","168"] -> ["192","168","",""]
    while (parts.size() < 4) {
        parts.append("");
    }

    // 重新组合，如果某段为空则留空（最后在失去焦点时可能还需要进一步处理）
    input = parts.join('.');
}

