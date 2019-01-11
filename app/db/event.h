#ifndef EVENT_H
#define EVENT_H

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

#endif // EVENT_H
