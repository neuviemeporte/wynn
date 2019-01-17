#include "backend.h"
#include "ixplog_inactive.h"

#include <QSortFilterProxyModel>
#include <QDir>

using namespace wynn::db;

namespace  {

const QString 
    VERSION     = "0.9.6",
	APPDIR      = QDir::homePath() + "/.wynn",
	SETT_EXTDIR = "extdir", 
	SETT_NODUPS = "nodbdups", 
	SETT_CURDB  = "curdb";

} // namespace

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

QString Backend::dbasePath(const Database *db) const
{
    if (!db) return {};
    return APPDIR + QDir::separator() + db->name() + Database::XML_EXT;
}

void Backend::addDatabase(const QString &name) 
{
    if (name.isEmpty()) {
        QLOGX("Database name is empty!");
        return;
    }
    
    for (int i = 0; i < databases_.count(); ++i) 
    {
        if (databases_.at(i)->name() == name) 
        {
            emit error(tr("Database exists"), tr("Could not add database '%1' because a database of this name already exists.").arg(name));
            return;
        }
    }
    
    Database *dbase = new Database(this, name);
    Q_ASSERT(dbase);
    QLOGX("Adding handle for database '" << dbase->name() << "'");
    databases_.append(dbase);
    emit dbaseAdded(name);
}

void Backend::saveDatabase()
{
    if (!curDbase_ ) 
    {
        QLOGX("Current database is null!");
        return;
    }
    
    const auto path = dbasePath(curDbase_);
    curDbase_->saveXML(path);
    const auto msg = tr("Database saved to %1 (%2 items)").arg(path).arg(curDbase_->entryCount());    
    emit status(msg);
}

void Backend::deleteDatabase()
{
    if (!curDbase_) 
    {
        QLOGX("Current database is null!");
        return;        
    }
    
    const auto name = curDbase_->name();
    const auto path = dbasePath(curDbase_);
	QLOG("Removing data file '" << xmlpath << "'");
	QFile file(path);
	if (!file.remove())	
    {
		QLOG("Unable to remove file! (Error: " << file.error() << ")");
        return;
	}

    // finally, destroy database object    
	databases_.removeOne(curDbase_);
    delete curDbase_;
    curDbase_ = nullptr;
    emit dbaseRemoved(name);
}

void Backend::switchDatabase(const QString &name) 
{
    curDbase_ = database(name);
    // in case the selection became empty (last database removed),
    // set current dbase handle to null and disconnect signals from model to database
	dbaseModel_->setDatabase(curDbase_);
    // notify UI of entry count change so that controls can be adjusted
    const int entryCount = (curDbase_ ? curDbase_->entryCount() : 0);
    emit dbaseItemCount(entryCount);
}

void Backend::addToDatabase()
{
	if ( !curDbase_ ) 
    {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    })
}

void Backend::enterToDatabase(bool ignoreDuplicates)
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
