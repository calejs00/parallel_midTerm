/********************************************************************************
** Form generated from reading UI file 'password_cracker.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PASSWORD_CRACKER_H
#define UI_PASSWORD_CRACKER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PasswordCracker
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *label_password;
    QLineEdit *passwordInput;
    QLabel *label_salt;
    QLineEdit *saltInput;
    QLabel *hashLabel;
    QTextEdit *threadStatus;
    QLabel *statusLabel;
    QPushButton *pushButton_start;

    void setupUi(QWidget *PasswordCracker)
    {
        if (PasswordCracker->objectName().isEmpty())
            PasswordCracker->setObjectName(QString::fromUtf8("PasswordCracker"));
        PasswordCracker->resize(400, 600);
        verticalLayout = new QVBoxLayout(PasswordCracker);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_password = new QLabel(PasswordCracker);
        label_password->setObjectName(QString::fromUtf8("label_password"));

        gridLayout->addWidget(label_password, 0, 0, 1, 1);

        passwordInput = new QLineEdit(PasswordCracker);
        passwordInput->setObjectName(QString::fromUtf8("passwordInput"));

        gridLayout->addWidget(passwordInput, 0, 1, 1, 1);

        label_salt = new QLabel(PasswordCracker);
        label_salt->setObjectName(QString::fromUtf8("label_salt"));

        gridLayout->addWidget(label_salt, 1, 0, 1, 1);

        saltInput = new QLineEdit(PasswordCracker);
        saltInput->setObjectName(QString::fromUtf8("saltInput"));

        gridLayout->addWidget(saltInput, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        hashLabel = new QLabel(PasswordCracker);
        hashLabel->setObjectName(QString::fromUtf8("hashLabel"));

        verticalLayout->addWidget(hashLabel);

        threadStatus = new QTextEdit(PasswordCracker);
        threadStatus->setObjectName(QString::fromUtf8("threadStatus"));
        threadStatus->setReadOnly(true);

        verticalLayout->addWidget(threadStatus);

        statusLabel = new QLabel(PasswordCracker);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));

        verticalLayout->addWidget(statusLabel);

        pushButton_start = new QPushButton(PasswordCracker);
        pushButton_start->setObjectName(QString::fromUtf8("pushButton_start"));

        verticalLayout->addWidget(pushButton_start);


        retranslateUi(PasswordCracker);

        QMetaObject::connectSlotsByName(PasswordCracker);
    } // setupUi

    void retranslateUi(QWidget *PasswordCracker)
    {
        PasswordCracker->setWindowTitle(QCoreApplication::translate("PasswordCracker", "Password Cracker", nullptr));
        label_password->setText(QCoreApplication::translate("PasswordCracker", "Password:", nullptr));
        label_salt->setText(QCoreApplication::translate("PasswordCracker", "Salt (2 characters):", nullptr));
        hashLabel->setText(QCoreApplication::translate("PasswordCracker", "Generated Hash:", nullptr));
        statusLabel->setText(QCoreApplication::translate("PasswordCracker", "Status:", nullptr));
        pushButton_start->setText(QCoreApplication::translate("PasswordCracker", "Start Cracking", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PasswordCracker: public Ui_PasswordCracker {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PASSWORD_CRACKER_H
