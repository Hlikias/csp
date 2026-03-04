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
#include <QtWidgets/QButtonGroup>
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
    QLabel *titleLabel;
    QPushButton *mainBtn;
    QPushButton *userBtn;
    QPushButton *settingBtn;
    QPushButton *serviceBtn;
    QPushButton *logBtn;
    QPushButton *monitoringBtn;
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
    QPushButton *minBtn;
    QPushButton *closeBtn;
    QStackedWidget *stackedWidget;
    QButtonGroup *buttonGroup;

    void setupUi(QWidget *serverwidget)
    {
        if (serverwidget->objectName().isEmpty())
            serverwidget->setObjectName("serverwidget");
        serverwidget->resize(696, 580);
        horizontalLayout_3 = new QHBoxLayout(serverwidget);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        titleLabel = new QLabel(serverwidget);
        titleLabel->setObjectName("titleLabel");

        verticalLayout->addWidget(titleLabel);

        mainBtn = new QPushButton(serverwidget);
        buttonGroup = new QButtonGroup(serverwidget);
        buttonGroup->setObjectName("buttonGroup");
        buttonGroup->addButton(mainBtn);
        mainBtn->setObjectName("mainBtn");
        mainBtn->setStyleSheet(QString::fromUtf8(""));
        mainBtn->setCheckable(true);

        verticalLayout->addWidget(mainBtn);

        userBtn = new QPushButton(serverwidget);
        buttonGroup->addButton(userBtn);
        userBtn->setObjectName("userBtn");
        userBtn->setStyleSheet(QString::fromUtf8(""));
        userBtn->setCheckable(true);

        verticalLayout->addWidget(userBtn);

        settingBtn = new QPushButton(serverwidget);
        buttonGroup->addButton(settingBtn);
        settingBtn->setObjectName("settingBtn");
        settingBtn->setStyleSheet(QString::fromUtf8(""));
        settingBtn->setCheckable(true);

        verticalLayout->addWidget(settingBtn);

        serviceBtn = new QPushButton(serverwidget);
        buttonGroup->addButton(serviceBtn);
        serviceBtn->setObjectName("serviceBtn");
        serviceBtn->setCheckable(true);

        verticalLayout->addWidget(serviceBtn);

        logBtn = new QPushButton(serverwidget);
        buttonGroup->addButton(logBtn);
        logBtn->setObjectName("logBtn");
        logBtn->setCheckable(true);

        verticalLayout->addWidget(logBtn);

        monitoringBtn = new QPushButton(serverwidget);
        buttonGroup->addButton(monitoringBtn);
        monitoringBtn->setObjectName("monitoringBtn");
        monitoringBtn->setCheckable(true);

        verticalLayout->addWidget(monitoringBtn);

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
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(-1, 5, 2, -1);
        label = new QLabel(serverwidget);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        minBtn = new QPushButton(serverwidget);
        minBtn->setObjectName("minBtn");

        horizontalLayout_2->addWidget(minBtn);

        closeBtn = new QPushButton(serverwidget);
        closeBtn->setObjectName("closeBtn");

        horizontalLayout_2->addWidget(closeBtn);


        verticalLayout_2->addLayout(horizontalLayout_2);

        stackedWidget = new QStackedWidget(serverwidget);
        stackedWidget->setObjectName("stackedWidget");

        verticalLayout_2->addWidget(stackedWidget);


        horizontalLayout_3->addLayout(verticalLayout_2);


        retranslateUi(serverwidget);

        QMetaObject::connectSlotsByName(serverwidget);
    } // setupUi

    void retranslateUi(QWidget *serverwidget)
    {
        serverwidget->setWindowTitle(QCoreApplication::translate("serverwidget", "Form", nullptr));
        titleLabel->setText(QCoreApplication::translate("serverwidget", "Title", nullptr));
        mainBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        userBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        settingBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        serviceBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        logBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        monitoringBtn->setText(QCoreApplication::translate("serverwidget", "PushButton", nullptr));
        pushButton_4->setText(QString());
        pushButton_5->setText(QString());
        pushButton_6->setText(QString());
        label->setText(QCoreApplication::translate("serverwidget", "\346\234\215\345\212\241\345\231\250\346\234\252\345\220\257\345\212\250", nullptr));
        minBtn->setText(QString());
        closeBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class serverwidget: public Ui_serverwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERWIDGET_H
