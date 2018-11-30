#include "database.h"
#include "common.h"
#include "ixplog_active.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"
#define DBXML_DEBUG 1

namespace wynn {
namespace db {

const QString 
    Database::XML_EXT    = ".xml",
    Database::XML_HEADER = "wynndb",
    Database::XML_NAME   = "name";

Database::Database(QObject *parent) : QObject(parent),
    dirty_(false), error_(false), locked_(false), external_(false)
{
    QLOGX("Creating empty");
}

Database::Database(QObject *parent, const QString &name) : QObject(parent),
    name_(name),
    created_(QDateTime::currentDateTime()), updated_(QDateTime::currentDateTime()),
    dirty_(true), error_(false), locked_(false), external_(false)
{
    QLOGX("Creating new, name: '" << name_);
    
    // TODO: check if this not being connected from the other constructor is a problem
    if (parent) {
        connect(this, SIGNAL(countUpdate()), parent, SLOT(slot_database_updateCount()));
    }
}

int Database::findEntry(const QString &text, int startIndex) const {
    if (startIndex < 0 || startIndex >= entryCount()) {
        QLOGX("Start index " << startIndex << "ouf ot range!");
        return -1;
    }
    
    QLOGX("Database '" << name() << " searching for match with '" << text << "' starting from idx " << startIndex);
    for (int i = startIndex; i < entryCount(); ++i) {
        const Entry &e = entry(i);
        const QString 
                &item = e.item(),
                &desc = e.description();
        
        if (item.contains(text, Qt::CaseInsensitive) || desc.contains(text, Qt::CaseInsensitive)) {
            QLOG("Match on entry " << i << ": " << e);
            return i;
        }
    }
    return -1;
}

int Database::findDuplicate(const Entry &entry, int exceptIdx) const {
    for (int i = 0; i < entries_.count(); ++i) {
        if ( i == exceptIdx ) 
            continue; // skip over 'this' item
        
        const Entry &curitem = entries_.at(i);
        if (curitem.item().contains(entry.item(), Qt::CaseInsensitive) ||
            curitem.description().contains(entry.description(), Qt::CaseInsensitive) ||
            entry.item().contains(curitem.item(), Qt::CaseInsensitive) ||
            entry.description().contains(curitem.description(), Qt::CaseInsensitive))
        {
            QLOGX("Duplicate hit at idx " << i << ", item '" << curitem.item() << "', desc '" << curitem.description()
                  << "' while searching for duplicates of item '"
                  << entry.item() << "', desc '" << entry.description() << "'");
            return i;
        }
    }
    return -1;
}

Error Database::add(const QString &item, const QString &desc, const QUuid &uuid, const bool dupIgnore) {
    QLOGX("Attempting to add entry, item: '" << item << "', desc: '" << desc << "'");
    if (item.isEmpty() || desc.isEmpty()) {
        QLOG("Bad arguments for operation");
        return Error::ARGS;
    }
    
    const Entry addme{uuid, created_, item, desc};
    // check for pre-existing duplicates, won't add unless duplication ignore mode specified
    int dupidx = 0;
    if ( !dupIgnore && ( dupidx = findDuplicate( addme ) ) >= 0 ) {
        QLOG("Found duplicate, not applying");
        return { Error::DUPLI, "duplicate found on add", dupidx };
    }
    
    // add new entry to database, mark database as altered, notify subscribers of change
    QLOG("Adding entry: " << addme);
    emit beginInsert();
    entries_.append(addme);
    emit endInsert();
    emit countUpdate();
    return Error::OK; 
}

Error Database::remove(int idx) {
    QLOGX("Attempting to remove entry, idx = " << idx);
    if (idx < 0 || idx >= entryCount()) {
        QLOG("Index out of range!");
        return { Error::ARGS, "Index out of range", idx };
    }
    
    // perform the remove
    emit beginRemove(idx);
    entries_.removeAt(idx);
    emit endRemove();
    emit countUpdate();
    
    return Error::OK;
}

Error Database::remove(const QList<int> &idxs)
{
    QLOGX("Removing " << idxs.count() << " items");
    // TODO: ensure index list is sorted, otherwise strange things will happen. Start with a test of course.
    for (int i = idxs.size() - 1; i >= 0; --i) {
        int idx = idxs.at(i);
        const Error e = remove(idx);
        if (e != Error::OK) {
            QLOG("Error at item " << i << " (" << idx << "): " << e);
            return e;
        }
    }
    
    return Error::OK;
}

Error Database::alter(int idx, const QString &item, const QString &desc, bool dupIgnore) {
    QLOGX("Attempting to alter entry, idx = " << idx << ", item: '" << item << "', desc: '" << desc << "'");
    
    // obtain original entry, copy to updated entry and set the new item and description text on that one
    const Entry &original = entry(idx);
    QLOG("Original: " << original << " to '" << item << "'/'" << desc << "'");
    Entry updated(original);
    updated.setItem(item);
    updated.setDescription(desc);
    QLOG("Updated: " << updated);
    
    // if needed, check for duplicates of the updated entry
    int dupidx = 0;
    if ( !dupIgnore && ( dupidx = findDuplicate(updated, idx) ) >= 0 ) {
        QLOG("Found duplicate, abandonding alter");
        return { Error::DUPLI, "duplicate found for updated item on alter", dupidx };
    }
    
    // replace the original entry by the updated one
    QLOG("Applying alter");
    entries_.replace(idx, updated);
    
    return Error::OK;
}

Error Database::point(const int idx, const QuizDirection type) {
    QLOGX("Attempting to add a point for index " << idx << ", type: " << type);
    if (type != DIR_SHOWDESC && type != DIR_SHOWITEM) {
        QLOG("Invalid args for operation");
        return { Error::ARGS, "bad args for point add" };
    }
    
    if (idx < 0 || idx >= entryCount()) {
        QLOG("Index out of range!");
        return { Error::ARGS, "Index out of range", idx };
    }
    
    Entry &e = entries_[idx];
    QLOG("Adding point for entry: " << e);
    e.addPoint(type);
    e.update(created_);
    return Error::OK;
}

Error Database::fail(const int idx, const QuizDirection type) {
    QLOGX("Attempting to add a fail for idx " << idx << ", type: " << type);
    // check if args are correct
    if (type != DIR_SHOWDESC && type != DIR_SHOWITEM) {
        QLOG("Invalid args for operation");
        return { Error::ARGS, "bad args for fail add" };
    }
    
    if (idx < 0 || idx >= entryCount()) {
        QLOG("Index out of range!");
        return { Error::ARGS, "Index out of range", idx };
    }
    
    Entry &e = entries_[idx];
    QLOG("Adding fail for entry: " << e);
    e.addFail(type);
    e.update(created_);
    return Error::OK;
}

Error Database::reset() {
    if (locked_) { 
        QLOGX("Database locked"); 
        return Error::LOCK;
    }
    
    for (auto &entry : entries_) {
        entry.reset();
        entry.update(created_);
    }
    
    dirty_ = true;
    return Error::OK;
}

void Database::saveXML(const QString &path) const {
    QLOGX("Saving database '" << name_ << "' to '" << path << "'");
    
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        QLOG("Unable to open file for writing! (Error: " << file.error() << ")");
        return;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    
    xml.writeStartDocument();
    xml.writeStartElement(XML_HEADER);
    xml.writeAttribute(XML_NAME, name_);
    xml.writeAttribute(xml::XML_CREATED, created_.toString(xml::XML_DATEFORMAT));
    xml.writeAttribute(xml::XML_UPDATED, updated_.toString(xml::XML_DATEFORMAT));
    saveXMLExtraAttributes(xml);
    
    for (const auto &item : entries_)
        item.toXML(xml);
    
    saveXMLExtraElements(xml);
    xml.writeEndElement(); // XML_HEADER
    xml.writeEndDocument();
    file.close();
    dirty_ = false;
}

Error Database::loadXML(const QString &path) {
    QFile file(path);
    QLOGX("Parsing file: " << file.fileName());
    if (!file.open(QIODevice::ReadOnly)) {
        QLOG("Unable to open file for reading! (Error: " << file.error() << ")");
        error_ = true;
        return { Error::PARSE, tr("Unable to open file for reading:\n") + path + tr("\nError code: ") + QString::number(file.error()) };
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd() && !xml.hasError())
    {
        xml.readNext();
        
        if ( !xml.isStartElement() )
            continue;
        
        const QString xmlName = xml.name().toString();
        if (xmlName == XML_HEADER)
        {
            QXmlStreamAttributes attr = xml.attributes();
            const QString
                    nameStr = attr.value(XML_NAME).toString(),
                    creatStr = attr.value(xml::XML_CREATED).toString(),
                    updStr = attr.value(xml::XML_UPDATED).toString();
            
            QLOGC("Database start, name: '" << nameStr << "', created: '" << creatStr << "', updated: '" << updStr << "'", DBXML_DEBUG);
            
            const QDateTime
                    creatDate(QDateTime::fromString(creatStr, xml::XML_DATEFORMAT)),
                    updDate(QDateTime::fromString(updStr, xml::XML_DATEFORMAT));
            
            loadXMLExtraAttributes(xml);
            
            if (nameStr.isEmpty() || !creatDate.isValid() || !updDate.isValid()) {
                xml.raiseError("Invalid attributes");
            }
            else {
                name_ = nameStr;
                created_ = creatDate;
                updated_ = updDate;
            }
        }
        else if (xmlName == Entry::XML_HEADER)
        {
            Entry e(xml);
            if (!xml.hasError())  {
                QLOGC("Appending entry", DBXML_DEBUG);
                // TODO: emplace?
                entries_.append(e);
            }
        }
        else loadXMLExtraElements(xml, xmlName);
    } // iterate over xml elements
    
    if (xml.hasError())
    {
        QString error = xml.errorString();
        error_ = true;
        QString errstr = tr("Error while parsing XML file\n") + path + tr("\nXML Error: ") + error 
                + tr("\nLine: ") + QString::number(xml.lineNumber()) + tr("\nColumn: ") + QString::number(xml.columnNumber());
        QLOG("XML parse error: " << error << ", line: " << xml.lineNumber());
        return { Error::PARSE, errstr };
    }
    
    auto error = loadXmlPostCheck();
    if ( error != Error::OK )
        return error;
    
    dirty_ = false;
    return Error::OK;
}

void Database::loadXMLExtraElements(QXmlStreamReader &xml, const QString &name)
{
    // The basic Database class does not know any elements outside of those parsed directly in loadXML()
    xml.raiseError(tr("Unrecognized element '") + name + "'");
}

bool Database::htmlExport(const QString &path, const QList<int> &idxs) {
    QLOGX("Exporting database as html to '" << path);
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QLOG("Unable to open file for writing! (Error: " << file.error() << ")");
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
    // TODO: move contents to template file, don't hardcode in source
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
        const Entry &entry= entries_.at(idx);
        const QString date = created_.addSecs(entry.createStamp()).toString("dd.MM.yyyy.hh.mm");
        
        stream << "\t\t\t<tr>\n" << "\t\t\t\t<td>"
               << QString::number(idx + 1) << "</td>\n\t\t\t\t<td>"
               << entry.item() << "</td>\n\t\t\t\t<td>"
               << entry.description() << "</td>\n\t\t\t\t<td>"
               << date << "</td>\n\t\t\t</tr>\n";
    }
    
    stream << "\t\t</tbody>\n\t</table>\n</body>\n</html>";
    file.close();
    return true;
}

} // namespace db
} // namespace wynn
