#include "backend.h"
#include "ixplog_inactive.h"

#include <QSortFilterProxyModel>
#include <QDir>
#include <map>

using namespace wynn::db;

namespace wynn {
namespace app {

static const QString VERSION     = "0.9.6";
static const QString APPDIR      = QDir::homePath() + "/.wynn";
static const QString SETT_EXTDIR = "extdir";
static const QString SETT_NODUPS = "nodbdups";
static const QString SETT_CURDB  = "curdb";

static const std::map<Backend::Operation, std::string> OP_STR = {
  { Backend::OP_NONE,       "NONE"       },
  { Backend::OP_ENTRY_ADD,  "ENTRY_ADD"  },
  { Backend::OP_ENTRY_DEL,  "ENTRY_DEL"  },
  { Backend::OP_ENTRY_COPY, "ENTRY_COPY" }, 
  { Backend::OP_ENTRY_MOVE, "ENTRY_MOVE" },
  { Backend::OP_ENTRY_EDIT, "ENTRY_EDIT" },
  { Backend::OP_ENTRY_FIND, "ENTRY_FIND" },
  { Backend::OP_QUIZ,       "QUIZ"       } 
};

static const std::map<Backend::Answer, std::string> ANS_STR = {
  { Backend::ANS_NONE,   "NONE" },  
  { Backend::ANS_YES,    "YES" },   
  { Backend::ANS_YESALL, "YESALL" },
  { Backend::ANS_NO,     "NO" },    
  { Backend::ANS_NOALL,  "NOALL" },
  { Backend::ANS_CANCEL, "CANCEL" },
};

// TODO: move all strings to UI?
static const QString 
  TITLE_DUPLICATE = tr("Accept duplicate?"),
  TITLE_ERROR = tr("Error"),
  TITLE_ENTRYDEL = tr("Delete items?"),
  TITLE_TARGET = tr("Select target"),
  TITLE_EDIT = tr("Edit item"),
  MSG_DUPLICATE = tr(
      "A similar entry was found to already exist in the database:\n"
      "'%1' / '%2' (%3).\n"
      "While trying to add entry:\n"
      "'%4' / '%5'\n"
      "Do you want to add it anyway?"),
  MSG_ENTRYDEL = tr("Are you sure you want to remove %1 items from database '%2'?"),
  MSG_NOTARGET = tr("No eligible target database exists. Create one and retry."),
  MSG_MOVE = tr("Move %1 entries to database:"),
  MSG_COPY = tr("Copy %1 entries to database:");

Backend::Backend() : QObject(),
  curDbase_(nullptr), 
  dbaseModel_(nullptr), 
  dbaseProxyModel_(nullptr), 
  quiz_(nullptr), 
  curOp_(OP_NONE), 
  answer_(ANS_NONE) {
}

Backend::~Backend() {
  delete dbaseModel_;
  delete quiz_;
  
  QLOG("Iterating over user databases and deleting their handles");
  for (auto db : databases_)
    delete db;
}

void Backend::setEntry(const QString &item, const QString &desc) {
  QLOGX("Setting entry: " << item << desc);
  entryItem_ = item;
  entryDesc_ = desc;
  continueOperation();
}

void Backend::setAnswer(const Backend::Answer ans) {
  QLOGX("Setting answer: " << ANS_STR[ans] << ", current operation: " << OP_STR[curOp_]);
  if (ans == ANS_NONE || curOp_ == OP_NONE) {
    QLOG("Invalid answer or no operation in progress!");
    return;
  }
  answer_ = ans;
  continueOperation();
}

void Backend::setItem(const QString &item) {
  QLOGX("Setting item: " << item << ", current operation: " << OP_STR[curOp_]);
  if (item.isEmpty()) {
    QLOG("No item selected, canceling operation");
    return;
  }
  itemName_ = item;
  continueOperation();
}

void Backend::dbaseSwitch(const QString &name) {
  curDbase_ = database(name);
  // in case the selection became empty (last database removed),
  // set current dbase handle to null and disconnect signals from model to database
  dbaseModel_->setDatabase(curDbase_);
  // notify UI of entry count change so that controls can be adjusted
  const int entryCount = (curDbase_ ? curDbase_->entryCount() : 0);
  emit dbaseItemCount(entryCount);
}

void Backend::dbaseCreate(const QString &name) {
  if (name.isEmpty()) {
    QLOGX("Database name is empty!");
    return;
  }
  
  for (int i = 0; i < databases_.count(); ++i) {
    if (databases_.at(i)->name() == name) {
      emit error(tr("Database exists"), 
                 tr("Could not add database '%1' because a database of this name already exists.").arg(name));
      return;
    }
  }
  
  Database *dbase = new Database(this, name);
  Q_ASSERT(dbase);
  QLOGX("Adding handle for database '" << dbase->name() << "'");
  databases_.append(dbase);
  emit dbaseAdded(name);
}

void Backend::dbaseSave() {
  if (!curDbase_ ) {
    QLOGX("Current database is null!");
    return;
  }
  
  const auto path = dbasePath(curDbase_);
  curDbase_->saveXML(path);
  const auto msg = tr("Database saved to %1 (%2 items)")
      .arg(path)
      .arg(curDbase_->entryCount());    
  emit status(msg);
}

void Backend::dbaseDelete() {
  if (!checkCurrentDatabase()) {
    QLOGX("Current database unavailable!");
    return;        
  }
  
  const auto name = curDbase_->name();
  const auto path = dbasePath(curDbase_);
  QLOG("Removing data file '" << xmlpath << "'");
  QFile file(path);
  if (!file.remove())	{
    QLOG("Unable to remove file! (Error: " << file.error() << ")");
    return;
  }
  
  // finally, destroy database object    
  databases_.removeOne(curDbase_);
  delete curDbase_;
  curDbase_ = nullptr;
  emit dbaseRemoved(name);
}

void Backend::dbaseEntryAdd() {
  if (!checkCurrentDatabase()) return;
  cleanupOperation();
  curOp_ = OP_ENTRY_ADD;
  emit dbaseEntry(tr("Add entry to database"), {}, {});
}

void Backend::dbaseEntryRemove(const QModelIndexList &selection) {
  if (!checkCurrentDatabase()) return;
  if (selection.empty()) {
    QLOGX("Selection is empty!");
    return;
  }
  cleanupOperation();
  curOp_ = OP_ENTRY_DEL;
  selection_ = mapToDatabase(selection, true);
  emit question(TITLE_ENTRYDEL, 
                MSG_ENTRYDEL.arg(curDbase_->entryCount()).arg(curDbase_->name()), 
                { ANS_YES, ANS_NO });
}

void Backend::dbaseEntryCopy(const QModelIndexList &selection, const bool move) {
  if (!checkCurrentDatabase()) return;
  if (selection.empty()) {
    QLOGX("Selection is empty!");
    return;
  }
  cleanupOperation();
  curOp_ = (move ? OP_ENTRY_MOVE : OP_ENTRY_COPY);
  selection_ = mapToDatabase(selection, true);
  QStringList dbNames;
  for (auto db : databases_) 
    if (db != curDbase_) dbNames.append(db->name());
  
  if (dbNames.empty()) {
    emit error(TITLE_ERROR, MSG_NOTARGET);
    return;
  }
  
  const QString &msg = (move ? MSG_MOVE : MSG_COPY);
  emit item(TITLE_TARGET, msg.arg(selection.count()), dbNames);
}

void Backend::dbaseEntryEdit(const QModelIndexList &selection)
{
  if (!checkCurrentDatabase()) return;
  if (selection.empty()) {
    QLOGX("Selection is empty!");
    return;
  }
  cleanupOperation();
  curOp_ = OP_ENTRY_EDIT;
  selection_ = mapToDatabase(selection, false);
  Q_ASSERT(!selection_.empty());
  Q_ASSERT(curDbase_);
  const auto &entry = curDbase_->entry(selection_.front());
  emit dbaseEntry(TITLE_EDIT, entry.item(), entry.description());
}

// TODO: execute actual work on separate thread
void Backend::continueOperation() {
  QLOGX("Continuing operation: " << OP_STR[curOp_]);
  if (curOp_ == OP_ENTRY_ADD) {
    if (answer_ == ANS_NO) {
      QLOG("Operation was canceled");
      return;
    }
    const bool dupIgnore = (answer_ == ANS_YES || answer_ == ANS_YESALL);
    QLOG("Adding entry, item = " << entryItem_ << ", desc = " << entryDesc_ << " dupIgnore: " << dupIgnore);
    Q_ASSERT(curDbase_);
    Q_ASSERT(!entryItem_.isEmpty() && !entryDesc_.isEmpty());
    const auto error = curDbase_->add(entryItem_, entryDesc_, {}, dupIgnore);
    if (handleDuplicate(curDbase_, error, entryItem_, entryDesc_)) 
      return;
    
    if (error == Error::OK) {
      emit dbaseUpdated(curDbase_->entryCount());
    } else {
      QLOG("Unexpected error when adding entry to dbase: " << error);
    }
  } else if (curOp_ == OP_ENTRY_DEL) {
    if (answer_ == ANS_YES) {
      Q_ASSERT(curDbase_)
      Q_ASSERT(!selection_.empty());
      auto error = curDbase_->remove(selection_);
      if (error != Error::OK) {
        QLOG("Unexpected error when removing entry from dbase: " << error);
      }
    }
    // TODO: reconsider focus point, maybe before first removed index?
    emit dbaseUpdated(0);
  } else if (curOp_ == OP_ENTRY_COPY || curOp_ == OP_ENTRY_MOVE) {
    auto target = database(itemName_);
    Q_ASSERT(curDbase_);
    Q_ASSERT(target);
    Q_ASSERT(!selection_.empty());
    // iterate over entries for copy/move
    for (auto it = selection_.begin(); it != selection_.end();) {
      const int idx = *it;
      QLOG("Processing index: " << idx << ", answer = " << ANS_STR[answer_]);
      if (answer_ == ANS_CANCEL) {
        QLOG("Operation canceled by user");
        break;
      }
      // when user answered "no", skip copy/move
      if (answer_ != ANS_NO) {
        bool dupIgnore = (answer_ == ANS_YES || ANS_YESALL);
        auto entry = curDbase_->entry(idx);
        // try to add entry to target database
        auto addError = target->add(entry.item(), entry.description(), {}, dupIgnore);
        // ask user about handling of duplicate unless they already answered "no to all"
        if (answer_ != ANS_NOALL && handleDuplicate(target, error, entry.item(), entry.description())) 
          return;
        // remove entry from source database when add to target successful and we are in move mode
        // TODO: implement move operation as atomic on database level
        if (addError == Error::OK && curOp_ == OP_ENTRY_MOVE) {
          auto removeError = curDbase_->remove(idx);
          if (removeError != Error::OK) {
            QLOG("Unexpected error while removing entry: " << removeError);
          }
        }
      }
      // yes/no valid only for one iteration, reset answer for next one
      if (answer_ == ANS_YES || answer_ == ANS_NO) answer_ = ANS_NONE;
      // remove index processed in this iteration from list
      it = selection_.erase(it);
    }
  } else if (curOp_ == OP_ENTRY_EDIT) {
    
  }
  
}

bool Backend::handleDuplicate(const Database *dbase, const db::Error &error, const QString &item, const QString &desc)
{
  if (error != db::Error::DUPLI)
    return false;
  
  const int dupIdx = error.index();
  QLOGX("Handing duplicate of " << item << "/" << desc << " in database '" << dbase->name() << "' at index " << dupIdx);
  // TODO: could be duplicate of multiple items, show all of them in the dialog, preferably sorted in order of similarity
  const Entry &dupEntry = dbase->entry(dupIdx);
  emit question(TITLE_DUPLICATE, 
                MSG_DUPLICATE.arg(dupEntry.item()).arg(dupEntry.description()).arg(dupIdx + 1).arg(item).arg(desc), 
                { ANS_YES, ANS_NO });
  return true;
}
  
//void Backend::copyFromDatabase(const Backend::Operation op, bool move)
//{
//  if (!checkCurrentDatabase()) 
//  {
//    QLOGX("Current database unavailable!");
//    return;        
//  }
//  // return if no selection
//  else if ( dbaseSelection_.isEmpty() )
//  {
//    QLOG("Nothing selected, done");
//    return;
//  }
  
//  if (op == OP_INIT) 
//  {
//    QString title, item, desc;
//    if (move) 
//      title = tr("Move item(s) to another database");
//    else 
//      title = tr("Copy item(s) to another database");
    
//    if (dbaseSelection_.count() == 1) 
//    {
//      const auto idxs = selectedDbaseIndexes();
//      Q_ASSERT(idxs.count() == 1);
//      const int idx = idxs.front();
//      const Entry& entry = curDbase_->entry(idx);
//      item = entry.item();
//      desc = entry.description();
//    }
//    else 
//    {
//      item = desc = tr("-- multiple --");
//    }
    
//    emit dbaseCopyMoveConfirm(title, item, desc, move);
//  }
//  else if ( op == OP_PROCESS ) 
//  {
//    QList<int> moveIdxs;
//    for (int curIdx : dbIdxs) {
//      const Entry &entry = curDbase_->entry( curIdx );
//      auto itemText = entry.item();
//      auto descText = entry.description();
      
//      // if go-ahead given, ignore duplicates
//      if ( answer == QMessageBox::YesToAll ) {
//        Error error = target->add(itemText, descText, {}, true);
//        Q_ASSERT(error == Error::OK);
//        if ( move ) moveIdxs.append( curIdx );
//      }
//      else {
//        // don't ignore duplicates (yet)
//        Error error = target->add(itemText, descText, {}, false);
//        // check error type,
//        if (error == Error::OK) {
//          // in move mode, add index to successfully copied list for later removal from source
//          if ( move ) moveIdxs.append( curIdx );
//          continue;
//        }
//        // if duplicate detected, warn
//        else if ( error == Error::DUPLI && answer != QMessageBox::NoToAll ) {
//          const Entry &dupEntry = target->entry(error.index());
//          answer = QMessageBox::information(this, tr("Duplicate entry"),
//                                            tr("Possible duplicate found for '") + entry.item() + "' (" + entry.description() +
//                                            tr(") in target database: '") + dupEntry.item() + "' (" + dupEntry.description() +
//                                            tr("). Continue?"),
//                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
//        }
        
//        // retry item add with duplicates ignored on "yes" or "yes to all"
//        if ( answer == QMessageBox::Yes || answer == QMessageBox::YesToAll ) {
//          error = target->add(itemText, descText, {}, true);
//          if (error != Error::OK) {
//            QLOG("Unable to add to database despite ignoring duplicates?! (" << error.msg() << ")");
//            return;
//          }
//          if ( move ) moveIdxs.append( curIdx );
//        }
//      } // not YesToAll
//    } // iterate over input indexes        
    
//  }
  
//}

Database* Backend::database(const QString &name) const {
  if ( name.isEmpty() ) return nullptr;
  QLOG("Getting database handle for name: '" << name << "'");
  for ( auto db : databases_ )
    if ( db->name() == name ) return db;
  
  return nullptr;
}

QString Backend::dbasePath(const Database *db) const {
  if (!db) return {};
  return APPDIR + QDir::separator() + db->name() + Database::XML_EXT;
}

void Backend::notify(const Backend::Notification type) {
  switch (type) {
  case DBASE_NULL:
    emit warning(tr("No database"),
                 tr("No user database exists. You must first create one\nto be able to add items to it."));
    break;
  case DBASE_LOCKED:
    emit warning(tr("Database locked"),
                 tr("The current database is being used and cannot be altered right now."));
    break;
  }
}

std::list<int> Backend::mapToDatabase(const QModelIndexList &model, const bool sort) {
  QLOGX("Selected indexes in database table: " << selection_.count());
  
  std::list<int> rowIdxs;
  for (const auto &idx : model)	{
    if (idx.column() != 0) continue;
    QLOG("row: " << idx.row() << ", col: " << idx.column());
    int dbaseidx = dbaseModel_->data( dbaseProxyModel_->mapToSource( idx ), Qt::UserRole).toInt();
    QLOG("New dbase idx: " << dbaseidx);
    rowIdxs.push_back(dbaseidx);
  }
  // need indices sorted in descending order for iterative removal operation
  if (sort) rowIdxs.sort(std::greater<int>());
  return rowIdxs;
}

bool Backend::checkCurrentDatabase() {
  if ( !curDbase_ )  {
    QLOGX("Current database handle is null!");
    notify(DBASE_NULL);
    return false;
  } else if ( curDbase_->locked() ) {
    QLOGX("Current database is locked!");
    notify(DBASE_LOCKED);
    return false;
  }
  return true;
}

void Backend::cleanupOperation() {
  QLOG("Cleaning up operation (was: " << OP_STR[curOp_] << ")");
  curOp_ = OP_NONE;
  answer_ = ANS_NONE;
  entryItem_.clear();
  entryDesc_.clear();
  itemName_.clear();
  text_.clear();
  selection_.clear();
}

} //namespace app 
} //namespace wynn
