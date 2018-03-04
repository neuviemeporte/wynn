#include "database.h"
#include "ixplog_active.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextStream>

#define DEBUG_EVENTS 0
#define DEBUG_CLOCKS 1

extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"

// ==============================================================================================
// ============================== DbClock
// ==============================================================================================

/// Synchronize with an another clock with the same uuid; replace the tick counts with the maximum of both.
void DbClock::sync(DbClock &other)
{
    if (instanceID_ != other.instanceID_) return;

    QLOGCX("Synchronizing clocks for " << instanceID_.toString(), DEBUG_CLOCKS);
	QLOGINC;
	qint64 maxTicks = std::max(ticks_, other.ticks_);
	ticks_ = other.ticks_ = maxTicks;
	QLOGC("Ticks = " << ticks_, DEBUG_EVENTS);
	QLOGDEC;
}

QString DbClock::toString() const
{
    return QString(instanceID().toString() + ": " + QString::number(ticks()));
}

QTextStream& operator<<(QTextStream &os, const DbClock &arg)
{
    return (os << arg.toString());
}

// ==============================================================================================
// ============================== DbClocks
// ==============================================================================================

DbClocks::DbClocks(const QUuid &masterID) :
    masterID_(masterID)
{
    clocks_.insert(masterID, DbClock(masterID));
}

// test two DbClocks' instances for equivalence
bool DbClocks::operator==(const DbClocks &other) const
{
	QLOGCX("Comparing clocks, master: " << masterID_.toString() << " other master: " << other.masterID().toString(), DEBUG_CLOCKS);
	QLOGINC;

	const QList<QUuid>
		thisIds = ids(),
		otherIds = other.ids();

	// check all uuids from this instance for matches in other
	for (int i = 0; i < thisIds.size(); ++i)
	{
		const QUuid &thisId = thisIds.at(i);
		const qint64 
			thisTicks = ticks(thisId),
			otherTicks = other.ticks(thisId);

		if (otherTicks != thisTicks) 
		{
			QLOG("For id from this " << thisId.toString() << " " << thisTicks << " != " << otherTicks);
			QLOGDEC;
			return false;
		}
	}

	// check all uids from other instance for matches in this one
	for (int i = 0; i < otherIds.size(); ++i)
	{
		const QUuid &otherId = otherIds.at(i);
		const qint64 
			otherTicks = other.ticks(otherId),
			thisTicks = ticks(otherId);

		if (otherTicks != thisTicks) 
		{
			QLOG("For id from other " << otherId.toString() << " " << thisTicks << " != " << otherTicks);
			QLOGDEC;
			return false;
		}
	}

	QLOGDEC;
	return true;
}

bool DbClocks::operator!=(const DbClocks &other) const
{
	return !(*this == other);
}

qint64 DbClocks::ticks(const QUuid &id) const
{
    if (clocks_.contains(id))
    {
        const DbClock clock = clocks_.value(id);
        Q_ASSERT(!clock.isNull());
        return clock.ticks();
    }
    else return -1;
}

void DbClocks::tick(const QUuid &id)
{
    QLOGCX("Performing clock tick for id " << id.toString(), DEBUG_CLOCKS);
    QLOGINC;
	Q_ASSERT(hasMaster());

	if(!clocks_.contains(id)) clocks_.insert(id, DbClock(id));
    DbClock &clock = clocks_[id];
	Q_ASSERT(!clock.isNull());

    QLOGC("Old = " << clock.ticks(), DEBUG_CLOCKS);
    clock.tick();
    QLOGC("New = " << clock.ticks(), DEBUG_CLOCKS);

    QLOGDEC;
}

void DbClocks::setMaster(const QUuid &masterID)
{
    masterID_ = masterID;
}

void DbClocks::fork(DbClocks &source)
{
    Q_ASSERT(hasMaster());
    Q_ASSERT(clocks_.size() == 1);
    Q_ASSERT(masterTicks() == 0);
    QLOGCX("Forking clocks structure, masterID: " << masterID_.toString() << ", source: " << source.masterID().toString(), DEBUG_CLOCKS);
    QLOGINC;

    const QList<QUuid> ids = source.clocks_.keys();
    QLOGC("Pulling clocks for " << ids.size() << " instances", DEBUG_CLOCKS);

    for (int i = 0; i < ids.size(); ++i)
    {
        const QUuid &id = ids.at(i);
        clocks_.insert(id, source.clocks_.value(id));
    }

    QLOGC("Pushing own master clock instance to source", DEBUG_CLOCKS);
    source.clocks_.insert(masterID_, clocks_.value(masterID_));

    QLOGDEC;
}

QStringList DbClocks::toString() const
{
    QStringList ret;
    QList<QUuid> keys = clocks_.keys();
    for (int i = 0; i < keys.size(); ++i)
    {
        const QUuid &id = keys.at(i);
        Q_ASSERT(clocks_.contains(id));
        const DbClock& clock = clocks_[id];
        ret.append(clock.toString());
    }

    return ret;
}

// ==============================================================================================
// ============================== DbEvent
// ==============================================================================================

DbEvent::DbEvent(const DbEvent::Type type, const QUuid &sourceID, const QDateTime &dbCreated, const qint64 clock,
                 const QVariant &arg1, const QVariant &arg2, const QVariant &arg3) :
    type_(type),
    sourceID_(sourceID),
    timestamp_(dbCreated.secsTo(QDateTime::currentDateTime())),
    clock_(clock),
    arg1_(arg1), arg2_(arg2), arg3_(arg3)
{
    QLOGX("Created event of type " << type_ << ", source: " << sourceID_.toString() << ", timestamp = " << timestamp_ << ", clock = " << clock_);
}

bool DbEvent::operator ==(const DbEvent &arg) const
{
    return (type_ == arg.type_ && sourceID_ == arg.sourceID_ && timestamp_ == arg.timestamp_ && clock_ == arg.clock_ &&
            arg1_ == arg.arg1_ && arg2_ == arg.arg2_ && arg3_ == arg.arg3_);
}

bool DbEvent::operator !=(const DbEvent &arg) const
{
    return !(*this == arg);
}

QTextStream& operator<<(QTextStream &os, const DbEvent &arg)
{
    return (os << "type: " << arg.type() << ", source: " << arg.sourceID().toString() << ", timestamp = " << arg.timestamp() << ", clock = " << arg.clock());
}

// ==============================================================================================
// ============================== DbEventLog
// ==============================================================================================

void DbEventLog::addEvent(const DbEvent &event)
{
    QLOGCX("Appending event: " << event, DEBUG_EVENTS);
    events_.append(event);
}

void DbEventLog::remove(const int index)
{
    Q_ASSERT(index >= 0 && index < events_.size());
	QLOGCX("Removing event " << index, DEBUG_EVENTS);
    events_.removeAt(index);
}


// Check if the event log contains an event matching the provided source ID and logical clock state.
// Used when checking event log vs. clocks consistency inside Database.
int DbEventLog::eventMatch(const QUuid &sourceID, const qint64 clock) const
{
	for (int i = 0; i < eventCount(); ++i)
	{
		const DbEvent &e = event(i);
		if (e.sourceID() == sourceID && e.clock() == clock) return i;
	}

	return -1;
}

// ==============================================================================================
// ============================== DbEntry
// ==============================================================================================

DbEntry::DbEntry() :
    createStamp_(-1), updateStamp_(-1),
	descShownPoints_(0), descShownFails_(0), 
	itemShownPoints_(0), itemShownFails_(0)
{
}

/// Create new by user.
DbEntry::DbEntry(const QUuid &uuid, const QDateTime &dbCreated, const QString &item, const QString &desc) :
    uuid_(uuid),
    createStamp_(dbCreated.secsTo(QDateTime::currentDateTime())),
    updateStamp_(-1),
	item_(item), desc_(desc), 
	descShownPoints_(0), descShownFails_(0), 
	itemShownPoints_(0), itemShownFails_(0)
{
}

// compare two DbEntry instances, everything checked if "complete" requested, otherwise item and description text will be ignored
bool DbEntry::equal(const DbEntry &other, const bool complete) const
{
    return (uuid_ == other.uuid_
            && (item_ == other.item_ || !complete)
            && (desc_ == other.desc_ || !complete)
            && descShownPoints_ == other.descShownPoints_
            && itemShownPoints_ == other.itemShownPoints_
            && descShownFails_ == other.descShownFails_
            && itemShownFails_ == other.itemShownFails_);
}

int DbEntry::points(const QuizDirection type) const
{
	int ret = -1;
	switch(type)
	{
		case DIR_SHOWDESC: ret = descShownPoints_; break;
		case DIR_SHOWITEM: ret = itemShownPoints_; break;
		default: break;
	}
	return ret;
}

int DbEntry::fails(const QuizDirection type) const
{
	int ret = -1;
	switch(type)
	{
		case DIR_SHOWDESC: ret = descShownFails_; break;
		case DIR_SHOWITEM: ret = itemShownFails_; break;
		default: break;
	}
	return ret;
}

void DbEntry::addPoint(const QuizDirection type)
{
    switch(type)
    {
    case DIR_SHOWDESC:
        if (descShownPoints_ < 100) descShownPoints_++;
        break;
    case DIR_SHOWITEM:
        if (itemShownPoints_ < 100) itemShownPoints_++;
        break;
    default:
        break;
    }
}

void DbEntry::addFail(const QuizDirection type)
{
    switch(type)
    {
    case DIR_SHOWDESC:
        descShownPoints_ = 0;
        descShownFails_++;
        break;
    case DIR_SHOWITEM:
        itemShownPoints_ = 0;
        itemShownFails_++;
        break;
    default:
        break;
    }
}

void DbEntry::undoPoint(const DbEntry::QuizDirection type)
{
    QLOGX("Undoing point for type: " << type);
    QLOGINC;

    switch(type)
    {
    case DIR_SHOWDESC:
        if (descShownPoints_ > 0) descShownPoints_--;
        break;
    case DIR_SHOWITEM:
        if (itemShownPoints_ > 0) itemShownPoints_--;
        break;
    default:
        break;
    }

    QLOGDEC;
}

void DbEntry::undoFail(const DbEntry::QuizDirection type, const int points)
{
    QLOGX("Undoing fail for type: " << type << ", points: " << points);
    QLOGINC;

    switch(type)
    {
    case DIR_SHOWDESC:
        descShownPoints_ = points;
        descShownFails_--;
        break;
    case DIR_SHOWITEM:
        itemShownPoints_ = points;
        itemShownFails_--;
        break;
    default:
        break;
    }

    QLOGDEC;
}

void DbEntry::update(const QDateTime &dbCreated)
{ 
    updateStamp_ = dbCreated.secsTo(QDateTime::currentDateTime());
}

void DbEntry::reset() 
{ 
	descShownPoints_ = itemShownPoints_ = descShownFails_ = itemShownFails_ = 0; 
}

QTextStream& operator<<(QTextStream &os, const DbEntry &arg)
{
    return (os << "'" << arg.item() << "'/'" << arg.description() << "' " << arg.uuid().toString()
		<< " (points: " << arg.points(DbEntry::DIR_SHOWDESC) << "/L" << arg.level(DbEntry::DIR_SHOWDESC) 
		<< ", "         << arg.points(DbEntry::DIR_SHOWITEM) << "/L" << arg.level(DbEntry::DIR_SHOWITEM)
		<< ") (fails: " <<  arg.fails(DbEntry::DIR_SHOWDESC) << "/"  << arg.fails(DbEntry::DIR_SHOWITEM) 
        << "), creat: " << arg.createStamp() << ", upd: " << arg.updateStamp());
}

// ==============================================================================================
// ============================== DbError
// ==============================================================================================

QTextStream& operator<<(QTextStream &os, const DbError &arg)
{
    return (os << arg.type()
            << (arg.msg().isEmpty() ? "" : QString(", '") + arg.msg() + "'")
            << (arg.index() < 0 ? "" : QString(", idx = ") + QString::number(arg.index())));
}

// ==============================================================================================
// ============================== Database
// ==============================================================================================

Database::Database(QObject *parent) : QObject(parent),
	dirty_(false), error_(false), locked_(false), external_(false)
{
	QLOGX("Creating empty");
}

Database::Database(QObject *parent, const QString &name) : QObject(parent),
    dbUuid_(QUuid::createUuid()), insUuid_(QUuid::createUuid()),
	name_(name),
	created_(QDateTime::currentDateTime()), updated_(QDateTime::currentDateTime()),
    clocks_(insUuid_),
	dirty_(true), error_(false), locked_(false), external_(false)
{
    QLOGX("Creating new, name: '" << name_ << "', dbUuid: " << dbUuid_.toString() << ", insUuid: " << insUuid_.toString());

    if (parent)
    {
        connect(this, SIGNAL(countUpdate()), parent, SLOT(slot_database_updateCount()));
    }
}

bool Database::equal(const Database &arg, const bool complete) const
{
    QLOGX("Comparing instances of database '" << name() << "': " << dbUuid_.toString() << ", complete: " << complete);
    QLOGINC;

    if (name() != arg.name())
    {
        QLOG("Names mismatch");
        QLOGDEC;
        return false;
    }
    else if (dbUuid_ != arg.dbUuid_)
    {
        QLOG("Uuids mismatch");
        QLOGDEC;
        return false;
    }
    else if (entryCount() != arg.entryCount())
    {
        QLOG("Entry count mismatch");
        QLOGDEC;
        return false;
    }

    // create list of entries' ids for lhs and rhs operand
    QList<QUuid> lhsIDs, rhsIDs;
    for (int i = 0; i < entryCount(); ++i)
    {
        const DbEntry
                &e1 = entry(i),
                &e2 = arg.entry(i);

        lhsIDs.append(e1.uuid());
        rhsIDs.append(e2.uuid());
    }

    // iterate over obtained ids and create list of unique ones
    QList<QUuid> uniqueIDs;
    for (int i = 0; i < lhsIDs.size(); ++i)
    {
        const QUuid
                &lhsID = lhsIDs.at(i),
                &rhsID = rhsIDs.at(i);

        if (!rhsIDs.contains(lhsID))
        {
            QLOG("ID " << lhsID.toString() << " not present in RHS argument");
            QLOGDEC;
            return false;
        }
        else if (!lhsIDs.contains(rhsID))
        {
            QLOG("ID " << rhsID.toString() << " not present in LHS argument");
            QLOGDEC;
            return false;
        }

        if (!uniqueIDs.contains(lhsID)) uniqueIDs.append(lhsID);
        if (!uniqueIDs.contains(rhsID)) uniqueIDs.append(rhsID);
    }
    QLOG("Obtained " << uniqueIDs.size() << " unique IDs");

    // iterate over unique id list, compare equivalent entries
    for (int i = 0; i < uniqueIDs.size(); ++i)
    {
        const QUuid &id = uniqueIDs.at(i);
        const int
                lhsIdx = entryIndex(id),
                rhsIdx = arg.entryIndex(id);
        const DbEntry
                &lhsEntry = entry(lhsIdx),
                &rhsEntry = entry(rhsIdx);

        if (!lhsEntry.equal(rhsEntry, complete))
        {
            QLOG("Entry mismatch between lhs: " << lhsEntry << " (" << lhsIdx << ") and rhs: " << rhsEntry << " (" << rhsIdx << ")");
            return false;
        }
    }

    QLOGDEC;
    return true;
}

// todo: change entries_ to QHash<QUuid, DbEntry>
int Database::entryIndex(const QUuid &uuid) const
{
    int ret = -1;

    for (int i = 0; i < entryCount(); ++i)
    {
        const DbEntry &e = entry(i);
        if (e.uuid() == uuid) return i;
    }

    return ret;
}

int Database::findEntry(const QString &text, int startIndex) const
{
	int ret = -1;
	Q_ASSERT(startIndex >= 0 && startIndex < entryCount());
    QLOGX("Database '" << name() << " searching for match with '" << text << "' starting from idx " << startIndex);
	QLOGINC;
	for (int i = startIndex; i < entryCount(); ++i)
	{
        const DbEntry &e = entry(i);
		const QString 
			&item = e.item(),
			&desc = e.description();

		if (item.contains(text, Qt::CaseInsensitive) || desc.contains(text, Qt::CaseInsensitive))
		{
            QLOG("Match on entry " << i << ": " << e);
			ret = i; break;
		}
	}
	QLOGDEC;
	return ret;
}

int Database::findDuplicate(const DbEntry &entry, int exceptIdx) const
{
    for (int i = 0; i < entries_.count(); ++i)
    {
        if (i == exceptIdx) continue; //skip over 'this' item
        const DbEntry &curitem = entries_.at(i);
        if (curitem.item().contains(entry.item(), Qt::CaseInsensitive) ||
            curitem.description().contains(entry.description(), Qt::CaseInsensitive) ||
            entry.item().contains(curitem.item(), Qt::CaseInsensitive) ||
            entry.description().contains(curitem.description(), Qt::CaseInsensitive))
        {
            QLOGX("Duplicate hit at idx " << i << ", item '" << curitem.item() << "', desc '" << curitem.description()
                << "' while searching for duplicates of item '"
                << entry.item() << "', desc '" << entry.description() << "'");
            return i;
        }
    }
    return -1;
}

void Database::fork(Database &other)
{
    QLOGX("Forking off from database '" << other.name() << "', uuid: " << other.dbUuid().toString() << ", instance: " << other.instanceUuid().toString());
    QLOGINC;

    dbUuid_ = other.dbUuid();
    insUuid_ = QUuid::createUuid();
    QLOG("Instance uuid: " << insUuid_.toString());

    name_ = other.name();
    created_ = other.dateCreated();
    updated_ = other.dateUpdated();

    clocks_ = DbClocks(insUuid_);
    clocks_.fork(other.clocks_);

    entries_ = other.entries_;
    eventLog_ = other.eventLog_;

    QLOGDEC;
}

DbError Database::sync(Database &other)
{
    const QUuid
            idThis  = instanceUuid(),
            idOther = other.instanceUuid();

    QLOGX("Synchronizing database '" << name() << "', this instance: " << idThis.toString() << " with other instance: " << idOther.toString());
    QLOGINC;

    // synchronizing databases must have the same dbUuid, but different instance uuids
    if (dbUuid() != other.dbUuid() || idThis == idOther)
    {
        QLOG("Database ID mismatch");
        QLOGDEC;
        return DbError(DbError::ERROR_SYNC, "Database ID mismatch");
    }

    // extract current clocks' state from both databases
    const DbClocks
            clocksThis  = clocks(),
            clocksOther = other.clocks();
    QLOG("clocksThis: "  << clocksThis.toString());
    QLOG("clocksOther: " << clocksOther.toString());


    // obtain lists of events which happened in both databases since the last sync or fork
    const QList<DbEvent>
            eventsThis  = findMissingEvents(clocksOther),
            eventsOther = other.findMissingEvents(clocksThis);
    QLOG("eventsThis missing in other: " << eventsThis);
	QLOG("eventsOther missing in this: " << eventsOther);

    // Rollback both databases to the state they were in when they diverged
    // Actually, only point and fail operations are rolled back because the final (synchronized) state of all entries' points and fails
    // depends on the order of operations. There is no need to rollback adds, removes and alters only to redo them later.
    const bool rollbackThisOK = rollbackEvents(eventsThis);
    if (!rollbackThisOK)
    {
        QLOG("Failed rollback on this database, sync failed");
        QLOGDEC;
        return DbError(DbError::ERROR_SYNC, "Rollback failed in local database");
    }

    const bool rollbackOtherOK = other.rollbackEvents(eventsOther);
    if (!rollbackOtherOK)
    {
        // TODO: reapply rolled events in A, or separate rollback into rollbackIndices() and doRollback() to make sure both
        // rollbacks are feasible before going ahead with either
        QLOG("Failed rollback on other database, sync failed");
        QLOGDEC;
        return DbError(DbError::ERROR_SYNC, "Rollback failed in remote database");
    }

    // databases should be equal in regard to contents, at least as point counts are concerned
    const bool preMergeEqual = equal(other, false);
	QLOG("Pre-merge equal: " << preMergeEqual);
	Q_ASSERT(preMergeEqual);

    // merge events from both databases, order them according to timestamp value
    QList<DbEvent> mergedEvents = eventsThis + eventsOther;
    qSort(mergedEvents.begin(), mergedEvents.end());
    QLOG("Merged and sorted events: " << mergedEvents);

	QLOG("Reapplying events to both databases");
	QLOGINC;
    // apply merged events to both databases
    for (int i = 0; i < mergedEvents.size(); ++i)
    {
        const DbEvent &e = mergedEvents.at(i);
        QLOG("Event " << i << ": " << e);

        QLOG("Applying to this database");
        applyEvent(e, false);
        QLOG("Applying to other database");
        other.applyEvent(e, false);
    }
	QLOGDEC;

    // the databases should now be completely equal in regard to contents
    const bool postMergeEqual = equal(other, true);
	QLOG("Post-merge equal: " << postMergeEqual);
	Q_ASSERT(postMergeEqual);
    // TODO: compare clocks and event logs for equivalence as well

    QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::add(const QString &item, const QString &desc, bool dupIgnore)
{
    DbEvent ev(DbEvent::EVENT_EADD, insUuid_, created_, clocks_.masterTicks() + 1, QUuid::createUuid().toString(), QVariant(item), QVariant(desc));
    return applyEvent(ev, dupIgnore);
}

DbError Database::remove(int idx)
{
    DbEvent ev(DbEvent::EVENT_EDEL, insUuid_, created_, clocks_.masterTicks() + 1, QVariant(entry(idx).uuid().toString()));
    return applyEvent(ev, false);
}

DbError Database::remove(const QList<int> &idxs)
{
	QLOGX("Removing " << idxs.count() << " items");
	QLOGINC;

    for (int i = idxs.size() - 1; i >= 0; --i)
	{
		int idx = idxs.at(i);
		Q_ASSERT(idx < entryCount());

        const DbError e = remove(idx);
        if (e != DbError::ERROR_OK)
        {
            QLOG("Error at item " << i << " (" << idx << "): " << e);
            return e;
        }
	}

	QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::alter(int idx, const QString &item, const QString &desc, bool dupIgnore)
{
    DbEvent ev(DbEvent::EVENT_EALTER, insUuid_, created_, clocks_.masterTicks() + 1 ,
               QVariant(entry(idx).uuid().toString()),
               QVariant(item),
               QVariant(desc));
    return applyEvent(ev, dupIgnore);
}

DbError Database::point(const int idx, const DbEntry::QuizDirection type)
{
    DbEvent ev(DbEvent::EVENT_EPOINT, insUuid_, created_, clocks_.masterTicks() + 1,
               QVariant(entry(idx).uuid().toString()),
               QVariant(type));
    return applyEvent(ev, false);
}

DbError Database::fail(const int idx, const DbEntry::QuizDirection type)
{
    const DbEntry &e = entry(idx);
    DbEvent ev(DbEvent::EVENT_EFAIL, insUuid_, created_, clocks_.masterTicks() + 1,
               QVariant(e.uuid().toString()),
               QVariant(type),
               QVariant(e.points(type)));
    return applyEvent(ev, false);
}

DbError Database::reset()
{
    if (locked_) { QLOGX("Database locked"); return DbError(DbError::ERROR_LOCK); }

	for (int i = 0; i < entries_.count(); ++i)
	{
		DbEntry &entry = entries_[i];
		entry.reset();
        entry.update(created_);
	}

	dirty_ = true;
    return DbError(DbError::ERROR_OK);
}

DbError Database::applyEvent(const DbEvent &event, const bool force)
{
	QLOGCX("Applying event: " << event << ", force: " << force, DEBUG_EVENTS);
    QLOGINC;

    // exit if database locked
    if (locked_) 
	{ 
		QLOG("Database locked!"); 
		QLOGDEC; 
		return DbError(DbError::ERROR_LOCK); 
	}
    DbError opError(DbError::ERROR_OK);

	// this allows to differentiate between events that wre previously unknown and events which are being reapplied after rollback
	const qint64 ticks = clocks_.ticks(event.sourceID());
	const bool newEvent = ticks < event.clock();
	QLOGC("Ticks count for source in database = " << ticks << ", newEvent: " << newEvent, DEBUG_EVENTS);

    // dispatch event to appropriate handler
	// add/remove/alter operations are not really rolled back and they don't need to be reapplied if this is not a new event but a rollback'd one
    switch (event.type())
    {
        case DbEvent::EVENT_EADD:   
			if (newEvent) opError = applyAdd(event.arg1(), event.arg2(), event.arg3(), force); 
			break;
        case DbEvent::EVENT_EDEL:   
			if (newEvent) opError = applyRemove(event.arg1()); 
			break;
        case DbEvent::EVENT_EALTER: 
			if (newEvent) opError = applyAlter(event.arg1(), event.arg2(), event.arg3(), force); 
			break;
        case DbEvent::EVENT_EPOINT: 
			opError = applyPoint(event.arg1(), event.arg2()); 
			break;
        case DbEvent::EVENT_EFAIL:  
			opError = applyFail(event.arg1(), event.arg2()); 
			break;
        case DbEvent::EVENT_UNDEF:  
			break;
    }

    // add event to queue if operation succeeded
    if (opError == DbError::ERROR_OK)
    {
		QLOGC("Adding event to log", DEBUG_EVENTS);
        dirty_ = true;
        eventLog_.addEvent(event);

		// update clock only for non-rollback events 
		if (newEvent) 
		{
			clocks_.tick(event.sourceID());
			Q_ASSERT(clocks_.ticks(event.sourceID()) == event.clock());
		}
    }

    QLOGC("Operation result = " << opError, DEBUG_EVENTS);
    QLOGDEC;
	return opError;
}

DbError Database::applyAdd(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore)
{
    // convert arguments from QVariants
    const QUuid entryID(uuid.toString());
    const QString
            itemText = item.toString(),
            descText = desc.toString();

    QLOGCX("Attempting to add entry, uuid = " << entryID.toString() << ", item: '" << itemText << "', desc: '" << descText << "'", DEBUG_EVENTS);
    QLOGINC;
    // check if arguments are OK
    if (entryID.isNull() || itemText.isEmpty() || descText.isEmpty())
    {
        QLOG("Bad arguments for operation");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS);
    }

    // create a new entry
    DbEntry addme = DbEntry(entryID, created_, itemText, descText);
    QLOGC("Adding entry: " << addme, DEBUG_EVENTS);

    // check for pre-existing duplicates, won't add unless duplication ignore mode specified
    if (!dupIgnore)
    {
        int dupidx = findDuplicate(addme);
        if (dupidx >= 0)
        {
            QLOG("Found duplicate, not applying");
            QLOGDEC;
            return DbError(DbError::ERROR_DUPLI, "duplicate found on add", dupidx);
        }
    }

    // add new entry to database, mark database as altered, notify subscribers of change
    QLOGC("Applying add", DEBUG_EVENTS);
    emit beginInsert();
    entries_.append(addme);
    emit endInsert();
    emit countUpdate();

    QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::applyRemove(const QVariant &uuid)
{
    // retrieve uuid
    const QUuid entryID(uuid.toString());

    QLOGCX("Attempting to remove entry, uuid = " << entryID.toString(), DEBUG_EVENTS);
    QLOGINC;
    // check if uuid is OK
    if (entryID.isNull())
    {
        QLOG("Invalid uuid");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS);
    }

    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0)
    {
        QLOG("Element not found!");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "uuid not found", idx);
    }

    // perform the remove
    QLOGC("Found at index " << idx << ", removing", DEBUG_EVENTS);
    emit beginRemove(idx);
    entries_.removeAt(idx);
    emit endRemove();
    emit countUpdate();

    QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::applyAlter(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore)
{
    // convert arguments from QVariants
    const QUuid entryID(uuid.toString());
    const QString
            itemText = item.toString(),
            descText = desc.toString();

    QLOGCX("Attempting to alter entry, uuid = " << entryID.toString() << ", item: '" << itemText << "', desc: '" << descText << "'", DEBUG_EVENTS);
    QLOGINC;
    // check if arguments are OK
    if (entryID.isNull() || itemText.isEmpty() || descText.isEmpty())
    {
        QLOG("Bad arguments for operation");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "bad args for alter");
    }

    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0)
    {
        QLOG("Element not found!");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "uuid not found", idx);
    }

    // obtain original entry, copy to updated entry and set the new item and description text on that one
    const DbEntry &original = entry(idx);
    QLOGC("Original: " << original << " to '" << itemText << "'/'" << descText << "'", DEBUG_EVENTS);
    DbEntry updated(original);
    updated.setItem(itemText);
    updated.setDescription(descText);
    QLOGC("Updated: " << updated, DEBUG_EVENTS);

    // if needed, check for duplicates of the updated entry
    if (!dupIgnore)
    {
        int dupidx = findDuplicate(updated, idx);
        if (dupidx >= 0)
        {
            QLOG("Found duplicate, not applying");
            QLOGDEC;
            return DbError(DbError::ERROR_DUPLI, "duplicate found for updated item on alter", dupidx);
        }
    }

    // replace the original entry by the updated one
    QLOGC("Applying edit", DEBUG_EVENTS);
    entries_.replace(idx, updated);

    QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::applyPoint(const QVariant &uuid, const QVariant &type)
{
    const QUuid entryID(uuid.toString());
    const DbEntry::QuizDirection quizDir = static_cast<DbEntry::QuizDirection>(type.toInt());

    QLOGCX("Attempting to add a point for uuid " << entryID.toString() << ", type: " << quizDir, DEBUG_EVENTS);
    QLOGINC;
    // check if args are correct
    if (entryID.isNull() || (quizDir != DbEntry::DIR_SHOWDESC && quizDir != DbEntry::DIR_SHOWITEM))
    {
        QLOG("Invalid args for operation");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "bad args for point add");
    }

    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0)
    {
        QLOG("Element not found!");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "uuid not found", idx);
    }

    DbEntry &e = entries_[idx];
    QLOGC("Found at idx " << idx << ", point for entry: " << e, DEBUG_EVENTS);
    e.addPoint(quizDir);
    e.update(created_);

    QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

DbError Database::applyFail(const QVariant &uuid, const QVariant &type)
{
    const QUuid entryID(uuid.toString());
    const DbEntry::QuizDirection quizDir = static_cast<DbEntry::QuizDirection>(type.toInt());

    QLOGCX("Attempting to add a fail for uuid " << entryID.toString() << ", type: " << quizDir, DEBUG_EVENTS);
    QLOGINC;
    // check if args are correct
    if (entryID.isNull() || (quizDir != DbEntry::DIR_SHOWDESC && quizDir != DbEntry::DIR_SHOWITEM))
    {
        QLOG("Invalid args for operation");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "bad args for fail add");
    }

    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0)
    {
        QLOG("Element not found!");
        QLOGDEC;
        return DbError(DbError::ERROR_ARGS, "uuid not found", idx);
    }

    DbEntry &e = entries_[idx];
    QLOGC("Found at idx " << idx << ", fail for entry: " << e, DEBUG_EVENTS);
    e.addFail(quizDir);
    e.update(created_);
    QLOGDEC;

    return DbError(DbError::ERROR_OK);
}

/// Obtain a list of events that happened inside this database since the provided clocks' state.
QList<DbEvent> Database::findMissingEvents(const DbClocks &clocks) const
{
    QList<DbEvent> events;

    // iterate over all events, find those unknown to the other database based on the vector clocks' ticks for the respective instance ids
    for (int i = 0; i < eventLog().eventCount(); ++i)
    {
        const DbEvent &e = eventLog().event(i);
        if (e.clock() > clocks.ticks(e.sourceID())) events.append(e);
    }

    return events;
}

bool Database::rollbackEvents(const QList<DbEvent> &events)
{
    QLOGX("Rolling back " << events.size() << " events");
    QLOGINC;

    QList<int> indices;

    // iterate through list events, generate list of indices
    for (int i = 0; i < events.size(); ++i)
    {
        const DbEvent &e = events.at(i);
        // find index of event
        const int idx = eventLog_.index(e);

        // add to list if found, terminate whole operation if not
        if (idx >= 0) indices.append(idx);
        else
        {
            QLOGDEC;
            return false;
        }
    }

    // sort indices in descending order
    qSort(indices.begin(), indices.end(), qGreater<int>());

    // remove events from log, reverting changes as we go along
    for (int i = 0; i < indices.size(); ++i)
    {
        const int idx = indices.at(i);
        const DbEvent &e = eventLog_.event(idx);
        QLOG("Event " << i << " (" << idx << "): " << e);
        if (e.type() == DbEvent::EVENT_EPOINT || e.type() == DbEvent::EVENT_EFAIL) rollbackEvent(e);
        eventLog_.remove(idx);
    }

    QLOGDEC;
    return true;
}

bool Database::rollbackEvent(const DbEvent &event)
{
    QLOGCX("Rolling back event: " << event, DEBUG_EVENTS);
    QLOGINC;

    DbEvent::Type eventType = event.type();

    if (eventType == DbEvent::EVENT_EPOINT)
    {
        const QUuid entryID(event.arg1().toString());
        const DbEntry::QuizDirection type = static_cast<DbEntry::QuizDirection>(event.arg2().toInt());
        const int idx = entryIndex(entryID);
        if (idx >= 0)
        {
            DbEntry &entry = entries_[idx];
            entry.undoPoint(type);
        }
        else
        {
            QLOG("Entry not found for point undo");
            QLOGDEC;
            return false;
        }
    }
    else if (eventType == DbEvent::EVENT_EFAIL)
    {
        const QUuid entryID(event.arg1().toString());
        const DbEntry::QuizDirection type = static_cast<DbEntry::QuizDirection>(event.arg2().toInt());
        bool pointsOK = true;
        const int points = event.arg3().toInt(&pointsOK);
        Q_ASSERT(pointsOK);
        const int idx = entryIndex(entryID);
        if (idx >= 0)
        {
            DbEntry &entry = entries_[idx];
            entry.undoFail(type, points);
        }
        else
        {
            QLOG("Entry not found for fail undo");
            QLOGDEC;
            return false;
        }
    }
    else
    {
        QLOG("Unsupported event type for rollback");
    }

    QLOGDEC;
    return false;
}

bool Database::checkEventsClocksConsistency() const
{
	QLOGX("Checking consistency of clocks and events inside database instance " << insUuid_.toString());
	QLOGINC;

	QList<QUuid> clockIds = clocks_.ids();

	// part 1, iterate over instance clocks, retrieve tick count for instance and check that an event exists for every tick value from 1 .. tick count
	for (int c = 0; c < clockIds.size(); ++c)
	{
		const QUuid &clockId = clockIds.at(c);
		const qint64 ticks = clocks_.ticks(clockId);
		QLOG("Source: " << clockId.toString() << ", clock = " << ticks);

		for (qint64 t = 1; t <= ticks; ++t)
		{
			const int matchIdx = eventLog_.eventMatch(clockId, t);
			if (matchIdx < 0) 
			{
				QLOG("Unable to find match for event from " << clockId.toString() << ", clocks: " << t);
				QLOGDEC;
				return false;
			}
		}
	}

	// part 2, iterate over events, check that current clock tick count for the event source is at least as much as the value inside the event
	for (int e = 0; e < eventLog_.eventCount(); ++e)
	{
		const DbEvent &event = eventLog_.event(e);
		const qint64 ticks = clocks_.ticks(event.sourceID());
		if (ticks < event.clock())
		{
			QLOG("Event " << event << " exceeds tick count in clocks structure: " << ticks);
			QLOGDEC;
			return false;
		}
	}

	QLOGDEC;

	return true;
}
