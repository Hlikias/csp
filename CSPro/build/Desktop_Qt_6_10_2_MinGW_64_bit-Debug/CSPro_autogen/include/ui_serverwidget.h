/********************************************************************************
** Form generated from reading UI file 'serverwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERWIDGET_H
#define UI_SERVERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_serverwidget
{
public:
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QSpacerItem *verticalSpacer;
    QFrame *line;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QStackedWidget *stackedWidget;
    QWidget *page;
    QWidget *page_2;

    void setupUi(QWidget *serverwidget)
    {
        if (serverwidget->objectName().isEmpty())
            serverwidget->setObjectName("serverwidget");
        serverwidget->resize(696, 580);
        horizontalLayout_3 = new QHBoxLayout(serverwidget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        pushButton_2 = new QPushButton(serverwidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setStyleSheet(QString::fromUtf8(""));

        verticalLayout->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(serverwidget);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setStyleSheet(QString::fromUtf8(""));

        verticalLayout->addWidget(pushButton_3);

        pushButton = new QPushButton(serverwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setStyleSheet(QString::fromUtf8(""));

        verticalLayout->addWidget(pushButton);

        pushButton_7 = new QPushButton(serverwidget);
        pushButton_7->setObjectName("pushButton_7");

        verticalLayout->addWidget(pushButton_7);

        pushButton_8 = new QPushButton(serverwidget);
        pushButton_8->setObjectName("pushButton_8");

        verticalLayout->addWidget(pushButton_8);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        line = new QFrame(serverwidget);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::HLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        verticalLayout->addWidget(line);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton_4 = new QPushButton(serverwidget);
        pushButton_4->setObjectName("pushButton_4");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_4->sizePolicy().hasHeightForWidth());
        pushButton_4->setSizePolicy(sizePolicy);
        pushButton_4->setMinimumSize(QSize(25, 25));
        pushButton_4->setMaximumSize(QSize(25, 25));

        horizontalLayout->addWidget(pushButton_4);

        pushButton_5 = new QPushButton(serverwidget);
        pushButton_5->setObjectName("pushButton_5");
        sizePolicy.setHeightForWidth(pushButton_5->sizePolicy().hasHeightForWidth());
        pushButton_5->setSizePolicy(sizePolicy);
        pushButton_5->setMaximumSize(QSize(25, 25));

        horizontalLayout->addWidget(pushButton_5);

        pushButton_6 = new QPushButton(serverwidget);
        pushButton_6->setObjectName("pushButton_6");
        sizePolicy.setHeightForWidth(pushButton_6->sizePolicy().hasHeightForWidth());
        pushButton_6->setSizePolicy(sizePolicy);
        pushButton_6->setMaximumSize(QSize(25, 25));

        horizontalLayout->addWidget(pushButton_6);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_3->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(serverwidget);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        pushButton_9 = new QPushButton(serverwidget);
        pushButton_9->setObjectName("pushButton_9");

        horizontalLayout_2->addWidget(pushButton_9);

        pushButton_10 = new QPushButton(serverwidget);
        pushButton_10->setObjectName("pushButton_10");

        horizontalLayout_2->addWidget(pushButton_10);


        verticalLayout_2->addLayout(horizontalLayout_2);

        stackedWidget = new QStackedWidget(serverwidget);
        stackedWidget->setObjectName("stackedWidget");
        page = new QWidget();
        page->setObjectName("page");
        stackedWidget->addWidget(page);
        page_2 = new QWidget();
        page_2->setObjectName("page_2");
        stackedWidget->addWidget(page_2);

        verticalLayout_2->addWidget(stackedWidget);


        horizontalLayout_3->addLayout(verticalLayout_2);


        retranslateUi(serverwidget);

        QMetaObject::connectSlotsByName(serverwidget);
    } // setupUi

    void retranslateUi(QWidget *serverwidget)
    {
        serverwidget->setWindowTitle(QCoreApplication::translate("serverwidget", "Form", nullptr));
        pushButton_2->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_3->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_7->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_8->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_4->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_5->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_6->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        label->setText(QCoreApplication::translate("serverwidget", "TextLabel", nullptr));
        pushButton_9->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_10->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class serverwidget: public Ui_serverwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWIDGET_H
