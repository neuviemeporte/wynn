#ifndef WINDOW_H
#define WINDOW_H

#include "ui_mainform.h"
#include "ui_dbase_entry.h"
#include "ui_quiz.h"
#include "db/common.h"

#include <QMainWindow>
#include <QSortFilterProxyModel>

class Word;
class HanCharacter;
class QProgressBar;

class QLabel;
class QDialog;
class QToolButton;
class QModelIndex;
class QMouseEvent;
class QFont;
class QPluginLoader;

class DictionaryModel;
class DictionaryPlugin;

namespace wynn {

namespace db {
class Database;
class Model;
class Quiz;
}

namespace app {
class SetupThread;

class MainForm : public QMainWindow
{
	friend class SetupThread;
	Q_OBJECT

private:
    static const QString VERSION, APPDIR, SETT_EXTDIR, SETT_NODUPS, SETT_CURDB;

    Ui::MainFormClass ui_;
    Ui::DbaseEntryDialog dbaseDialogUI_;
    Ui::QuizDialog quizDialogUI_;

    // TODO: change to vector everywhere
    QList<DictionaryPlugin*> plugins_;
    QList<QPluginLoader*> pluginLoaders_;
    DictionaryPlugin *curPlugin_;

    QList<db::Database*> databases_;
    db::Database *curDbase_;
    db::Model *dbaseModel_;
    QSortFilterProxyModel dbaseProxyModel_;
    QString dbaseFindText_, firstDbase_, extDir_;

    DictionaryModel *dictModel_;
    QDialog *dbaseDialog_, *quizDialog_;
    bool preventDuplicates_;

    SetupThread *setupThread_;
    db::Quiz *quiz_;

public:
	MainForm(QWidget *parent = nullptr);
	~MainForm();

	Ui::MainFormClass& ui() { return ui_; }

	int pluginCount() const { return plugins_.size(); }
	DictionaryPlugin* plugin(const int index) { return plugins_.at(index); }
	QPluginLoader* pluginLoader(const int index) { return pluginLoaders_.at(index); }

    db::Database* database(const QString &name) const;
    void addDatabase(db::Database *dbase);

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
	void slot_database_createClicked();
    void slot_database_saveClicked();
	void slot_database_deleteClicked();
	void slot_database_addToClicked();
	void slot_database_removeFromClicked();
	void slot_database_copyClicked();
	void slot_database_moveClicked();
	void slot_database_editClicked();
	void slot_database_findClicked();
	void slot_database_exportClicked();
	void slot_database_resetClicked();

    // database quiz funcionality
    void slot_quiz_clicked();
    void slot_quiz_typeChanged();
    void slot_quiz_correctClicked();
    void slot_quiz_incorrectClicked();
    void slot_quiz_unsureClicked();
    void slot_quiz_revealClicked();
    void slot_quiz_closeClicked();

	// settings
	void slot_settings_altdirCheckClicked(bool arg);
	void slot_settings_altdirButtonClicked();
	void slot_settings_nodupCheckClicked();
	void slot_settings_switchPlugin(int plugIdx);

	// other
	void slot_setupDone();
	void slot_updateSetupMsg(const QString &msg);
	void slot_testClicked();

protected:
	void clearPluginStacks(bool destroy);

	void loadPlugins();
	void verifyPlugins();

	void loadSettings();
	void saveSettings();
	void applySettings();
    void statusMessage(const QString &text, const int timeoutMs = 0);
    
    void dbaseCountUpdate();
    QString dbaseSavePath(const db::Database *db) const;

	void reloadExternalDbases(const QString &newdir);
	void reloadDbaseCombo(const bool restoreSelection);
	int findComboIndex(const QComboBox *combo, const QString &text);

    void setupDbaseDialogCombo(bool includeCurrent, bool selCurrent);
	QList<int> getSelectedDbaseTableIdxs();
	void copyToAnotherDatabase(const bool move);
    void popDbaseMissing();
	void popDbaseLocked();
    void addToDatabase(const QString &item, const QString &desc);

	void setQuizControlsEnabled(bool arg);
    db::QuizDirection curQuizType() const;
    void displayQuizQuestion();
    void reveaQuizAnswer();
    void finishQuiz(const bool saveResults);

	void closeEvent(QCloseEvent *event);
};

class QuizDialogEventFilter : public QObject {
	Q_OBJECT

protected:
    QDialog *parent_;
    Ui::QuizDialog *dialogUI_;

public:
	QuizDialogEventFilter(QDialog *parent, Ui::QuizDialog *dialogUI);

protected:
	bool eventFilter(QObject *obj, QEvent *event);
};

} // namespace app
} // namespace wynn

#endif // WINDOW_H
