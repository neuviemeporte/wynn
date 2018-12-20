#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "ixplog_active.h"
#include "entry.h"
#include "quiz.h"
#include "common.h"

extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"

#define DBXML_DEBUG 1

namespace wynn {
namespace db {

using namespace xml;

const QString
    Entry::XML_HEADER     = "entry",
    Entry::XML_UUID       = "id",
    Entry::XML_ITEM       = "item", 
    Entry::XML_DESC       = "desc",
    Entry::XML_DESCPOINTS = "dsPts",
    Entry::XML_DESCFAILS  = "dsFail",
    Entry::XML_ITEMPOINTS = "isPts",
    Entry::XML_ITEMFAILS  = "isFail";

Entry::Entry(QUuid uuid, qint64 createStamp, qint64 updateStamp, const QString &item, const QString desc, int descShownPoints, int descShownFails, int itemShownPoints, int itemShownFails) :
    uuid_(uuid), createStamp_(createStamp), updateStamp_(updateStamp),
    item_(item), desc_(desc),
    descShownPoints_(descShownPoints), descShownFails_(descShownFails),
    itemShownPoints_(itemShownPoints), itemShownFails_(itemShownFails)
{
}

Entry::Entry() : Entry({}, -1, -1, {}, {}, 0, 0, 0, 0)
{
}

Entry::Entry(const QUuid &uuid, const QDateTime &dbCreated, const QString &item, const QString &desc) :
    Entry(uuid, dbCreated.secsTo(QDateTime::currentDateTime()), -1, item, desc, 0, 0, 0, 0)
{
}

Entry::Entry(QXmlStreamReader &xml) : Entry()
{
    Q_ASSERT(xml.isStartElement() && xml.name() == XML_HEADER);
    QLOGCX("Entry element start", DBXML_DEBUG);

    QXmlStreamAttributes attr = xml.attributes();
    const QString
            uuidStr   = attr.value(XML_UUID).toString(),
            creatStr  = attr.value(XML_CREATED).toString(),
            updatStr  = attr.value(XML_UPDATED).toString(),
            itemStr   = attr.value(XML_ITEM).toString(),
            descStr   = attr.value(XML_DESC).toString(),
            dsPtsStr  = attr.value(XML_DESCPOINTS).toString(),
            dsFailStr = attr.value(XML_DESCFAILS).toString(),
            isPtsStr  = attr.value(XML_ITEMPOINTS).toString(),
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
          << ", item: '" << itemStr << "', desc: '" << descStr
          << "', dsPts = " << dsPts << ", dsFail = " << dsFail
          << ", isPts = " << isPts << ", isFail = " << isFail, DBXML_DEBUG);

    if ( !createdOK || !updatedOK || itemStr.isEmpty() || descStr.isEmpty()
            || !descPointsOK || !descFailsOK || !itemPointsOK || !itemFailsOK )
    {
        xml.raiseError("Invalid Entry attribute value");
    }
    else {
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
}

// compare two DbEntry instances, everything checked if "complete" requested, otherwise item and description text will be ignored
bool Entry::equal(const Entry &other, const bool complete) const {
    return (uuid_ == other.uuid_
            && (item_ == other.item_ || !complete)
            && (desc_ == other.desc_ || !complete)
            && descShownPoints_ == other.descShownPoints_
            && itemShownPoints_ == other.itemShownPoints_
            && descShownFails_ == other.descShownFails_
            && itemShownFails_ == other.itemShownFails_);
}

int Entry::points(const QuizDirection type) const {
    switch(type) {
    case DIR_SHOWDESC: return descShownPoints_;
    case DIR_SHOWITEM: return itemShownPoints_;
    default: return -1;
    }
}

int Entry::fails(const QuizDirection type) const {
    switch(type) {
    case DIR_SHOWDESC: return descShownFails_;
    case DIR_SHOWITEM: return itemShownFails_;
    default: return -1;
    }
}

void Entry::addPoint(const QuizDirection type) {
    switch(type) {
    case DIR_SHOWDESC: if (descShownPoints_ < MAX_POINTS) descShownPoints_++; break;
    case DIR_SHOWITEM: if (itemShownPoints_ < MAX_POINTS) itemShownPoints_++; break;
    default: break;
    }
}

void Entry::addFail(const QuizDirection type) {
    QLOGX("Adding fail for type: " << type);
    switch(type) {
    case DIR_SHOWDESC:
        descShownPoints_ = 0;
        descShownFails_++;
        break;
    case DIR_SHOWITEM:
        itemShownPoints_ = 0;
        itemShownFails_++;
        break;
    default: break;
    }
}

void Entry::undoPoint(const QuizDirection type)
{
    QLOGX("Undoing point for type: " << type);
    switch(type) {
    case DIR_SHOWDESC:
        if (descShownPoints_ > 0) descShownPoints_--;
        break;
    case DIR_SHOWITEM:
        if (itemShownPoints_ > 0) itemShownPoints_--;
        break;
    default: break;
    }
}

void Entry::undoFail(const QuizDirection type, const int points) {
    QLOGX("Undoing fail for type: " << type << ", points: " << points);
    switch(type) {
    case DIR_SHOWDESC:
        descShownPoints_ = points;
        descShownFails_--;
        break;
    case DIR_SHOWITEM:
        itemShownPoints_ = points;
        itemShownFails_--;
        break;
    default: break;
    }
}

void Entry::update(const QDateTime &dbCreated) { 
    updateStamp_ = dbCreated.secsTo(QDateTime::currentDateTime());
}

void Entry::reset() { 
    descShownPoints_ = itemShownPoints_ = descShownFails_ = itemShownFails_ = 0; 
}

void Entry::toXML(QXmlStreamWriter &xml) const {
    xml.writeStartElement(XML_HEADER);
    if ( !uuid_.isNull() )
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

} // namespace db
} // namespace wynn

QTextStream& operator<<(QTextStream &os, const wynn::db::Entry &arg)
{
    using namespace wynn::db;
    return (os << "'" << arg.item() << "'/'" << arg.description() << "'"
            << ( arg.uuid().isNull() ? "" : " " + arg.uuid().toString() )
            << " (points: " << arg.points(DIR_SHOWDESC) << "/L" << arg.level(DIR_SHOWDESC) 
            << ", "         << arg.points(DIR_SHOWITEM) << "/L" << arg.level(DIR_SHOWITEM)
            << ") (fails: " <<  arg.fails(DIR_SHOWDESC) << "/"  << arg.fails(DIR_SHOWITEM) 
            << "), creat: " << arg.createStamp() << ", upd: " << arg.updateStamp());
}
