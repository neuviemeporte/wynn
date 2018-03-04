/********************************************************************************
** Form generated from reading UI file 'mainform.ui'
**
** Created by: Qt User Interface Compiler version 5.4.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFORM_H
#define UI_MAINFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainFormClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QFrame *mainFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *dictionaryButton;
    QPushButton *manageButton;
    QPushButton *settingsButton;
    QPushButton *aboutButton;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *databaseCombo;
    QPushButton *createButton;
    QPushButton *deleteButton;
    QFrame *stackFrame;
    QVBoxLayout *verticalLayout_12;
    QStackedWidget *stackedWidget;
    QWidget *pageDictionary;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QComboBox *langCombo;
    QPushButton *langAboutButton;
    QSpacerItem *horizontalSpacer_2;
    QStackedWidget *dictPanelStack;
    QWidget *page;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_5;
    QLineEdit *lineEdit;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_3;
    QTableView *dictionaryTable;
    QHBoxLayout *horizontalLayout_7;
    QSpacerItem *horizontalSpacer_13;
    QPushButton *dictDetailsButton;
    QPushButton *dictDatabaseButton;
    QWidget *pageManage;
    QVBoxLayout *verticalLayout_8;
    QHBoxLayout *horizontalLayout_20;
    QGroupBox *groupBox_13;
    QHBoxLayout *horizontalLayout_16;
    QGroupBox *quizDirectionGroupBox;
    QVBoxLayout *verticalLayout_6;
    QRadioButton *quizShowDescRadio;
    QRadioButton *quizShowItemRadio;
    QSpacerItem *verticalSpacer_4;
    QGroupBox *quizCriteriaGroupBox;
    QHBoxLayout *horizontalLayout_15;
    QFormLayout *formLayout_3;
    QCheckBox *quizRangeCheck;
    QSpinBox *quizRangeFromSpin;
    QHBoxLayout *horizontalLayout_8;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_3;
    QSpinBox *quizRangeToSpin;
    QFormLayout *formLayout_2;
    QHBoxLayout *horizontalLayout_9;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_4;
    QCheckBox *quizPointsCheck;
    QSpinBox *quizPointsFromSpin;
    QSpinBox *quizPointsToSpin;
    QVBoxLayout *verticalLayout_7;
    QHBoxLayout *horizontalLayout_10;
    QCheckBox *quizTakeCheck;
    QSpinBox *quizTakeSpin;
    QRadioButton *quizTakeRandomRadio;
    QHBoxLayout *horizontalLayout_11;
    QRadioButton *quizTakeFailRadio;
    QRadioButton *quizTakeOldieRadio;
    QPushButton *quizButton;
    QSpacerItem *horizontalSpacer_6;
    QTableView *databaseTable;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *dbaseAddButton;
    QPushButton *dbaseRemoveButton;
    QPushButton *dbaseCopyButton;
    QPushButton *dbaseMoveButton;
    QPushButton *dbaseEditButton;
    QPushButton *dbaseFindButton;
    QPushButton *dbaseExportButton;
    QPushButton *dbaseResetButton;
    QWidget *pageSettings;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_12;
    QVBoxLayout *verticalLayout_9;
    QHBoxLayout *horizontalLayout_19;
    QCheckBox *settExtDirCheck;
    QLineEdit *settExtDirEdit;
    QPushButton *settExtDirButton;
    QCheckBox *settNoDuplCheck;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_6;
    QComboBox *settPluginCombo;
    QSpacerItem *horizontalSpacer_8;
    QStackedWidget *settPluginStack;
    QWidget *page_3;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_2;
    QSpacerItem *horizontalSpacer_9;
    QSpacerItem *verticalSpacer;
    QWidget *pageAbout;
    QHBoxLayout *horizontalLayout_14;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_10;
    QLabel *splashLabel;
    QSpacerItem *horizontalSpacer_11;
    QFrame *frame;
    QVBoxLayout *verticalLayout_11;
    QLabel *label_9;
    QLabel *label_10;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_12;
    QLabel *label_11;
    QLabel *buildLabel;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainFormClass)
    {
        if (MainFormClass->objectName().isEmpty())
            MainFormClass->setObjectName(QStringLiteral("MainFormClass"));
        MainFormClass->setEnabled(true);
        MainFormClass->resize(915, 694);
        QIcon icon;
        icon.addFile(QStringLiteral(":/MainForm/gfx/wynn-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MainFormClass->setWindowIcon(icon);
        centralWidget = new QWidget(MainFormClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        mainFrame = new QFrame(centralWidget);
        mainFrame->setObjectName(QStringLiteral("mainFrame"));
        mainFrame->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(mainFrame->sizePolicy().hasHeightForWidth());
        mainFrame->setSizePolicy(sizePolicy);
        mainFrame->setFrameShape(QFrame::Box);
        mainFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(mainFrame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        dictionaryButton = new QPushButton(mainFrame);
        dictionaryButton->setObjectName(QStringLiteral("dictionaryButton"));

        horizontalLayout->addWidget(dictionaryButton);

        manageButton = new QPushButton(mainFrame);
        manageButton->setObjectName(QStringLiteral("manageButton"));

        horizontalLayout->addWidget(manageButton);

        settingsButton = new QPushButton(mainFrame);
        settingsButton->setObjectName(QStringLiteral("settingsButton"));

        horizontalLayout->addWidget(settingsButton);

        aboutButton = new QPushButton(mainFrame);
        aboutButton->setObjectName(QStringLiteral("aboutButton"));

        horizontalLayout->addWidget(aboutButton);

        horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(mainFrame);
        label->setObjectName(QStringLiteral("label"));
        label->setFrameShadow(QFrame::Raised);
        label->setWordWrap(true);

        horizontalLayout->addWidget(label);

        databaseCombo = new QComboBox(mainFrame);
        databaseCombo->setObjectName(QStringLiteral("databaseCombo"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(databaseCombo->sizePolicy().hasHeightForWidth());
        databaseCombo->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(databaseCombo);

        createButton = new QPushButton(mainFrame);
        createButton->setObjectName(QStringLiteral("createButton"));

        horizontalLayout->addWidget(createButton);

        deleteButton = new QPushButton(mainFrame);
        deleteButton->setObjectName(QStringLiteral("deleteButton"));

        horizontalLayout->addWidget(deleteButton);


        verticalLayout->addWidget(mainFrame);

        stackFrame = new QFrame(centralWidget);
        stackFrame->setObjectName(QStringLiteral("stackFrame"));
        stackFrame->setFrameShape(QFrame::Box);
        stackFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_12 = new QVBoxLayout(stackFrame);
        verticalLayout_12->setSpacing(3);
        verticalLayout_12->setContentsMargins(0, 0, 0, 0);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        stackedWidget = new QStackedWidget(stackFrame);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setEnabled(true);
        stackedWidget->setContextMenuPolicy(Qt::DefaultContextMenu);
        stackedWidget->setLayoutDirection(Qt::LeftToRight);
        stackedWidget->setAutoFillBackground(false);
        pageDictionary = new QWidget();
        pageDictionary->setObjectName(QStringLiteral("pageDictionary"));
        verticalLayout_2 = new QVBoxLayout(pageDictionary);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(pageDictionary);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        langCombo = new QComboBox(pageDictionary);
        langCombo->setObjectName(QStringLiteral("langCombo"));

        horizontalLayout_2->addWidget(langCombo);

        langAboutButton = new QPushButton(pageDictionary);
        langAboutButton->setObjectName(QStringLiteral("langAboutButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(langAboutButton->sizePolicy().hasHeightForWidth());
        langAboutButton->setSizePolicy(sizePolicy2);

        horizontalLayout_2->addWidget(langAboutButton);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);

        dictPanelStack = new QStackedWidget(pageDictionary);
        dictPanelStack->setObjectName(QStringLiteral("dictPanelStack"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(dictPanelStack->sizePolicy().hasHeightForWidth());
        dictPanelStack->setSizePolicy(sizePolicy3);
        page = new QWidget();
        page->setObjectName(QStringLiteral("page"));
        horizontalLayout_3 = new QHBoxLayout(page);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_5 = new QLabel(page);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_3->addWidget(label_5);

        lineEdit = new QLineEdit(page);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_3->addWidget(lineEdit);

        pushButton = new QPushButton(page);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_3->addWidget(pushButton);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);

        dictPanelStack->addWidget(page);

        verticalLayout_2->addWidget(dictPanelStack);

        dictionaryTable = new QTableView(pageDictionary);
        dictionaryTable->setObjectName(QStringLiteral("dictionaryTable"));
        dictionaryTable->setSelectionMode(QAbstractItemView::SingleSelection);
        dictionaryTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        dictionaryTable->setCornerButtonEnabled(false);
        dictionaryTable->horizontalHeader()->setHighlightSections(false);
        dictionaryTable->horizontalHeader()->setStretchLastSection(true);

        verticalLayout_2->addWidget(dictionaryTable);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_13);

        dictDetailsButton = new QPushButton(pageDictionary);
        dictDetailsButton->setObjectName(QStringLiteral("dictDetailsButton"));

        horizontalLayout_7->addWidget(dictDetailsButton);

        dictDatabaseButton = new QPushButton(pageDictionary);
        dictDatabaseButton->setObjectName(QStringLiteral("dictDatabaseButton"));

        horizontalLayout_7->addWidget(dictDatabaseButton);


        verticalLayout_2->addLayout(horizontalLayout_7);

        stackedWidget->addWidget(pageDictionary);
        pageManage = new QWidget();
        pageManage->setObjectName(QStringLiteral("pageManage"));
        verticalLayout_8 = new QVBoxLayout(pageManage);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setSpacing(6);
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        groupBox_13 = new QGroupBox(pageManage);
        groupBox_13->setObjectName(QStringLiteral("groupBox_13"));
        groupBox_13->setFlat(false);
        groupBox_13->setCheckable(false);
        horizontalLayout_16 = new QHBoxLayout(groupBox_13);
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalLayout_16->setContentsMargins(-1, 3, -1, -1);
        quizDirectionGroupBox = new QGroupBox(groupBox_13);
        quizDirectionGroupBox->setObjectName(QStringLiteral("quizDirectionGroupBox"));
        verticalLayout_6 = new QVBoxLayout(quizDirectionGroupBox);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(-1, 6, -1, 6);
        quizShowDescRadio = new QRadioButton(quizDirectionGroupBox);
        quizShowDescRadio->setObjectName(QStringLiteral("quizShowDescRadio"));
        quizShowDescRadio->setChecked(true);

        verticalLayout_6->addWidget(quizShowDescRadio);

        quizShowItemRadio = new QRadioButton(quizDirectionGroupBox);
        quizShowItemRadio->setObjectName(QStringLiteral("quizShowItemRadio"));
        quizShowItemRadio->setChecked(false);

        verticalLayout_6->addWidget(quizShowItemRadio);

        verticalSpacer_4 = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Maximum);

        verticalLayout_6->addItem(verticalSpacer_4);


        horizontalLayout_16->addWidget(quizDirectionGroupBox);

        quizCriteriaGroupBox = new QGroupBox(groupBox_13);
        quizCriteriaGroupBox->setObjectName(QStringLiteral("quizCriteriaGroupBox"));
        horizontalLayout_15 = new QHBoxLayout(quizCriteriaGroupBox);
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 6, -1, 6);
        formLayout_3 = new QFormLayout();
        formLayout_3->setSpacing(6);
        formLayout_3->setObjectName(QStringLiteral("formLayout_3"));
        quizRangeCheck = new QCheckBox(quizCriteriaGroupBox);
        quizRangeCheck->setObjectName(QStringLiteral("quizRangeCheck"));

        formLayout_3->setWidget(0, QFormLayout::LabelRole, quizRangeCheck);

        quizRangeFromSpin = new QSpinBox(quizCriteriaGroupBox);
        quizRangeFromSpin->setObjectName(QStringLiteral("quizRangeFromSpin"));
        quizRangeFromSpin->setEnabled(false);
        quizRangeFromSpin->setMinimum(0);
        quizRangeFromSpin->setMaximum(0);

        formLayout_3->setWidget(0, QFormLayout::FieldRole, quizRangeFromSpin);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        horizontalSpacer_4 = new QSpacerItem(63, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_4);

        label_3 = new QLabel(quizCriteriaGroupBox);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_8->addWidget(label_3);


        formLayout_3->setLayout(1, QFormLayout::LabelRole, horizontalLayout_8);

        quizRangeToSpin = new QSpinBox(quizCriteriaGroupBox);
        quizRangeToSpin->setObjectName(QStringLiteral("quizRangeToSpin"));
        quizRangeToSpin->setEnabled(false);
        quizRangeToSpin->setMinimum(0);
        quizRangeToSpin->setMaximum(0);

        formLayout_3->setWidget(1, QFormLayout::FieldRole, quizRangeToSpin);


        horizontalLayout_15->addLayout(formLayout_3);

        formLayout_2 = new QFormLayout();
        formLayout_2->setSpacing(6);
        formLayout_2->setObjectName(QStringLiteral("formLayout_2"));
        formLayout_2->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        horizontalSpacer_5 = new QSpacerItem(57, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_5);

        label_4 = new QLabel(quizCriteriaGroupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_9->addWidget(label_4);


        formLayout_2->setLayout(1, QFormLayout::LabelRole, horizontalLayout_9);

        quizPointsCheck = new QCheckBox(quizCriteriaGroupBox);
        quizPointsCheck->setObjectName(QStringLiteral("quizPointsCheck"));

        formLayout_2->setWidget(0, QFormLayout::LabelRole, quizPointsCheck);

        quizPointsFromSpin = new QSpinBox(quizCriteriaGroupBox);
        quizPointsFromSpin->setObjectName(QStringLiteral("quizPointsFromSpin"));
        quizPointsFromSpin->setEnabled(false);
        quizPointsFromSpin->setMaximum(100);

        formLayout_2->setWidget(0, QFormLayout::FieldRole, quizPointsFromSpin);

        quizPointsToSpin = new QSpinBox(quizCriteriaGroupBox);
        quizPointsToSpin->setObjectName(QStringLiteral("quizPointsToSpin"));
        quizPointsToSpin->setEnabled(false);
        quizPointsToSpin->setMaximum(10);
        quizPointsToSpin->setValue(10);

        formLayout_2->setWidget(1, QFormLayout::FieldRole, quizPointsToSpin);


        horizontalLayout_15->addLayout(formLayout_2);

        verticalLayout_7 = new QVBoxLayout();
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        quizTakeCheck = new QCheckBox(quizCriteriaGroupBox);
        quizTakeCheck->setObjectName(QStringLiteral("quizTakeCheck"));

        horizontalLayout_10->addWidget(quizTakeCheck);

        quizTakeSpin = new QSpinBox(quizCriteriaGroupBox);
        quizTakeSpin->setObjectName(QStringLiteral("quizTakeSpin"));
        quizTakeSpin->setEnabled(false);
        quizTakeSpin->setMinimumSize(QSize(55, 20));
        quizTakeSpin->setMaximumSize(QSize(55, 20));
        quizTakeSpin->setMaximum(0);

        horizontalLayout_10->addWidget(quizTakeSpin);

        quizTakeRandomRadio = new QRadioButton(quizCriteriaGroupBox);
        quizTakeRandomRadio->setObjectName(QStringLiteral("quizTakeRandomRadio"));
        quizTakeRandomRadio->setEnabled(false);

        horizontalLayout_10->addWidget(quizTakeRandomRadio);


        verticalLayout_7->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        quizTakeFailRadio = new QRadioButton(quizCriteriaGroupBox);
        quizTakeFailRadio->setObjectName(QStringLiteral("quizTakeFailRadio"));
        quizTakeFailRadio->setEnabled(false);
        quizTakeFailRadio->setChecked(true);

        horizontalLayout_11->addWidget(quizTakeFailRadio);

        quizTakeOldieRadio = new QRadioButton(quizCriteriaGroupBox);
        quizTakeOldieRadio->setObjectName(QStringLiteral("quizTakeOldieRadio"));
        quizTakeOldieRadio->setEnabled(false);

        horizontalLayout_11->addWidget(quizTakeOldieRadio);


        verticalLayout_7->addLayout(horizontalLayout_11);


        horizontalLayout_15->addLayout(verticalLayout_7);


        horizontalLayout_16->addWidget(quizCriteriaGroupBox);

        quizButton = new QPushButton(groupBox_13);
        quizButton->setObjectName(QStringLiteral("quizButton"));
        quizButton->setMinimumSize(QSize(51, 77));
        quizButton->setMaximumSize(QSize(51, 70));

        horizontalLayout_16->addWidget(quizButton);


        horizontalLayout_20->addWidget(groupBox_13);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_20->addItem(horizontalSpacer_6);


        verticalLayout_8->addLayout(horizontalLayout_20);

        databaseTable = new QTableView(pageManage);
        databaseTable->setObjectName(QStringLiteral("databaseTable"));
        databaseTable->setTabKeyNavigation(false);
        databaseTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
        databaseTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        databaseTable->setSortingEnabled(true);
        databaseTable->setCornerButtonEnabled(false);
        databaseTable->horizontalHeader()->setHighlightSections(false);
        databaseTable->horizontalHeader()->setProperty("showSortIndicator", QVariant(true));
        databaseTable->horizontalHeader()->setStretchLastSection(true);
        databaseTable->verticalHeader()->setVisible(false);

        verticalLayout_8->addWidget(databaseTable);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_7);

        dbaseAddButton = new QPushButton(pageManage);
        dbaseAddButton->setObjectName(QStringLiteral("dbaseAddButton"));

        horizontalLayout_13->addWidget(dbaseAddButton);

        dbaseRemoveButton = new QPushButton(pageManage);
        dbaseRemoveButton->setObjectName(QStringLiteral("dbaseRemoveButton"));

        horizontalLayout_13->addWidget(dbaseRemoveButton);

        dbaseCopyButton = new QPushButton(pageManage);
        dbaseCopyButton->setObjectName(QStringLiteral("dbaseCopyButton"));

        horizontalLayout_13->addWidget(dbaseCopyButton);

        dbaseMoveButton = new QPushButton(pageManage);
        dbaseMoveButton->setObjectName(QStringLiteral("dbaseMoveButton"));

        horizontalLayout_13->addWidget(dbaseMoveButton);

        dbaseEditButton = new QPushButton(pageManage);
        dbaseEditButton->setObjectName(QStringLiteral("dbaseEditButton"));

        horizontalLayout_13->addWidget(dbaseEditButton);

        dbaseFindButton = new QPushButton(pageManage);
        dbaseFindButton->setObjectName(QStringLiteral("dbaseFindButton"));

        horizontalLayout_13->addWidget(dbaseFindButton);

        dbaseExportButton = new QPushButton(pageManage);
        dbaseExportButton->setObjectName(QStringLiteral("dbaseExportButton"));

        horizontalLayout_13->addWidget(dbaseExportButton);

        dbaseResetButton = new QPushButton(pageManage);
        dbaseResetButton->setObjectName(QStringLiteral("dbaseResetButton"));

        horizontalLayout_13->addWidget(dbaseResetButton);


        verticalLayout_8->addLayout(horizontalLayout_13);

        stackedWidget->addWidget(pageManage);
        pageSettings = new QWidget();
        pageSettings->setObjectName(QStringLiteral("pageSettings"));
        verticalLayout_5 = new QVBoxLayout(pageSettings);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox_12 = new QGroupBox(pageSettings);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        verticalLayout_9 = new QVBoxLayout(groupBox_12);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setSpacing(6);
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        settExtDirCheck = new QCheckBox(groupBox_12);
        settExtDirCheck->setObjectName(QStringLiteral("settExtDirCheck"));

        horizontalLayout_19->addWidget(settExtDirCheck);

        settExtDirEdit = new QLineEdit(groupBox_12);
        settExtDirEdit->setObjectName(QStringLiteral("settExtDirEdit"));
        settExtDirEdit->setEnabled(false);
        settExtDirEdit->setMinimumSize(QSize(200, 0));
        settExtDirEdit->setReadOnly(true);

        horizontalLayout_19->addWidget(settExtDirEdit);

        settExtDirButton = new QPushButton(groupBox_12);
        settExtDirButton->setObjectName(QStringLiteral("settExtDirButton"));
        settExtDirButton->setEnabled(false);

        horizontalLayout_19->addWidget(settExtDirButton);


        verticalLayout_9->addLayout(horizontalLayout_19);

        settNoDuplCheck = new QCheckBox(groupBox_12);
        settNoDuplCheck->setObjectName(QStringLiteral("settNoDuplCheck"));
        settNoDuplCheck->setChecked(false);

        verticalLayout_9->addWidget(settNoDuplCheck);


        verticalLayout_5->addWidget(groupBox_12);

        groupBox = new QGroupBox(pageSettings);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_3 = new QVBoxLayout(groupBox);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_4->addWidget(label_6);

        settPluginCombo = new QComboBox(groupBox);
        settPluginCombo->setObjectName(QStringLiteral("settPluginCombo"));

        horizontalLayout_4->addWidget(settPluginCombo);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_8);


        verticalLayout_3->addLayout(horizontalLayout_4);

        settPluginStack = new QStackedWidget(groupBox);
        settPluginStack->setObjectName(QStringLiteral("settPluginStack"));
        sizePolicy3.setHeightForWidth(settPluginStack->sizePolicy().hasHeightForWidth());
        settPluginStack->setSizePolicy(sizePolicy3);
        page_3 = new QWidget();
        page_3->setObjectName(QStringLiteral("page_3"));
        verticalLayout_4 = new QVBoxLayout(page_3);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        pushButton_2 = new QPushButton(page_3);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));

        horizontalLayout_5->addWidget(pushButton_2);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_9);


        verticalLayout_4->addLayout(horizontalLayout_5);

        settPluginStack->addWidget(page_3);

        verticalLayout_3->addWidget(settPluginStack);


        verticalLayout_5->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 228, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);

        stackedWidget->addWidget(pageSettings);
        pageAbout = new QWidget();
        pageAbout->setObjectName(QStringLiteral("pageAbout"));
        horizontalLayout_14 = new QHBoxLayout(pageAbout);
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_10);

        splashLabel = new QLabel(pageAbout);
        splashLabel->setObjectName(QStringLiteral("splashLabel"));
        splashLabel->setPixmap(QPixmap(QString::fromUtf8(":/MainForm/gfx/wynn.png")));
        splashLabel->setScaledContents(false);

        horizontalLayout_12->addWidget(splashLabel);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_11);


        horizontalLayout_14->addLayout(horizontalLayout_12);

        frame = new QFrame(pageAbout);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        verticalLayout_11 = new QVBoxLayout(frame);
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setContentsMargins(11, 11, 11, 11);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        label_9 = new QLabel(frame);
        label_9->setObjectName(QStringLiteral("label_9"));

        verticalLayout_11->addWidget(label_9);

        label_10 = new QLabel(frame);
        label_10->setObjectName(QStringLiteral("label_10"));

        verticalLayout_11->addWidget(label_10);

        verticalSpacer_2 = new QSpacerItem(20, 268, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_11->addItem(verticalSpacer_2);

        label_12 = new QLabel(frame);
        label_12->setObjectName(QStringLiteral("label_12"));

        verticalLayout_11->addWidget(label_12);

        label_11 = new QLabel(frame);
        label_11->setObjectName(QStringLiteral("label_11"));

        verticalLayout_11->addWidget(label_11);

        buildLabel = new QLabel(frame);
        buildLabel->setObjectName(QStringLiteral("buildLabel"));

        verticalLayout_11->addWidget(buildLabel);


        horizontalLayout_14->addWidget(frame);

        stackedWidget->addWidget(pageAbout);

        verticalLayout_12->addWidget(stackedWidget);


        verticalLayout->addWidget(stackFrame);

        MainFormClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainFormClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainFormClass->setStatusBar(statusBar);

        retranslateUi(MainFormClass);
        QObject::connect(quizRangeCheck, SIGNAL(toggled(bool)), quizRangeFromSpin, SLOT(setEnabled(bool)));
        QObject::connect(quizRangeCheck, SIGNAL(toggled(bool)), quizRangeToSpin, SLOT(setEnabled(bool)));
        QObject::connect(quizTakeCheck, SIGNAL(toggled(bool)), quizTakeSpin, SLOT(setEnabled(bool)));
        QObject::connect(quizTakeCheck, SIGNAL(toggled(bool)), quizTakeFailRadio, SLOT(setEnabled(bool)));
        QObject::connect(quizTakeCheck, SIGNAL(toggled(bool)), quizTakeOldieRadio, SLOT(setEnabled(bool)));
        QObject::connect(quizPointsCheck, SIGNAL(toggled(bool)), quizPointsFromSpin, SLOT(setEnabled(bool)));
        QObject::connect(quizPointsCheck, SIGNAL(toggled(bool)), quizPointsToSpin, SLOT(setEnabled(bool)));
        QObject::connect(quizTakeCheck, SIGNAL(toggled(bool)), quizTakeRandomRadio, SLOT(setEnabled(bool)));
        QObject::connect(settExtDirCheck, SIGNAL(toggled(bool)), settExtDirEdit, SLOT(setEnabled(bool)));
        QObject::connect(settExtDirCheck, SIGNAL(toggled(bool)), settExtDirButton, SLOT(setEnabled(bool)));

        databaseCombo->setCurrentIndex(-1);
        stackedWidget->setCurrentIndex(3);
        dictPanelStack->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainFormClass);
    } // setupUi

    void retranslateUi(QMainWindow *MainFormClass)
    {
        MainFormClass->setWindowTitle(QApplication::translate("MainFormClass", "Wynn", 0));
        dictionaryButton->setText(QApplication::translate("MainFormClass", "&Dictionary", 0));
        manageButton->setText(QApplication::translate("MainFormClass", "Data&base", 0));
        settingsButton->setText(QApplication::translate("MainFormClass", "Se&ttings", 0));
        aboutButton->setText(QApplication::translate("MainFormClass", "About", 0));
        label->setText(QApplication::translate("MainFormClass", "Database:", 0));
        createButton->setText(QApplication::translate("MainFormClass", "Create", 0));
        deleteButton->setText(QApplication::translate("MainFormClass", "Delete", 0));
        label_2->setText(QApplication::translate("MainFormClass", "Language:", 0));
        langCombo->clear();
        langCombo->insertItems(0, QStringList()
         << QApplication::translate("MainFormClass", "Klingon", 0)
        );
        langAboutButton->setText(QApplication::translate("MainFormClass", "About", 0));
        label_5->setText(QApplication::translate("MainFormClass", "Word:", 0));
        pushButton->setText(QApplication::translate("MainFormClass", "Search", 0));
#ifndef QT_NO_TOOLTIP
        dictDetailsButton->setToolTip(QApplication::translate("MainFormClass", "Show details for item. Also available by double-clicking an item inside the table.", 0));
#endif // QT_NO_TOOLTIP
        dictDetailsButton->setText(QApplication::translate("MainFormClass", "D&etails", 0));
#ifndef QT_NO_TOOLTIP
        dictDatabaseButton->setToolTip(QApplication::translate("MainFormClass", "Add selected item to current study database.", 0));
#endif // QT_NO_TOOLTIP
        dictDatabaseButton->setText(QApplication::translate("MainFormClass", "&Store", 0));
        groupBox_13->setTitle(QApplication::translate("MainFormClass", "Quiz", 0));
        quizDirectionGroupBox->setTitle(QApplication::translate("MainFormClass", "Direction", 0));
#ifndef QT_NO_TOOLTIP
        quizShowDescRadio->setToolTip(QApplication::translate("MainFormClass", "The item will be shown and you'll have to guess the description.", 0));
#endif // QT_NO_TOOLTIP
        quizShowDescRadio->setText(QApplication::translate("MainFormClass", "show description", 0));
#ifndef QT_NO_TOOLTIP
        quizShowItemRadio->setToolTip(QApplication::translate("MainFormClass", "The description will be shown and you'll have to guess the item.", 0));
#endif // QT_NO_TOOLTIP
        quizShowItemRadio->setText(QApplication::translate("MainFormClass", "show item", 0));
        quizCriteriaGroupBox->setTitle(QApplication::translate("MainFormClass", "Criteria", 0));
#ifndef QT_NO_TOOLTIP
        quizRangeCheck->setToolTip(QApplication::translate("MainFormClass", "Select items by their index in the database.", 0));
#endif // QT_NO_TOOLTIP
        quizRangeCheck->setText(QApplication::translate("MainFormClass", "Index from:", 0));
        label_3->setText(QApplication::translate("MainFormClass", "to:", 0));
        label_4->setText(QApplication::translate("MainFormClass", "to:", 0));
#ifndef QT_NO_TOOLTIP
        quizPointsCheck->setToolTip(QApplication::translate("MainFormClass", "Select items by their level.", 0));
#endif // QT_NO_TOOLTIP
        quizPointsCheck->setText(QApplication::translate("MainFormClass", "Level from:", 0));
#ifndef QT_NO_TOOLTIP
        quizTakeCheck->setToolTip(QApplication::translate("MainFormClass", "Select items by failures or test date criterium.", 0));
#endif // QT_NO_TOOLTIP
        quizTakeCheck->setText(QApplication::translate("MainFormClass", "Take:", 0));
#ifndef QT_NO_TOOLTIP
        quizTakeRandomRadio->setToolTip(QApplication::translate("MainFormClass", "Select a number of random questions from the database.", 0));
#endif // QT_NO_TOOLTIP
        quizTakeRandomRadio->setText(QApplication::translate("MainFormClass", "random", 0));
#ifndef QT_NO_TOOLTIP
        quizTakeFailRadio->setToolTip(QApplication::translate("MainFormClass", "Select items with the most failures. Items with zero failures will not be included.", 0));
#endif // QT_NO_TOOLTIP
        quizTakeFailRadio->setText(QApplication::translate("MainFormClass", "most failures", 0));
#ifndef QT_NO_TOOLTIP
        quizTakeOldieRadio->setToolTip(QApplication::translate("MainFormClass", "Select items which have the oldest last quiz date.", 0));
#endif // QT_NO_TOOLTIP
        quizTakeOldieRadio->setText(QApplication::translate("MainFormClass", "oldest test date", 0));
        quizButton->setText(QApplication::translate("MainFormClass", "Start\n"
"quiz", 0));
        dbaseAddButton->setText(QApplication::translate("MainFormClass", "&Add", 0));
        dbaseRemoveButton->setText(QApplication::translate("MainFormClass", "&Remove", 0));
        dbaseCopyButton->setText(QApplication::translate("MainFormClass", "&Copy", 0));
        dbaseMoveButton->setText(QApplication::translate("MainFormClass", "&Move", 0));
        dbaseEditButton->setText(QApplication::translate("MainFormClass", "&Edit", 0));
        dbaseFindButton->setText(QApplication::translate("MainFormClass", "&Find", 0));
        dbaseExportButton->setText(QApplication::translate("MainFormClass", "E&xport", 0));
        dbaseResetButton->setText(QApplication::translate("MainFormClass", "Reset", 0));
        groupBox_12->setTitle(QApplication::translate("MainFormClass", "Databases", 0));
#ifndef QT_NO_TOOLTIP
        settExtDirCheck->setToolTip(QApplication::translate("MainFormClass", "Use an alternate directory to keep the databases.\n"
"Otherwise the default ('.wynn' under the user's\n"
"home directory) will be used. Makes it easy to put\n"
"the deatabases on a removable drive for easy\n"
"synchronization between machines. I the media is\n"
"unavailable, the default directory will be reverted to.", 0));
#endif // QT_NO_TOOLTIP
        settExtDirCheck->setText(QApplication::translate("MainFormClass", "External location", 0));
        settExtDirButton->setText(QApplication::translate("MainFormClass", "Browse...", 0));
#ifndef QT_NO_TOOLTIP
        settNoDuplCheck->setToolTip(QApplication::translate("MainFormClass", "User databases will guard against duplicate items.", 0));
#endif // QT_NO_TOOLTIP
        settNoDuplCheck->setText(QApplication::translate("MainFormClass", "Prevent duplicate entries in databases", 0));
        groupBox->setTitle(QApplication::translate("MainFormClass", "Plugins", 0));
        label_6->setText(QApplication::translate("MainFormClass", "Language:", 0));
        settPluginCombo->clear();
        settPluginCombo->insertItems(0, QStringList()
         << QApplication::translate("MainFormClass", "Klingon", 0)
        );
        pushButton_2->setText(QApplication::translate("MainFormClass", "Setup", 0));
        label_9->setText(QApplication::translate("MainFormClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:18px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; font-weight:600; color:#ff0000;\">Wynn v0.95</span></p></body></html>", 0));
        label_10->setText(QApplication::translate("MainFormClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">Learn and maintain knowledge</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">of foreign languages </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">with ease and efficiency!</span></p></body></html>", 0));
        label_12->setText(QApplication::translate("MainFormClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">This program is available under</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">the terms of the </span><a href=\"http://www.gnu.org/licenses/gpl-2.0-standalone.html\"><span style=\" font-weight:600; text-decoration: underline; color:#0000ff;\">GPL v2</span></a><span style=\" font-size:9pt; font-weight:600;\"> license.</span></p></body></html>", 0));
        label_11->setText(QApplication::translate("MainFormClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">Different terms may apply </span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">to language plugins.</span></p></body></html>", 0));
        buildLabel->setText(QApplication::translate("MainFormClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:9pt; font-weight:600;\">\302\251 2008-2013 Darek Szczepanek</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><a href=\"mailto:szczepanek.d@gmail.com\"><span style=\" font-size:9pt; font-weight:600; text-decoration: underline; color:#0000ff;\">&lt;szczepanek.d@gmail.com&gt;</span></a></p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class MainFormClass: public Ui_MainFormClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFORM_H
