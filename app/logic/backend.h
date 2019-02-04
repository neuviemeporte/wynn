#ifndef BACKEND_H
#define BACKEND_H

#include "db/database.h"
#include "db/model.h"
#include "db/quiz.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QSortFilterProxyModel>
#include <list>

namespace wynn {
namespace app {

// Basic backend without dictionary capabilities, just manages vocabulary databases
class Backend : public QObject {
  Q_OBJECT
  
public:
  // TODO: use string lists+translation, don't expose enum?
  enum Answer {
    ANS_NONE, ANS_YES, ANS_YESALL, ANS_NO, ANS_NOALL, ANS_CANCEL
  };
  Q_ENUM(Answer)
  
private:
  QList<db::Database*> databases_;
  db::Database *curDbase_;
  db::Model *dbaseModel_;
  QSortFilterProxyModel *dbaseProxyModel_;
  db::Quiz *quiz_;
  Operation curOp_;
  Answer answer_;
  QString entryItem_, entryDesc_, response_;
  std::list<int> selection_;
  
public:
  Backend();
  ~Backend();
  
  void setEntry(const QString &item, const QString &desc);
  void setAnswer(const Answer ans);
  void setResponse(const QString &item);
  
  void dbaseSwitch(const QString &name);
  void dbaseCreate(const QString &name);
  void dbaseSave();
  void dbaseDelete();
  void dbaseEntryAdd();
  void dbaseEntryRemove(const QModelIndexList &selection);
  void dbaseEntryCopy(const QModelIndexList &selection, const bool move);
  void dbaseEntryEdit(const QModelIndexList &selection);
  void dbaseEntryFind(const QModelIndexList &selection);
  void dbaseExport(const QModelIndexList &selection, const QString &path);
  
signals:
  void warning(const QString &title, const QString &msg);
  void error(const QString &title, const QString &msg);
  void status(const QString &msg);
  void question(const QString &title, const QString &msg, const QVector<Answer> &options);
  void getItem(const QString &title, const QString &msg, const QStringList &options);
  void getText(const QString &title, const QString &msg);
  
  void dbaseItemCount(int count);
  void dbaseAdded(const QString &name);
  void dbaseRemoved(const QString &name);
  void dbaseUpdated(const int where);
  void dbaseEntry(const QString &title, const QString &item, const QString &desc);
  
protected:
  enum Operation {
    OP_NONE, OP_ENTRY_ADD, OP_ENTRY_DEL, OP_ENTRY_COPY, OP_ENTRY_MOVE, OP_ENTRY_EDIT, OP_ENTRY_FIND, OP_QUIZ 
  };
  enum Notification { DBASE_NULL, DBASE_LOCKED };
  
  db::Database* database(const QString &name) const;
  QString dbasePath(const db::Database *db) const;
  void notify(const Notification type);
  std::list<int> mapToDatabase(const QModelIndexList &model, const bool sort);
  bool checkCurrentDatabase();
  void cleanupOperation();
  void continueOperation();
  bool handleDuplicate(const db::Database *dbase, const db::Error &error, const QString &item, const QString &desc);
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
