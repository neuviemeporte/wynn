/********************************************************************************
** Form generated from reading UI file 'kanji_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KANJI_DIALOG_H
#define UI_KANJI_DIALOG_H

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

class Ui_KanjiDialogUI
{
public:
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QFormLayout *formLayout_4;
    QLabel *label_6;
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QLabel *detCharLabel;
    QFormLayout *formLayout_2;
    QLabel *simpleHeader;
    QFrame *simpleFrame;
    QVBoxLayout *verticalLayout_4;
    QLabel *simpleLabel;
    QFormLayout *formLayout_3;
    QLabel *traditionalHeader;
    QFrame *traditionalFrame;
    QVBoxLayout *verticalLayout_5;
    QLabel *traditionalLabel;
    QFormLayout *formLayout;
    QLabel *label_2;
    QLabel *detStrokeLabel;
    QLabel *label_3;
    QLabel *detRadLabel;
    QSpacerItem *horizontalSpacer;
    QLabel *label_7;
    QFrame *frame_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *detReadLabel;
    QLabel *label_8;
    QFrame *frame_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *detDefLabel;
    QHBoxLayout *hboxLayout;
    QSpacerItem *spacerItem;
    QPushButton *detAddButton;
    QPushButton *detCopyButton;
    QPushButton *detCloseButton;

    void setupUi(QDialog *KanjiDialogUI)
    {
        if (KanjiDialogUI->objectName().isEmpty())
            KanjiDialogUI->setObjectName(QStringLiteral("KanjiDialogUI"));
        KanjiDialogUI->resize(493, 349);
        KanjiDialogUI->setModal(true);
        verticalLayout_2 = new QVBoxLayout(KanjiDialogUI);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        scrollArea = new QScrollArea(KanjiDialogUI);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 471, 288));
        formLayout_4 = new QFormLayout(scrollAreaWidgetContents);
        formLayout_4->setObjectName(QStringLiteral("formLayout_4"));
        label_6 = new QLabel(scrollAreaWidgetContents);
        label_6->setObjectName(QStringLiteral("label_6"));

        formLayout_4->setWidget(0, QFormLayout::LabelRole, label_6);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        frame = new QFrame(scrollAreaWidgetContents);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        frame->setLineWidth(1);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        detCharLabel = new QLabel(frame);
        detCharLabel->setObjectName(QStringLiteral("detCharLabel"));

        verticalLayout->addWidget(detCharLabel);


        horizontalLayout->addWidget(frame);

        formLayout_2 = new QFormLayout();
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        simpleHeader = new QLabel(scrollAreaWidgetContents);
        simpleHeader->setObjectName(QStringLiteral("simpleHeader"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, simpleHeader);

        simpleFrame = new QFrame(scrollAreaWidgetContents);
        simpleFrame->setObjectName(QStringLiteral("simpleFrame"));
        simpleFrame->setFrameShape(QFrame::Box);
        simpleFrame->setFrameShadow(QFrame::Sunken);
        simpleFrame->setLineWidth(1);
        verticalLayout_4 = new QVBoxLayout(simpleFrame);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        simpleLabel = new QLabel(simpleFrame);
        simpleLabel->setObjectName(QStringLiteral("simpleLabel"));

        verticalLayout_4->addWidget(simpleLabel);


        formLayout_2->setWidget(0, QFormLayout::FieldRole, simpleFrame);


        horizontalLayout->addLayout(formLayout_2);

        formLayout_3 = new QFormLayout();
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        traditionalHeader = new QLabel(scrollAreaWidgetContents);
        traditionalHeader->setObjectName(QStringLiteral("traditionalHeader"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, traditionalHeader);

        traditionalFrame = new QFrame(scrollAreaWidgetContents);
        traditionalFrame->setObjectName(QStringLiteral("traditionalFrame"));
        traditionalFrame->setFrameShape(QFrame::Box);
        traditionalFrame->setFrameShadow(QFrame::Sunken);
        traditionalFrame->setLineWidth(1);
        verticalLayout_5 = new QVBoxLayout(traditionalFrame);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        traditionalLabel = new QLabel(traditionalFrame);
        traditionalLabel->setObjectName(QStringLiteral("traditionalLabel"));

        verticalLayout_5->addWidget(traditionalLabel);


        formLayout_3->setWidget(0, QFormLayout::FieldRole, traditionalFrame);


        horizontalLayout->addLayout(formLayout_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QStringLiteral("formLayout"));
        formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        label_2 = new QLabel(scrollAreaWidgetContents);
        label_2->setObjectName(QStringLiteral("label_2"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_2);

        detStrokeLabel = new QLabel(scrollAreaWidgetContents);
        detStrokeLabel->setObjectName(QStringLiteral("detStrokeLabel"));

        formLayout->setWidget(0, QFormLayout::FieldRole, detStrokeLabel);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));

        formLayout->setWidget(1, QFormLayout::LabelRole, label_3);

        detRadLabel = new QLabel(scrollAreaWidgetContents);
        detRadLabel->setObjectName(QStringLiteral("detRadLabel"));

        formLayout->setWidget(1, QFormLayout::FieldRole, detRadLabel);


        horizontalLayout->addLayout(formLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        formLayout_4->setLayout(0, QFormLayout::FieldRole, horizontalLayout);

        label_7 = new QLabel(scrollAreaWidgetContents);
        label_7->setObjectName(QStringLiteral("label_7"));

        formLayout_4->setWidget(1, QFormLayout::LabelRole, label_7);

        frame_2 = new QFrame(scrollAreaWidgetContents);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::Box);
        frame_2->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(frame_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        detReadLabel = new QLabel(frame_2);
        detReadLabel->setObjectName(QStringLiteral("detReadLabel"));
        detReadLabel->setWordWrap(true);
        detReadLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_2->addWidget(detReadLabel);


        formLayout_4->setWidget(1, QFormLayout::FieldRole, frame_2);

        label_8 = new QLabel(scrollAreaWidgetContents);
        label_8->setObjectName(QStringLiteral("label_8"));

        formLayout_4->setWidget(2, QFormLayout::LabelRole, label_8);

        frame_3 = new QFrame(scrollAreaWidgetContents);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setFrameShape(QFrame::Box);
        frame_3->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(frame_3);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        detDefLabel = new QLabel(frame_3);
        detDefLabel->setObjectName(QStringLiteral("detDefLabel"));
        detDefLabel->setWordWrap(true);
        detDefLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_4->addWidget(detDefLabel);


        formLayout_4->setWidget(2, QFormLayout::FieldRole, frame_3);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);

        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
        hboxLayout->setContentsMargins(0, 0, 0, 0);
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        spacerItem = new QSpacerItem(131, 31, QSizePolicy::Expanding, QSizePolicy::Minimum);

        hboxLayout->addItem(spacerItem);

        detAddButton = new QPushButton(KanjiDialogUI);
        detAddButton->setObjectName(QStringLiteral("detAddButton"));

        hboxLayout->addWidget(detAddButton);

        detCopyButton = new QPushButton(KanjiDialogUI);
        detCopyButton->setObjectName(QStringLiteral("detCopyButton"));

        hboxLayout->addWidget(detCopyButton);

        detCloseButton = new QPushButton(KanjiDialogUI);
        detCloseButton->setObjectName(QStringLiteral("detCloseButton"));

        hboxLayout->addWidget(detCloseButton);


        verticalLayout_2->addLayout(hboxLayout);


        retranslateUi(KanjiDialogUI);
        QObject::connect(detCloseButton, SIGNAL(clicked()), KanjiDialogUI, SLOT(accept()));

        QMetaObject::connectSlotsByName(KanjiDialogUI);
    } // setupUi

    void retranslateUi(QDialog *KanjiDialogUI)
    {
        KanjiDialogUI->setWindowTitle(QApplication::translate("KanjiDialogUI", "Character details", 0));
        label_6->setText(QApplication::translate("KanjiDialogUI", "Character:", 0));
        detCharLabel->setText(QApplication::translate("KanjiDialogUI", "A", 0));
        simpleHeader->setText(QApplication::translate("KanjiDialogUI", "Simplified:", 0));
        simpleLabel->setText(QApplication::translate("KanjiDialogUI", "B", 0));
        traditionalHeader->setText(QApplication::translate("KanjiDialogUI", "Traditional:", 0));
        traditionalLabel->setText(QApplication::translate("KanjiDialogUI", "C", 0));
        label_2->setText(QApplication::translate("KanjiDialogUI", "Strokes:", 0));
        detStrokeLabel->setText(QApplication::translate("KanjiDialogUI", "6", 0));
        label_3->setText(QApplication::translate("KanjiDialogUI", "Radicals:", 0));
        detRadLabel->setText(QApplication::translate("KanjiDialogUI", "ABCD", 0));
        label_7->setText(QApplication::translate("KanjiDialogUI", "Readings:", 0));
        detReadLabel->setText(QApplication::translate("KanjiDialogUI", "TextLabel", 0));
        label_8->setText(QApplication::translate("KanjiDialogUI", "Definition:", 0));
        detDefLabel->setText(QApplication::translate("KanjiDialogUI", "TextLabel", 0));
#ifndef QT_NO_TOOLTIP
        detAddButton->setToolTip(QApplication::translate("KanjiDialogUI", "Add character to study database.", 0));
#endif // QT_NO_TOOLTIP
        detAddButton->setText(QApplication::translate("KanjiDialogUI", "&Store", 0));
#ifndef QT_NO_TOOLTIP
        detCopyButton->setToolTip(QApplication::translate("KanjiDialogUI", "Copy character to clipboard.", 0));
#endif // QT_NO_TOOLTIP
        detCopyButton->setText(QApplication::translate("KanjiDialogUI", "&Copy", 0));
        detCloseButton->setText(QApplication::translate("KanjiDialogUI", "Clos&e", 0));
    } // retranslateUi

};

namespace Ui {
    class KanjiDialogUI: public Ui_KanjiDialogUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KANJI_DIALOG_H
