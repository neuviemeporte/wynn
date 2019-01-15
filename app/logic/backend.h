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
    void addToDatabase(bool ignoreDuplicates = false);

signals:
    void information(const QString &name, const QString &desc);
    void error(const Error type, const QString &msg);
    void dbaseEnter(const QString &item, const QString &desc);
    void dbaseDuplicate();
    
protected:
    db::Database* database(const QString &name) const;
};

} // namespace app
} // namespace wynn

#endif // BACKEND_H
