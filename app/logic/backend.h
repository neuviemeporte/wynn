#ifndef BACKEND_H
#define BACKEND_H

#include "db/database.h"
#include "db/model.h"
#include "db/quiz.h"

#include <QObject>
#include <QList>
#include <QString>
#include <QSortFilterProxyModel>

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
  QString entryItem_, entryDesc_, itemName_, text_;
  QModelIndexList selection_;
  
public:
  Backend();
  ~Backend();
  
  void setEntry(const QString &item, const QString &desc);
  void setAnswer(const Answer ans);
  void setItem(const QString &item);
  
  void dbaseSwitch(const QString &name);
  void dbaseCreate(const QString &name);
  void dbaseSave();
  void dbaseDelete();
  void dbaseEntryAdd();
  void dbaseEntryRemove(const QModelIndexList &selection);
  void dbaseEntryCopy(const QModelIndexList &selection, const bool move);
  
signals:
  void warning(const QString &title, const QString &msg);
  void error(const QString &title, const QString &msg);
  void status(const QString &msg);
  void question(const QString &title, const QString &msg, const QVector<Answer> &options);
  void item(const QString &title, const QString &msg, const QStringList &options);
  
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
  QList<int> selectedDbaseIndexes();
  bool checkCurrentDatabase();
  void cleanupOperation();
  void continueOperation();
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
