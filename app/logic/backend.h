#ifndef BACKEND_H
#define BACKEND_H

#include "db/database.h"
#include "db/model.h"
#include "db/quiz.h"

#include <QObject>
#include <QList>

class QSortFilterProxyModel;

namespace wynn {

namespace app {

class Backend : public QObject
{
    Q_OBJECT
    
    QList<db::Database*> databases_;
    db::Database *curDbase_;
    db::Model *dbaseModel_;
    QSortFilterProxyModel *dbaseProxyModel_;
    db::Quiz *quiz_;
    
public:
    Backend();
    ~Backend();
    
    const db::Database* database() const { return curDbase_; }
    db::State databaseState() const;
    QString databaseName() const;
    void addDatabase(db::Database *dbase);
    void switchDatabase(const QString &name);
    db::Error addToDatabase(const QString &dbName, const QString& item, const QString &desc, const bool dupIgnore = false);
    
protected:
    db::Database* database(const QString &name) const;
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
