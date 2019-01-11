#ifndef ERROR_H
#define ERROR_H

#include <QTextStream>
#include <QString>

namespace wynn {
namespace db {

class Error
{
public:
    enum Type
    {
        OK,    // everything OK, no error
        ARGS,  // bad arguments for operation
        LOCK,  // database was locked
        DUPLI, // database reported duplicate
        PARSE, // database parsed failed
        SYNC   // synchronization failed
    };
    
private:
    Type type_;
    QString msg_;
    int index_;
    
public:
    Error(const Type type, const QString &msg = QString(), int index = -1) : type_(type), msg_(msg), index_(index) {}
    
    Type type() const { return type_; }
    const QString msg() const { return msg_; }
    int index() const { return index_; }
    
    bool operator==(const Type type) const { return type_ == type; }
    bool operator!=(const Type type) const { return !(*this == type); }
};

} // namespace db
} // namespace wynn

inline QTextStream& operator<<(QTextStream &os, const wynn::db::Error &arg)
{
    return (os << arg.type()
            << (arg.msg().isEmpty() ? "" : QString(", '") + arg.msg() + "'")
            << (arg.index() < 0 ? "" : QString(", idx = ") + QString::number(arg.index())));
}

#endif // ERROR_H
