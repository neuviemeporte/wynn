/********************************************************************************
** Form generated from reading UI file 'word_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORD_DIALOG_H
#define UI_WORD_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WordDialogUI
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QFormLayout *formLayout;
    QLabel *label;
    QFrame *wordFrame;
    QLabel *catLabel;
    QFrame *catFrame;
    QHBoxLayout *horizontalLayout;
    QLabel *detCatLabel;
    QLabel *label_2;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *detReadLabel;
    QLabel *label_3;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *detDefLabel;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *detAddButton;
    QPushButton *detCopyButton;
    QPushButton *okButton;

    void setupUi(QDialog *WordDialogUI)
    {
        if (WordDialogUI->objectName().isEmpty())
            WordDialogUI->setObjectName(QStringLiteral("WordDialogUI"));
        WordDialogUI->resize(409, 265);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WordDialogUI->sizePolicy().hasHeightForWidth());
        WordDialogUI->setSizePolicy(sizePolicy);
        WordDialogUI->setSizeGripEnabled(false);
        WordDialogUI->setModal(true);
        verticalLayout = new QVBoxLayout(WordDialogUI);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        scrollArea = new QScrollArea(WordDialogUI);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 387, 204));
        formLayout = new QFormLayout(scrollAreaWidgetContents);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        label = new QLabel(scrollAreaWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        wordFrame = new QFrame(scrollAreaWidgetContents);
        wordFrame->setObjectName(QStringLiteral("wordFrame"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(wordFrame->sizePolicy().hasHeightForWidth());
        wordFrame->setSizePolicy(sizePolicy1);
        wordFrame->setFrameShape(QFrame::Box);
        wordFrame->setFrameShadow(QFrame::Sunken);
        wordFrame->setLineWidth(1);

        formLayout->setWidget(0, QFormLayout::FieldRole, wordFrame);

        catLabel = new QLabel(scrollAreaWidgetContents);
        catLabel->setObjectName(QStringLiteral("catLabel"));

        formLayout->setWidget(1, QFormLayout::LabelRole, catLabel);

        catFrame = new QFrame(scrollAreaWidgetContents);
        catFrame->setObjectName(QStringLiteral("catFrame"));
        sizePolicy1.setHeightForWidth(catFrame->sizePolicy().hasHeightForWidth());
        catFrame->setSizePolicy(sizePolicy1);
        catFrame->setFrameShape(QFrame::Box);
        catFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(catFrame);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        detCatLabel = new QLabel(catFrame);
        detCatLabel->setObjectName(QStringLiteral("detCatLabel"));
        detCatLabel->setWordWrap(true);
        detCatLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(detCatLabel);


        formLayout->setWidget(1, QFormLayout::FieldRole, catFrame);

        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(2, QFormLayout::LabelRole, label_2);

        frame_2 = new QFrame(scrollAreaWidgetContents);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        sizePolicy1.setHeightForWidth(frame_2->sizePolicy().hasHeightForWidth());
        frame_2->setSizePolicy(sizePolicy1);
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(frame_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        detReadLabel = new QLabel(frame_2);
        detReadLabel->setObjectName(QStringLiteral("detReadLabel"));
        detReadLabel->setWordWrap(true);
        detReadLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_2->addWidget(detReadLabel);


        formLayout->setWidget(2, QFormLayout::FieldRole, frame_2);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(3, QFormLayout::LabelRole, label_3);

        frame_3 = new QFrame(scrollAreaWidgetContents);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        sizePolicy1.setHeightForWidth(frame_3->sizePolicy().hasHeightForWidth());
        frame_3->setSizePolicy(sizePolicy1);
        frame_3->setFrameShape(QFrame::Box);
        frame_3->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(frame_3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        detDefLabel = new QLabel(frame_3);
        detDefLabel->setObjectName(QStringLiteral("detDefLabel"));
        detDefLabel->setWordWrap(true);
        detDefLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_3->addWidget(detDefLabel);


        formLayout->setWidget(3, QFormLayout::FieldRole, frame_3);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        detAddButton = new QPushButton(WordDialogUI);
        detAddButton->setObjectName(QStringLiteral("detAddButton"));

        hboxLayout->addWidget(detAddButton);

        detCopyButton = new QPushButton(WordDialogUI);
        detCopyButton->setObjectName(QStringLiteral("detCopyButton"));

        hboxLayout->addWidget(detCopyButton);

        okButton = new QPushButton(WordDialogUI);
        okButton->setObjectName(QStringLiteral("okButton"));

        hboxLayout->addWidget(okButton);


        verticalLayout->addLayout(hboxLayout);


        retranslateUi(WordDialogUI);
        QObject::connect(okButton, SIGNAL(clicked()), WordDialogUI, SLOT(accept()));

        QMetaObject::connectSlotsByName(WordDialogUI);
    } // setupUi

    void retranslateUi(QDialog *WordDialogUI)
    {
        WordDialogUI->setWindowTitle(QApplication::translate("WordDialogUI", "Word details", 0));
        label->setText(QApplication::translate("WordDialogUI", "Word:", 0));
        catLabel->setText(QApplication::translate("WordDialogUI", "Categories:", 0));
        detCatLabel->setText(QApplication::translate("WordDialogUI", "TextLabel", 0));
        label_2->setText(QApplication::translate("WordDialogUI", "Reading:", 0));
        detReadLabel->setText(QApplication::translate("WordDialogUI", "TextLabel", 0));
        label_3->setText(QApplication::translate("WordDialogUI", "Meaning:", 0));
        detDefLabel->setText(QApplication::translate("WordDialogUI", "TextLabel", 0));
#ifndef QT_NO_TOOLTIP
        detAddButton->setToolTip(QApplication::translate("WordDialogUI", "Add word to current study database.", 0));
#endif // QT_NO_TOOLTIP
        detAddButton->setText(QApplication::translate("WordDialogUI", "&Store", 0));
#ifndef QT_NO_TOOLTIP
        detCopyButton->setToolTip(QApplication::translate("WordDialogUI", "Copy word to clipboard.", 0));
#endif // QT_NO_TOOLTIP
        detCopyButton->setText(QApplication::translate("WordDialogUI", "&Copy", 0));
        okButton->setText(QApplication::translate("WordDialogUI", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class WordDialogUI: public Ui_WordDialogUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORD_DIALOG_H
