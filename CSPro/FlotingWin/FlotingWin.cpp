#include "floatingwindow.h"
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScreen>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QQmlComponent>

// 自定义事件过滤器，用于捕获双击
class DoubleClickFilter : public QObject
{
    Q_OBJECT
public:
    DoubleClickFilter(FloatingWindow *win) : m_win(win) {}
protected:
    bool eventFilter(QObject *obj, QEvent *event) override
    {
        if (event->type() == QEvent::MouseButtonDblClick) {
            m_win->onWindowDoubleClicked();
        }
        return QObject::eventFilter(obj, event);
    }
private:
    FloatingWindow *m_win;
};

FloatingWindow::FloatingWindow(QObject *parent)
    : QObject(parent)
    , m_view(nullptr)
    , m_visible(false)
{
    m_view = new QQuickView;
    m_view->setFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_view->setResizeMode(QQuickView::SizeRootObjectToView);
    m_view->setSource(QUrl("qrc:/FlotingWin/Main.qml"));  // 注意资源路径

    // 安装事件过滤器以捕获双击
    m_view->installEventFilter(new DoubleClickFilter(this));

    // 连接可见性变化
    connect(m_view, &QQuickView::visibleChanged, this, [this](){
        if (m_visible != m_view->isVisible()) {
            m_visible = m_view->isVisible();
            emit visibleChanged();
        }
    });

    // 可选：将自身暴露给 QML 上下文
    m_view->rootContext()->setContextProperty("floatingWindow", this);
}

FloatingWindow::~FloatingWindow()
{
    delete m_view;
}

bool FloatingWindow::visible() const
{
    return m_visible;
}

void FloatingWindow::setVisible(bool visible)
{
    if (m_visible == visible) return;
    m_visible = visible;
    m_view->setVisible(visible);
    if (visible) {
        // 可以设置默认位置，例如屏幕右下角
        QScreen *screen = QGuiApplication::primaryScreen();
        if (screen) {
            QRect geo = screen->availableGeometry();
            m_view->setPosition(geo.right() - m_view->width(), geo.bottom() - m_view->height());
        }
    }
    emit visibleChanged();
}

void FloatingWindow::onWindowDoubleClicked()
{
    emit doubleClicked();
}

#include "floatingwindow.moc"
