#include "ixplog_active.h"

#include <cstdarg>
#include <sstream>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QPointF>
#include <QRectF>
#include <QLineF>
#include <QDateTime>

using namespace std;

// ===========================================================================================================
// ==================== QTSLogger
// ===========================================================================================================

QTSLogger::QTSLogger(const QString& filename) : 
	path_(filename), indent_(0), store_(0), ok_(true)
{
	mutex_.lock(); 

	if (!path_.isEmpty())
	{
		debugFile_.setFileName(path_);
		if (debugFile_.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			debugStream_.setDevice(&debugFile_);
			debugStream_.setCodec(QTextCodec::codecForName("UTF-8"));
			debugStream_ << bom << "Logger object created successfully" << endl;
		}
		else ok_ = false;
	}
	else ok_ = false;

	mutex_.unlock(); 
}

QString QTSLogger::indentStr()
{
	QString ret = "";
	for (int i = 0; i < indent_; ++i)
	{
		ret += QLOGINDSTR;
	}
	if (indent_ < 0) ret = "(NEGATIVE INDENT): ";
	return ret;
}

// ===========================================================================================================
// ==================== Stream operators
// ===========================================================================================================

QTextStream& operator<<(QTextStream &os, const QSize &arg)
{
	return (os << arg.width() << "x" << arg.height());
}

QTextStream& operator<<(QTextStream &os, const QPoint &arg)
{
	return (os << arg.x() << "," << arg.y());
}

QTextStream& operator<<(QTextStream &os, const QPointF &arg)
{
	return (os << arg.x() << "," << arg.y());
}

QTextStream& operator<<(QTextStream &os, const QLine &arg)
{
	return (os << arg.p1() << "," << arg.p2());
}

QTextStream& operator<<(QTextStream &os, const QLineF &arg)
{
	return (os << arg.p1() << "," << arg.p2());
}

QTextStream& operator<<(QTextStream &os, const QRectF &arg)
{
	return (os << arg.topLeft() << "," << arg.bottomRight());
}

QTextStream& operator<<(QTextStream& os, const std::string &arg)
{
	return (os << QString::fromStdString(arg));
}

QTextStream& operator<<(QTextStream &os, const QDateTime &arg)
{
	return (os << arg.toString("dd.MM.yyyy.hh.mm.ss.zzz"));
}

