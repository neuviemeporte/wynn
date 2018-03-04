#include "japanese.h"

#include <ixplog_active.h>
#include <QXmlStreamReader>

#define QLOGSTREAM IXPDICT_LOGSTREAM
#define QLOGTAG "[IXP_DICT]"

// ==============================================================================================
// ============================== JpWord
// ==============================================================================================

JpWord::JpWord()
{
}

JpWord::JpWord(const QStringList &expression, const QStringList &reading, 
		   const QStringList &definition, const QStringList &category) :
expressions_(expression), readings_(reading), 
definitions_(definition), categories_(category)
{
}

bool JpWord::expressionMatches(const QString &arg, MatchType match) const 
{
	return (Util::strListMatch(expressions_, arg, match) >= 0); 
}

bool JpWord::readingMatches(const QString &arg, MatchType match) const 
{ 
	return (Util::strListMatch(readings_, arg, match) >= 0); 
}

bool JpWord::definitionMatches(const QString &arg, MatchType match) const 
{ 
	return (Util::strListMatch(definitions_, arg, match) >= 0); 
}

QDataStream& operator<<(QDataStream &stream, const JpWord &item)
{
	return (stream << item.expressions_ << item.readings_ << item.definitions_ << item.categories_);
}

QDataStream& operator>>(QDataStream &stream, JpWord &item)
{
	return (stream >> item.expressions_ >> item.readings_ >> item.definitions_ >> item.categories_);
}

// ==============================================================================================
// ============================== JmDict
// ==============================================================================================

JmDict::~JmDict()
{
	QLOGX("Destroying dictionary");
}

void JmDict::parseXML(const QString &dictPath)
{
	// open XML file
	QFile file(dictPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		error_ = true;
		return;
	}

	// prepare temporary data storage
	QStringList expr, read, def, cat;
	QString text, name;
	double lineno;
	int curprogress, globprogress = 0;

	// create and initialize the XML parser
	QXmlStreamReader xml;
	xml.setDevice(&file);

	//dbg << "Started parsing XML file: " << file->fileName() );

	while (!xml.atEnd() && !xml.hasError())
	{
		// read next element and extract its name
		xml.readNext();
		name = xml.name().toString();

		if (xml.isStartElement())
		{
			// start of a dictionary entry, calculate the progress based on line number
			if (name == "entry")
			{
				lineno = static_cast<double>(xml.lineNumber());
				// the dictionary has 2730254 lines, calculate percentage
				curprogress = static_cast<int>((lineno / 2730254) * 100);
				// update progress if rounded integer percentage changed value
				if (curprogress > globprogress)
				{
					globprogress = curprogress;
				}
			}
			// kanji writing element
			else if (name == "keb")
			{
				text = xml.readElementText();
				//dbg << "kanji element: " << text );
				expr.append(text);
			}
			// word reading element
			else if (name == "reb")
			{
				text = xml.readElementText();
				//dbg << "reading element: " << text );
				read.append(text);
			}
			// word definition element
			else if (name == "gloss")
			{
				text = xml.readElementText();
				//dbg << "glossary element: " << text );
				def.append(text);
			}
			// category entity elements
			else if (name == "pos" || name == "misc")
			{
				text = xml.readElementText();
				//dbg << "category element: " << text );
				cat.append(text);
			}
		}
		// end of a dictionary entry, create new JpWord and add it to the dictionary,
		// clear temporary storage
		else if (name == "entry" && xml.isEndElement()) 
		{
			//dbg << "End of Entry element" );
			addEntry(JpWord(expr, read, def, cat));
			expr.clear();
			read.clear();
			def.clear();
			cat.clear();
		}
	}

	// handle parsing errors (basically: panic)
	if (xml.hasError())
	{
		QString error = xml.errorString();
		QString msg = "Error while processing JmDict dictionary file!\n" + dictPath
			+ "\nXML Error: " + error + "\nWord dictionary functionality will be disabled!";
		QLOGX("XML PARSE ERROR: " << error );
		//TODO: emit signal
		error_ = true;
		return;
	}

	// close the file and update dictionary item count
	file.close();
}

QList<int> JmDict::findJapanese(const QStringList &arg, MatchType match) const
{
	QLOGX("Searching for: '" << Util::strListMerge(arg, ",") << "', match type: " << match);
	QList<int> ret;
	QString item;
	int items = arg.count();

	for (int i = 0; i < entries_.size(); ++i) 
	{
		const JpWord &entry = entries_.at(i);
		for (int j = 0; j < items; ++j)
		{
			item = arg.at(j);
			if (entry.expressionMatches(item, match) || entry.readingMatches(item, match))
			{
				ret.append(i); break;
			}
		}
	}

	return ret;
}

QList<int> JmDict::findEnglish(const QString &arg, MatchType match) const
{
	QLOGX("Searching for: '" << arg << "', match type: " << match);
	QList<int> ret;

	for (int i = 0; i < entries_.size(); ++i) 
	{
		const JpWord &entry = entries_.at(i);
		if (entry.definitionMatches(arg, match)) ret.append(i);
	}

	return ret;
}

QTextStream& operator <<(QTextStream &stream, const JpWord &/*item*/)
{
	qFatal("Not implemented");
    return stream;
}
