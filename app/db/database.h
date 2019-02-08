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

class DatabaseInterface {
public:
    virtual ~DatabaseInterface() {}
    virtual const QString& name() const = 0;
    virtual int entryCount() const = 0;
    virtual const Entry& entry(int idx) const = 0;
    virtual Error add(const QString &item, const QString &desc, const QUuid &uuid, const bool dupIgnore) = 0;
    virtual Error remove(int idx) = 0;
    virtual Error remove(const std::list<int> &idxs) = 0;
    virtual Error alter(int idx, const QString &item, const QString &desc, bool dupIgnore) = 0;
    virtual Error point(const int idx, const QuizDirection type) = 0;
    virtual Error fail(const int idx, const QuizDirection type) = 0;
    virtual Error reset() = 0;
    virtual void saveXML(const QString &path) const = 0;
    virtual void saveXMLExtraAttributes(QXmlStreamWriter &) const = 0;
    virtual void saveXMLExtraElements(QXmlStreamWriter &) const = 0;
    virtual Error loadXML(const QString &path) = 0;
    virtual void loadXMLExtraAttributes(QXmlStreamReader&) = 0;
    virtual void loadXMLExtraElements(QXmlStreamReader &xml, const QString &name) = 0;
    virtual Error loadXmlPostCheck() = 0;
};

/// A database that keeps a list of DbEntry objects and manages them.
/// TODO: updated_ not updated, how ironic.
class Database : public QObject, public DatabaseInterface {
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
    
    const QString& name() const override { return name_;	}
    const QDateTime& dateCreated() const { return created_;	}
    const QDateTime& dateUpdated() const { return updated_; }
    
    int entryCount() const override { return entries_.size(); }
    const Entry& entry(int idx) const override {
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
    
    // TODO: accept Entry refs instead of raw strings, support move semantics
    Error add(const QString &item, const QString &desc, const QUuid &uuid = {}, const bool dupIgnore = false) override;
    Error remove(int idx) override;
    Error remove(const std::list<int> &idxs) override;
    Error alter(int idx, const QString &item, const QString &desc, bool dupIgnore = false) override;
    Error point(const int idx, const QuizDirection type) override;
    Error fail(const int idx, const QuizDirection type) override;
    Error reset() override;
    
    void saveXML(const QString &path) const override;
    void saveXMLExtraAttributes(QXmlStreamWriter &) const override {}
    void saveXMLExtraElements(QXmlStreamWriter &) const override {}
    
    Error loadXML(const QString &path) override;
    void loadXMLExtraAttributes(QXmlStreamReader&) override {}
    void loadXMLExtraElements(QXmlStreamReader &xml, const QString &name) override;
    Error loadXmlPostCheck() override { return Error::OK; }

    bool htmlExport(const QString &path, std::list<int> &idxs);
    
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
