#ifndef DATABASE_H_
#define DATABASE_H_

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QDateTime>
#include <QUuid>

#include "entry.h"
#include "error.h"

namespace wynn {
namespace db {

/// A database that keeps a list of DbEntry objects and manages them.
/// TODO: updated_ not updated, how ironic.
class Database : public QObject {
    Q_OBJECT
public:
    static const QString XML_EXT, XML_HEADER, XML_NAME;
    
protected:
    QString name_;
    QDateTime created_, updated_;
    QList<Entry> entries_;
    mutable bool dirty_, error_, locked_, external_;
    
public:
    Database(QObject *parent);
    Database(QObject *parent, const QString &name);
    
    const QString& name() const { return name_;	}
    const QDateTime& dateCreated() const { return created_;	}
    const QDateTime& dateUpdated() const { return updated_; }
    
    int entryCount() const { return entries_.size(); }
    const Entry& entry(int idx) const { 
        Q_ASSERT(idx >= 0 && idx < entryCount()); 
        return entries_.at(idx); 
    }
    bool dirty() const { return dirty_; }
    bool error() const { return error_; }
    bool locked() const { return locked_; }
    bool external() const { return external_; }
    void setLocked(bool arg) { locked_ = arg; }
    void setExternal(bool arg) { external_ = arg; }
    int findEntry(const QString &text, int startIndex = 0) const;
    int findDuplicate(const Entry &entry, int exceptIdx = -1) const;
    
    virtual Error add(const QString &item, const QString &desc, const QUuid &uuid = {}, const bool dupIgnore = false);
    virtual Error remove(int idx);
    virtual Error remove(const QList<int> &idxs);
    virtual Error alter(int idx, const QString &item, const QString &desc, bool dupIgnore = false);
    virtual Error point(const int idx, const QuizDirection type);
    virtual Error fail(const int idx, const QuizDirection type);
    virtual Error reset();
    
    virtual void saveXML(const QString &path) const;
    virtual void saveXMLExtraAttributes(QXmlStreamWriter &) const {}
    virtual void saveXMLExtraElements(QXmlStreamWriter &) const {}
    
    virtual Error loadXML(const QString &path);
    virtual void loadXMLExtraAttributes(QXmlStreamReader&) {}
    virtual void loadXMLExtraElements(QXmlStreamReader &xml, const QString &name);
    virtual Error loadXmlPostCheck() { return Error::OK; }
    bool htmlExport(const QString &path, const QList<int> &idxs);
    
signals:
    void countUpdate();
    void beginInsert();
    void endInsert();
    void beginRemove(int index);
    void endRemove();    
};

} // namespace db
} // namespace wynn

#endif // DATABASE_H_
