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

// Ideas:
// 1. Make backend methods return custom Error type registered with Qt which contains the args of the failed operation. Ugly, meaningless arg1, arg2... Not even sure if QML will be able to parse it. Also, would need synchronous slot execution in QML, don't know if that's possible.
// 2. Emit backendError() signal on fail, but where to put args for retry?
// 3. Extract args from UI, but what if they changed in the meantime? Messy.
// 4. Create properties for all input args in the backend, bind them to UI values (performance?)

class Backend : public QObject
{
    Q_OBJECT
    
    QList<db::Database*> databases_;
    db::Database *curDbase_;
    db::Model *dbaseModel_;
    QSortFilterProxyModel *dbaseProxyModel_;
    db::Quiz *quiz_;
    
public:
    enum Error
    {
        ERR_DBADD_CONFLICT
    };
    Q_ENUM(Error)

    Backend();
    ~Backend();
    
    void addDatabase(db::Database *dbase);
    void switchDatabase(const QString &name);
    bool addToDatabase(const QString &dbName, const QString& item, const QString &desc, const bool dupIgnore = false);

signals:
    void backendError(const Error type, const QString &msg);
    
protected:
    db::Database* database(const QString &name) const;
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
