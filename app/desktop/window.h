#ifndef WINDOW_H
#define WINDOW_H

#include "ui_mainform.h"
#include "ui_dbase_entry.h"
#include "ui_quiz.h"
#include "db/common.h"
#include "db/error.h"
#include "ext_backend.h"

#include <QMainWindow>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QModelIndex>

class QDialog;
class QModelIndex;
class QPluginLoader;
class DictionaryModel;
class DictionaryPlugin;

namespace wynn {

namespace app {
class SetupThread;

class MainForm : public QMainWindow
{
  friend class SetupThread;
  Q_OBJECT
  
private:
  static const QString VERSION, APPDIR, SETT_EXTDIR, SETT_NODUPS, SETT_CURDB;
  
  ExtBackend *backend_;
  // TODO: make into smart pointers, move ui header includes to cpp
  Ui::MainFormClass ui_;
  Ui::DbaseEntryDialog dbaseDialogUI_;
  Ui::QuizDialog quizDialogUI_;
  
  QString dbaseFindText_, firstDbase_, extDir_;
  
  QDialog *dbaseDialog_, *quizDialog_;
  bool preventDuplicates_;
  
  SetupThread *setupThread_;
  
public:
  MainForm(ExtBackend *backend);
  ~MainForm();
  
  Ui::MainFormClass& ui() { return ui_; }
  
signals:
  void dictColumnResized(int curPlugIdx, int colIdx, int oldSize, int newSize);
  
public slots:
  // top-level
  void slot_dictionaryButtonClicked();
  void slot_manageButtonClicked();
  void slot_settingsButtonClicked();
  void slot_aboutButtonClicked();
  
  // backend
  void slot_backend_warning(const QString& title, const QString& msg);
  void slot_backend_error(const QString &title, const QString &msg);
  void slot_backend_status(const QString &msg);
  void slot_backend_question(const QString &title, const QString &msg, const QVector<Backend::Answer> &options);
  void slot_backend_getItem(const QString &title, const QString &msg, const QStringList &options);
  void slot_backend_getText(const QString &title, const QString &msg);
  void slot_backend_dbaseEntry(const QString &title, const QString &item, const QString &desc);
  
  // dictionary panel
  void slot_dict_switchPlugin(int plugIdx);
  void slot_dict_langAboutClicked();
  void slot_dict_detailsClicked();
  void slot_dict_toDbaseClicked();
  void slot_dict_searchStart();
  void slot_dict_results();
  void slot_dict_tableItemActivated(const QModelIndex &index);
  void slot_dict_columnResized(int index, int oldSize, int newSize);
  void slot_dict_resizeColumns(const QList<int> &widths);
  
  // database functionality
  void slot_database_changeCurrent(const QString& text);
  void slot_database_countUpdate(const int count);
  void slot_database_createClicked();
  void slot_database_added(const QString &name);
  void slot_database_saveClicked();
  void slot_database_deleteClicked();
  void slot_database_removed(const QString &name);
  void slot_database_entryAddClicked();
  void slot_database_entryAccepted();
  void slot_database_entryRejected();
  void slot_database_updated(const int where);
  void slot_database_entryRemoveClicked();
  void slot_database_copyClicked();
  void slot_database_moveClicked();
  void slot_database_editClicked();
  void slot_database_findClicked();
  void slot_database_exportClicked();
  void slot_database_resetClicked();
  
  // database quiz funcionality
  void slot_database_quizClicked();
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
  void slot_statusMessage(const QString &text);
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
  
  QString dbaseSavePath(const db::Database *db) const;
  QString dbaseName() const;
  QModelIndexList dbaseSelection() const;
  
  void reloadExternalDbases(const QString &newdir);
  void reloadDbaseCombo(const bool restoreSelection);
  int findComboIndex(const QComboBox *combo, const QString &text);
  
  QList<int> getSelectedDbaseTableIdxs();
  void copyToAnotherDatabase(const bool move);
  void popDbaseNullDialog();
  void popDbaseLockedDialog();
  void addToDatabase(const QString &item, const QString &desc);
  
  void setQuizControlsEnabled(bool arg);
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
