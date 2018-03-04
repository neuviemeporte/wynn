#ifndef DATABASE_H_
#define DATABASE_H_

#include <cmath>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDateTime>
#include <QUuid>

class QFile;
class DatabaseModel;
class QXmlStreamReader;
class QXmlStreamWriter;
class QTextStream;


/// The database logical clock, used to implement vector clocks for database synchronization.
class DbClock
{
public:
    static const QString
        XML_HEADER, XML_INSTANCE, XML_TICKS;

private:
    QUuid instanceID_;
	qint64 ticks_;

public:
    DbClock() : ticks_(0) {}
    DbClock(const QUuid &instanceID) : instanceID_(instanceID), ticks_(0) {}
    DbClock(QXmlStreamReader &xml);

    const QUuid& instanceID() const { return instanceID_; }
	qint64 ticks() const { return ticks_; }
    bool isNull() const { return instanceID_.isNull(); }

	void tick() { ticks_++; }
	void sync(DbClock &other);

    void toXML(QXmlStreamWriter &xml) const;

    QString toString() const;
};

QTextStream& operator<<(QTextStream &os, const DbClock &arg);

/// The vector clocks, i.e. a collection of clocks held by a database instance
/// that indicate the state of its own and other instances' clocks.
class DbClocks
{
public:
    static const QString XML_HEADER;

private:
    QUuid masterID_;
    QHash<QUuid, DbClock> clocks_;

public:
    DbClocks() {}
    DbClocks(const QUuid &masterID);

	bool operator==(const DbClocks &other) const;
	bool operator!=(const DbClocks &other) const;

    QUuid masterID() const { return masterID_; }
	QList<QUuid> ids() const { return clocks_.keys(); }
    bool isNull() const { return masterID_.isNull(); }
    bool hasMaster() const { return clocks_.contains(masterID_); }
    qint64 masterTicks() const { return ticks(masterID_); }
    qint64 ticks(const QUuid &id) const;

    void tick(const QUuid &id);
    void setMaster(const QUuid &masterID);

    void fork(DbClocks &source);

    void toXML(QXmlStreamWriter &xml) const;
    void loadXML(QXmlStreamReader &xml);

    QStringList toString() const;
};

/// A database event; an action that alters the contents of the database
class DbEvent
{
public:
	static const QString
        XML_HEADER, XML_TYPE, XML_SOURCE, XML_TIMESTAMP, XML_CLOCK,
		XML_ARG1, XML_ARG2, XML_ARG3;

	enum Type 
	{ 
        EVENT_UNDEF,  // unknown or error
        EVENT_EADD,   // an entry added to the database, arg1 = uuid, arg2 = item text, arg3 = description text
        EVENT_EDEL,   // an entry removed from the database, arg1 = uuid, arg2 = item text, arg3 = description text
        EVENT_EALTER, // item and/or description text changed in entry, arg1 = uuid, arg2 = old, arg3 = new
        EVENT_EPOINT, // point awarded to entry, arg1 = uuid, arg2 = quiz type
        EVENT_EFAIL   // fail recorded on entry, arg1 = uuid, arg2 = quiz type
	};

private:
	Type type_;
    QUuid sourceID_;
    qint64 timestamp_, clock_;
	QVariant arg1_, arg2_, arg3_;

public:
    DbEvent(const Type type, const QUuid &sourceID,
            const QDateTime &dbCreated, const qint64 clock,
		const QVariant &arg1 = QVariant(), 
		const QVariant &arg2 = QVariant(), 
		const QVariant &arg3 = QVariant());
    DbEvent(QXmlStreamReader &xml);

    bool operator==(const DbEvent &arg) const;
    bool operator!=(const DbEvent &arg) const;
    bool operator<(const DbEvent &arg) const { return (timestamp_ < arg.timestamp_); }

	Type type() const { return type_; }
    QUuid sourceID() const { return sourceID_; }
	qint64 timestamp() const { return timestamp_; }
    qint64 clock() const { return clock_; }
	const QVariant& arg1() const { return arg1_; }
	const QVariant& arg2() const { return arg2_; }
	const QVariant& arg3() const { return arg3_; }

    void toXML(QXmlStreamWriter &xml) const;
};

QTextStream& operator<<(QTextStream &os, const DbEvent &arg);

/// A log of database events. Used to replicate events across divergent database instances.
class DbEventLog
{
public:
	static const QString
		XML_HEADER;

private:
	QList<DbEvent> events_;

public:
    int eventCount() const { return events_.size(); }
    const DbEvent& event(const int idx) const { Q_ASSERT(idx >= 0 && idx < eventCount()); return events_.at(idx); }
    int index(const DbEvent &event) const { return events_.indexOf(event); }

    void addEvent(const DbEvent &event);
    void remove(const int index);

	void saveXML(QXmlStreamWriter &xml) const;
	void loadXML(QXmlStreamReader &xml);

	int eventMatch(const QUuid &sourceID, const qint64 clock) const;
};

/// A user-entered entry that is stored in a database.
class DbEntry
{
public:
	static const QString
		XML_HEADER,	XML_ITEM, XML_DESC,
		XML_DESCPOINTS, XML_DESCFAILS, 
		XML_ITEMPOINTS, XML_ITEMFAILS;

	enum QuizDirection { DIR_UNDEF = -1, DIR_SHOWDESC = 101, DIR_SHOWITEM = 202 };

private:
	QUuid uuid_;
    qint64 createStamp_, updateStamp_;
	QString item_, desc_;
	int	descShownPoints_, descShownFails_, 
		itemShownPoints_, itemShownFails_;

public:
	DbEntry();
    DbEntry(const QUuid &uuid, const QDateTime &dbCreated,
            const QString &item, const QString &desc);
    DbEntry(QXmlStreamReader &xml);

    bool equal(const DbEntry &other, const bool complete) const;
    bool operator==(const DbEntry &arg) const { return equal(arg, true); }
    bool operator!=(const DbEntry &arg) const { return !equal(arg, true); }

    const QUuid& uuid() const { return uuid_; }
    qint64 createStamp() const { return createStamp_; }
    qint64 updateStamp() const { return updateStamp_; }
	const QString& item() const	{ return item_; }
	const QString& description() const { return desc_; }
    bool isValid() const { return !uuid_.isNull(); }

	int points(const QuizDirection type) const;
	int level(const QuizDirection type) const { return level(points(type)); }
	int fails(const QuizDirection type) const;

	void setItem(const QString &arg) { item_ = arg; }
	void setDescription(const QString &arg) { desc_ = arg; }
	
	void addPoint(const QuizDirection type);
	void addFail(const QuizDirection type);

    void undoPoint(const QuizDirection type);
    void undoFail(const QuizDirection type, const int points);

    void update(const QDateTime &dbCreated);
    void reset();

	void toXML(QXmlStreamWriter &xml) const;

protected:
	int level(const int &points) const { return static_cast<int>(sqrt(static_cast<float>(points))); }
};

QTextStream& operator<<(QTextStream &os, const DbEntry &arg);

class DbError
{
public:
    enum Type
    {
        ERROR_OK, 	 // everything OK, no error
        ERROR_ARGS,  // bad arguments for operation
        ERROR_LOCK,  // database was locked
        ERROR_DUPLI, // database reported duplicate
        ERROR_PARSE, // database parsed failed
        ERROR_SYNC   // synchronization failed
    };

private:
    Type type_;
    QString msg_;
    int index_;

public:
    DbError(const Type type, const QString &msg = QString(), int index = -1) : type_(type), msg_(msg), index_(index) {}

    Type type() const { return type_; }
    const QString msg() const { return msg_; }
    int index() const { return index_; }

    bool operator==(const Type type) const { return type_ == type; }
    bool operator!=(const Type type) const { return !(*this == type); }

};

QTextStream& operator<<(QTextStream &os, const DbError &arg);

/// A database that keeps a list of DbEntry objects and manages them.
/// TODO: updated_ not updated, how ironic.
class Database :
    public QObject
{
    Q_OBJECT
    friend class DatabaseTest;

public:
	static const QString
        XML_EXT, XML_HEADER, XML_NAME, XML_DBID, XML_INSID;

private:
    QUuid dbUuid_, insUuid_;
	QString name_;
	QDateTime created_, updated_;
    DbClocks clocks_;
	QList<DbEntry> entries_;
	DbEventLog eventLog_;
	bool dirty_, error_, locked_, external_;

public:
    Database(QObject *parent);
    Database(QObject *parent, const QString &name);

    bool equal(const Database &arg, const bool complete) const;

    const QUuid& dbUuid() const { return dbUuid_; }
    const QUuid& instanceUuid() const { return insUuid_; }
	const QString& name() const { return name_;	}
	const QDateTime& dateCreated() const { return created_;	}
	const QDateTime& dateUpdated() const { return updated_; }
    const DbClocks& clocks() const { return clocks_; }

    int entryCount() const { return entries_.size(); }
    const DbEntry& entry(int idx) const { Q_ASSERT(idx >= 0 && idx < entryCount()); return entries_.at(idx); }
    int entryIndex(const QUuid &uuid) const;

    const DbEventLog& eventLog() const { return eventLog_; }

	bool dirty() const { return dirty_; }
	bool error() const { return error_; }
	bool locked() const { return locked_; }
	bool external() const { return external_; }
    int findEntry(const QString &text, int startIndex = 0) const;
	int findDuplicate(const DbEntry &entry, int exceptIdx = -1) const;

    void setLocked(bool arg) { locked_ = arg; }
    void setExternal(bool arg) { external_ = arg; }
    void fork(Database &other);
    DbError sync(Database &other);

    DbError add(const QString &item, const QString &desc, bool dupIgnore = false);
    DbError remove(int idx);
    DbError remove(const QList<int> &idxs);
    DbError alter(int idx, const QString &item, const QString &desc, bool dupIgnore = false);
    DbError point(const int idx, const DbEntry::QuizDirection type);
    DbError fail(const int idx, const DbEntry::QuizDirection type);
	DbError reset();

    void saveXML(const QString &path);
    DbError loadXML(const QString &path);
    bool htmlExport(const QString &path, const QList<int> &idxs);

signals:
    void countUpdate();
    void beginInsert();
    void endInsert();
    void beginRemove(int index);
    void endRemove();

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

#endif // DATABASE_H_
