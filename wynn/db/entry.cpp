#include <QDateTime>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "ixplog_active.h"
#include "entry.h"
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

Entry::Entry() :
    createStamp_(-1), updateStamp_(-1),
    descShownPoints_(0), descShownFails_(0), 
    itemShownPoints_(0), itemShownFails_(0)
{
}

Entry::Entry(const QUuid &uuid, const QDateTime &dbCreated, const QString &item, const QString &desc) :
    uuid_(uuid),
    createStamp_(dbCreated.secsTo(QDateTime::currentDateTime())),
    updateStamp_(-1),
    item_(item), desc_(desc), 
    descShownPoints_(0), descShownFails_(0), 
    itemShownPoints_(0), itemShownFails_(0)
{
}

// compare two DbEntry instances, everything checked if "complete" requested, otherwise item and description text will be ignored
bool Entry::equal(const Entry &other, const bool complete) const
{
    return (uuid_ == other.uuid_
            && (item_ == other.item_ || !complete)
            && (desc_ == other.desc_ || !complete)
            && descShownPoints_ == other.descShownPoints_
            && itemShownPoints_ == other.itemShownPoints_
            && descShownFails_ == other.descShownFails_
            && itemShownFails_ == other.itemShownFails_);
}

int Entry::points(const QuizDirection type) const
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

int Entry::fails(const QuizDirection type) const
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

void Entry::addPoint(const QuizDirection type)
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

void Entry::addFail(const QuizDirection type)
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

void Entry::undoPoint(const QuizDirection type)
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

void Entry::undoFail(const QuizDirection type, const int points)
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

void Entry::update(const QDateTime &dbCreated) { 
    updateStamp_ = dbCreated.secsTo(QDateTime::currentDateTime());
}

void Entry::reset() { 
    descShownPoints_ = itemShownPoints_ = descShownFails_ = itemShownFails_ = 0; 
}

void Entry::toXML(QXmlStreamWriter &xml) const {
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

Entry::Entry(QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == XML_HEADER);
    QLOGCX("Entry element start", DBXML_DEBUG);
    QLOGINC;
    
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

} // namespace db
} // namespace wynn


QTextStream& operator<<(QTextStream &os, const wynn::db::Entry &arg)
{
    using namespace wynn::db;
    return (os << "'" << arg.item() << "'/'" << arg.description() << "' " << arg.uuid().toString()
            << " (points: " << arg.points(DIR_SHOWDESC) << "/L" << arg.level(DIR_SHOWDESC) 
            << ", "         << arg.points(DIR_SHOWITEM) << "/L" << arg.level(DIR_SHOWITEM)
            << ") (fails: " <<  arg.fails(DIR_SHOWDESC) << "/"  << arg.fails(DIR_SHOWITEM) 
            << "), creat: " << arg.createStamp() << ", upd: " << arg.updateStamp());
}
