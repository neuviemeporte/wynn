#pragma once

#include "database.h"
#include "dict_table.h"

#include "ui_mainform.h"
#include "ui_dbase_entry.h"
#include "ui_quiz.h"

#include <QMainWindow>
#include <QSortFilterProxyModel>

// TODO:
// ability to deactivate plugins in settings
// clear dict result table after switch between languages
// store column widths of tables in QSettings
// word search click - most of this stuff belongs in the dictionary search function, not in the UI code
// undo button in database manage panel?
// find button in dictionary to search in dictionary search results
// grab focus to appropriate line edit in database-popped dialogs
// keep window size in settings (maximized status too?)
// facilitate clipboard search
// set text interaction flags to 'selectable by mouse' on some qlabels

class Word;
class HanCharacter;
class Quiz;
class Database;
class SetupThread;
class QProgressBar;

class QLabel;
class QDialog;
class QToolButton;
class QModelIndex;
class QMouseEvent;
class QFont;
class QPluginLoader;

class MainForm : public QMainWindow
{
	friend class SetupThread;
	Q_OBJECT

private:
    static const QString APPDIR, SETT_EXTDIR, SETT_NODUPS, SETT_CURDB;

    Ui::MainFormClass ui_;
    Ui::DbaseEntryDialog dbaseDialogUI_;
    Ui::QuizDialog quizDialogUI_;

    QList<DictionaryPlugin*> plugins_;
    QList<QPluginLoader*> pluginLoaders_;
    DictionaryPlugin *curPlugin_;

    QList<Database*> databases_;
    Database *curDbase_;
    DatabaseModel *dbaseModel_;
    QSortFilterProxyModel dbaseProxyModel_;
    QString dbaseFindText_, firstDbase_, extDir_;

    DictionaryModel *dictModel_;
    QDialog *dbaseDialog_, *quizDialog_;
    bool preventDuplicates_;

    SetupThread *setupThread_;
    Quiz *quiz_;

public:
	MainForm(QWidget *parent = 0);
	~MainForm();

	Ui::MainFormClass& ui() { return ui_; }

	int pluginCount() const { return plugins_.size(); }
	DictionaryPlugin* plugin(const int index) { return plugins_.at(index); }
	QPluginLoader* pluginLoader(const int index) { return pluginLoaders_.at(index); }

    Database* database(const QString &name) const;
    void addDatabase(Database *dbase);

signals:
	void dictColumnResized(int curPlugIdx, int colIdx, int oldSize, int newSize);

public slots:
	// top-level
	void slot_dictionaryButtonClicked();
	void slot_manageButtonClicked();
	void slot_settingsButtonClicked();
	void slot_aboutButtonClicked();

	// dictionary panel
	void slot_dict_switchPlugin(int plugIdx);
	void slot_dict_langAboutClicked();
	void slot_dict_detailsClicked();
	void slot_dict_toDbaseClicked();
	void slot_dict_searchStart();
	void slot_dict_searchDone();
	void slot_dict_tableItemActivated(const QModelIndex &index);
	void slot_dict_columnResized(int index, int oldSize, int newSize);
	void slot_dict_resizeColumns(const QList<int> &widths);

	// database functionality
	void slot_database_changeCurrent(const QString& text);
	void slot_database_updateCount();
	void slot_database_createClicked();
	void slot_database_deleteClicked();
	void slot_database_addToClicked();
	void slot_database_removeFromClicked();
	void slot_database_copyClicked();
	void slot_database_moveClicked();
	void slot_database_editClicked();
	void slot_database_findClicked();
	void slot_database_exportClicked();
	void slot_database_resetClicked();
	void slot_database_quizClicked();
	void slot_database_quizTypeChanged();

	// settings
	void slot_settings_altdirCheckClicked(bool arg);
	void slot_settings_altdirButtonClicked();
	void slot_settings_nodupCheckClicked();
	void slot_settings_switchPlugin(int plugIdx);

	// other
	void slot_setupDone();
	void slot_quizDone();
	void slot_updateSetupMsg(const QString &msg);
	void slot_testClicked();

protected:
	void clearPluginStacks(bool destroy);

	void loadPlugins();
	void verifyPlugins();

	void loadSettings();
	void saveSettings();
	void applySettings();

	void reloadExternalDbases(const QString &newdir);
	void updateDbaseCombo(bool restore);
	int findComboIndex(const QComboBox *combo, const QString &text);

    void setupDbaseDialogCombo(bool includeCurrent, bool selCurrent);
	QList<int> getSelectedDbaseTableIdxs();
	void copyToAnotherDatabase(bool move);
	void popDbaseLocked();

	void setQuizControlsEnabled(bool arg);
	DbEntry::QuizDirection curQuizType() const;

	void closeEvent(QCloseEvent *event);
};

class QuizDialogEventFilter : public QObject
{
	Q_OBJECT

protected:
    QDialog *parent_;
    Ui::QuizDialog *dialogUI_;

public:
	QuizDialogEventFilter(QDialog *parent, Ui::QuizDialog *dialogUI);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
};
