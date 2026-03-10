// Common/qsshelper.h
#ifndef QSSHELPER_H
#define QSSHELPER_H

#include <QObject>
#include <QString>
#include <QWidget>

class QSSHelper : public QObject
{
    Q_OBJECT

public:
    // 单例模式（用于需要信号的功能）
    static QSSHelper& instance();

    // 全局样式 - 非静态（需要信号）
    bool loadGlobalQss(const QString &qssPath);
    QString currentGlobalStyle() const;

    // 局部样式 - 静态函数
    static bool loadLocalQss(QWidget *widget, const QString &qssPath, bool debug = true);

    // 测试工具 - 静态函数
    static bool testStyleSection(const QString &section);
    static bool testStyleOnWidget(QWidget *widget, const QString &section);

signals:
    void globalStyleChanged(const QString &newStyle);

private:
    explicit QSSHelper(QObject *parent = nullptr);
    ~QSSHelper();
    Q_DISABLE_COPY(QSSHelper)

    QString m_globalStyle;
};

#endif // QSSHELPER_H
