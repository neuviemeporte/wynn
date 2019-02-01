#ifndef IXPLOG_ACTIVE_H
#define IXPLOG_ACTIVE_H

#include "libixplog_global.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <vector>
#include <sstream>

//#define QLOG_INDENT_DEBUG // enable indent debugging mode, useful when indentation is broken
// TODO: error, warning, info... macros

class QSize;
class QPoint;
class QPointF;
class QLine;
class QLineF;
class QRectF;
class QColor;

#ifndef QLOG_DISABLE // debug log enabled, define debug macros
	// current indent string
	#define QLOGINDENT QLOGSTREAM->indentStr()
	// single indentation unit
    #define QLOGINDSTR "    "
	
#ifndef QLOG_INDENT_DEBUG // normal operation
	// log a message along with the signature of the current function
    #define QLOGX(x)   if (QLOGSTREAM) *QLOGSTREAM << QLOGTAG <<  QLOGINDENT << __FUNCTION__ << "(): " << x << endl
	// log a message, no signature
    #define QLOG(x)    if (QLOGSTREAM) *QLOGSTREAM << QLOGTAG <<  QLOGINDENT << x << endl
	// log without trailing newline
    #define QLOGNL(x)  if (QLOGSTREAM) *QLOGSTREAM << QLOGTAG <<  QLOGINDENT << x
	// log without indent before and newline after message
    #define QLOGRAW(x) if (QLOGSTREAM) *QLOGSTREAM << QLOGTAG <<  x

	// the same as the above 4, just conditional depending on y
	// useful for creating groups of messages which can be activated and deactivated
	// based on some #define
	#define QLOGCX(x,y)		if (y) QLOGX(x)
	#define QLOGC(x,y)		if (y) QLOG(x)
	#define QLOGCNL(x,y)	if (y) QLOGNL(x)
	#define QLOGCRAW(x,y)	if (y) QLOGRAW(x)

	// manipulate indent level
	#define QLOGINC  if (QLOGSTREAM) QLOGSTREAM->incIndent()
	#define QLOGDEC  if (QLOGSTREAM) QLOGSTREAM->decIndent()
	#define QLOGPOP  if (QLOGSTREAM) QLOGSTREAM->popIndent()
	#define QLOGPUSH if (QLOGSTREAM) QLOGSTREAM->pushIndent()
	#define QLOGCLR  if (QLOGSTREAM) QLOGSTREAM->clrIndent()
#else // indent debugging, useful for tracing missed/surplus indent directives
	// disable all normal messages
	#define QLOGX(x)
	#define QLOG(x)
	#define QLOGNL(x)
	#define QLOGRAW(x)

	#define QLOGCX(x,y)
	#define QLOGC(x,y)
	#define QLOGCNL(x,y)
	#define QLOGCRAW(x,y)

	// output opening and closing braces to log whenever indent level increases or decreases along with the current function signature
	// indent instruction mismatches can then easy be found in the log using an editor with automatic matching brace highlighting
    #define QLOGINC  if (QLOGSTREAM) { *QLOGSTREAM << QLOGTAG << QLOGINDENT << __FUNCTION__ << "() {"  << endl; QLOGSTREAM->incIndent(); }
    #define QLOGDEC  if (QLOGSTREAM) { QLOGSTREAM->decIndent(); *QLOGSTREAM << QLOGTAG <<  QLOGINDENT << __FUNCTION__ << "() }"  << endl; }
    #define QLOGPOP  if (QLOGSTREAM) { *QLOGSTREAM << QLOGTAG << QLOGINDENT << __FUNCTION__ << "() !}" << endl; QLOGSTREAM->popIndent(); }
	#define QLOGPUSH if (QLOGSTREAM) QLOGSTREAM->pushIndent()
	#define QLOGCLR  if (QLOGSTREAM) QLOGSTREAM->clrIndent()
#endif

	#define QLOGFLUSH if (QLOGSTREAM) QLOGSTREAM->flush();

#else // debug log disabled
	#define QT_NO_DEBUG
	#include "qlog_inactive.h"
#endif

/// Debug logger class. Accepts input from its implementation of operator<< and writes formatted output to the underlying file.
/// Not meant to be used directly, but with the LOG... macros.
class LIBQTLOG_EXPORT QTSLogger
{
private:
	QString path_;
	QFile debugFile_;
	QTextStream debugStream_;
	QMutex mutex_;
	int indent_, store_;
	bool ok_;

    // A wee bit of SFINAE to spice things up.

    // Checks a boolean condition and evaluates to a struct containing a typedef for T as its sole member if true
    // (by default, T is void).
    // Otherwise, enable_if evaluates to an empty struct.
    // This is already defined in C++11 in the type_traits header, but we are compiling without C++11 support
    // so we need to roll our own.
    template <bool Cond, typename T = void> struct enable_if {};
    template <typename T> struct enable_if<true, T> { typedef T type; };

    // Evaluates to a struct with a single static boolean field which contains true if type T is a container and false
    // otherwise. Whether T is a container or not is determined based on whether it contains a typedef for const_iterator.
    template <typename T> struct is_container
    {
        typedef char yes[1];
        typedef char no[2];

        template <typename U> static yes& test(typename U::const_iterator*);
        template <typename U> static  no& test(...);

        static const bool value = sizeof(test<T>(0)) == sizeof(yes);
    };

public:
	QTSLogger(const QString &filename);

	void open();
	void flush() { mutex_.lock(); debugStream_.flush(); mutex_.unlock(); }
	QString indentStr();


    template <typename T> QTSLogger& operator<<(const T &arg)
    {
        mutex_.lock();
        if (ok_) debugStream_ << arg;
        mutex_.unlock();
        return *this;
    }

    template <typename T> QTSLogger& operator<<(const QList<T> &arg) { return logContainer(arg); }
    template <typename T> QTSLogger& operator<<(const std::vector<T> &arg) { return logContainer(arg); }
    QTSLogger& operator<<(const QStringList &arg) { return logContainer(arg); }

    void incIndent()  { mutex_.lock(); indent_++;        mutex_.unlock(); }
	void decIndent()  { mutex_.lock(); indent_--;        mutex_.unlock(); }
	void clrIndent()  { mutex_.lock(); indent_ = 0;	     mutex_.unlock(); }
	void pushIndent() { mutex_.lock(); store_ = indent_; mutex_.unlock(); }
	void popIndent()  { mutex_.lock(); indent_ = store_; mutex_.unlock(); }

private:

    template <typename T> QTSLogger& logContainer(const T &arg)
    {
        if (!ok_) return *this;

        debugStream_ << "[" << arg.size() << "] (" << endl;
        incIndent();

        const QString indent(indentStr());

        for (int i = 0; i < arg.size(); ++i)
        {
            *this << indent << arg.at(i) << endl;
        }

        decIndent();
        debugStream_ << indentStr() << ")";

        return *this;
    }
};

LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream& os, const std::string &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QSize &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QPoint &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QPointF &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QLine &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QLineF &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QRectF &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QColor &arg);
LIBQTLOG_EXPORT QTextStream& operator<<(QTextStream &os, const QDateTime &arg);

#endif // IXPLOG_ACTIVE_H
