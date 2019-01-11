#include "cn_plugin.h"
#include "ixplog_active.h"

QTSLogger *PLUGIN_LOGSTREAM = NULL;
#define QLOGSTREAM PLUGIN_LOGSTREAM
#define QLOGTAG "[PLUG_CN]"

// need to export plugin class like this for Qt4
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(cn_plugin, CnPlugin)
#endif

const QString CnPlugin::DICT_WORD = "dictionaries/cedict.dat";

CnPlugin::CnPlugin() : JpCnPluginBase()
{
	PLUGIN_LOGSTREAM = new QTSLogger("cn-plugin_log.txt");
	QLOGX("Constructing instance");

	alterInterface();
}

CnPlugin::~CnPlugin()
{
	//delete PLUGIN_LOGSTREAM;
}

QString CnPlugin::description() const
{
	return QString("This is the original Mandarin language plugin for Wynn.<br><br>") +
		"Dictionary content was compiled from the following sources:" +
		"<ul>" +
		"<li>KANJIDIC2 dictionary (1)</li>" +
		"<li>RADKFILE dictionary (1)</li>" +
		"<li>CC-CEDICT dictionary (2)</li>" +
		"<li>Unihan database (3)</li>" +
		"</ul>" +
		"(1): Copyright by <a href=\"http://www.edrdg.org/\">Electronic Dictionary Research Group</a><br>" +
		"(2): Released under CC-BY-SA 3.0 by <a href=\"http://www.mdbg.net/\">MDBG</a><br>" +
		"(3): Copyright by <a href=\"http://unicode.org/\">Unicode Inc.</a><br><br>" +
		"Plugin (C) 2013 Darek Szczepanek <a href=\"mailto:szczepanek.d@gmail.com\">&lt;szczepanek.d@gmail.com&gt;</a><br>" +
		"Plugin code released under the terms of <a href=\"http://www.gnu.org/licenses/gpl-2.0-standalone.html\">GPLv2</a>.";
}

QString CnPlugin::resultData(const int itemIndex, const int columnIndex) const
{
	if (kanjiInTable_)
	{
		const int itemDictIndex = resultIndices_.at(itemIndex);
		const HanCharacter &entry = charDict_.entry(itemDictIndex);
		switch(columnIndex)
		{
			case 0:	 return entry.character(); break;
			case 1:	 return Util::pinyinToAccents(entry.pinyinStr()); break;
			case 2:	 return entry.cnMeaningsStr(); break;
		}
	}
	else if (wordsInTable_)
	{
		const int itemDictIndex = resultIndices_.at(itemIndex);
		const CnWord &entry = wordDict_.entry(itemDictIndex);
		switch(columnIndex)
		{
			case 0:	return entry.expressionStr(); break;
			case 1:	return Util::pinyinToAccents(entry.pinyin()); break;
			case 2:	return entry.definitionStr(); break;
		}
	}

	return QString();
}

void CnPlugin::wordDictLoad()
{
    QLOGX("Loading Mandarin word dictionary from " << DICT_WORD);
	QLOGINC;

    postMessage("Loading CEDICT dictionary...");
    wordDict_.loadData(DICT_WORD);
	QLOG("Word dictionary: " << wordDict_.entryCount());

	QLOGDEC;
}

QString CnPlugin::wordDictReading(const int itemIdx) const
{
	const CnWord &item = wordDict_.entry(itemIdx);
	return Util::pinyinToAccents(item.pinyin());
}

QString CnPlugin::wordDictDefinition(const int itemIdx) const
{
	const CnWord &item = wordDict_.entry(itemIdx);
	return item.definitionStr();
}

QString CnPlugin::wordDictCategory(const int itemIdx) const
{
	Q_UNUSED(itemIdx);
	return QString();
}

QString CnPlugin::wordDictEntry(const int itemIdx) const
{
	const CnWord &item = wordDict_.entry(itemIdx);
	return item.expressionStr();
}

QString CnPlugin::wordDictDesc(const int itemIdx) const
{
	return wordDictDefinition(itemIdx);
}

QList<int> CnPlugin::wordDictSearchLang(const QStringList &arg, const MatchType mode)
{
	Q_ASSERT(arg.size() == 1);
	const QString query = arg.front();
	return wordDict_.findMandarin(query, mode);
}

QList<int> CnPlugin::wordDictSearchNative(const QString &arg, const MatchType mode)
{
	return wordDict_.findEnglish(arg, mode);
}

QList<int> CnPlugin::charSearch(const QString &read, const QString &mean, const QString &radical, const int strokeCount, const StrokeMode strokeMode) const
{
	return charDict_.findCharacters(LANG_CN, read, mean, radical, strokeCount, strokeMode);
}

// override from base to provide chinese-specific character details
void CnPlugin::fillKanjiDetails(const HanCharacter &item)
{
	kanjiUI_.simpleLabel->setText(item.simplifiedStr().isEmpty() ? "N/A" : item.simplifiedStr());
	kanjiUI_.traditionalLabel->setText(item.traditionalStr().isEmpty() ? "N/A" : item.traditionalStr());
	kanjiUI_.detReadLabel->setText(Util::pinyinToAccents(item.pinyinStr()));
	kanjiUI_.detDefLabel->setText(item.cnMeaningsStr());
}

// do small alterations to the inherited interface of the Japanese plugin which are supposed to be
// different for Mandarin.
void CnPlugin::alterInterface()
{
	// change caption on the radio button for searching in Japanese
	dictUI_.wordSearchLangRadio->setText("Mandarin");

	// hide category label and info frame on word details dialog
	wordUI_.catLabel->hide();
	wordUI_.catFrame->hide();
}

