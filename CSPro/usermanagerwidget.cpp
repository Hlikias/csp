#include "usermanagerwidget.h"
#include "ui_usermanagerwidget.h"
#include "XmlManager/xmlmanager.h"
#include <QFileInfo>
#include "qsshelper.h"
UserManagerWidget::UserManagerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserManagerWidget)
{
    ui->setupUi(this);

    init();
}

UserManagerWidget::~UserManagerWidget()
{
    delete ui;
}

void UserManagerWidget::setConnectionLogs(const QList<ConnectionLog *> &logs)
{

    if (!m_connModel) {
        qWarning() << "m_connModel is null!";
        m_connModel = new QStandardItemModel(this);   // 必须 new
        return;
    }
    // 清空模型
    m_connModel->clear();
    m_connModel->setHorizontalHeaderLabels({"ID", "客户端IP", "端口", "协议", "事件类型", "时间"});

    for (auto log : logs) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(log->id()));
        row << new QStandardItem(log->clientIp());
        row << new QStandardItem(QString::number(log->clientPort()));
        row << new QStandardItem(log->protocol());
        row << new QStandardItem(log->eventType());
        row << new QStandardItem(log->createdAt().toString("yyyy-MM-dd hh:mm:ss"));
        m_connModel->appendRow(row);
    }

    ui->userTable->setModel(m_connModel);
}

void UserManagerWidget::setProcessedData(const QList<ProcessedDataEntity *> &data)
{
    if (!m_dataModel) {
        qWarning() << "m_dataModel is null!";
        m_dataModel = new QStandardItemModel(this);   // 必须 new
        return;
    }

    m_dataModel->clear();
    m_dataModel->setHorizontalHeaderLabels({"ID", "原始数据", "解码消息", "发送者IP", "端口", "协议", "时间"});

    for (auto d : data) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::number(d->id()));
        row << new QStandardItem(QString(d->originalData().toHex())); // 原始数据以十六进制显示
        row << new QStandardItem(d->decodedMessage());
        row << new QStandardItem(d->senderIp());
        row << new QStandardItem(QString::number(d->senderPort()));
        row << new QStandardItem(d->protocol());
        row << new QStandardItem(d->createdAt().toString("yyyy-MM-dd hh:mm:ss"));
        m_dataModel->appendRow(row);
    }

    ui->processTable->setModel(m_dataModel);
}


void UserManagerWidget::init()
{
    xmlmanager &xml = xmlmanager::getinstance();
    QString qssPath = xml.get_string("style", "ServerUserManagerQssPath");
    if (!qssPath.isEmpty()) {
        // 处理相对路径：如果路径是相对的，则加上应用程序目录
        QFileInfo info(qssPath);
        if (info.isRelative()) {
            qssPath = QCoreApplication::applicationDirPath() + "/" + qssPath;
        }
        QSSHelper::loadLocalQss(this, qssPath);  // 加载样式到当前窗口
    } else {
        qDebug() << "QSS路径未配置，跳过样式加载";
    }
    m_connModel = new QStandardItemModel(this);   // 必须 new
    m_dataModel = new QStandardItemModel(this);
    // 设置用户表格
    ui->userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->userTable->horizontalHeader()->setStretchLastSection(true);
    // 可选的：初始时根据内容调整列宽
    // ui->userTable->resizeColumnsToContents();

    // 设置处理数据表格
    ui->processTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->processTable->horizontalHeader()->setStretchLastSection(true);
}
