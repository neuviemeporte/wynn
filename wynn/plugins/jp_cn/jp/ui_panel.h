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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PanelUI
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *kanjiGroupBox;
    QVBoxLayout *verticalLayout_2;
    QFormLayout *formLayout;
    QCheckBox *charReadingCheck;
    QLineEdit *charReadingEdit;
    QCheckBox *charMeaningCheck;
    QLineEdit *charMeaningEdit;
    QCheckBox *charRadicalsCheck;
    QHBoxLayout *horizontalLayout_21;
    QLineEdit *charRadicalsEdit;
    QPushButton *charRadicalsButton;
    QCheckBox *charStrokeCheck;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *charStrokeButton;
    QSpinBox *charStrokeSpin;
    QSpacerItem *horizontalSpacer_12;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *charSearchButton;
    QGroupBox *wordGroupBox;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *wordSearchLangRadio;
    QRadioButton *wordSearchEnglishRadio;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout;
    QRadioButton *wordSearchMatchRadio;
    QRadioButton *wordSearchStartRadio;
    QRadioButton *wordSearchContainRadio;
    QRadioButton *wordSearchEndRadio;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *wordSearchPersistentCheck;
    QSpacerItem *verticalSpacer_3;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QLineEdit *wordSearchQueryEdit;
    QPushButton *wordSearchButton;

    void setupUi(QWidget *PanelUI)
    {
        if (PanelUI->objectName().isEmpty())
            PanelUI->setObjectName(QStringLiteral("PanelUI"));
        PanelUI->resize(718, 167);
        horizontalLayout = new QHBoxLayout(PanelUI);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(3, 0, 3, 6);
        kanjiGroupBox = new QGroupBox(PanelUI);
        kanjiGroupBox->setObjectName(QStringLiteral("kanjiGroupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(kanjiGroupBox->sizePolicy().hasHeightForWidth());
        kanjiGroupBox->setSizePolicy(sizePolicy);
        kanjiGroupBox->setMinimumSize(QSize(421, 0));
        verticalLayout_2 = new QVBoxLayout(kanjiGroupBox);
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 3, -1, 6);
        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setHorizontalSpacing(6);
        formLayout->setVerticalSpacing(6);
        charReadingCheck = new QCheckBox(kanjiGroupBox);
        charReadingCheck->setObjectName(QStringLiteral("charReadingCheck"));

        formLayout->setWidget(0, QFormLayout::LabelRole, charReadingCheck);

        charReadingEdit = new QLineEdit(kanjiGroupBox);
        charReadingEdit->setObjectName(QStringLiteral("charReadingEdit"));
        charReadingEdit->setEnabled(false);

        formLayout->setWidget(0, QFormLayout::FieldRole, charReadingEdit);

        charMeaningCheck = new QCheckBox(kanjiGroupBox);
        charMeaningCheck->setObjectName(QStringLiteral("charMeaningCheck"));

        formLayout->setWidget(1, QFormLayout::LabelRole, charMeaningCheck);

        charMeaningEdit = new QLineEdit(kanjiGroupBox);
        charMeaningEdit->setObjectName(QStringLiteral("charMeaningEdit"));
        charMeaningEdit->setEnabled(false);

        formLayout->setWidget(1, QFormLayout::FieldRole, charMeaningEdit);

        charRadicalsCheck = new QCheckBox(kanjiGroupBox);
        charRadicalsCheck->setObjectName(QStringLiteral("charRadicalsCheck"));

        formLayout->setWidget(2, QFormLayout::LabelRole, charRadicalsCheck);

        horizontalLayout_21 = new QHBoxLayout();
        horizontalLayout_21->setObjectName(QStringLiteral("horizontalLayout_21"));
        charRadicalsEdit = new QLineEdit(kanjiGroupBox);
        charRadicalsEdit->setObjectName(QStringLiteral("charRadicalsEdit"));
        charRadicalsEdit->setEnabled(false);
        charRadicalsEdit->setReadOnly(true);

        horizontalLayout_21->addWidget(charRadicalsEdit);

        charRadicalsButton = new QPushButton(kanjiGroupBox);
        charRadicalsButton->setObjectName(QStringLiteral("charRadicalsButton"));
        charRadicalsButton->setEnabled(false);
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(charRadicalsButton->sizePolicy().hasHeightForWidth());
        charRadicalsButton->setSizePolicy(sizePolicy1);
        charRadicalsButton->setMinimumSize(QSize(90, 0));

        horizontalLayout_21->addWidget(charRadicalsButton);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout_21);

        charStrokeCheck = new QCheckBox(kanjiGroupBox);
        charStrokeCheck->setObjectName(QStringLiteral("charStrokeCheck"));

        formLayout->setWidget(3, QFormLayout::LabelRole, charStrokeCheck);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        charStrokeButton = new QPushButton(kanjiGroupBox);
        charStrokeButton->setObjectName(QStringLiteral("charStrokeButton"));
        charStrokeButton->setEnabled(false);
        charStrokeButton->setMaximumSize(QSize(22, 20));

        horizontalLayout_2->addWidget(charStrokeButton);

        charStrokeSpin = new QSpinBox(kanjiGroupBox);
        charStrokeSpin->setObjectName(QStringLiteral("charStrokeSpin"));
        charStrokeSpin->setEnabled(false);

        horizontalLayout_2->addWidget(charStrokeSpin);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_12);


        formLayout->setLayout(3, QFormLayout::FieldRole, horizontalLayout_2);


        verticalLayout_2->addLayout(formLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        charSearchButton = new QPushButton(kanjiGroupBox);
        charSearchButton->setObjectName(QStringLiteral("charSearchButton"));

        horizontalLayout_3->addWidget(charSearchButton);


        verticalLayout_2->addLayout(horizontalLayout_3);


        horizontalLayout->addWidget(kanjiGroupBox);

        wordGroupBox = new QGroupBox(PanelUI);
        wordGroupBox->setObjectName(QStringLiteral("wordGroupBox"));
        sizePolicy.setHeightForWidth(wordGroupBox->sizePolicy().hasHeightForWidth());
        wordGroupBox->setSizePolicy(sizePolicy);
        verticalLayout_4 = new QVBoxLayout(wordGroupBox);
        verticalLayout_4->setSpacing(3);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(-1, 3, -1, 6);
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        groupBox_3 = new QGroupBox(wordGroupBox);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setSpacing(3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(9, 3, 9, 3);
        wordSearchLangRadio = new QRadioButton(groupBox_3);
        wordSearchLangRadio->setObjectName(QStringLiteral("wordSearchLangRadio"));
        wordSearchLangRadio->setChecked(true);

        verticalLayout_3->addWidget(wordSearchLangRadio);

        wordSearchEnglishRadio = new QRadioButton(groupBox_3);
        wordSearchEnglishRadio->setObjectName(QStringLiteral("wordSearchEnglishRadio"));

        verticalLayout_3->addWidget(wordSearchEnglishRadio);


        horizontalLayout_6->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(wordGroupBox);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout = new QGridLayout(groupBox_4);
        gridLayout->setSpacing(3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(-1, 3, -1, 3);
        wordSearchMatchRadio = new QRadioButton(groupBox_4);
        wordSearchMatchRadio->setObjectName(QStringLiteral("wordSearchMatchRadio"));
        wordSearchMatchRadio->setChecked(true);

        gridLayout->addWidget(wordSearchMatchRadio, 0, 0, 1, 1);

        wordSearchStartRadio = new QRadioButton(groupBox_4);
        wordSearchStartRadio->setObjectName(QStringLiteral("wordSearchStartRadio"));

        gridLayout->addWidget(wordSearchStartRadio, 0, 1, 1, 1);

        wordSearchContainRadio = new QRadioButton(groupBox_4);
        wordSearchContainRadio->setObjectName(QStringLiteral("wordSearchContainRadio"));

        gridLayout->addWidget(wordSearchContainRadio, 1, 0, 1, 1);

        wordSearchEndRadio = new QRadioButton(groupBox_4);
        wordSearchEndRadio->setObjectName(QStringLiteral("wordSearchEndRadio"));

        gridLayout->addWidget(wordSearchEndRadio, 1, 1, 1, 1);


        horizontalLayout_6->addWidget(groupBox_4);


        horizontalLayout_4->addLayout(horizontalLayout_6);

        horizontalSpacer_3 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        verticalLayout_4->addLayout(horizontalLayout_4);

        wordSearchPersistentCheck = new QCheckBox(wordGroupBox);
        wordSearchPersistentCheck->setObjectName(QStringLiteral("wordSearchPersistentCheck"));

        verticalLayout_4->addWidget(wordSearchPersistentCheck);

        verticalSpacer_3 = new QSpacerItem(20, 24, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        label_2 = new QLabel(wordGroupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_5->addWidget(label_2);

        wordSearchQueryEdit = new QLineEdit(wordGroupBox);
        wordSearchQueryEdit->setObjectName(QStringLiteral("wordSearchQueryEdit"));

        horizontalLayout_5->addWidget(wordSearchQueryEdit);

        wordSearchButton = new QPushButton(wordGroupBox);
        wordSearchButton->setObjectName(QStringLiteral("wordSearchButton"));

        horizontalLayout_5->addWidget(wordSearchButton);


        verticalLayout_4->addLayout(horizontalLayout_5);


        horizontalLayout->addWidget(wordGroupBox);


        retranslateUi(PanelUI);
        QObject::connect(charReadingCheck, SIGNAL(toggled(bool)), charReadingEdit, SLOT(setEnabled(bool)));
        QObject::connect(charMeaningCheck, SIGNAL(toggled(bool)), charMeaningEdit, SLOT(setEnabled(bool)));
        QObject::connect(charRadicalsCheck, SIGNAL(toggled(bool)), charRadicalsButton, SLOT(setEnabled(bool)));
        QObject::connect(charStrokeCheck, SIGNAL(toggled(bool)), charStrokeButton, SLOT(setEnabled(bool)));
        QObject::connect(charStrokeCheck, SIGNAL(toggled(bool)), charStrokeSpin, SLOT(setEnabled(bool)));
        QObject::connect(charRadicalsCheck, SIGNAL(toggled(bool)), charRadicalsEdit, SLOT(setEnabled(bool)));

        QMetaObject::connectSlotsByName(PanelUI);
    } // setupUi

    void retranslateUi(QWidget *PanelUI)
    {
        PanelUI->setWindowTitle(QApplication::translate("PanelUI", "Form", 0));
        kanjiGroupBox->setTitle(QApplication::translate("PanelUI", "Character search", 0));
#ifndef QT_NO_TOOLTIP
        charReadingCheck->setToolTip(QApplication::translate("PanelUI", "Enable reading search criterium", 0));
#endif // QT_NO_TOOLTIP
        charReadingCheck->setText(QApplication::translate("PanelUI", "Reading:", 0));
#ifndef QT_NO_TOOLTIP
        charReadingEdit->setToolTip(QApplication::translate("PanelUI", "Hepburn romanization, pinyin, hiragana, katakana and character themselves will be accepted", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        charMeaningCheck->setToolTip(QApplication::translate("PanelUI", "Enable meaning search criterium (matches for items containing the query anywhere)", 0));
#endif // QT_NO_TOOLTIP
        charMeaningCheck->setText(QApplication::translate("PanelUI", "Meaning:", 0));
#ifndef QT_NO_TOOLTIP
        charMeaningEdit->setToolTip(QApplication::translate("PanelUI", "Enter English text", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        charRadicalsCheck->setToolTip(QApplication::translate("PanelUI", "Enable radical search criterium; use radical table to select desired radicals", 0));
#endif // QT_NO_TOOLTIP
        charRadicalsCheck->setText(QApplication::translate("PanelUI", "Radicals:", 0));
#ifndef QT_NO_TOOLTIP
        charRadicalsEdit->setToolTip(QApplication::translate("PanelUI", "Edit field is read-only, use radical table to specify radicals", 0));
#endif // QT_NO_TOOLTIP
        charRadicalsButton->setText(QApplication::translate("PanelUI", "Radical table...", 0));
#ifndef QT_NO_TOOLTIP
        charStrokeCheck->setToolTip(QApplication::translate("PanelUI", "Enable stroke count search criterium", 0));
#endif // QT_NO_TOOLTIP
        charStrokeCheck->setText(QApplication::translate("PanelUI", "Stroke count:", 0));
#ifndef QT_NO_TOOLTIP
        charStrokeButton->setToolTip(QApplication::translate("PanelUI", "Stroke count search mode (equals, more, less)", 0));
#endif // QT_NO_TOOLTIP
        charStrokeButton->setText(QApplication::translate("PanelUI", "=", 0));
#ifndef QT_NO_TOOLTIP
        charSearchButton->setToolTip(QApplication::translate("PanelUI", "Search for characters satisfying the active criteria", 0));
#endif // QT_NO_TOOLTIP
        charSearchButton->setText(QApplication::translate("PanelUI", "Search", 0));
        wordGroupBox->setTitle(QApplication::translate("PanelUI", "Word search", 0));
        groupBox_3->setTitle(QApplication::translate("PanelUI", "Search for", 0));
        wordSearchLangRadio->setText(QApplication::translate("PanelUI", "Japanese", 0));
        wordSearchEnglishRadio->setText(QApplication::translate("PanelUI", "English", 0));
        groupBox_4->setTitle(QApplication::translate("PanelUI", "Search mode", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchMatchRadio->setToolTip(QApplication::translate("PanelUI", "Search for words matching the query exactly", 0));
#endif // QT_NO_TOOLTIP
        wordSearchMatchRadio->setText(QApplication::translate("PanelUI", "exact", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchStartRadio->setToolTip(QApplication::translate("PanelUI", "Search for words starting with query", 0));
#endif // QT_NO_TOOLTIP
        wordSearchStartRadio->setText(QApplication::translate("PanelUI", "starts with", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchContainRadio->setToolTip(QApplication::translate("PanelUI", "Search for words containing query", 0));
#endif // QT_NO_TOOLTIP
        wordSearchContainRadio->setText(QApplication::translate("PanelUI", "contains", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchEndRadio->setToolTip(QApplication::translate("PanelUI", "Search for words ending with query", 0));
#endif // QT_NO_TOOLTIP
        wordSearchEndRadio->setText(QApplication::translate("PanelUI", "ends with", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchPersistentCheck->setToolTip(QApplication::translate("PanelUI", "Try wider search modes if search from current mode comes up empty", 0));
#endif // QT_NO_TOOLTIP
        wordSearchPersistentCheck->setText(QApplication::translate("PanelUI", "persistent search", 0));
        label_2->setText(QApplication::translate("PanelUI", "Query:", 0));
#ifndef QT_NO_TOOLTIP
        wordSearchQueryEdit->setToolTip(QApplication::translate("PanelUI", "Hepburn romanization, pinyin, hiragana, katakana and characters all accepted", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        wordSearchButton->setToolTip(QApplication::translate("PanelUI", "Search for words matching the query for the selected search mode", 0));
#endif // QT_NO_TOOLTIP
        wordSearchButton->setText(QApplication::translate("PanelUI", "Search", 0));
    } // retranslateUi

};

namespace Ui {
    class PanelUI: public Ui_PanelUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PANEL_H
