#ifndef CLOCKS_H
#define CLOCKS_H

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

#endif // CLOCKS_H
