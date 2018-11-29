#ifndef DISTRIBUTED_DATABASE_H
#define DISTRIBUTED_DATABASE_H

#include "database.h"
#include "clocks.h"
#include "event.h"

namespace wynn {
namespace db {

// A database which has the capability to fork copies to be used in a different instance.
// In theory, every copy can undergo different changes and it will be later possible to sync them.
// This is based on the vector clocks/lazy replication mechanism, but it was never finished 
// and I don't remember how it works any more, so all of the distributed stuff goes in a separate class
// to be completed later, or removed once I come up with a better approach for data synchronization.
// This will be important for the Android version.
class DistributedDatabase : public Database
{
    Q_OBJECT

public:
	static const QString XML_DBID, XML_INSID;

private:
    QUuid dbUuid_, insUuid_;
    DbClocks clocks_;
	DbEventLog eventLog_;

public:
    DistributedDatabase(QObject *parent);
    DistributedDatabase(QObject *parent, const QString &name);

    bool equal(const DistributedDatabase &arg, const bool complete) const;

    const QUuid& dbUuid() const { return dbUuid_; }
    const QUuid& instanceUuid() const { return insUuid_; }
    const DbClocks& clocks() const { return clocks_; }
    const DbEventLog& eventLog() const { return eventLog_; }
    int entryIndex(const QUuid &uuid) const;    

    void fork(DistributedDatabase &other);
    DbError sync(DistributedDatabase &other);

    DbError add(const QString &item, const QString &desc, bool dupIgnore = false) override;
    DbError remove(int idx) override;
    DbError remove(const QList<int> &idxs) override;
    DbError alter(int idx, const QString &item, const QString &desc, bool dupIgnore = false) override;
    DbError point(const int idx, const DbEntry::QuizDirection type) override;
    DbError fail(const int idx, const DbEntry::QuizDirection type) override;

    virtual void saveXMLExtraAttributes(QXmlStreamWriter &xml) const override;
    virtual void saveXMLExtraElements(QXmlStreamWriter &xml) const override;
    virtual void loadXMLExtraAttributes(QXmlStreamReader &xml) override;
    virtual void loadXMLExtraElements(QXmlStreamReader &xml, const QString &name) override;
    virtual Error loadXmlPostCheck();
    
private:
    DbError applyEvent(const DbEvent &event, const bool force);
    DbError applyAdd(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore = false);
    DbError applyRemove(const QVariant &uuid);
    DbError applyAlter(const QVariant &uuid, const QVariant &item, const QVariant &desc, bool dupIgnore = false);
    DbError applyPoint(const QVariant &uuid, const QVariant &type);
    DbError applyFail(const QVariant &uuid, const QVariant &type);

    QList<DbEvent> findMissingEvents(const DbClocks &clocks) const;
    bool rollbackEvents(const QList<DbEvent> &events);
    bool rollbackEvent(const DbEvent &event);
	bool checkEventsClocksConsistency() const;
};

} // namespace db
} // namespace wynn 

#endif // DISTRIBUTED_DATABASE_H
