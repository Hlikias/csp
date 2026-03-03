#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QWidget>
#include <QTabBar>
#include <QPainter>
#include <QStyleOptionTab>
namespace Ui {
class serverwidget;
}

class serverwidget : public QWidget
{
    Q_OBJECT

public:
    explicit serverwidget(QWidget *parent = nullptr);
    ~serverwidget();

private:
    Ui::serverwidget *ui;
};

// 自定义TabBar：取消文字旋转，保持水平/或自定义旋转方向
class NoRotateTabBar : public QTabBar
{
protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        for (int i = 0; i < count(); ++i) {
            QStyleOptionTab tabOption;
            initStyleOption(&tabOption, i);

            // 1. 绘制标签背景（保留原生样式）
            style()->drawControl(QStyle::CE_TabBarTab, &tabOption, &painter, this);

            // 2. 绘制文字：取消默认旋转，自定义显示方式
            painter.save();

            QRect textRect = tabRect(i);
            textRect.adjust(5, 5, -5, -5); // 留边距避免文字贴边

            // ========== 方案1：文字完全水平（推荐） ==========
            painter.drawText(textRect, Qt::AlignCenter, tabOption.text);

            // ========== 方案2：文字逆时针旋转90度（可选） ==========
            // painter.translate(textRect.center()); // 移动到文字区域中心
            // painter.rotate(-90); // 逆时针旋转90度（区别于默认的顺时针）
            // painter.drawText(QRect(-textRect.width()/2, -textRect.height()/2,
            //                        textRect.width(), textRect.height()),
            //                 Qt::AlignCenter, tabOption.text);

            painter.restore();
        }
    }

    // 自定义标签大小：适配左侧水平文字
    QSize tabSizeHint(int index) const override
    {
        QSize size = QTabBar::tabSizeHint(index);
        // 宽度足够容纳水平文字，高度适中
        size.setWidth(100);  // 水平文字需要的宽度
        size.setHeight(60);  // 标签高度
        return size;
    }
};

#endif // SERVERWIDGET_H
