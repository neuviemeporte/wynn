/********************************************************************************
** Form generated from reading UI file 'quiz.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUIZ_H
#define UI_QUIZ_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_QuizDialog
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *questionLabel;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *answerLabel;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *correctButton;
    QPushButton *incorrectButton;
    QPushButton *unsureButton;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *progressLabel;
    QSpacerItem *horizontalSpacer;
    QPushButton *revealButton;
    QPushButton *closeButton;

    void setupUi(QDialog *QuizDialog)
    {
        if (QuizDialog->objectName().isEmpty())
            QuizDialog->setObjectName(QStringLiteral("QuizDialog"));
        QuizDialog->resize(481, 212);
        verticalLayout = new QVBoxLayout(QuizDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        groupBox = new QGroupBox(QuizDialog);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        questionLabel = new QLabel(groupBox);
        questionLabel->setObjectName(QStringLiteral("questionLabel"));
        questionLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout->addWidget(questionLabel);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(QuizDialog);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_2 = new QHBoxLayout(groupBox_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        answerLabel = new QLabel(groupBox_2);
        answerLabel->setObjectName(QStringLiteral("answerLabel"));
        answerLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByMouse);

        horizontalLayout_2->addWidget(answerLabel);


        verticalLayout->addWidget(groupBox_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        groupBox_3 = new QGroupBox(QuizDialog);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_3);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 3, -1, 6);
        correctButton = new QPushButton(groupBox_3);
        correctButton->setObjectName(QStringLiteral("correctButton"));

        horizontalLayout_3->addWidget(correctButton);

        incorrectButton = new QPushButton(groupBox_3);
        incorrectButton->setObjectName(QStringLiteral("incorrectButton"));

        horizontalLayout_3->addWidget(incorrectButton);

        unsureButton = new QPushButton(groupBox_3);
        unsureButton->setObjectName(QStringLiteral("unsureButton"));

        horizontalLayout_3->addWidget(unsureButton);


        horizontalLayout_5->addWidget(groupBox_3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        progressLabel = new QLabel(QuizDialog);
        progressLabel->setObjectName(QStringLiteral("progressLabel"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        progressLabel->setFont(font);

        horizontalLayout_4->addWidget(progressLabel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);

        revealButton = new QPushButton(QuizDialog);
        revealButton->setObjectName(QStringLiteral("revealButton"));

        horizontalLayout_4->addWidget(revealButton);

        closeButton = new QPushButton(QuizDialog);
        closeButton->setObjectName(QStringLiteral("closeButton"));

        horizontalLayout_4->addWidget(closeButton);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(QuizDialog);

        QMetaObject::connectSlotsByName(QuizDialog);
    } // setupUi

    void retranslateUi(QDialog *QuizDialog)
    {
        QuizDialog->setWindowTitle(QApplication::translate("QuizDialog", "Quiz", 0));
        groupBox->setTitle(QApplication::translate("QuizDialog", "Question", 0));
        questionLabel->setText(QApplication::translate("QuizDialog", "Something", 0));
        groupBox_2->setTitle(QApplication::translate("QuizDialog", "Answer", 0));
        answerLabel->setText(QApplication::translate("QuizDialog", "Something else", 0));
        groupBox_3->setTitle(QApplication::translate("QuizDialog", "Result", 0));
        correctButton->setText(QApplication::translate("QuizDialog", "Correct (A)", 0));
        incorrectButton->setText(QApplication::translate("QuizDialog", "Incorrect (S)", 0));
        unsureButton->setText(QApplication::translate("QuizDialog", "Unsure (D)", 0));
        progressLabel->setText(QApplication::translate("QuizDialog", "0/100", 0));
        revealButton->setText(QApplication::translate("QuizDialog", "Reveal (X)", 0));
        closeButton->setText(QApplication::translate("QuizDialog", "Close", 0));
    } // retranslateUi

};

namespace Ui {
    class QuizDialog: public Ui_QuizDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUIZ_H
