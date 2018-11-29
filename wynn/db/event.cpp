#include "event.h"

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

// ==============================================================================================
// ============================== DbEvent
// ==============================================================================================

const QString
	DbEvent::XML_HEADER    = "event", 
	DbEvent::XML_TYPE      = "type", 
    DbEvent::XML_SOURCE    = "source",
    DbEvent::XML_TIMESTAMP = "time",
    DbEvent::XML_CLOCK     = "clock",
	DbEvent::XML_ARG1      = "arg1", 
	DbEvent::XML_ARG2      = "arg2", 
	DbEvent::XML_ARG3      = "arg3";

DbEvent::DbEvent(QXmlStreamReader &xml) :
    type_(EVENT_UNDEF), timestamp_(0)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == XML_HEADER);
    QLOGCX("Event element start", DBXML_DEBUG);
    QLOGINC;

    QXmlStreamAttributes attrs = xml.attributes();

    const QString
            typeStr  = attrs.value(XML_TYPE).toString(),
            srcStr   = attrs.value(XML_SOURCE).toString(),
            timeStr  = attrs.value(XML_TIMESTAMP).toString(),
            clockStr = attrs.value(XML_CLOCK).toString(),
            arg1Str  = attrs.value(XML_ARG1).toString(),
            arg2Str  = attrs.value(XML_ARG2).toString(),
            arg3Str  = attrs.value(XML_ARG3).toString();

    bool typeOK = false;
    const Type type = static_cast<Type>(typeStr.toInt(&typeOK));
    if (!typeOK)
    {
        xml.raiseError("Invalid event type");
        QLOGDEC;
        return;
    }

    const QUuid sourceID(srcStr);
    if (sourceID.isNull())
    {
        xml.raiseError("Invalid source id");
        QLOGDEC;
        return;
    }

    bool timeOK = false;
    qlonglong timestamp = timeStr.toLongLong(&timeOK);
    if (!timeOK)
    {
        xml.raiseError("Invalid event timestamp");
        QLOGDEC;
        return;
    }

    bool clockOK = false;
    qlonglong clock = clockStr.toLongLong(&clockOK);
    if (!clockOK)
    {
        xml.raiseError("Invalid clock value");
        QLOGDEC;
        return;
    }

    bool argsOK = true;
    switch(type)
    {
    case EVENT_EADD:
    case EVENT_EALTER:
    case EVENT_EFAIL:
        if (arg1Str.isEmpty() || arg2Str.isEmpty() || arg3Str.isEmpty()) argsOK = false;
        break;
    case EVENT_EPOINT:
        if (arg1Str.isEmpty() || arg2Str.isEmpty()) argsOK = false;
        break;
    case EVENT_EDEL:
        if (arg1Str.isEmpty()) argsOK = false;
        break;
    default:
        xml.raiseError("Bad event type");
        QLOGDEC;
        return;
    }

    if (!argsOK)
    {
        xml.raiseError("Empty arguments found");
        QLOGDEC;
        return;
    }

    QLOG("Initializing with type = " << type << ", source: " << sourceID.toString() << ", time = " << timestamp << ", clock = " << clock);
    type_ = type;
    sourceID_ = sourceID;
    timestamp_ = timestamp;
    clock_ = clock;
    arg1_ = QVariant(arg1Str);
    arg2_ = QVariant(arg2Str);
    arg3_ = QVariant(arg3Str);
    QLOGDEC;
}

void DbEvent::toXML(QXmlStreamWriter &xml) const
{
	xml.writeStartElement(XML_HEADER);

	xml.writeAttribute(XML_TYPE, QString::number(type_));
    xml.writeAttribute(XML_SOURCE, sourceID_.toString());
    xml.writeAttribute(XML_TIMESTAMP, QString::number(timestamp_));
    xml.writeAttribute(XML_CLOCK, QString::number(clock_));

    if (!arg1_.isNull()) xml.writeAttribute(XML_ARG1, arg1_.toString());
    if (!arg2_.isNull()) xml.writeAttribute(XML_ARG2, arg2_.toString());
    if (!arg3_.isNull()) xml.writeAttribute(XML_ARG3, arg3_.toString());

	xml.writeEndElement(); // XML_HEADER
}

QTextStream& operator<<(QTextStream &os, const DbEvent &arg)
{
    return (os << "type: " << arg.type() << ", source: " << arg.sourceID().toString() << ", timestamp = " << arg.timestamp() << ", clock = " << arg.clock());
}

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
// ============================== DbEventLog
// ==============================================================================================

const QString
	DbEventLog::XML_HEADER = "eventlog";

void DbEventLog::saveXML(QXmlStreamWriter &xml) const
{
	xml.writeStartElement(XML_HEADER);

    for (int i = 0; i < eventCount(); ++i)
    {
        const DbEvent &e = event(i);
        e.toXML(xml);
    }

	xml.writeEndElement(); // XML_HEADER
}

void DbEventLog::loadXML(QXmlStreamReader &xml)
{
	const QString headerStr = xml.name().toString();
	if (headerStr != XML_HEADER) return;

	QLOGCX("Loading from XML stream", DBXML_DEBUG);
	QLOGINC;

	bool done = false;
    while (!xml.atEnd() && !xml.hasError() && !done)
	{
		xml.readNext();
		const QString xmlName = xml.name().toString();

		if (xml.isStartElement())
		{
            if (xmlName == DbEvent::XML_HEADER)
			{
                DbEvent e(xml);
                if (xml.hasError())
                {
                    QLOG("Error: " << xml.errorString());
                }
                else addEvent(e);
			}
			else
			{
				QLOGC("Unknown element: '" << xmlName << "'", DBXML_DEBUG);
				xml.raiseError("Unknown element: '" + xmlName + "'");
			}
		}
		else if (xml.isEndElement() && xmlName == XML_HEADER)
		{
			QLOGC("Event log end", DBXML_DEBUG);
			done = true;
		}
	}

	QLOGDEC;
}
