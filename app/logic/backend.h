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
    enum Operation {
        OP_NONE, OP_ENTRY_ADD, OP_ENTRY_DEL, OP_ENTRY_COPY, OP_ENTRY_MOVE, OP_ENTRY_EDIT, OP_ENTRY_FIND, OP_QUIZ 
    };
    enum Answer {
        ANS_NONE, ANS_YES, ANS_YESALL, ANS_NO, ANS_NOALL
    };
    Q_ENUM(Operation)
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

    void switchDatabase(const QString &name);
    void addDatabase(const QString &name);
    void saveDatabase();
    void deleteDatabase();
    void dbaseEntryAdd();
    void removeFromDatabase(const Operation op);
    void copyFromDatabase(const Operation op, bool move = false);

signals:
    void information(const QString &title, const QString &msg);
    void warning(const QString &title, const QString &msg);
    void error(const QString &title, const QString &msg);
    void status(const QString &msg);
    
    void dbaseItemCount(int count);
    void dbaseAdded(const QString &name);
    void dbaseRemoved(const QString &name);
    
protected:
    enum Notification { DBASE_NULL, DBASE_LOCKED };

    db::Database* database(const QString &name) const;
    QString dbasePath(const db::Database *db) const;
    void notify(const Notification type);
    QList<int> selectedDbaseIndexes();
    bool checkCurrentDatabase();
    void cleanupOperation();
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
