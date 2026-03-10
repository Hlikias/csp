/********************************************************************************
** Form generated from reading UI file 'serversetting.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVERSETTING_H
#define UI_SERVERSETTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ServerSetting
{
public:
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *host;
    QLabel *label_2;
    QLineEdit *port;
    QLabel *label_3;
    QComboBox *maxConnections;
    QLabel *label_4;
    QComboBox *workerThreads;
    QLabel *label_5;
    QComboBox *connectionTimeout;
    QFormLayout *formLayout_2;
    QLineEdit *maxUsers;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QHBoxLayout *horizontalLayout;
    QLineEdit *bufferSize;
    QLabel *label_18;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *sessionTimeout;
    QLabel *label_19;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *memoryLimitMB;
    QLabel *label_20;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QFormLayout *formLayout_3;
    QCheckBox *localLog;
    QCheckBox *enableLog;
    QLabel *label_10;
    QComboBox *logLevel;
    QLabel *label_12;
    QLabel *label_13;
    QSpinBox *maxBackups;
    QLabel *label_11;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *logPath;
    QPushButton *logBtn;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *filemaxSize;
    QPushButton *disable;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout_4;
    QLabel *label_14;
    QComboBox *allowGuest;
    QLabel *label_15;
    QLabel *label_16;
    QLineEdit *uploadPath;
    QLabel *label_17;
    QComboBox *encryptData;
    QLineEdit *downloadPath;

    void setupUi(QWidget *ServerSetting)
    {
        if (ServerSetting->objectName().isEmpty())
            ServerSetting->setObjectName("ServerSetting");
        ServerSetting->resize(632, 518);
        gridLayout_2 = new QGridLayout(ServerSetting);
        gridLayout_2->setObjectName("gridLayout_2");
        groupBox = new QGroupBox(ServerSetting);
        groupBox->setObjectName("groupBox");
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        formLayout = new QFormLayout();
        formLayout->setObjectName("formLayout");
        label = new QLabel(groupBox);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::ItemRole::LabelRole, label);

        host = new QLineEdit(groupBox);
        host->setObjectName("host");

        formLayout->setWidget(0, QFormLayout::ItemRole::FieldRole, host);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");

        formLayout->setWidget(1, QFormLayout::ItemRole::LabelRole, label_2);

        port = new QLineEdit(groupBox);
        port->setObjectName("port");

        formLayout->setWidget(1, QFormLayout::ItemRole::FieldRole, port);

        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");

        formLayout->setWidget(2, QFormLayout::ItemRole::LabelRole, label_3);

        maxConnections = new QComboBox(groupBox);
        maxConnections->setObjectName("maxConnections");

        formLayout->setWidget(2, QFormLayout::ItemRole::FieldRole, maxConnections);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");

        formLayout->setWidget(3, QFormLayout::ItemRole::LabelRole, label_4);

        workerThreads = new QComboBox(groupBox);
        workerThreads->setObjectName("workerThreads");

        formLayout->setWidget(3, QFormLayout::ItemRole::FieldRole, workerThreads);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        formLayout->setWidget(4, QFormLayout::ItemRole::LabelRole, label_5);

        connectionTimeout = new QComboBox(groupBox);
        connectionTimeout->setObjectName("connectionTimeout");

        formLayout->setWidget(4, QFormLayout::ItemRole::FieldRole, connectionTimeout);


        gridLayout->addLayout(formLayout, 0, 0, 1, 1);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName("formLayout_2");
        maxUsers = new QLineEdit(groupBox);
        maxUsers->setObjectName("maxUsers");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::FieldRole, maxUsers);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        formLayout_2->setWidget(0, QFormLayout::ItemRole::LabelRole, label_6);

        label_7 = new QLabel(groupBox);
        label_7->setObjectName("label_7");

        formLayout_2->setWidget(1, QFormLayout::ItemRole::LabelRole, label_7);

        label_8 = new QLabel(groupBox);
        label_8->setObjectName("label_8");

        formLayout_2->setWidget(2, QFormLayout::ItemRole::LabelRole, label_8);

        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");

        formLayout_2->setWidget(3, QFormLayout::ItemRole::LabelRole, label_9);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        bufferSize = new QLineEdit(groupBox);
        bufferSize->setObjectName("bufferSize");

        horizontalLayout->addWidget(bufferSize);

        label_18 = new QLabel(groupBox);
        label_18->setObjectName("label_18");

        horizontalLayout->addWidget(label_18);


        formLayout_2->setLayout(1, QFormLayout::ItemRole::FieldRole, horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        sessionTimeout = new QLineEdit(groupBox);
        sessionTimeout->setObjectName("sessionTimeout");

        horizontalLayout_5->addWidget(sessionTimeout);

        label_19 = new QLabel(groupBox);
        label_19->setObjectName("label_19");

        horizontalLayout_5->addWidget(label_19);


        formLayout_2->setLayout(2, QFormLayout::ItemRole::FieldRole, horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        memoryLimitMB = new QLineEdit(groupBox);
        memoryLimitMB->setObjectName("memoryLimitMB");

        horizontalLayout_6->addWidget(memoryLimitMB);

        label_20 = new QLabel(groupBox);
        label_20->setObjectName("label_20");

        horizontalLayout_6->addWidget(label_20);


        formLayout_2->setLayout(3, QFormLayout::ItemRole::FieldRole, horizontalLayout_6);


        gridLayout->addLayout(formLayout_2, 0, 1, 1, 1);


        gridLayout_2->addWidget(groupBox, 0, 0, 1, 2);

        groupBox_2 = new QGroupBox(ServerSetting);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName("formLayout_3");
        localLog = new QCheckBox(groupBox_2);
        localLog->setObjectName("localLog");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::LabelRole, localLog);

        enableLog = new QCheckBox(groupBox_2);
        enableLog->setObjectName("enableLog");

        formLayout_3->setWidget(0, QFormLayout::ItemRole::FieldRole, enableLog);

        label_10 = new QLabel(groupBox_2);
        label_10->setObjectName("label_10");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::LabelRole, label_10);

        logLevel = new QComboBox(groupBox_2);
        logLevel->setObjectName("logLevel");

        formLayout_3->setWidget(1, QFormLayout::ItemRole::FieldRole, logLevel);

        label_12 = new QLabel(groupBox_2);
        label_12->setObjectName("label_12");

        formLayout_3->setWidget(3, QFormLayout::ItemRole::LabelRole, label_12);

        label_13 = new QLabel(groupBox_2);
        label_13->setObjectName("label_13");

        formLayout_3->setWidget(4, QFormLayout::ItemRole::LabelRole, label_13);

        maxBackups = new QSpinBox(groupBox_2);
        maxBackups->setObjectName("maxBackups");

        formLayout_3->setWidget(4, QFormLayout::ItemRole::FieldRole, maxBackups);

        label_11 = new QLabel(groupBox_2);
        label_11->setObjectName("label_11");

        formLayout_3->setWidget(2, QFormLayout::ItemRole::LabelRole, label_11);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        logPath = new QLineEdit(groupBox_2);
        logPath->setObjectName("logPath");

        horizontalLayout_4->addWidget(logPath);

        logBtn = new QPushButton(groupBox_2);
        logBtn->setObjectName("logBtn");

        horizontalLayout_4->addWidget(logBtn);


        formLayout_3->setLayout(2, QFormLayout::ItemRole::FieldRole, horizontalLayout_4);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        filemaxSize = new QLineEdit(groupBox_2);
        filemaxSize->setObjectName("filemaxSize");

        horizontalLayout_7->addWidget(filemaxSize);

        disable = new QPushButton(groupBox_2);
        disable->setObjectName("disable");

        horizontalLayout_7->addWidget(disable);


        formLayout_3->setLayout(3, QFormLayout::ItemRole::FieldRole, horizontalLayout_7);


        horizontalLayout_2->addLayout(formLayout_3);


        gridLayout_2->addWidget(groupBox_2, 1, 0, 1, 1);

        groupBox_3 = new QGroupBox(ServerSetting);
        groupBox_3->setObjectName("groupBox_3");
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        formLayout_4 = new QFormLayout();
        formLayout_4->setObjectName("formLayout_4");
        label_14 = new QLabel(groupBox_3);
        label_14->setObjectName("label_14");

        formLayout_4->setWidget(0, QFormLayout::ItemRole::LabelRole, label_14);

        allowGuest = new QComboBox(groupBox_3);
        allowGuest->setObjectName("allowGuest");

        formLayout_4->setWidget(0, QFormLayout::ItemRole::FieldRole, allowGuest);

        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName("label_15");

        formLayout_4->setWidget(1, QFormLayout::ItemRole::LabelRole, label_15);

        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName("label_16");

        formLayout_4->setWidget(2, QFormLayout::ItemRole::LabelRole, label_16);

        uploadPath = new QLineEdit(groupBox_3);
        uploadPath->setObjectName("uploadPath");

        formLayout_4->setWidget(2, QFormLayout::ItemRole::FieldRole, uploadPath);

        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName("label_17");

        formLayout_4->setWidget(3, QFormLayout::ItemRole::LabelRole, label_17);

        encryptData = new QComboBox(groupBox_3);
        encryptData->setObjectName("encryptData");

        formLayout_4->setWidget(1, QFormLayout::ItemRole::FieldRole, encryptData);

        downloadPath = new QLineEdit(groupBox_3);
        downloadPath->setObjectName("downloadPath");

        formLayout_4->setWidget(3, QFormLayout::ItemRole::FieldRole, downloadPath);


        horizontalLayout_3->addLayout(formLayout_4);


        gridLayout_2->addWidget(groupBox_3, 1, 1, 1, 1);


        retranslateUi(ServerSetting);

        QMetaObject::connectSlotsByName(ServerSetting);
    } // setupUi

    void retranslateUi(QWidget *ServerSetting)
    {
        ServerSetting->setWindowTitle(QCoreApplication::translate("ServerSetting", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("ServerSetting", "\346\234\215\345\212\241\347\253\257\345\217\202\346\225\260", nullptr));
        label->setText(QCoreApplication::translate("ServerSetting", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200", nullptr));
        label_2->setText(QCoreApplication::translate("ServerSetting", "\346\234\215\345\212\241\345\231\250\347\253\257\345\217\243", nullptr));
        label_3->setText(QCoreApplication::translate("ServerSetting", "\346\234\200\345\244\247\345\271\266\345\217\221\350\277\236\346\216\245\346\225\260", nullptr));
        label_4->setText(QCoreApplication::translate("ServerSetting", "\345\267\245\344\275\234\347\272\277\347\250\213\346\225\260", nullptr));
        label_5->setText(QCoreApplication::translate("ServerSetting", "\350\266\205\346\227\266\346\227\266\351\231\220", nullptr));
        label_6->setText(QCoreApplication::translate("ServerSetting", "\346\234\200\345\244\247\345\234\250\347\272\277\347\224\250\346\210\267\346\225\260", nullptr));
        label_7->setText(QCoreApplication::translate("ServerSetting", "\347\275\221\347\273\234\350\257\273\345\206\231\347\274\223\345\206\262\345\214\272\345\244\247\345\260\217", nullptr));
        label_8->setText(QCoreApplication::translate("ServerSetting", "\347\224\250\346\210\267\344\274\232\350\257\235\350\266\205\346\227\266", nullptr));
        label_9->setText(QCoreApplication::translate("ServerSetting", "\346\234\200\345\244\247\345\206\205\345\255\230", nullptr));
        label_18->setText(QCoreApplication::translate("ServerSetting", "MB", nullptr));
        label_19->setText(QCoreApplication::translate("ServerSetting", "ms ", nullptr));
        label_20->setText(QCoreApplication::translate("ServerSetting", "MB", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("ServerSetting", "\346\227\245\345\277\227", nullptr));
        localLog->setText(QCoreApplication::translate("ServerSetting", "\346\234\254\345\234\260\346\227\245\345\277\227", nullptr));
        enableLog->setText(QCoreApplication::translate("ServerSetting", "\345\220\257\347\224\250\346\227\245\345\277\227", nullptr));
        label_10->setText(QCoreApplication::translate("ServerSetting", "\346\227\245\345\277\227\347\272\247\345\210\253", nullptr));
        label_12->setText(QCoreApplication::translate("ServerSetting", "\346\234\200\345\244\247\346\227\245\345\277\227\346\226\207\344\273\266", nullptr));
        label_13->setText(QCoreApplication::translate("ServerSetting", "\346\233\264\346\226\260\345\244\247\345\260\217", nullptr));
        label_11->setText(QCoreApplication::translate("ServerSetting", "\346\227\245\345\277\227\346\226\207\344\273\266\350\267\257\345\276\204", nullptr));
        logBtn->setText(QString());
        disable->setText(QCoreApplication::translate("ServerSetting", "MB", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("ServerSetting", "\344\270\232\345\212\241", nullptr));
        label_14->setText(QCoreApplication::translate("ServerSetting", "\346\270\270\345\256\242\350\256\277\351\227\256", nullptr));
        label_15->setText(QCoreApplication::translate("ServerSetting", "\346\225\260\346\215\256\344\274\240\350\276\223\345\212\240\345\257\206", nullptr));
        label_16->setText(QCoreApplication::translate("ServerSetting", "\346\226\207\344\273\266\344\270\212\344\274\240\345\255\230\345\202\250\347\233\256\345\275\225", nullptr));
        label_17->setText(QCoreApplication::translate("ServerSetting", "\346\226\207\344\273\266\344\270\213\350\275\275\344\270\264\346\227\266\347\233\256\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ServerSetting: public Ui_ServerSetting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVERSETTING_H
