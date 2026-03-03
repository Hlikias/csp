/********************************************************************************
** Form generated from reading UI file 'csproject.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSPROJECT_H
#define UI_CSPROJECT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CSproject
{
public:
    QWidget *centralwidget;
    QPushButton *serverModel;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *autoLogin;
    QSpacerItem *horizontalSpacer;
    QRadioButton *forgetPwd;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lblService;
    QFrame *line;
    QLabel *lblRegister;
    QLabel *lblAvatar;
    QComboBox *leQQNum;
    QLineEdit *leQQPwd;
    QPushButton *btnLogin;
    QPushButton *closeBtn;
    QPushButton *minBtn;

    void setupUi(QMainWindow *CSproject)
    {
        if (CSproject->objectName().isEmpty())
            CSproject->setObjectName("CSproject");
        CSproject->resize(320, 460);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CSproject->sizePolicy().hasHeightForWidth());
        CSproject->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(CSproject);
        centralwidget->setObjectName("centralwidget");
        serverModel = new QPushButton(centralwidget);
        serverModel->setObjectName("serverModel");
        serverModel->setGeometry(QRect(100, 520, 81, 81));
        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName("horizontalLayoutWidget_2");
        horizontalLayoutWidget_2->setGeometry(QRect(30, 250, 251, 22));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        autoLogin = new QRadioButton(horizontalLayoutWidget_2);
        autoLogin->setObjectName("autoLogin");

        horizontalLayout_2->addWidget(autoLogin);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        forgetPwd = new QRadioButton(horizontalLayoutWidget_2);
        forgetPwd->setObjectName("forgetPwd");

        horizontalLayout_2->addWidget(forgetPwd);

        horizontalLayoutWidget_3 = new QWidget(centralwidget);
        horizontalLayoutWidget_3->setObjectName("horizontalLayoutWidget_3");
        horizontalLayoutWidget_3->setGeometry(QRect(30, 410, 271, 20));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        lblService = new QLabel(horizontalLayoutWidget_3);
        lblService->setObjectName("lblService");
        lblService->setAlignment(Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing|Qt::AlignmentFlag::AlignVCenter);

        horizontalLayout_3->addWidget(lblService);

        line = new QFrame(horizontalLayoutWidget_3);
        line->setObjectName("line");
        line->setFrameShape(QFrame::Shape::VLine);
        line->setFrameShadow(QFrame::Shadow::Sunken);

        horizontalLayout_3->addWidget(line);

        lblRegister = new QLabel(horizontalLayoutWidget_3);
        lblRegister->setObjectName("lblRegister");

        horizontalLayout_3->addWidget(lblRegister);

        lblAvatar = new QLabel(centralwidget);
        lblAvatar->setObjectName("lblAvatar");
        lblAvatar->setGeometry(QRect(120, 90, 53, 15));
        leQQNum = new QComboBox(centralwidget);
        leQQNum->setObjectName("leQQNum");
        leQQNum->setGeometry(QRect(30, 150, 251, 41));
        sizePolicy.setHeightForWidth(leQQNum->sizePolicy().hasHeightForWidth());
        leQQNum->setSizePolicy(sizePolicy);
        leQQNum->setEditable(true);
        leQQPwd = new QLineEdit(centralwidget);
        leQQPwd->setObjectName("leQQPwd");
        leQQPwd->setGeometry(QRect(30, 200, 251, 41));
        sizePolicy.setHeightForWidth(leQQPwd->sizePolicy().hasHeightForWidth());
        leQQPwd->setSizePolicy(sizePolicy);
        btnLogin = new QPushButton(centralwidget);
        btnLogin->setObjectName("btnLogin");
        btnLogin->setGeometry(QRect(30, 310, 251, 41));
        sizePolicy.setHeightForWidth(btnLogin->sizePolicy().hasHeightForWidth());
        btnLogin->setSizePolicy(sizePolicy);
        closeBtn = new QPushButton(centralwidget);
        closeBtn->setObjectName("closeBtn");
        closeBtn->setGeometry(QRect(240, 0, 35, 35));
        sizePolicy.setHeightForWidth(closeBtn->sizePolicy().hasHeightForWidth());
        closeBtn->setSizePolicy(sizePolicy);
        minBtn = new QPushButton(centralwidget);
        minBtn->setObjectName("minBtn");
        minBtn->setGeometry(QRect(280, 0, 35, 35));
        sizePolicy.setHeightForWidth(minBtn->sizePolicy().hasHeightForWidth());
        minBtn->setSizePolicy(sizePolicy);
        CSproject->setCentralWidget(centralwidget);

        retranslateUi(CSproject);

        QMetaObject::connectSlotsByName(CSproject);
    } // setupUi

    void retranslateUi(QMainWindow *CSproject)
    {
        CSproject->setWindowTitle(QCoreApplication::translate("CSproject", "CSproject", nullptr));
        serverModel->setText(QCoreApplication::translate("CSproject", "\346\234\215\345\212\241\345\231\250\346\250\241\345\274\217", nullptr));
        autoLogin->setText(QCoreApplication::translate("CSproject", "\350\207\252\345\212\250\347\231\273\345\275\225", nullptr));
        forgetPwd->setText(QCoreApplication::translate("CSproject", "\344\277\235\345\255\230\345\257\206\347\240\201", nullptr));
        lblService->setText(QCoreApplication::translate("CSproject", "\346\234\215\345\212\241\345\231\250\346\250\241\345\274\217  ", nullptr));
        lblRegister->setText(QCoreApplication::translate("CSproject", "\346\263\250\345\206\214\350\264\246\345\217\267", nullptr));
        lblAvatar->setText(QCoreApplication::translate("CSproject", "TextLabel", nullptr));
        btnLogin->setText(QCoreApplication::translate("CSproject", "\347\231\273\345\275\225", nullptr));
        closeBtn->setText(QString());
        minBtn->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class CSproject: public Ui_CSproject {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSPROJECT_H
