/********************************************************************************
** Form generated from reading UI file 'dashboardwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DASHBOARDWIDGET_H
#define UI_DASHBOARDWIDGET_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DashboardWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QFormLayout *formLayout;
    QLabel *cpuCard;
    QLabel *memoryCard;
    QLabel *port;
    QLabel *diskCard;
    QLabel *thread;
    QLabel *netCard;
    QLabel *connect;
    QLabel *host;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_4;
    QGridLayout *gridLayout;
    QLabel *timeOut;
    QLabel *upPath;
    QLabel *ency;
    QLabel *maxLog;
    QLabel *maxUser;
    QLabel *logPath;
    QLabel *buffSize;
    QLabel *Guest;
    QLabel *logLevel;
    QLabel *maxMemery;
    QLabel *refalshLog;
    QLabel *downLoad;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnStart;
    QPushButton *btnRefresh;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_3;
    QChartView *memoryChartView;
    QChartView *cpuChartView;
    QChartView *trafficChartView;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout_7;
    QListWidget *recentActivityList;
    QHBoxLayout *horizontalLayout_6;
    QLabel *diskFreeLabel;
    QSpacerItem *horizontalSpacer;
    QProgressBar *diskFreeBar;

    void setupUi(QWidget *DashboardWidget)
    {
        if (DashboardWidget->objectName().isEmpty())
            DashboardWidget->setObjectName("DashboardWidget");
        DashboardWidget->resize(1193, 696);
        verticalLayout = new QVBoxLayout(DashboardWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        groupBox = new QGroupBox(DashboardWidget);
        groupBox->setObjectName("groupBox");
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName("horizontalLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        cpuCard = new QLabel(groupBox);
        cpuCard->setObjectName("cpuCard");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, cpuCard);

        memoryCard = new QLabel(groupBox);
        memoryCard->setObjectName("memoryCard");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, memoryCard);

        port = new QLabel(groupBox);
        port->setObjectName("port");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, port);

        diskCard = new QLabel(groupBox);
        diskCard->setObjectName("diskCard");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, diskCard);

        thread = new QLabel(groupBox);
        thread->setObjectName("thread");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, thread);

        netCard = new QLabel(groupBox);
        netCard->setObjectName("netCard");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, netCard);

        connect = new QLabel(groupBox);
        connect->setObjectName("connect");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, connect);

        host = new QLabel(groupBox);
        host->setObjectName("host");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, host);


        horizontalLayout->addLayout(formLayout);


        horizontalLayout_5->addWidget(groupBox);

        groupBox_3 = new QGroupBox(DashboardWidget);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout_4 = new QHBoxLayout(groupBox_3);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        timeOut = new QLabel(groupBox_3);
        timeOut->setObjectName("timeOut");

        gridLayout->addWidget(timeOut, 2, 0, 1, 1);

        upPath = new QLabel(groupBox_3);
        upPath->setObjectName("upPath");

        gridLayout->addWidget(upPath, 2, 2, 1, 1);

        ency = new QLabel(groupBox_3);
        ency->setObjectName("ency");

        gridLayout->addWidget(ency, 1, 2, 1, 1);

        maxLog = new QLabel(groupBox_3);
        maxLog->setObjectName("maxLog");

        gridLayout->addWidget(maxLog, 2, 1, 1, 1);

        maxUser = new QLabel(groupBox_3);
        maxUser->setObjectName("maxUser");

        gridLayout->addWidget(maxUser, 0, 0, 1, 1);

        logPath = new QLabel(groupBox_3);
        logPath->setObjectName("logPath");

        gridLayout->addWidget(logPath, 1, 1, 1, 1);

        buffSize = new QLabel(groupBox_3);
        buffSize->setObjectName("buffSize");

        gridLayout->addWidget(buffSize, 1, 0, 1, 1);

        Guest = new QLabel(groupBox_3);
        Guest->setObjectName("Guest");

        gridLayout->addWidget(Guest, 0, 2, 1, 1);

        logLevel = new QLabel(groupBox_3);
        logLevel->setObjectName("logLevel");

        gridLayout->addWidget(logLevel, 0, 1, 1, 1);

        maxMemery = new QLabel(groupBox_3);
        maxMemery->setObjectName("maxMemery");

        gridLayout->addWidget(maxMemery, 3, 0, 1, 1);

        refalshLog = new QLabel(groupBox_3);
        refalshLog->setObjectName("refalshLog");

        gridLayout->addWidget(refalshLog, 3, 1, 1, 1);

        downLoad = new QLabel(groupBox_3);
        downLoad->setObjectName("downLoad");

        gridLayout->addWidget(downLoad, 3, 2, 1, 1);


        horizontalLayout_4->addLayout(gridLayout);


        horizontalLayout_5->addWidget(groupBox_3);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        btnStart = new QPushButton(DashboardWidget);
        btnStart->setObjectName("btnStart");

        horizontalLayout_2->addWidget(btnStart);

        btnRefresh = new QPushButton(DashboardWidget);
        btnRefresh->setObjectName("btnRefresh");

        horizontalLayout_2->addWidget(btnRefresh);


        verticalLayout->addLayout(horizontalLayout_2);

        groupBox_2 = new QGroupBox(DashboardWidget);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_3 = new QHBoxLayout(groupBox_2);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        memoryChartView = new QChartView(groupBox_2);
        memoryChartView->setObjectName("memoryChartView");

        horizontalLayout_3->addWidget(memoryChartView);

        cpuChartView = new QChartView(groupBox_2);
        cpuChartView->setObjectName("cpuChartView");

        horizontalLayout_3->addWidget(cpuChartView);

        trafficChartView = new QChartView(groupBox_2);
        trafficChartView->setObjectName("trafficChartView");

        horizontalLayout_3->addWidget(trafficChartView);


        verticalLayout->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(DashboardWidget);
        groupBox_4->setObjectName("groupBox_4");
        horizontalLayout_7 = new QHBoxLayout(groupBox_4);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        recentActivityList = new QListWidget(groupBox_4);
        recentActivityList->setObjectName("recentActivityList");

        horizontalLayout_7->addWidget(recentActivityList);


        verticalLayout->addWidget(groupBox_4);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        diskFreeLabel = new QLabel(DashboardWidget);
        diskFreeLabel->setObjectName("diskFreeLabel");

        horizontalLayout_6->addWidget(diskFreeLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        diskFreeBar = new QProgressBar(DashboardWidget);
        diskFreeBar->setObjectName("diskFreeBar");
        diskFreeBar->setValue(24);

        horizontalLayout_6->addWidget(diskFreeBar);


        verticalLayout->addLayout(horizontalLayout_6);


        retranslateUi(DashboardWidget);

        QMetaObject::connectSlotsByName(DashboardWidget);
    } // setupUi

    void retranslateUi(QWidget *DashboardWidget)
    {
        DashboardWidget->setWindowTitle(QCoreApplication::translate("DashboardWidget", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("DashboardWidget", "\346\234\215\345\212\241\345\231\250\345\217\202\346\225\260", nullptr));
        cpuCard->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        memoryCard->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        port->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        diskCard->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        thread->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        netCard->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        connect->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        host->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("DashboardWidget", "\346\234\215\345\212\241\345\231\250\351\205\215\347\275\256", nullptr));
        timeOut->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        upPath->setText(QCoreApplication::translate("DashboardWidget", "upPath", nullptr));
        ency->setText(QCoreApplication::translate("DashboardWidget", "ency", nullptr));
        maxLog->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        maxUser->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        logPath->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        buffSize->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        Guest->setText(QCoreApplication::translate("DashboardWidget", "Guest", nullptr));
        logLevel->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        maxMemery->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        refalshLog->setText(QCoreApplication::translate("DashboardWidget", "TextLabel", nullptr));
        downLoad->setText(QCoreApplication::translate("DashboardWidget", "downLoad", nullptr));
        btnStart->setText(QCoreApplication::translate("DashboardWidget", "\345\220\257\345\212\250\346\234\215\345\212\241\345\231\250", nullptr));
        btnRefresh->setText(QCoreApplication::translate("DashboardWidget", "\351\207\215\345\220\257\346\234\215\345\212\241\345\231\250", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("DashboardWidget", "\347\263\273\347\273\237\346\200\247\350\203\275", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("DashboardWidget", "\346\227\245\345\277\227", nullptr));
        diskFreeLabel->setText(QCoreApplication::translate("DashboardWidget", "\345\275\223\345\211\215\345\255\230\345\202\250\347\251\272\351\227\264\345\211\251\344\275\231\357\274\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DashboardWidget: public Ui_DashboardWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DASHBOARDWIDGET_H
