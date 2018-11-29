#include <QXmlStreamReader>

#include "common.h"
#include "ixplog_active.h"

// TODO: eliminate the need for this mantra everywhere
extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"

#define DBXML_DEBUG 1

QString wynn::db::xml::readText(QXmlStreamReader &xml, const QString &element)
{
	QString text = xml.readElementText();
	QLOGC("Text element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	if (text.isEmpty()) xml.raiseError(QObject::tr("Empty text element '") + element + "'");
	return text;
}

QDateTime wynn::db::xml::readDate(QXmlStreamReader &xml, const QString &element, bool permitEmpty)
{
	QString text = xml.readElementText();
	QLOGC("Date element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	QDateTime ret = QDateTime::fromString(text, XML_DATEFORMAT);

	if (text.isEmpty())
	{
		if (!permitEmpty) xml.raiseError(QObject::tr("Empty date element '") + element + "'");
	}
	else if (!ret.isValid()) xml.raiseError(QObject::tr("Invalid date format for element '") + element + "'");
	
	return ret;
}

int wynn::db::xml::readInteger(QXmlStreamReader &xml, const QString &element)
{
	QString text = xml.readElementText();
	QLOGC("Integer element '" << element << "' = '" << text << "'", DBXML_DEBUG);
	bool ok = true;
	int ret = text.toInt(&ok);
	if (!ok) xml.raiseError(QObject::tr("Invalid integer value for element '") + element + "'");
	return ret;
}