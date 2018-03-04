/********************************************************************************
** Form generated from reading UI file 'radical_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RADICAL_DIALOG_H
#define UI_RADICAL_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RadicalDialogUI
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *okButton;
    QPushButton *clearButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *RadicalDialogUI)
    {
        if (RadicalDialogUI->objectName().isEmpty())
            RadicalDialogUI->setObjectName(QStringLiteral("RadicalDialogUI"));
        RadicalDialogUI->resize(358, 67);
        RadicalDialogUI->setModal(true);
        verticalLayout = new QVBoxLayout(RadicalDialogUI);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        frame = new QFrame(RadicalDialogUI);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(frame);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        okButton = new QPushButton(RadicalDialogUI);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);

        clearButton = new QPushButton(RadicalDialogUI);
        clearButton->setObjectName(QStringLiteral("clearButton"));

        hboxLayout->addWidget(clearButton);

        cancelButton = new QPushButton(RadicalDialogUI);
        cancelButton->setObjectName(QStringLiteral("cancelButton"));

        hboxLayout->addWidget(cancelButton);


        verticalLayout->addLayout(hboxLayout);


        retranslateUi(RadicalDialogUI);
        QObject::connect(okButton, SIGNAL(clicked()), RadicalDialogUI, SLOT(accept()));
        QObject::connect(cancelButton, SIGNAL(clicked()), RadicalDialogUI, SLOT(reject()));

        QMetaObject::connectSlotsByName(RadicalDialogUI);
    } // setupUi

    void retranslateUi(QDialog *RadicalDialogUI)
    {
        RadicalDialogUI->setWindowTitle(QApplication::translate("RadicalDialogUI", "Radical table", 0));
        okButton->setText(QApplication::translate("RadicalDialogUI", "OK", 0));
        clearButton->setText(QApplication::translate("RadicalDialogUI", "Clear", 0));
        cancelButton->setText(QApplication::translate("RadicalDialogUI", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class RadicalDialogUI: public Ui_RadicalDialogUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RADICAL_DIALOG_H
