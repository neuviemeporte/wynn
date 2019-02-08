#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QDateTime>

class QXmlStreamReader;

namespace wynn {
namespace db {

enum QuizDirection { DIR_UNDEF = -1, DIR_SHOWDESC = 101, DIR_SHOWITEM = 202 };
enum QuizTakeMode { TAKE_UNDEF = -1, TAKE_FAILS, TAKE_OLDIES, TAKE_RANDOM };
enum QuizResult { QUIZ_NOCHANGE, QUIZ_SUCCESS, QUIZ_FAIL };

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
