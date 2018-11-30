#include "clocks.h"

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

// ==============================================================================================
// ============================== DbClock
// ==============================================================================================

const QString
    DbClock::XML_HEADER   = "clock",
    DbClock::XML_INSTANCE = "instance",
    DbClock::XML_TICKS    = "ticks";

DbClock::DbClock(QXmlStreamReader &xml)
{
    QLOGCX("Attempting to load clock from XML", DBXML_DEBUG);
    QLOGINC;

    if (xml.hasError()) return;
    if (!xml.isStartElement() || xml.name() != XML_HEADER)
    {
        xml.raiseError("Not at start of clock in XML");
        QLOGDEC;
        return;
    }

    QXmlStreamAttributes attrs = xml.attributes();
    const QUuid instance(attrs.value(XML_INSTANCE).toString());

    bool ticksOK = false;
    const qint64 ticks = attrs.value(XML_TICKS).toString().toLongLong(&ticksOK);

    if (instance.isNull() || !ticksOK)
    {
        xml.raiseError("Invalid clock attributes");
        QLOGDEC;
        return;
    }

    instanceID_ = instance;
    ticks_ = ticks;

    QLOGDEC;
}

void DbClock::toXML(QXmlStreamWriter &xml) const
{
    if (xml.hasError()) return;
    xml.writeStartElement(XML_HEADER);
    xml.writeAttribute(XML_INSTANCE, instanceID_.toString());
    xml.writeAttribute(XML_TICKS, QString::number(ticks_));
    xml.writeEndElement(); // XML_HEADER
}

QTextStream& operator<<(QTextStream &os, const DbClock &arg)
{
    return (os << arg.toString());
}

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

const QString DbClocks::XML_HEADER = "clocks";

void DbClocks::toXML(QXmlStreamWriter &xml) const
{
    if (xml.hasError()) return;

    QLOGCX("Saving clocks structure to XML", DBXML_DEBUG);
    QLOGINC;

    xml.writeStartElement(XML_HEADER);

    QList<QUuid> uuids = clocks_.keys();
    QLOG("Need to save " << uuids.size() << " clock items");

    for (int i = 0; i < uuids.size(); ++i)
    {
        const QUuid &id = uuids.at(i);
        const DbClock &c = clocks_.value(id);
        if (!c.isNull()) c.toXML(xml);
    }

    xml.writeEndElement(); // XML_HEADER
    QLOGDEC;
}

void DbClocks::loadXML(QXmlStreamReader &xml)
{
    QLOGCX("Attempting to load clocks from XML", DBXML_DEBUG);
    QLOGINC;

    if (xml.hasError()) return;
    if (!xml.isStartElement() || xml.name() != XML_HEADER)
    {
        xml.raiseError("Not at start of clocks in XML");
        QLOGDEC;
        return;
    }

    bool done = false;
    while (!xml.atEnd() && !xml.hasError() && !done)
    {
        xml.readNext();
        const QString name = xml.name().toString();

        if (xml.isStartElement())
        {
            if (name == DbClock::XML_HEADER)
            {
                DbClock c(xml);
                if (!xml.hasError())
                {
                    QLOGC("Adding clock: " << c, DBXML_DEBUG);
                    clocks_.insert(c.instanceID(), c);
                }
            }
            else
            {
                xml.raiseError("Unknown element: '" + name + "'");
            }
        }
        else if (xml.isEndElement() && name == XML_HEADER)
        {
            QLOGC("End of clocks element", DBXML_DEBUG);
            done = true;
        }
    }

    QLOGDEC;
}