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

void Backend::notify(const Backend::Notification type)
{
    switch (type)
    {
    case DBASE_NULL:
        emit information(tr("No database"),
                         tr("No user database exists. You must first create one\nto be able to add items to it."));
        break;
    case DBASE_LOCKED:
        emit information(tr("Database locked"),
                         tr("The current database is being used and cannot be altered right now."));
        break;
    default: break;
    }
}

QList<int> Backend::selectedDbaseIndexes() {
    QLOGX("Selected indexes in database table: " << allIdxs.count());

    QList<int> rowIdxs;
    for (const auto &idx : dbaseSelection_)	{
        if (idx.column() != 0) continue;
        QLOG("row: " << idx.row() << ", col: " << idx.column());
        int dbaseidx = dbaseModel_->data( dbaseProxyModel_->mapToSource( idx ), Qt::UserRole).toInt();
        QLOG("New dbase idx: " << dbaseidx);
        rowIdxs.append( dbaseidx );
    }
    return rowIdxs;
}

bool Backend::checkCurrentDatabase()
{
    if ( !curDbase_ )
    {
        notify(DBASE_NULL);
        return false;
    }
    else if ( curDbase_->locked() )
    {
        notify(DBASE_LOCKED);
        return false;
    }
    return true;
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
    if (!checkCurrentDatabase()) 
    {
        QLOGX("Current database unavailable!");
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

void Backend::addToDatabase(const Operation op, bool ignoreDuplicates)
{
    if (!checkCurrentDatabase()) 
    {
        QLOGX("Current database unavailable!");
        return;        
    }

    if ( op == OP_INIT )
    {
        emit dbaseEnterNew();
    }
    else if ( op == OP_PROCESS )
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
            return;
        }

        emit dbaseEntryAdded(dbase->entryCount());
    }
}

void Backend::removeFromDatabase(const Operation op)
{
    if (!checkCurrentDatabase()) 
    {
        QLOGX("Current database unavailable!");
        return;        
    }
    // return if no selection
    else if ( dbaseSelection_.isEmpty() )
    {
        QLOG("Nothing selected, done");
        return;
    }

    if ( op == OP_INIT )
    {
        emit dbaseRemoveFromConfirm(tr("Remove item(s)"),
                                    tr("Are you sure you want to remove %1 items?").arg(dbaseSelection_.count()));
    }
    else if ( op == OP_PROCESS )
    {
        QList<int> dbIdxs = selectedDbaseIndexes();
        Q_ASSERT(curDbase_);
        const auto error = curDbase_->remove(dbIdxs);
        if ( error == db::Error::OK )
        {
            emit dbaseEntriesRemoved(curDbase_->entryCount());
        }
        else
        {
            QLOG("Unexpected error: " << error);
        }
    }
}

void Backend::copyFromDatabase(const Backend::Operation op, bool move)
{
    if (!checkCurrentDatabase()) 
    {
        QLOGX("Current database unavailable!");
        return;        
    }
    // return if no selection
    else if ( dbaseSelection_.isEmpty() )
    {
        QLOG("Nothing selected, done");
        return;
    }
    
    if (op == OP_INIT) 
    {
        QString title, item, desc;
        if (move) 
            title = tr("Move item(s) to another database");
        else 
            title = tr("Copy item(s) to another database");
        
        if (dbaseSelection_.count() == 1) 
        {
            const auto idxs = selectedDbaseIndexes();
            Q_ASSERT(idxs.count() == 1);
            const int idx = idxs.front();
            const Entry& entry = curDbase_->entry(idx);
            item = entry.item();
            desc = entry.description();
        }
        else 
        {
            item = desc = tr("-- multiple --");
        }
        
        emit dbaseCopyMoveConfirm(title, item, desc, move);
    }
    else if ( op == OP_PROCESS ) 
    {
        QList<int> moveIdxs;
        for (int curIdx : dbIdxs) {
            const Entry &entry = curDbase_->entry( curIdx );
            auto itemText = entry.item();
            auto descText = entry.description();
    
            // if go-ahead given, ignore duplicates
            if ( answer == QMessageBox::YesToAll ) {
                Error error = target->add(itemText, descText, {}, true);
                Q_ASSERT(error == Error::OK);
                if ( move ) moveIdxs.append( curIdx );
            }
            else {
                // don't ignore duplicates (yet)
                Error error = target->add(itemText, descText, {}, false);
                // check error type,
                if (error == Error::OK) {
                    // in move mode, add index to successfully copied list for later removal from source
                    if ( move ) moveIdxs.append( curIdx );
                    continue;
                }
                // if duplicate detected, warn
                else if ( error == Error::DUPLI && answer != QMessageBox::NoToAll ) {
                    const Entry &dupEntry = target->entry(error.index());
                    answer = QMessageBox::information(this, tr("Duplicate entry"),
                                                      tr("Possible duplicate found for '") + entry.item() + "' (" + entry.description() +
                                                      tr(") in target database: '") + dupEntry.item() + "' (" + dupEntry.description() +
                                                      tr("). Continue?"),
                                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
                }
                
                // retry item add with duplicates ignored on "yes" or "yes to all"
                if ( answer == QMessageBox::Yes || answer == QMessageBox::YesToAll ) {
                    error = target->add(itemText, descText, {}, true);
                    if (error != Error::OK) {
                        QLOG("Unable to add to database despite ignoring duplicates?! (" << error.msg() << ")");
                        return;
                    }
                    if ( move ) moveIdxs.append( curIdx );
                }
            } // not YesToAll
        } // iterate over input indexes        
        
    }
    
}

} //namespace app 
} //namespace wynn
