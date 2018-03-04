/********************************************************************************
** Form generated from reading UI file 'panel.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PANEL_H
#define UI_PANEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DePanelUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *germanRadio;
    QRadioButton *englishRadio;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout;
    QRadioButton *matchRadio;
    QRadioButton *startRadio;
    QRadioButton *containRadio;
    QRadioButton *endRadio;
    QCheckBox *persistentCheck;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *queryEdit;
    QPushButton *searchButton;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *DePanelUI)
    {
        if (DePanelUI->objectName().isEmpty())
            DePanelUI->setObjectName(QStringLiteral("DePanelUI"));
        DePanelUI->resize(377, 124);
        verticalLayout_2 = new QVBoxLayout(DePanelUI);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 0, 3, 6);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetFixedSize);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setSizeConstraint(QLayout::SetFixedSize);
        groupBox_3 = new QGroupBox(DePanelUI);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(9, 3, 9, 3);
        germanRadio = new QRadioButton(groupBox_3);
        germanRadio->setObjectName(QStringLiteral("germanRadio"));
        germanRadio->setChecked(true);

        verticalLayout_3->addWidget(germanRadio);

        englishRadio = new QRadioButton(groupBox_3);
        englishRadio->setObjectName(QStringLiteral("englishRadio"));

        verticalLayout_3->addWidget(englishRadio);


        horizontalLayout_6->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(DePanelUI);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        sizePolicy.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox_4);
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(-1, 3, -1, 3);
        matchRadio = new QRadioButton(groupBox_4);
        matchRadio->setObjectName(QStringLiteral("matchRadio"));
        matchRadio->setChecked(true);

        gridLayout->addWidget(matchRadio, 0, 0, 1, 1);

        startRadio = new QRadioButton(groupBox_4);
        startRadio->setObjectName(QStringLiteral("startRadio"));

        gridLayout->addWidget(startRadio, 0, 1, 1, 1);

        containRadio = new QRadioButton(groupBox_4);
        containRadio->setObjectName(QStringLiteral("containRadio"));

        gridLayout->addWidget(containRadio, 1, 0, 1, 1);

        endRadio = new QRadioButton(groupBox_4);
        endRadio->setObjectName(QStringLiteral("endRadio"));

        gridLayout->addWidget(endRadio, 1, 1, 1, 1);


        horizontalLayout_6->addWidget(groupBox_4);


        verticalLayout->addLayout(horizontalLayout_6);

        persistentCheck = new QCheckBox(DePanelUI);
        persistentCheck->setObjectName(QStringLiteral("persistentCheck"));

        verticalLayout->addWidget(persistentCheck);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(DePanelUI);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_5->addWidget(label_2);

        queryEdit = new QLineEdit(DePanelUI);
        queryEdit->setObjectName(QStringLiteral("queryEdit"));

        horizontalLayout_5->addWidget(queryEdit);

        searchButton = new QPushButton(DePanelUI);
        searchButton->setObjectName(QStringLiteral("searchButton"));

        horizontalLayout_5->addWidget(searchButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_5);

#ifndef QT_NO_SHORTCUT
        label_2->setBuddy(queryEdit);
#endif // QT_NO_SHORTCUT

        retranslateUi(DePanelUI);
        QObject::connect(matchRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(startRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(containRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(endRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(germanRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(englishRadio, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));
        QObject::connect(persistentCheck, SIGNAL(clicked()), queryEdit, SLOT(setFocus()));

        QMetaObject::connectSlotsByName(DePanelUI);
    } // setupUi

    void retranslateUi(QWidget *DePanelUI)
    {
        DePanelUI->setWindowTitle(QApplication::translate("DePanelUI", "Form", 0));
        groupBox_3->setTitle(QApplication::translate("DePanelUI", "Search for", 0));
        germanRadio->setText(QApplication::translate("DePanelUI", "German", 0));
        englishRadio->setText(QApplication::translate("DePanelUI", "English", 0));
        groupBox_4->setTitle(QApplication::translate("DePanelUI", "Search mode", 0));
#ifndef QT_NO_TOOLTIP
        matchRadio->setToolTip(QApplication::translate("DePanelUI", "Search for words matching the query exactly", 0));
#endif // QT_NO_TOOLTIP
        matchRadio->setText(QApplication::translate("DePanelUI", "exact", 0));
#ifndef QT_NO_TOOLTIP
        startRadio->setToolTip(QApplication::translate("DePanelUI", "Search for words starting with query", 0));
#endif // QT_NO_TOOLTIP
        startRadio->setText(QApplication::translate("DePanelUI", "starts with", 0));
#ifndef QT_NO_TOOLTIP
        containRadio->setToolTip(QApplication::translate("DePanelUI", "Search for words containing query", 0));
#endif // QT_NO_TOOLTIP
        containRadio->setText(QApplication::translate("DePanelUI", "contains", 0));
#ifndef QT_NO_TOOLTIP
        endRadio->setToolTip(QApplication::translate("DePanelUI", "Search for words ending with query", 0));
#endif // QT_NO_TOOLTIP
        endRadio->setText(QApplication::translate("DePanelUI", "ends with", 0));
#ifndef QT_NO_TOOLTIP
        persistentCheck->setToolTip(QApplication::translate("DePanelUI", "Try wider search modes if search from current mode comes up empty", 0));
#endif // QT_NO_TOOLTIP
        persistentCheck->setText(QApplication::translate("DePanelUI", "persistent search", 0));
        label_2->setText(QApplication::translate("DePanelUI", "&Query:", 0));
#ifndef QT_NO_TOOLTIP
        queryEdit->setToolTip(QApplication::translate("DePanelUI", "German characters (\303\244, \303\266, \303\274, \303\237) accepted, also 'ae', 'oe', and 'ue' may be substituted for umlauts, and '3' will be treated as '\303\237'", 0));
#endif // QT_NO_TOOLTIP
        searchButton->setText(QApplication::translate("DePanelUI", "Search", 0));
    } // retranslateUi

};

namespace Ui {
    class DePanelUI: public Ui_DePanelUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANEL_H
