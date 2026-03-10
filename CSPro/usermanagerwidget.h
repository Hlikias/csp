#ifndef USERMANAGERWIDGET_H
#define USERMANAGERWIDGET_H

#include <QWidget>

#include<QStandardItemModel>
#include"NetManager/connectionlog.h"
namespace Ui {
class UserManagerWidget;
}

class UserManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserManagerWidget(QWidget *parent = nullptr);
    ~UserManagerWidget();

public slots:
           // 设置连接日志数据到表格
    void setConnectionLogs(const QList<ConnectionLog*>& logs);
    // 设置处理数据到表格
    void setProcessedData(const QList<ProcessedDataEntity*>& data);

private:

    void init();
private:
    Ui::UserManagerWidget *ui;
    // 可添加模型成员，如 QSqlTableModel 或 QStandardItemModel
    QStandardItemModel *m_connModel;
    QStandardItemModel *m_dataModel;
};

#endif // USERMANAGERWIDGET_H
