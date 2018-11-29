#include "distributed.h"
#include "ixplog_active.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#define DEBUG_EVENTS 0
#define DEBUG_CLOCKS 1

namespace wynn {
namespace db {

const QString 
    DistributedDatabase::XML_DBID  = "id",
    DistributedDatabase::XML_INSID = "instance";

DistributedDatabase::DistributedDatabase(QObject *parent) : Database(parent)
{
}

DistributedDatabase::DistributedDatabase(QObject *parent, const QString &name) : Database(parent),
    dbUuid_(QUuid::createUuid()), insUuid_(QUuid::createUuid()),
    clocks_(insUuid_)
{
    QLOGX("Creating new, name: '" << name_ << "', dbUuid: " << dbUuid_.toString() << ", insUuid: " << insUuid_.toString());
}

bool DistributedDatabase::equal(const DistributedDatabase &arg, const bool complete) const
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
int DistributedDatabase::entryIndex(const QUuid &uuid) const
{
    int ret = -1;
    
    for (int i = 0; i < entryCount(); ++i)
    {
        const DbEntry &e = entry(i);
        if (e.uuid() == uuid) return i;
    }
    
    return ret;
}

void DistributedDatabase::fork(Database &other)
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

DbError DistributedDatabase::sync(Database &other)
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

DbError DistributedDatabase::add(const QString &item, const QString &desc, bool dupIgnore)
{
    DbEvent ev(DbEvent::EVENT_EADD, insUuid_, created_, clocks_.masterTicks() + 1, QUuid::createUuid().toString(), QVariant(item), QVariant(desc));
    return applyEvent(ev, dupIgnore);
}

DbError DistributedDatabase::remove(int idx)
{
    DbEvent ev(DbEvent::EVENT_EDEL, insUuid_, created_, clocks_.masterTicks() + 1, QVariant(entry(idx).uuid().toString()));
    return applyEvent(ev, false);
}

DbError DistributedDatabase::alter(int idx, const QString &item, const QString &desc, bool dupIgnore)
{
    DbEvent ev(DbEvent::EVENT_EALTER, insUuid_, created_, clocks_.masterTicks() + 1 ,
               QVariant(entry(idx).uuid().toString()),
               QVariant(item),
               QVariant(desc));
    return applyEvent(ev, dupIgnore);
}

DbError DistributedDatabase::point(const int idx, const DbEntry::QuizDirection type)
{
    DbEvent ev(DbEvent::EVENT_EPOINT, insUuid_, created_, clocks_.masterTicks() + 1,
               QVariant(entry(idx).uuid().toString()),
               QVariant(type));
    return applyEvent(ev, false);
}

DbError DistributedDatabase::fail(const int idx, const DbEntry::QuizDirection type)
{
    const DbEntry &e = entry(idx);
    DbEvent ev(DbEvent::EVENT_EFAIL, insUuid_, created_, clocks_.masterTicks() + 1,
               QVariant(e.uuid().toString()),
               QVariant(type),
               QVariant(e.points(type)));
    return applyEvent(ev, false);
}

void DistributedDatabase::saveXMLExtraAttributes(const QXmlStreamWriter &xml)
{
    xml.writeAttribute(XML_DBID, dbUuid_.toString());
    xml.writeAttribute(XML_INSID, insUuid_.toString());    
}

void DistributedDatabase::saveXMLExtraElements(const QXmlStreamWriter &xml)
{
    clocks_.toXML(xml);
    eventLog_.saveXML(xml);    
}

void DistributedDatabase::loadXMLExtraAttributes(const QXmlStreamReader &xml)
{
    QXmlStreamAttributes attr = xml.attributes();
    const QString
            dbUuidStr = attr.value(XML_DBID).toString(),
            insUuidStr = attr.value(XML_INSID).toString();
    
    QLOGC("Database start, dbUuid: '" << dbUuidStr << "', insUuid: '" << insUuidStr << "'", DBXML_DEBUG);
    
    const QUuid dbUuid(dbUuidStr), insUuid(insUuidStr);
    
    if (dbUuid.isNull() || insUuid.isNull()) {
        xml.raiseError("Invalid attributes");
    }
    else {
        dbUuid_ = dbUuid;
        insUuid_ = insUuid;
        clocks_.setMaster(insUuid_);
    }    
}

void DistributedDatabase::loadXMLExtraElements(const QXmlStreamReader &xml, const QString &name)
{
    if (name == DbClocks::XML_HEADER)
    {
        clocks_.loadXML(xml);
    }
    else if (name == DbEventLog::XML_HEADER)
    {
        eventLog_.loadXML(xml);
    }
    else {
        // raise error
        Database::loadXMLExtraElements(xml, name);
    }
}

Error DistributedDatabase::loadXmlPostCheck()
{
    if (!clocks_.hasMaster())
    {
        error_ = true;
        const QString errorStr("Database master clock not found in XML");
        QLOG("Error: " << errorStr);
        return { Error::PARSE, errorStr };
    }
    else return Error::OK;
}

DbError DistributedDatabase::applyEvent(const DbEvent &event, const bool force)
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

DbError DistributedDatabase::applyAdd(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore)
{
    // convert arguments from QVariants
    const QUuid entryID(uuid.toString());
    const QString
            itemText = item.toString(),
            descText = desc.toString();
    
    QLOGCX("Attempting to add entry, uuid = " << entryID.toString() << ", item: '" << itemText << "', desc: '" << descText << "'", DEBUG_EVENTS);
    // check if arguments are OK
    if (entryID.isNull() || itemText.isEmpty() || descText.isEmpty()) {
        QLOG("Bad arguments for operation");
        return DbError::ERROR_ARGS;
    }
    
    return Database::add(itemText, descText, entryID, dupIgnore);
}

DbError DistributedDatabase::applyRemove(const QVariant &uuid) {
    // retrieve uuid
    const QUuid entryID(uuid.toString());
    
    QLOGCX("Attempting to remove entry, uuid = " << entryID.toString(), DEBUG_EVENTS);
    // check if uuid is OK
    if (entryID.isNull()) {
        QLOG("Invalid uuid");
        return DbError::ERROR_ARGS;
    }
    
    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0) {
        QLOG("Element not found!");
        return { DbError::ERROR_ARGS, "uuid not found", idx };
    }
    
    return Database::remove(idx);
}

DbError DistributedDatabase::applyAlter(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore) {
    const QUuid entryID(uuid.toString());
    const QString
            itemText = item.toString(),
            descText = desc.toString();
    
    QLOGCX("Attempting to alter entry, uuid = " << entryID.toString() << ", item: '" << itemText << "', desc: '" << descText << "'", DEBUG_EVENTS);
    // check if arguments are OK
    if (entryID.isNull() || itemText.isEmpty() || descText.isEmpty()) {
        QLOG("Bad arguments for operation");
        return { DbError::ERROR_ARGS, "bad args for alter" };
    }
    
    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0) {
        QLOG("Element not found!");
        return { DbError::ERROR_ARGS, "uuid not found", idx };
    }
    
    return Database::alter(idx, itemText, descText, dupIgnore);
}

DbError DistributedDatabase::applyPoint(const QVariant &uuid, const QVariant &type) {
    const QUuid entryID(uuid.toString());
    const DbEntry::QuizDirection quizDir = static_cast<DbEntry::QuizDirection>(type.toInt());
    
    QLOGCX("Attempting to add a point for uuid " << entryID.toString() << ", type: " << quizDir, DEBUG_EVENTS);
    // check if args are correct
    if (entryID.isNull()) {
        QLOGX("Invalid args for operation");
        return { DbError::ERROR_ARGS, "bad args for point add" };
    }
    
    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0) {
        QLOGX("Element not found!");
        return { DbError::ERROR_ARGS, "uuid not found", idx };
    }
    
    return Database::point(idx, quizDir);
}

DbError DistributedDatabase::applyFail(const QVariant &uuid, const QVariant &type) {
    const QUuid entryID(uuid.toString());
    const DbEntry::QuizDirection quizDir = static_cast<DbEntry::QuizDirection>(type.toInt());
    
    QLOGCX("Attempting to add a fail for uuid " << entryID.toString() << ", type: " << quizDir, DEBUG_EVENTS);
    // check if args are correct
    if (entryID.isNull()) {
        QLOG("Invalid args for operation");
        return { DbError::ERROR_ARGS, "bad args for fail add" };
    }
    
    // find index of entry for this uuid
    const int idx = entryIndex(entryID);
    if (idx < 0) {
        QLOG("Element not found!");
        return { DbError::ERROR_ARGS, "uuid not found", idx };
    }
    
    return Database::fail(idx, quizDir);
}

/// Obtain a list of events that happened inside this database since the provided clocks' state.
QList<DbEvent> DistributedDatabase::findMissingEvents(const DbClocks &clocks) const
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

bool DistributedDatabase::rollbackEvents(const QList<DbEvent> &events)
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

bool DistributedDatabase::rollbackEvent(const DbEvent &event)
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

bool DistributedDatabase::checkEventsClocksConsistency() const
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

} // namespace db
} // namespace wynn 
