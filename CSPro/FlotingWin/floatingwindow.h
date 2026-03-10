#ifndef FLOATINGWINDOW_H
#define FLOATINGWINDOW_H

#include <QObject>
#include <QQmlEngine>
#include "floatingwindow_global.h"

class QQuickView;

class FLOTINGWIN_EXPORT FloatingWindow : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
    explicit FloatingWindow(QObject *parent = nullptr);
    ~FloatingWindow();

    bool visible() const;
    void setVisible(bool visible);

signals:
    void visibleChanged();
    void doubleClicked();  // 双击信号，供主界面捕获

public slots:
    void onWindowDoubleClicked();

private:
    QQuickView *m_view;
    bool m_visible;
};

#endif // FLOATINGWINDOW_H
