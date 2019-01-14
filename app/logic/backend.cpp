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

bool Backend::addToDatabase(const QString &dbName, const QString &item, const QString &desc, const bool dupIgnore)
{
    QLOG("Adding entry to database '" << dbName << "', item: '" << item << "', description: '" << desc << "'" );

    auto dbase = database(dbName);
    auto error = dbase->add(userItem, userDesc);

    if (error == Error::DUPLI)
    {
        // TODO: could be duplicate of multiple items, show all of them in the dialog
        int dupidx = error.index();
        const Entry &dupEntry = dbase->entry(dupidx);
        const auto button = QMessageBox::question(this, tr("Possible duplicate"),
            tr("A similar entry was found to already exist in the database:\n"
               "'%1' / '%2' (%3).\n"
               "While trying to add entry:\n"
               "'%4' / '%5'\n"
               "Do you want to add it anyway?").arg(dupEntry.item()).arg(dupEntry.description()).arg(dupidx + 1).arg(item).arg(desc),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            QLOG("User wants to add anyway");
            error = dbase->add(userItem, userDesc, {}, true);
            QLOG("Result: " << QString::number(error.type()));
            // todo handle nested error
            if (error != Error::OK)
            {
                QLOG("Unable to add to database even with duplicates ignored?! (" << error.msg() << ")");
                return;
            }
        }
        else
        {
            QLOG("User doesn't want duplicate");
            return;
        }
    }
}

} //namespace app 
} //namespace wynn
