#include "backend.h"
#include "ixplog_inactive.h"

#include <QSortFilterProxyModel>

using namespace wynn::db;

namespace wynn {
namespace app {

Backend::Backend() : QObject(),
    curDbase_(nullptr), dbaseModel_(nullptr), dbaseProxyModel_(nullptr), quiz_(nullptr)
{
}

Backend::~Backend() 
{
    delete dbaseModel_;
    delete quiz_;


    QLOG("Iterating over user databases and deleting their handles");
    for (auto db : databases_)
        delete db;
}

State Backend::databaseState() const 
{
    if (database() == nullptr) return DB_NULL;
    else if (database()->locked()) return DB_LOCK;
    else return DB_NORM;
}

QString Backend::databaseName() const
{
    if (curDbase_) return curDbase_->name();
    else return {};
}

Database* Backend::database(const QString &name) const 
{
    if ( name.isEmpty() ) return nullptr;
    QLOG("Getting database handle for name: '" << name << "'");
    for ( auto db : databases_ )
        if ( db->name() == name ) return db;

    return nullptr;
}

void Backend::addDatabase(Database *dbase) 
{
    Q_ASSERT(dbase);
    QLOGX("Adding handle for database '" << dbase->name() << "'");
    databases_.append(dbase);
}

void Backend::switchDatabase(const QString &name) 
{
    curDbase_ = database(name);
}

Error Backend::addToDatabase(const QString &dbName, const QString &item, const QString &desc, const bool dupIgnore)
{
    auto db = database(dbName);
    if (!db)
        return E
    return db->add(item, desc, {}, dupIgnore);
}

} //namespace app 
} //namespace wynn
