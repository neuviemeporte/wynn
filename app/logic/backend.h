#ifndef BACKEND_H
#define BACKEND_H

#include "db/database.h"
#include "db/model.h"
#include "db/quiz.h"

#include <QObject>
#include <QList>
#include <QString>

class QSortFilterProxyModel;

namespace wynn {
namespace app {

// Basic backend without dictionary capabilities, just manages vocabulary databases
class Backend : public QObject
{
    Q_OBJECT
    
    QList<db::Database*> databases_;
    db::Database *curDbase_;
    db::Model *dbaseModel_;
    QSortFilterProxyModel *dbaseProxyModel_;
    db::Quiz *quiz_;
    
    // properties bound to UI controls
    Q_PROPERTY(QString dbaseEnterName MEMBER dbaseEnterName_)
    Q_PROPERTY(QString dbaseEnterItem MEMBER dbaseEnterItem_)
    Q_PROPERTY(QString dbaseEnterDesc MEMBER dbaseEnterDesc_)
    Q_PROPERTY(QList<int> dbaseSelection MEMBER dbaseSelection_)
    QString dbaseEnterName_, dbaseEnterItem_, dbaseEnterDesc_;
    QList<int> dbaseSelection_;
    
protected:
    enum Notification
    {
        DBASE_NULL,
        DBASE_LOCKED
    };

    db::Database* database(const QString &name) const;
    QString dbasePath(const db::Database *db) const;
    void notify(const Notification type);
    QList<int> getSelectedDbaseTableIdxs();
    
public:
    Backend();
    ~Backend();

    enum Operation
    {
        OP_INIT,
        OP_PROCESS,
        OP_POST
    };
    Q_ENUM(Operation)
    
    void addDatabase(const QString &name);
    void saveDatabase();
    void deleteDatabase();
    void switchDatabase(const QString &name);
    void addToDatabase(const Operation op, bool ignoreDuplicates = false);
    void removeFromDatabase(const Operation op);

signals:
    void information(const QString &title, const QString &msg);
    void warning(const QString &title, const QString &msg);
    void error(const QString &title, const QString &msg);
    void status(const QString msg);
    
    void dbaseEnterNew(const QString &item = {}, const QString &desc = {});
    void dbaseDuplicate(const QString &title, const QString &msg);
    void dbaseItemCount(const int itemCount);
    void dbaseAdded(const QString &name);
    void dbaseRemoved(const QString &name);
    void dbaseEntryAdded(int entryCount);
    void dbaseRemoveFromConfirm(const QString &title, const QString &msg);
    void dbaseEntriesRemoved(int entryCount);
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
