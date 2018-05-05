#include "jp_plugin.h"
#include "ixplog_active.h"

QTSLogger *PLUGIN_LOGSTREAM = NULL;
#define QLOGSTREAM PLUGIN_LOGSTREAM
#define QLOGTAG "[PLUG_JP]"

// need to export plugin class like this for Qt4
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(jp_plugin, JpPlugin)
#endif

const QString JpPlugin::DICT_WORD = "dictionaries/JMDict_en.dat";

// ==============================================================================================
// ============================== JpPlugin
// ==============================================================================================

JpPlugin::JpPlugin() : JpCnPluginBase()
{
	// initialize log stream, this will be replaced by the application's stream
	// at pre-setup time.
	PLUGIN_LOGSTREAM = new QTSLogger("jp_plugin_log.txt");
	QLOGX("Constructing instance");
	QLOGINC;

	QLOG("Altering interface");

	// hide the elements for simplified/traditional character variants
	kanjiUI_.simpleHeader->hide();
	kanjiUI_.simpleFrame->hide();
	kanjiUI_.traditionalHeader->hide();
	kanjiUI_.traditionalFrame->hide();

	QLOGDEC;
}

JpPlugin::~JpPlugin()
{
	// cannot delete after redirection
	//delete PLUGIN_LOGSTREAM;
}

QString JpPlugin::resultData(const int itemIndex, const int columnIndex) const
{
	if (kanjiInTable_)
	{
		const int itemDictIndex = resultIndices_.at(itemIndex);
		const HanCharacter &entry = charDict_.entry(itemDictIndex);
		switch(columnIndex)
		{
			case 0:	 return entry.character(); break;
			case 1:	 return entry.jpReadingsStr(); break;
			case 2:	 return entry.jpMeaningsStr(); break;
		}
	}
	else if (wordsInTable_)
	{
		const int itemDictIndex = resultIndices_.at(itemIndex);
		const JpWord &entry = wordDict_.entry(itemDictIndex);
		switch(columnIndex)
		{
			case 0:	 return entry.expressionStr(); break;
			case 1:	 return entry.readingStr(); break;
			case 2:	 return entry.definitionStr(); break;
		}
	}

	return QString();
}

void JpPlugin::wordDictLoad()
{
    QLOGX("Loading Japanese word dictionary from " << DICT_WORD);
	QLOGINC;

	postMessage("Loading JMdict dictionary...");
    wordDict_.loadData(DICT_WORD);
	QLOG("Word dictionary: " << wordDict_.entryCount());

	QLOGDEC;
}

QString JpPlugin::wordDictReading(const int itemIdx) const
{
	const JpWord &item = wordDict_.entry(itemIdx);
	return item.readingStr();
}

QString JpPlugin::wordDictDefinition(const int itemIdx) const
{
	const JpWord &item = wordDict_.entry(itemIdx);
	return item.definitionStr();
}

QString JpPlugin::wordDictCategory(const int itemIdx) const
{
	const JpWord &item = wordDict_.entry(itemIdx);
	return item.categoryStr();
}

QString JpPlugin::wordDictEntry(const int itemIdx) const
{
	const JpWord &item = wordDict_.entry(itemIdx);
	return (item.expressionStr().isEmpty() ? item.readingStr() : item.expressionStr());
}

QString JpPlugin::wordDictDesc(const int itemIdx) const
{
	const JpWord &item = wordDict_.entry(itemIdx);
	return item.definitionStr() + (item.expressionStr().isEmpty() ? "" : " (" + item.readingStr() + ")");
}

QList<int> JpPlugin::wordDictSearchLang(const QStringList &arg, const MatchType mode)
{
	return wordDict_.findJapanese(arg, mode);
}

QList<int> JpPlugin::wordDictSearchNative(const QString &arg, const MatchType mode)
{
	return wordDict_.findEnglish(arg, mode);
}

QStringList JpPlugin::wordSearchQuery(const QString &arg) const
{
	QStringList kana;	
	kana.append(Util::hepburnToKana(arg, HIRAGANA));
	kana.append(Util::hepburnToKana(arg, KATAKANA));
	kana.append(arg);
	return kana;
}

