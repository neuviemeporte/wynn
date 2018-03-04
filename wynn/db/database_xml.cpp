#include "database.h"
#include "ixplog_active.h"

#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"

#define DBXML_DEBUG 1

static const QString
	XML_DATEFORMAT = "dd.MM.yyyy.hh.mm.ss",
    XML_UUID       = "id",
    XML_CREATED    = "init",
    XML_UPDATED    = "update";

QString xml_readText(QXmlStreamReader &xml, const QString &element)
{
	QString text = xml.readElementText();
	QLOGC("Text element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	if (text.isEmpty()) xml.raiseError(QObject::tr("Empty text element '") + element + "'");
	return text;
}

QDateTime xml_readDate(QXmlStreamReader &xml, const QString &element, bool permitEmpty)
{
	QString text = xml.readElementText();
	QLOGC("Date element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	QDateTime ret = QDateTime::fromString(text, XML_DATEFORMAT);

	if (text.isEmpty())
	{
		if (!permitEmpty) xml.raiseError(QObject::tr("Empty date element '") + element + "'");
	}
	else if (!ret.isValid()) xml.raiseError(QObject::tr("Invalid date format for element '") + element + "'");
	
	return ret;
}

int xml_readInteger(QXmlStreamReader &xml, const QString &element)
{
	QString text = xml.readElementText();
	QLOGC("Integer element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	bool ok = true;
	int ret = text.toInt(&ok);
	if (!ok) xml.raiseError(QObject::tr("Invalid integer value for element '") + element + "'");
	return ret;
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

// ==============================================================================================
// ============================== DbClocks
// ==============================================================================================

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

// ==============================================================================================
// ============================== DbEntry
// ==============================================================================================

const QString
	DbEntry::XML_HEADER     = "entry",
	DbEntry::XML_ITEM       = "item", 
    DbEntry::XML_DESC       = "desc",
    DbEntry::XML_DESCPOINTS = "dsPts",
    DbEntry::XML_DESCFAILS  = "dsFail",
    DbEntry::XML_ITEMPOINTS = "isPts",
    DbEntry::XML_ITEMFAILS  = "isFail";

void DbEntry::toXML(QXmlStreamWriter &xml) const
{
	xml.writeStartElement(XML_HEADER);
	xml.writeAttribute(XML_UUID, uuid_.toString());
    xml.writeAttribute(XML_CREATED, QString::number(createStamp_));
    xml.writeAttribute(XML_UPDATED, QString::number(updateStamp_));
    xml.writeAttribute(XML_ITEM, item_);
    xml.writeAttribute(XML_DESC, desc_);
    xml.writeAttribute(XML_DESCPOINTS, QString::number(descShownPoints_));
    xml.writeAttribute(XML_DESCFAILS,  QString::number(descShownFails_));
    xml.writeAttribute(XML_ITEMPOINTS, QString::number(itemShownPoints_));
    xml.writeAttribute(XML_ITEMFAILS,  QString::number(itemShownFails_));

	xml.writeEndElement(); // XML_HEADER
}

DbEntry::DbEntry(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == XML_HEADER);
    QLOGCX("Entry element start", DBXML_DEBUG);
	QLOGINC;

	QXmlStreamAttributes attr = xml.attributes();
    const QString
            uuidStr  = attr.value(XML_UUID).toString(),
            creatStr = attr.value(XML_CREATED).toString(),
            updatStr = attr.value(XML_UPDATED).toString(),
            itemStr  = attr.value(XML_ITEM).toString(),
            descStr  = attr.value(XML_DESC).toString(),
            dsPtsStr = attr.value(XML_DESCPOINTS).toString(),
            dsFailStr = attr.value(XML_DESCFAILS).toString(),
            isPtsStr = attr.value(XML_ITEMPOINTS).toString(),
            isFailStr = attr.value(XML_ITEMFAILS).toString();

    bool
        createdOK = false, updatedOK = false,
        descPointsOK = false, descFailsOK = false,
        itemPointsOK = false, itemFailsOK = false;
    const QUuid uuid(uuidStr);
    const qlonglong
            created = creatStr.toLongLong(&createdOK),
            updated = updatStr.toLongLong(&updatedOK);
    const int
            dsPts  = dsPtsStr.toInt(&descPointsOK),
            dsFail = dsFailStr.toInt(&descFailsOK),
            isPts  = isPtsStr.toInt(&itemPointsOK),
            isFail = isFailStr.toInt(&itemFailsOK);

    QLOGC("uuid: '" << uuid.toString() << ", created = " << created << ", updated = " << updated
          << "item: '" << itemStr << "', desc: '" << descStr
          << "', dsPts = " << dsPts << ", dsFail = " << dsFail
          << ", isPts = " << isPts << ", isFail = " << isFail, DBXML_DEBUG);

    if (uuid.isNull()
            || !createdOK || !updatedOK
            || itemStr.isEmpty() || descStr.isEmpty()
            || !descPointsOK || !descFailsOK
            || !itemPointsOK || !itemFailsOK)
    {
        xml.raiseError("Invalid attributes");
    }
    else
    {
        uuid_ = uuid;
        createStamp_ = created;
        updateStamp_ = updated;
        item_ = itemStr;
        desc_ = descStr;
        descShownPoints_ = dsPts;
        descShownFails_ = dsFail;
        itemShownPoints_ = isPts;
        itemShownFails_ = isFail;
	}

	QLOGDEC;
}

// ==============================================================================================
// ============================== Database
// ==============================================================================================

const QString
    Database::XML_EXT    = ".xml",
    Database::XML_HEADER = "wynndb",
    Database::XML_NAME   = "name",
    Database::XML_DBID   = "id",
    Database::XML_INSID  = "instance";

void Database::saveXML(const QString &path)
{
	QFile file(path);
	QLOGX("Saving database '" << name_ << "' to '" << file.fileName() << "'");
	QLOGINC;
	if (!file.open(QIODevice::WriteOnly))
	{
		QLOG("Unable to open file for writing! (Error: " << file.error() << ")");
		QLOGDEC;
		return;
	}

	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);

	xml.writeStartDocument();
	xml.writeStartElement(XML_HEADER);
    xml.writeAttribute(XML_DBID, dbUuid_.toString());
    xml.writeAttribute(XML_INSID, insUuid_.toString());
	xml.writeAttribute(XML_NAME, name_);
    xml.writeAttribute(XML_CREATED, created_.toString(XML_DATEFORMAT));
    xml.writeAttribute(XML_UPDATED, updated_.toString(XML_DATEFORMAT));

    clocks_.toXML(xml);

	for (int i = 0; i < entries_.size(); ++i)
	{
		const DbEntry &item = entries_.at(i);
		item.toXML(xml);
	}

	eventLog_.saveXML(xml);

	xml.writeEndElement(); // XML_HEADER
	xml.writeEndDocument();
	file.close();
	dirty_ = false;
	QLOGDEC;
}

DbError Database::loadXML(const QString &path)
{
	QFile file(path);
	QLOGX("Parsing file: " << file.fileName());
	QLOGINC;
	if (!file.open(QIODevice::ReadOnly)) 
	{
		QLOG("Unable to open file for reading! (Error: " << file.error() << ")");
		QLOGDEC;
		error_ = true;
        return DbError(DbError::ERROR_PARSE, QObject::tr("Unable to open file for reading:\n") + path
            + QObject::tr("\nError code: ") + QString::number(file.error()), -1);
	}

	QXmlStreamReader xml(&file);

	while (!xml.atEnd() && !xml.hasError())
	{
		// read next entity and extract its name
		xml.readNext();
		const QString xmlName = xml.name().toString();

		if (xml.isStartElement())
		{
			if (xmlName == XML_HEADER)
			{
				QXmlStreamAttributes attr = xml.attributes();
				const QString
                    dbUuidStr = attr.value(XML_DBID).toString(),
                    insUuidStr = attr.value(XML_INSID).toString(),
                    nameStr = attr.value(XML_NAME).toString(),
                    creatStr = attr.value(XML_CREATED).toString(),
                    updStr = attr.value(XML_UPDATED).toString();

                QLOGC("Database start, dbUuid: '" << dbUuidStr << "', insUuid: '" << insUuidStr << "', name: '" << nameStr
                      << "', created: '" << creatStr << "', updated: '" << updStr << "'", DBXML_DEBUG);

                const QUuid
                        dbUuid(dbUuidStr),
                        insUuid(insUuidStr);
                const QDateTime
                        creatDate(QDateTime::fromString(creatStr, XML_DATEFORMAT)),
                        updDate(QDateTime::fromString(updStr, XML_DATEFORMAT));

                if (dbUuid.isNull() || insUuid.isNull() || nameStr.isEmpty() || !creatDate.isValid() || !updDate.isValid())
                {
                    xml.raiseError("Invalid attributes");
                }
				else
				{
                    dbUuid_ = dbUuid;
                    insUuid_ = insUuid;
					name_ = nameStr;
                    created_ = creatDate;
                    updated_ = updDate;
                    clocks_.setMaster(insUuid_);
				}
			}
            else if (xmlName == DbClocks::XML_HEADER)
            {
                clocks_.loadXML(xml);
            }
			else if (xmlName == DbEntry::XML_HEADER)
			{
                DbEntry e(xml);
                if (!xml.hasError())
                {
                    QLOGC("Appending entry", DBXML_DEBUG);
                    entries_.append(e);
                }
			}
			else if (xmlName == DbEventLog::XML_HEADER)
			{
				eventLog_.loadXML(xml);
			}
			else 
			{
				xml.raiseError(QObject::tr("Unrecognized element '") + xmlName + "'");
			}
		} // is element start
	} // keep loading xml elements

	if (xml.hasError())
	{
		QString error = xml.errorString();
		error_ = true;
		QString errstr = QObject::tr("Error while parsing XML file\n") + path + QObject::tr("\nXML Error: ") + error 
			+ tr("\nLine: ") + QString::number(xml.lineNumber()) + tr("\nColumn: ") + QString::number(xml.columnNumber());
		QLOG("XML PARSE ERROR: " << error << ", line: " << xml.lineNumber());
		QLOGDEC;
        return DbError(DbError::ERROR_PARSE, errstr, -1);
	}
    else if (!clocks_.hasMaster())
    {
        error_ = true;
        const QString error("Database master clock not found in XML");
        QLOG("Error: " << error);
        QLOGDEC;
        return DbError(DbError::ERROR_PARSE, error);
    }

	dirty_ = false;
	QLOGDEC;
    return DbError(DbError::ERROR_OK);
}

bool Database::htmlExport(const QString &path, const QList<int> &idxs)
{
    QLOGX("Exporting database as html to '" << path);
    QLOGINC;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QLOG("Unable to open file for writing! (Error: " << file.error() << ")");
        QLOGDEC;
        return false;
    }

    QList<int> dbaseIdxs;
    // there was nothing selected in the table, or just one active row, use all idxs
    if (idxs.count() <= 1)
    {
        for (int i = 0; i < entries_.size(); ++i) dbaseIdxs.append(i);
    }
    else dbaseIdxs = idxs;
    Q_ASSERT(dbaseIdxs.count() > 1);

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << "<html>\n<head>\n\t<title>Wynn database (" << name() << ")</title>\n"
        << "\t<meta http-equiv=\"content-type\" content=\"text/html;charset=UTF-8\" />\n"
        << "\t<style type=\"text/css\">\n"
        << "\t\ttable { border: 1px solid black; border-collapse: collapse; }\n"
        << "\t\ttd { border: 1px solid black; padding-left: 5px; padding-right: 5px; }\n"
        << "\t\tth { border: 1px solid black; padding: 5px; }\n\t</style>\n"
        << "</head>\n<body>\n"
        << "\t<table border=\"1\">\n\t\t<thead>\n\t\t\t<tr>\n"
        << "\t\t\t\t<th>Index</th>\n"
        << "\t\t\t\t<th>Item</th>\n"
        << "\t\t\t\t<th>Description</th>\n"
        << "\t\t\t\t<th>Created</th>\n"
        << "\t\t\t</tr>\n\t\t</thead>\n\t\t<tbody>\n";

    for (int i = 0; i < dbaseIdxs.size(); ++i)
    {
        const int idx = dbaseIdxs.at(i);
        const DbEntry &entry= entries_.at(idx);
        const QString date = created_.addSecs(entry.createStamp()).toString("dd.MM.yyyy.hh.mm");

        stream << "\t\t\t<tr>\n" << "\t\t\t\t<td>"
            << QString::number(idx + 1) << "</td>\n\t\t\t\t<td>"
            << entry.item() << "</td>\n\t\t\t\t<td>"
            << entry.description() << "</td>\n\t\t\t\t<td>"
            << date << "</td>\n\t\t\t</tr>\n";
    }

    stream << "\t\t</tbody>\n\t</table>\n</body>\n</html>";
    file.close();
    QLOGDEC;
    return true;
}
