#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QStringList>

#include "german.h"
#include "ixplog_active.h"

#if QT_VERSION >= 0x050000
#include <QRegularExpression>
#define REGEXP QRegularExpression
#else
#include <QRegExp>
#define REGEXP QRegExp
#endif

#define QLOGSTREAM IXPDICT_LOGSTREAM
#define QLOGTAG "[IXP_DICT]"
QLocale localeDE(QLocale::German, QLocale::Germany);

// ==============================================================================================
// ============================== BeoResult
// ==============================================================================================

LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const BeoResult &item)
{
	return (stream << "item = " << item.item << ", def = " << item.def << ", phrase = " << item.phrase);
}

// ==============================================================================================
// ============================== BeoPhrase
// ==============================================================================================

BeoPhrase::BeoPhrase(const BeoLanguage lang, const QString &text) :
	text_(text)
{
    //QLOGX("Constructing phrase");
    //QLOGINC;

    //QLOG("Text: '" << text << "'");

	switch(lang)
	{
		case BEO_DE: core_ = localeDE.toLower(text); break;
		case BEO_EN: core_ = text.toLower(); break;
		case BEO_NONE: break;
	}

    //QLOG("Core before: '" << core_ << "'");
		
	if (!core_.isEmpty())
	{
		// remove everything inside [], (), {} and // delimiters to form phrase core
		// (this is the text inside which searches will be performed)"
		core_.remove(REGEXP("\\[[^]]*\\]"));
		core_.remove(REGEXP("\\([^)]*\\)"));
		core_.remove(REGEXP("\\{[^}]*\\}"));
		core_.remove(REGEXP("/[^/]*/"));
		core_ = core_.simplified();
	}

    //QLOG("Core after: '" << core_ << "'");

    //QLOGDEC;
}

bool BeoPhrase::match(const QStringList &arg, const MatchType type) const
{
	const int matchIdx = Util::strListMatch(arg, core_, type, false);
	if (matchIdx >= 0)
	{
		//QLOGX("Match of '" << core_ << "' (" << text_ << ") against " << arg << ", type: " << type);
		return true;
	}

	return false;
}

QDataStream& operator<<(QDataStream &stream, const BeoPhrase &item)
{
	return (stream << item.text() << item.core());
}

QDataStream& operator>>(QDataStream &stream, BeoPhrase &item)
{
	return (stream >> item.text_ >> item.core_);
}

QTextStream& operator<<(QTextStream &stream, const BeoPhrase &item)
{
	return (stream << "'" << item.core() << "'");
}

// ==============================================================================================
// ============================== BeoDefinition
// ==============================================================================================

BeoDefinition::BeoDefinition(const QString &deText, const QString &enText)
{
	//QLOGX("Constructing definition");
	//QLOGINC;

	const QStringList
		dePhrases = deText.split(";", QString::SkipEmptyParts),
		enPhrases = enText.split(";", QString::SkipEmptyParts);

	//QLOG("DE: '" << deText << "'");

	for (int i = 0; i < dePhrases.size(); ++i)
	{
		const QString phraseTxt = dePhrases.at(i).trimmed();
		const BeoPhrase phrase(BEO_DE, phraseTxt);
        if (phrase.ok()) de_.append(phrase);
        else
        {
            QLOGX("Unable to construct DE phrase from '" << phraseTxt << "', skipping!!!");
        }

	}

	//QLOG("EN: '" << enText << "'");

	for (int i = 0; i < enPhrases.size(); ++i)
	{
		const QString phraseTxt = enPhrases.at(i).trimmed();
		const BeoPhrase phrase(BEO_EN, phraseTxt);
        if (phrase.ok()) en_.append(phrase);
        else
        {
            QLOGX("Unable to construct EN phrase from '" << phraseTxt << "', skipping!!!");
        }
	}

	//QLOGDEC;
}

QString BeoDefinition::deText() const
{
	QString ret;

	for (int i = 0; i < dePhraseCount(); ++i)
	{
		ret += dePhrase(i).text();
		if (i < dePhraseCount() - 1) ret += "; ";
	}

	return ret;
}

QString BeoDefinition::enText() const
{
	QString ret;

	for (int i = 0; i < enPhraseCount(); ++i)
	{
		ret += enPhrase(i).text();
		if (i < enPhraseCount() - 1) ret += "; ";
	}

	return ret;
}

BeoResult BeoDefinition::match(const BeoLanguage lang, const QStringList &arg, const MatchType type) const
{
	// iterate over phrases in the correct language
	if (lang == BEO_DE) for (int i = 0; i < de_.size(); ++i)
	{
		const BeoPhrase& phrase = de_.at(i);
		if (phrase.match(arg, type)) return BeoResult(-1, -1, i);
	}
	else if (lang == BEO_EN) for (int i = 0; i < en_.size(); ++i)
	{
		const BeoPhrase& phrase = en_.at(i);
		if (phrase.match(arg, type)) return BeoResult(-1, -1, i);
	}

	return BeoResult();
}

QDataStream& operator<<(QDataStream &stream, const BeoDefinition &item)
{
	return (stream << item.dePhrases() << item.enPhrases());
}

QDataStream& operator>>(QDataStream &stream, BeoDefinition &item)
{
	return (stream >> item.de_ >> item.en_);
}

QTSLogger& operator<<(QTSLogger &stream, const BeoDefinition &item)
{
	stream << "DE: ";
	stream << item.dePhrases();
	stream << ", EN: ";
	stream << item.enPhrases();
	return stream;
}

// ==============================================================================================
// ============================== BeoItem
// ==============================================================================================

BeoItem::BeoItem(const QString &text)
{
	//QLOGX("Constructing item");
	//QLOGINC;

	// split into german and english part
	const QStringList de_en = text.split("::", QString::SkipEmptyParts);
	Q_ASSERT(de_en.size() == 2);
	const QString
		dePart = de_en.first().trimmed(),
		enPart = de_en.last().trimmed();
	const QStringList
		deDefs = dePart.split("|", QString::SkipEmptyParts),
		enDefs = enPart.split("|", QString::SkipEmptyParts);

	//QLOG("DE = " << deDefs.size() << ", EN = " << enDefs.size());

	// iterate over definitions in DE and EN parts, create definition items from both
	// skip items with definition count mismatch or empty 
	if (deDefs.size() && enDefs.size() && deDefs.size() == enDefs.size()) for (int i = 0; i < deDefs.size(); ++i)
	{
        const QString
                deText = deDefs.at(i).trimmed(),
                enText = enDefs.at(i).trimmed();

        BeoDefinition def(deText, enText);
        if (def.ok()) defs_.append(def);
        else
        {
            QLOGX("Could not create definition from DE '" << deText << "', EN '" << enText << "', skipping!!!");
        }
	}
    else
    {
        QLOGX("Could not create definition, mismatch with DE '" << dePart << "', EN '" << enPart << "', skipping!!!");
    }

	//QLOGDEC;
}

BeoResult BeoItem::match(const BeoLanguage lang, const QStringList &arg, const MatchType type) const
{
	// iterate over definitions
	for (int i = 0; i < defs_.size(); ++i)
	{
		const BeoDefinition &def = defs_.at(i);
		BeoResult res = def.match(lang, arg, type);
		if (res.phraseFound())
		{
			res.def = i;
			return res;
		}
	}

	return BeoResult();
}

QDataStream& operator<<(QDataStream &stream, const BeoItem &item)
{
	return (stream << item.definitions());
}

QDataStream& operator>>(QDataStream &stream, BeoItem &item)
{
	return (stream >> item.defs_);
}

QTSLogger& operator<<(QTSLogger &stream, const BeoItem &item)
{
	return (stream << item.definitions());
}

// ==============================================================================================
// ============================== BeoDict
// ==============================================================================================

void BeoDict::parse(const QString &path)
{
	QLOGX("Parsing BeoLinugs dictionary database file: '" << path << "'");
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

	size_t ok = 0, fail = 0;
	for (size_t i = 0; !filestr.atEnd(); ++i)
	{
		const QString line = filestr.readLine();
		if (line.startsWith("#") || line.isEmpty()) continue;

        //QLOG("Line " << i + 1 << ":");
        //QLOGINC;

		const BeoItem newItem(line);
		if (newItem.ok()) 
		{
            //QLOG("Item: " << newItem);
			addEntry(newItem);
			ok++;
		}
        else
        {
            QLOGX("Fail on line " << i + 1 << " !!!");
            QLOG("Item: " << newItem);
            fail++;
        }

        //QLOGDEC;
	}

	QLOG("Parse completed, ok = " << ok << ", fail = " << fail);

	QLOGDEC;
}

QString BeoDict::resultText(const BeoLanguage lang, const BeoResult &result) const
{
	// only the phrase index may be uninitialized in the result object
	Q_ASSERT(result.itemFound() && result.defFound());

	const int
		item = result.item,
		def = result.def,
		phrase = result.phrase;

	// if the phrase index is valid, return the text for that specific phrase
	if (result.phraseFound()) switch(lang)
	{
		case BEO_DE: return entry(item).definition(def).dePhrase(phrase).text(); break;
		case BEO_EN: return entry(item).definition(def).enPhrase(phrase).text(); break;
		case BEO_NONE: break;
	}
	// otherwise return the merged text of all phrases in the parent definition
	else switch(lang)
	{
		case BEO_DE: return entry(item).definition(def).deText(); break;
		case BEO_EN: return entry(item).definition(def).enText(); break;
		case BEO_NONE: break;
	}

	return QString();
}

QList<BeoResult> BeoDict::search(const BeoLanguage lang, const QStringList &query, const MatchType type) const
{
	QList<BeoResult> ret;

	for (int i = 0; i < entryCount(); ++i)
	{
		const BeoItem& item = entry(i);
		BeoResult match = item.match(lang, query, type);
		if (match.phraseFound() && match.defFound())
		{
			QLOGX("Matching item: " << item);
			match.item = i;
			ret.append(match);
		}
	}

	return ret;
}
