#include "mandarin.h"

#include <ixplog_active.h>
#include <QTextCodec>

#define QLOGSTREAM IXPDICT_LOGSTREAM
#define QLOGTAG "[IXP_DICT]"

// ==============================================================================================
// ============================== CnWord
// ==============================================================================================

CnWord::CnWord() 
{
}

CnWord::CnWord(const QString &traditional, const QString &simplified, const QString &pinyin, 
			   const QStringList &definitions) : 
traditional_(traditional), simplified_(simplified), pinyin_(pinyin), 
definitions_(definitions) 
{
}

bool CnWord::expressionMatches(const QString &arg, MatchType match) const
{
	return (Util::strMatch(traditional_, arg, match) || Util::strMatch(simplified_, arg, match));
}

bool CnWord::pinyinMatches(const QString &arg, MatchType match) const
{
	return (Util::strMatch(pinyin_, arg, match) || Util::strMatch(Util::pinyinNoDigits(pinyin_), arg, match) ||
		Util::strMatch(Util::pinyinToAccents(pinyin_), arg, match));
}

bool CnWord::definitionMatches(const QString &arg, MatchType match) const
{
	return (Util::strListMatch(definitions_, arg, match) >= 0); 
}

QDataStream& operator<<(QDataStream &stream, const CnWord &item)
{
	return (stream << item.traditional_ << item.simplified_ << item.pinyin_ << item.definitions_);
}

QDataStream& operator>>(QDataStream &stream, CnWord &item)
{
	return (stream >> item.traditional_ >> item.simplified_ >> item.pinyin_ >> item.definitions_);
}

// ==============================================================================================
// ============================== Cedict
// ==============================================================================================

Cedict::~Cedict()
{
	QLOGX("Destroying dictionary");
}

void Cedict::parse(const QString &path)
{
	QLOGX("Parsing dictionary file: '" << path << "'");
	QLOGINC;

	QFile f(path);
	if (!f.open(QIODevice::ReadOnly))
	{
		QLOG("Could not open file for reading: '" << path << "'");
		error_ = true;
		QLOGDEC;
		return;
	}

	QTextStream filestr(&f);
	filestr.setCodec(QTextCodec::codecForName("UTF-8"));

	QString line, token, trad, simp, pinyin, def;
	QStringList defs;

	for (int i = 0; !filestr.atEnd(); ++i)
	{
		line = filestr.readLine();
		if (line.startsWith("#")) continue;

		QTextStream linestr(&line, QIODevice::ReadOnly);
		linestr.setCodec(QTextCodec::codecForName("UTF-8"));
		trad.clear();
		simp.clear();

		// process single line of input
		while (!linestr.atEnd())
		{
			linestr >> token;
			//QLOG("Token: '" << token << "'");
			QLOGINC;

			// start of pinyin entry
			if (token.startsWith("["))
			{
				//QLOG("This is the start of pinyin");
				QLOGINC;

				pinyin.clear();
				bool done = false;

				do
				{
					if (token.startsWith("["))
					{
						//QLOG("Removing [");
						token.remove(0,1);
					}
					if (token.endsWith("]"))
					{
						//QLOG("Removing ]");
						token.remove(token.size()-1,1);
						done = true;
					}
					pinyin.append(token);
					//QLOG("After append: '" << pinyin << "'");
					if (done) break; 

					linestr >> token;
				} while (!linestr.atEnd());
				//QLOG("Done");
				QLOGDEC;
			}

			// start of definitions
			else if (token.startsWith("/"))
			{
				//QLOG("This is the start of definitions");
				QLOGINC;
				bool done = false;

				def.clear();
				do
				{
					if (linestr.atEnd()) done = true;
					def.append(token + " ");
					linestr >> token;
				} while(!done);
				def.remove(def.size()-1,1);
				//QLOG("Definition string: '" << def << "'");
				defs = def.split('/', QString::SkipEmptyParts);
				QLOGDEC;
			}

			else if (trad.isEmpty())
			{
				//QLOG("Something else and traditional empty");
				QLOGINC;
				do
				{
					trad.append(token);
					//QLOG("After append: '" << trad << "'");
					if (!token.endsWith(",")) break;
					linestr >> token;
				} while(!linestr.atEnd());
				QLOGDEC;
			}

			else if (simp.isEmpty())
			{
				//QLOG("Something else and simplified empty");
				QLOGINC;
				do
				{
					simp.append(token);
					//QLOG("After append: '" << simp << "'");
					if (!token.endsWith(",")) break;
					linestr >> token;
				} while(!linestr.atEnd());
				QLOGDEC;
			}
			else
			{
				QLOG("Something is wrong, terminating");
				error_ = true;
				QLOGDEC; QLOGDEC;
				return;
			}

			QLOGDEC;
		} // process line

		addEntry(CnWord(trad, simp, pinyin, defs));
		//QLOG("Line " << i << ": (" << trad << ")-(" << simp << ") " << pinyin << " --- '" << Util::strListMerge(defs, "|") << "'");
	} // process file
	QLOGDEC;
}

QList<int> Cedict::findMandarin(const QString &arg, MatchType match) const
{
	QLOGX("Searching for: '" << arg << "', match type: " << match << ", dictionary has " << entries_.size() << " entries");
	QLOGINC;
	QList<int> ret;

	for (int i = 0; i < entries_.size(); ++i) 
	{
		const CnWord &entry = entries_.at(i);
		//QLOG(i << ": " << entry.expressionStr() << ": " << entry.definitionStr() << " (" << entry.pinyin() << ")");
		if (entry.expressionMatches(arg, match) || entry.pinyinMatches(arg, match)) ret.append(i);
	}
	QLOGDEC;
	return ret;
}

QList<int> Cedict::findEnglish(const QString &arg, MatchType match) const
{
	QLOGX("Searching for: '" << arg << "', match type: " << match);
	QList<int> ret;

	for (int i = 0; i < entries_.size(); ++i) 
	{
		const CnWord &entry = entries_.at(i);
		if (entry.definitionMatches(arg, match)) ret.append(i);
	}

	return ret;
}

QTextStream& operator <<(QTextStream &stream, const CnWord &/*item*/)
{
	qFatal("Not implemented");
    return stream;
}
