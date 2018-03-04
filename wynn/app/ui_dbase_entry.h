/********************************************************************************
** Form generated from reading UI file 'dbase_entry.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DBASE_ENTRY_H
#define UI_DBASE_ENTRY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DbaseEntryDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFormLayout *formLayout;
    QLabel *label;
    QLineEdit *entryEdit;
    QLabel *label_4;
    QLineEdit *descEdit;
    QLabel *label_5;
    QHBoxLayout *horizontalLayout;
    QComboBox *dbaseCombo;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *DbaseEntryDialog)
    {
        if (DbaseEntryDialog->objectName().isEmpty())
            DbaseEntryDialog->setObjectName(QStringLiteral("DbaseEntryDialog"));
        DbaseEntryDialog->resize(317, 133);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DbaseEntryDialog->sizePolicy().hasHeightForWidth());
        DbaseEntryDialog->setSizePolicy(sizePolicy);
        DbaseEntryDialog->setModal(true);
        verticalLayout = new QVBoxLayout(DbaseEntryDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(DbaseEntryDialog);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        entryEdit = new QLineEdit(DbaseEntryDialog);
        entryEdit->setObjectName(QStringLiteral("entryEdit"));

        formLayout->setWidget(0, QFormLayout::FieldRole, entryEdit);

        label_4 = new QLabel(DbaseEntryDialog);
        label_4->setObjectName(QStringLiteral("label_4"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_4);

        descEdit = new QLineEdit(DbaseEntryDialog);
        descEdit->setObjectName(QStringLiteral("descEdit"));

        formLayout->setWidget(1, QFormLayout::FieldRole, descEdit);

        label_5 = new QLabel(DbaseEntryDialog);
        label_5->setObjectName(QStringLiteral("label_5"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_5);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        dbaseCombo = new QComboBox(DbaseEntryDialog);
        dbaseCombo->setObjectName(QStringLiteral("dbaseCombo"));

        horizontalLayout->addWidget(dbaseCombo);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);


        verticalLayout->addLayout(formLayout);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(DbaseEntryDialog);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        cancelButton = new QPushButton(DbaseEntryDialog);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        verticalLayout->addLayout(hboxLayout);


        retranslateUi(DbaseEntryDialog);
        QObject::connect(okButton, SIGNAL(clicked()), DbaseEntryDialog, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), DbaseEntryDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(DbaseEntryDialog);
    } // setupUi

    void retranslateUi(QDialog *DbaseEntryDialog)
    {
        DbaseEntryDialog->setWindowTitle(QApplication::translate("DbaseEntryDialog", "Dialog", 0));
        label->setText(QApplication::translate("DbaseEntryDialog", "Entry:", 0));
        label_4->setText(QApplication::translate("DbaseEntryDialog", "Description:", 0));
        label_5->setText(QApplication::translate("DbaseEntryDialog", "Database:", 0));
        okButton->setText(QApplication::translate("DbaseEntryDialog", "OK", 0));
        cancelButton->setText(QApplication::translate("DbaseEntryDialog", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class DbaseEntryDialog: public Ui_DbaseEntryDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DBASE_ENTRY_H
