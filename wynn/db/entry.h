#ifndef ENTRY_H
#define ENTRY_H

#include <cmath>
#include <QString>
#include <QUuid>

class QTextStream;
class QDateTime;
class QXmlStreamReader;
class QXmlStreamWriter;

namespace wynn {
namespace db {

enum QuizDirection;

class Entry {
public:
    static const QString
    XML_HEADER, XML_UUID, XML_ITEM, XML_DESC, 
    XML_DESCPOINTS, XML_DESCFAILS, XML_ITEMPOINTS, XML_ITEMFAILS;
    
private:
    static const int MAX_POINTS = 100;
    QUuid uuid_;
    qint64 createStamp_, updateStamp_;
    QString item_, desc_;
    int	descShownPoints_, descShownFails_, 
    itemShownPoints_, itemShownFails_;
    
public:
    Entry(QUuid uuid, qint64 createStamp, qint64 updateStamp,
          const QString &item, const QString desc,
          int descShownPoints, int descShownFails,
          int itemShownPoints, int itemShownFails);
    Entry();
    Entry(const QUuid &uuid, const QDateTime &dbCreated,
          const QString &item, const QString &desc);
    explicit Entry(QXmlStreamReader &xml);
    
    bool equal(const Entry &other, const bool complete) const;
    bool operator==(const Entry &arg) const { return equal(arg, true); }
    bool operator!=(const Entry &arg) const { return !equal(arg, true); }
    
    const QUuid& uuid() const { return uuid_; }
    qint64 createStamp() const { return createStamp_; }
    qint64 updateStamp() const { return updateStamp_; }
    const QString& item() const	{ return item_; }
    const QString& description() const { return desc_; }
    bool isValid() const { return !uuid_.isNull(); }
    
    int points(const QuizDirection type) const;
    int level(const QuizDirection type) const { return level(points(type)); }
    int fails(const QuizDirection type) const;
    
    void setItem(const QString &arg) { item_ = arg; }
    void setDescription(const QString &arg) { desc_ = arg; }
    
    void addPoint(const QuizDirection type);
    void addFail(const QuizDirection type);
    
    void undoPoint(const QuizDirection type);
    void undoFail(const QuizDirection type, const int points);
    
    void update(const QDateTime &dbCreated);
    void reset();
    
    void toXML(QXmlStreamWriter &xml) const;
    
protected:
    int level(const int &points) const { return static_cast<int>(sqrt(static_cast<float>(points))); }
};

} // namespace db
} // namespace wynn

QTextStream& operator<<(QTextStream &os, const wynn::db::Entry &arg);

#endif // ENTRY_H
