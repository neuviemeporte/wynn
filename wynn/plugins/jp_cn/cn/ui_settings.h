/********************************************************************************
** Form generated from reading UI file 'settings.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsUI
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label_8;
    QLineEdit *settFontEdit;
    QPushButton *settFontButton;
    QSpacerItem *horizontalSpacer_9;

    void setupUi(QWidget *SettingsUI)
    {
        if (SettingsUI->objectName().isEmpty())
            SettingsUI->setObjectName(QStringLiteral("SettingsUI"));
        SettingsUI->resize(466, 43);
        verticalLayout = new QVBoxLayout(SettingsUI);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, -1, 0, -1);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_8 = new QLabel(SettingsUI);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout->addWidget(label_8);

        settFontEdit = new QLineEdit(SettingsUI);
        settFontEdit->setObjectName(QStringLiteral("settFontEdit"));
        settFontEdit->setEnabled(false);

        horizontalLayout->addWidget(settFontEdit);

        settFontButton = new QPushButton(SettingsUI);
        settFontButton->setObjectName(QStringLiteral("settFontButton"));

        horizontalLayout->addWidget(settFontButton);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_9);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(SettingsUI);

        QMetaObject::connectSlotsByName(SettingsUI);
    } // setupUi

    void retranslateUi(QWidget *SettingsUI)
    {
        SettingsUI->setWindowTitle(QApplication::translate("SettingsUI", "Form", 0));
        label_8->setText(QApplication::translate("SettingsUI", "Base font:", 0));
        settFontButton->setText(QApplication::translate("SettingsUI", "Select...", 0));
    } // retranslateUi

};

namespace Ui {
    class SettingsUI: public Ui_SettingsUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGS_H
