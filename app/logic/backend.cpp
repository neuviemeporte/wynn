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

void Backend::addToDatabase(bool ignoreDuplicates)
{
    const QString
            userItem  = dbaseEnterItem_.simplified(),
            userDesc  = dbaseEnterDesc_.simplified();
    
    // guard against empty input on line edits
    if (userItem.isEmpty() || userDesc.isEmpty())
    {
        emit warning(tr("Error"), tr("Entry and Description fields may not be empty!"));
        return;
    }

    QLOG("Adding entry to database '" << dbaseEnterName_ << "', item: '" << userItem << "', description: '" << userDesc << "'" );
    auto dbase = database(dbaseEnterName_);
    if (!dbase) 
    {
        QLOG("Database handle is null!");
        return;
    }
    
    auto error = dbase->add(userItem, userDesc, {}, ignoreDuplicates);
    if (error == db::Error::DUPLI)
    {
        // TODO: could be duplicate of multiple items, show all of them in the dialog
        int dupidx = error.index();
        const Entry &dupEntry = dbase->entry(dupidx);
        // TODO: move all strings to UI?
        const QString msg = tr("A similar entry was found to already exist in the database:\n"
               "'%1' / '%2' (%3).\n"
               "While trying to add entry:\n"
               "'%4' / '%5'\n"
               "Do you want to add it anyway?").arg(dupEntry.item()).arg(dupEntry.description()).arg(dupidx + 1).arg(userItem).arg(userDesc);
        
        emit dbaseDuplicate(tr("Possible duplicate"), msg);
    }
    else if (error != db::Error::OK) 
    {
        QLOG("Unexpected error: " << error);
        // TODO: handle
    }
}

} //namespace app 
} //namespace wynn
