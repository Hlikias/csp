// Common/qsshelper.cpp
#include "qsshelper.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QApplication>
#include <QWidget>

// ==================== 静态辅助函数 ====================
static QString readQssFileInternal(const QString &qssPath, bool *ok = nullptr)
{
    QFile file(qssPath);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "[ERROR] 打开QSS文件失败：" << file.errorString() << " Path:" << qssPath;
        if (ok) *ok = false;
        return QString();
    }

    QTextStream stream(&file);
    QString qss = stream.readAll();
    file.close();

    if (ok) *ok = true;
    return qss;
}

static bool applyStyleInternal(QWidget *widget, const QString &qss, bool debug = false)
{
    if (!widget) {
        qDebug() << "[ERROR] 窗口对象为空！";
        return false;
    }

    widget->setStyleSheet(qss);

    if (widget->styleSheet().isEmpty() && !qss.isEmpty()) {
        if (debug) {
            qDebug() << "[ERROR] QSS解析失败！";

            // 逐段测试：按空行分割，分别应用以定位错误段
            QStringList sections = qss.split("\n\n");
            for (int i = 0; i < sections.size(); i++) {
                QString section = sections[i].trimmed();
                if (section.isEmpty()) continue;

                widget->setStyleSheet(section);
                if (widget->styleSheet().isEmpty()) {
                    qDebug() << "[ERROR] 第" << i+1 << "段样式解析失败：\n" << section;
                }
                widget->setStyleSheet("");
            }
        }
        return false;
    }

    return true;
}

// ==================== QSSHelper 实现 ====================

QSSHelper::QSSHelper(QObject *parent) : QObject(parent)
{
}

QSSHelper::~QSSHelper()
{
}

QSSHelper& QSSHelper::instance()
{
    static QSSHelper helper;
    return helper;
}

// 静态局部样式加载
bool QSSHelper::loadLocalQss(QWidget *widget, const QString &qssPath, bool debug)
{
    bool ok;
    QString qss = readQssFileInternal(qssPath, &ok);
    if (!ok) return false;

    bool success = applyStyleInternal(widget, qss, debug);

    if (success) {
        qDebug() << "[SUCCESS] 局部QSS解析并应用成功！ 窗口:" << widget->objectName();
    }

    return success;
}

// 静态样式片段测试
bool QSSHelper::testStyleSection(const QString &section)
{
    qApp->setStyleSheet(section);
    bool success = !qApp->styleSheet().isEmpty();
    qApp->setStyleSheet("");
    return success;
}

bool QSSHelper::testStyleOnWidget(QWidget *widget, const QString &section)
{
    if (!widget) return false;
    widget->setStyleSheet(section);
    bool success = !widget->styleSheet().isEmpty();
    widget->setStyleSheet("");
    return success;
}

// 非静态全局样式加载
bool QSSHelper::loadGlobalQss(const QString &qssPath)
{
    bool ok;
    QString qss = readQssFileInternal(qssPath, &ok);
    if (!ok) return false;

    qApp->setStyleSheet(qss);
    m_globalStyle = qss;

    QString appliedQss = qApp->styleSheet();
    if (appliedQss.isEmpty() && !qss.isEmpty()) {
        qDebug() << "[ERROR] 全局QSS解析失败！";
        return false;
    }

    qDebug() << "[SUCCESS] 全局QSS解析并应用成功！";
    emit globalStyleChanged(m_globalStyle);
    return true;
}

// 获取当前全局样式
QString QSSHelper::currentGlobalStyle() const
{
    return m_globalStyle;
}
