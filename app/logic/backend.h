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
    QString dbaseEnterName_, dbaseEnterItem_, dbaseEnterDesc_;    
    
protected:
    db::Database* database(const QString &name) const;
    QString dbasePath(const db::Database *db) const;
    
public:
    Backend();
    ~Backend();
    
    void addDatabase(const QString &name);
    void saveDatabase();
    void deleteDatabase();
    void switchDatabase(const QString &name);
    void addToDatabase();
    void enterToDatabase(bool ignoreDuplicates = false);

signals:
    void information(const QString &title, const QString &msg);
    void warning(const QString &title, const QString &msg);
    void error(const QString &title, const QString &msg);
    void status(const QString msg);
    
    void dbaseEnter(const QString &item, const QString &desc);
    void dbaseDuplicate(const QString &title, const QString &msg);
    void dbaseItemCount(const int itemCount);
    void dbaseAdded(const QString &name);
    void dbaseRemoved(const QString &name);
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
