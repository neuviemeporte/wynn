#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>

class QXmlStreamReader;

namespace wynn {
namespace db {
namespace xml {

constexpr char 
    XML_DATEFORMAT[] = "dd.MM.yyyy.hh.mm.ss",
    XML_CREATED[]    = "init",
    XML_UPDATED[]    = "update";

QString readText(QXmlStreamReader &xml, const QString &element);
QDateTime readDate(QXmlStreamReader &xml, const QString &element, bool permitEmpty);
int readInteger(QXmlStreamReader &xml, const QString &element);

} // namespace xml
} // namespace db
} // namespace wynn

#endif // COMMON_H
