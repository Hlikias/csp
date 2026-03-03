#ifndef SERVERSETTING_H
#define SERVERSETTING_H

#include <QWidget>
#include <QFile>

#include <QFileInfo>
#include <QValidator>
#include <QRegularExpression>
namespace Ui {
class ServerSetting;
}
class IPValidator : public QValidator
{
    Q_OBJECT
public:
    explicit IPValidator(QObject *parent = nullptr);

    // 重写 validate 函数，返回当前输入的状态
    State validate(QString &input, int &pos) const override;

    // 重写 fixup 函数，当输入失去焦点时自动修正
    void fixup(QString &input) const override;
};

class ServerSetting : public QWidget
{
    Q_OBJECT

public:
    explicit ServerSetting(QWidget *parent = nullptr);
    ~ServerSetting();

private:
    void loadLocalQss(const QString &qssPath);
    void init();
    void initUI();

    bool eventFilter(QObject *obj, QEvent *event);
private:
    Ui::ServerSetting *ui;
private:
    IPValidator *validator;
};



#endif // SERVERSETTING_H
