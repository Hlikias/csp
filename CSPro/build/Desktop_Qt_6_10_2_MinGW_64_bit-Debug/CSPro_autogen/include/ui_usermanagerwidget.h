/********************************************************************************
** Form generated from reading UI file 'usermanagerwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERMANAGERWIDGET_H
#define UI_USERMANAGERWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserManagerWidget
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout_10;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_7;
    QComboBox *comboBox_3;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *radioButton;
    QLineEdit *lineEdit_4;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QComboBox *comboBox;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_8;
    QDateTimeEdit *dateTimeEdit;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *checkBox_3;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *radioButton_2;
    QLineEdit *lineEdit_3;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_9;
    QDateTimeEdit *dateTimeEdit_2;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBox;
    QLineEdit *lineEdit_2;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_6;
    QComboBox *comboBox_2;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_2;
    QLineEdit *lineEdit;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *pushButton_2;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_11;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_13;
    QTableView *userTable;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_14;
    QTableView *processTable;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer;
    QLabel *label;

    void setupUi(QWidget *UserManagerWidget)
    {
        if (UserManagerWidget->objectName().isEmpty())
            UserManagerWidget->setObjectName("UserManagerWidget");
        UserManagerWidget->resize(1478, 928);
        verticalLayout = new QVBoxLayout(UserManagerWidget);
        verticalLayout->setObjectName("verticalLayout");
        groupBox = new QGroupBox(UserManagerWidget);
        groupBox->setObjectName("groupBox");
        horizontalLayout_10 = new QHBoxLayout(groupBox);
        horizontalLayout_10->setObjectName("horizontalLayout_10");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        gridLayout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        label_7 = new QLabel(groupBox);
        label_7->setObjectName("label_7");

        horizontalLayout_7->addWidget(label_7);

        comboBox_3 = new QComboBox(groupBox);
        comboBox_3->setObjectName("comboBox_3");

        horizontalLayout_7->addWidget(comboBox_3);


        gridLayout->addLayout(horizontalLayout_7, 0, 2, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        radioButton = new QRadioButton(groupBox);
        radioButton->setObjectName("radioButton");

        horizontalLayout_4->addWidget(radioButton);

        lineEdit_4 = new QLineEdit(groupBox);
        lineEdit_4->setObjectName("lineEdit_4");

        horizontalLayout_4->addWidget(lineEdit_4);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_4, 0, 1, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");

        horizontalLayout_5->addWidget(label_5);

        comboBox = new QComboBox(groupBox);
        comboBox->setObjectName("comboBox");

        horizontalLayout_5->addWidget(comboBox);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_9);


        gridLayout->addLayout(horizontalLayout_5, 3, 0, 1, 1);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName("horizontalLayout_8");
        label_8 = new QLabel(groupBox);
        label_8->setObjectName("label_8");

        horizontalLayout_8->addWidget(label_8);

        dateTimeEdit = new QDateTimeEdit(groupBox);
        dateTimeEdit->setObjectName("dateTimeEdit");

        horizontalLayout_8->addWidget(dateTimeEdit);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_8, 2, 1, 1, 1);

        checkBox_3 = new QCheckBox(groupBox);
        checkBox_3->setObjectName("checkBox_3");

        gridLayout->addWidget(checkBox_3, 1, 2, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        radioButton_2 = new QRadioButton(groupBox);
        radioButton_2->setObjectName("radioButton_2");

        horizontalLayout_3->addWidget(radioButton_2);

        lineEdit_3 = new QLineEdit(groupBox);
        lineEdit_3->setObjectName("lineEdit_3");

        horizontalLayout_3->addWidget(lineEdit_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        gridLayout->addLayout(horizontalLayout_3, 1, 1, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName("horizontalLayout_9");
        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");

        horizontalLayout_9->addWidget(label_9);

        dateTimeEdit_2 = new QDateTimeEdit(groupBox);
        dateTimeEdit_2->setObjectName("dateTimeEdit_2");

        horizontalLayout_9->addWidget(dateTimeEdit_2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_5);


        gridLayout->addLayout(horizontalLayout_9, 3, 1, 1, 1);

        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName("pushButton");

        gridLayout->addWidget(pushButton, 2, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        checkBox = new QCheckBox(groupBox);
        checkBox->setObjectName("checkBox");

        horizontalLayout_2->addWidget(checkBox);

        lineEdit_2 = new QLineEdit(groupBox);
        lineEdit_2->setObjectName("lineEdit_2");

        horizontalLayout_2->addWidget(lineEdit_2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_6);


        gridLayout->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");

        horizontalLayout_6->addWidget(label_6);

        comboBox_2 = new QComboBox(groupBox);
        comboBox_2->setObjectName("comboBox_2");

        horizontalLayout_6->addWidget(comboBox_2);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_8);


        gridLayout->addLayout(horizontalLayout_6, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName("checkBox_2");

        horizontalLayout->addWidget(checkBox_2);

        lineEdit = new QLineEdit(groupBox);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        pushButton_2 = new QPushButton(groupBox);
        pushButton_2->setObjectName("pushButton_2");

        gridLayout->addWidget(pushButton_2, 3, 2, 1, 1);


        horizontalLayout_10->addLayout(gridLayout);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(UserManagerWidget);
        groupBox_2->setObjectName("groupBox_2");
        horizontalLayout_11 = new QHBoxLayout(groupBox_2);
        horizontalLayout_11->setObjectName("horizontalLayout_11");
        tabWidget = new QTabWidget(groupBox_2);
        tabWidget->setObjectName("tabWidget");
        tab = new QWidget();
        tab->setObjectName("tab");
        horizontalLayout_13 = new QHBoxLayout(tab);
        horizontalLayout_13->setObjectName("horizontalLayout_13");
        userTable = new QTableView(tab);
        userTable->setObjectName("userTable");

        horizontalLayout_13->addWidget(userTable);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        horizontalLayout_14 = new QHBoxLayout(tab_2);
        horizontalLayout_14->setObjectName("horizontalLayout_14");
        processTable = new QTableView(tab_2);
        processTable->setObjectName("processTable");

        horizontalLayout_14->addWidget(processTable);

        tabWidget->addTab(tab_2, QString());

        horizontalLayout_11->addWidget(tabWidget);


        verticalLayout->addWidget(groupBox_2);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName("horizontalLayout_12");
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer);

        label = new QLabel(UserManagerWidget);
        label->setObjectName("label");

        horizontalLayout_12->addWidget(label);


        verticalLayout->addLayout(horizontalLayout_12);


        retranslateUi(UserManagerWidget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(UserManagerWidget);
    } // setupUi

    void retranslateUi(QWidget *UserManagerWidget)
    {
        UserManagerWidget->setWindowTitle(QCoreApplication::translate("UserManagerWidget", "Form", nullptr));
        groupBox->setTitle(QCoreApplication::translate("UserManagerWidget", "GroupBox", nullptr));
        label_7->setText(QCoreApplication::translate("UserManagerWidget", "\346\237\245\350\257\242\346\235\241\346\225\260", nullptr));
        radioButton->setText(QCoreApplication::translate("UserManagerWidget", "\345\205\263\351\224\256\345\255\227      ", nullptr));
        label_5->setText(QCoreApplication::translate("UserManagerWidget", "\346\225\260\346\215\256\347\261\273\345\236\213", nullptr));
        label_8->setText(QCoreApplication::translate("UserManagerWidget", "\345\274\200\345\247\213\346\227\266\351\227\264", nullptr));
        checkBox_3->setText(QCoreApplication::translate("UserManagerWidget", "\345\220\257\347\224\250\346\235\241\344\273\266", nullptr));
        radioButton_2->setText(QCoreApplication::translate("UserManagerWidget", "\346\255\243\345\210\231\350\241\250\350\276\276\345\274\217", nullptr));
        label_9->setText(QCoreApplication::translate("UserManagerWidget", "\347\273\223\346\235\237\346\227\266\351\227\264", nullptr));
        pushButton->setText(QCoreApplication::translate("UserManagerWidget", "\345\210\267\346\226\260", nullptr));
        checkBox->setText(QCoreApplication::translate("UserManagerWidget", "IP\345\234\260\345\235\200", nullptr));
        label_6->setText(QCoreApplication::translate("UserManagerWidget", "\345\215\217\350\256\256\345\217\267   ", nullptr));
        checkBox_2->setText(QCoreApplication::translate("UserManagerWidget", "Port\345\217\267", nullptr));
        pushButton_2->setText(QCoreApplication::translate("UserManagerWidget", "\346\237\245\350\257\242", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("UserManagerWidget", "\346\225\260\346\215\256\350\241\250", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("UserManagerWidget", "\347\224\250\346\210\267\346\225\260\346\215\256\350\241\250", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("UserManagerWidget", "\351\200\232\344\277\241\345\206\205\345\256\271\350\241\250", nullptr));
        label->setText(QCoreApplication::translate("UserManagerWidget", "\346\234\252\350\277\236\346\216\245\346\225\260\346\215\256\345\272\223", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserManagerWidget: public Ui_UserManagerWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERMANAGERWIDGET_H
