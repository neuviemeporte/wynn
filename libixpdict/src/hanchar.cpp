#include "hanchar.h"

#include <ixplog_active.h>
#include <QXmlStreamReader>
#include <QTextCodec>

#define QLOGSTREAM IXPDICT_LOGSTREAM
#define QLOGTAG "[IXP_DICT]"

// ==============================================================================================
// ============================== HanCharacter
// ==============================================================================================

HanCharacter::HanCharacter() :
strokes_(0)
{
}

HanCharacter::HanCharacter(QChar character, const QStringList &onyomi, const QStringList &kunyomi, const QStringList &nanori, 
			 const QStringList &pinyin, const QStringList &meanings, int strokeCount) :
character_(character), 
onyomis_(onyomi), kunyomis_(kunyomi), nanoris_(nanori), pinyins_(pinyin), jpMeanings_(meanings), 
strokes_(strokeCount)
{
}

const QString HanCharacter::jpReadingsStr() const
{
	// need to create temporaries to determine output string form
	const QString onyomi = onyomiStr(),
				  kunyomi = kunyomiStr(),
				  nanori = nanoriStr();

	return (onyomi + (onyomi.isEmpty() || (kunyomi.isEmpty() && nanori.isEmpty()) ? "" : "; ") + 
			kunyomi + (kunyomi.isEmpty() || nanori.isEmpty() ? "" : "; ") + nanori);
}

bool HanCharacter::jpReadingsContain(const QString &arg) const
{
	if (arg.isEmpty()) return false;
	bool ret = false;
	if (Util::strListContains(onyomis_, arg, Util::KANA)) ret = true;
	else if (Util::strListContains(kunyomis_, arg, Util::KANA)) ret = true;
	else if (Util::strListContains(nanoris_, arg, Util::KANA)) ret = true;
	else if (characterStr() == arg) ret = true;
	return ret;
}

bool HanCharacter::jpReadingsContain(const QStringList &arg) const
{
	if (arg.empty()) return false;
	for (int i=0; i < arg.count(); ++i)
	{
		const QString &entry = arg.at(i);
		if (jpReadingsContain(entry)) return true;
	}
	return false;
}

bool HanCharacter::cnReadingsContain(const QString &arg) const
{
	if (arg.isEmpty()) return false;
	if (Util::strListContains(pinyins_, arg, Util::PINYIN)) return true;
	else if (characterStr() == arg) return true;
	else return false;
}

bool HanCharacter::jpMeaningsContain(const QString &arg) const
{
	return Util::strListContains(jpMeanings_, arg, Util::ROMAJI);
}

bool HanCharacter::cnMeaningsContain(const QString &arg) const
{
	return Util::strListContains(cnMeanings_, arg, Util::ROMAJI);
}

bool HanCharacter::strokesMatch(int strokes, StrokeMode mode) const
{
	if ((mode == EQUALS && strokes_ == strokes)
		|| (mode == LESS && strokes_ < strokes)
		|| (mode == MORE && strokes_ > strokes)) return true;
	else return false;
}

QDataStream& operator<<(QDataStream &stream, const HanCharacter &item)
{
	return (stream << item.character_ << item.simplified_ << item.traditional_
		<< item.onyomis_ << item.kunyomis_ << item.nanoris_ << item.pinyins_
		<< item.jpMeanings_ << item.cnMeanings_ << item.strokes_);
}

QDataStream& operator>>(QDataStream &stream, HanCharacter &item)
{
	return (stream >> item.character_ >> item.simplified_ >> item.traditional_
		>> item.onyomis_ >> item.kunyomis_ >> item.nanoris_ >> item.pinyins_
		>> item.jpMeanings_ >> item.cnMeanings_ >> item.strokes_);
}

// ==============================================================================================
// ============================== Radical
// ==============================================================================================


Radical::Radical(QChar radical, const QList<QChar> &kanjis, int strokes) :
	radical_(radical), strokes_(strokes), kanjis_(kanjis)
{
}

Radical::Radical() :
	radical_('\0'), strokes_(0)
{
}

QDataStream& operator<<(QDataStream &stream, const Radical &item)
{
	return (stream << item.radical_ << item.strokes_ << item.kanjis_);
}

QDataStream& operator>>(QDataStream &stream, Radical &item)
{
	return (stream >> item.radical_ >> item.strokes_ >> item.kanjis_);
}

// ==============================================================================================
// ============================== Kanjidic
// ==============================================================================================

Kanjidic::~Kanjidic()
{
	QLOGX("Destroying dictionary");
}

void Kanjidic::parseXML(const QString &dictPath)
{
	// open XML file
	QFile file(dictPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		error_ = true;
		return;
	}

	// prepare temporary data storage
	QStringList on, kun, nan, pin, mean;
	QString text, name, attr;
	QChar ch = '\0';
	//QDebug debug(*logStream);
	//HanCharacter entry;
	double lineno;
	int curprogress, globprogress = 0, strokes = -1;

	// create and initialize the XML parser
	QXmlStreamReader xml;
	xml.setDevice(&file);

	//debug << "Started parsing XML file: " << file.fileName() );

	while (!xml.atEnd() && !xml.hasError())
	{
		// read next element and extract its name
		xml.readNext();
		name = xml.name().toString();

		if (xml.isStartElement())
		{
			// start of a dictionary entry, calculate the progress based on line number
			if (name == "character")
			{
				lineno = static_cast<double>(xml.lineNumber());
				//debug << "Character entry on line" << lineno );
				// the dictionary has 503839 lines, calculate percentage
				curprogress = static_cast<int>((lineno / 503839) * 100);
				// update progress if rounded integer percentage changed value
				if (curprogress > globprogress)
				{
					globprogress = curprogress;
				}
			}
			else if (name == "literal")
			{
				text = xml.readElementText();
				ch = text.at(0);
				//debug << "Literal entry:" << ch );
			}
			else if (name == "stroke_count" && strokes < 0)
			{
				text = xml.readElementText();
				strokes = text.toInt();
				//debug << "Strokes entry:" << strokes );
			}
			else if (name == "reading")
			{
				attr = xml.attributes().value("", "r_type").toString();
				text = xml.readElementText();
				//debug << "Reading entry:" << text << "attr:" << attr );
				if (attr == "ja_on") on.append(text);
				else if (attr == "ja_kun") kun.append(text);
				else if (attr == "pinyin") pin.append(text);

			}
			else if (name == "meaning" && !xml.attributes().hasAttribute("_lang")) // parse EN only
			{
				text = xml.readElementText();
				mean.append(text);
				//debug << "Meaning entry:" << text );
			}
			else if (name == "nanori")
			{
				text = xml.readElementText();
				nan.append(text);
				//debug << "Nanori entry:" << text );
			}
		}
		// end of a dictionary entry, create new HanCharacter and add it to the dictionary,
		// clear temporary storage
		else if (name == "character" && xml.isEndElement()) 
		{
			//debug << "End of Character element" );
			if (Util::isKanji(ch) && (!on.empty() || !kun.empty() || !nan.empty()) 
				&& (!mean.isEmpty()) && (strokes > 0)) addEntry(HanCharacter(ch, on, kun, nan, pin, mean, strokes));
			ch = '\0';
			on.clear();
			kun.clear();
			nan.clear();
			pin.clear();
			mean.clear();
			strokes = -1;
		}
	}

	// handle parsing errors (basically: panic)
	if (xml.hasError())
	{
		QString error = xml.errorString();
		QString msg = "Error while processing Kanjidic dictionary file!\n" + dictPath
			+ "\nXML Error: " + error + "\nHanCharacter dictionary functionality will be disabled!";
		QLOGX("Kanjidic: XML PARSE ERROR: " << error );
		// TODO: emit signal
		error_ = true;
		return;
	}

	// close the file and update dictionary item count
	file.close();
}

int Kanjidic::findCharacter(QChar character) const
{
	int result = -1;
	for (int i = 0; i < entries_.size(); ++i)
	{
		const HanCharacter &kanji = entries_.at(i);
		if (kanji.character() == character) { result = i; break; }
	}
	return result;
}

/// Search for characters matching the specified criteria. Those are: the language (Japanese or Chinese), the reading
/// (which can be written in kana, Hepburn romanization or a kanji itself), the meaning (definition), the matching of
/// radicals, and stroke count (exact or using >/< criterium). Returns a list of indices of matching characters.
QList<int> Kanjidic::findCharacters(const HanCharLanguage lang, const QString &reading, const QString &meaning,
									 const QString &radicals, int strokes, StrokeMode strokeMode) const
{
	QLOGX("Searching, lang: " << lang << ", reading: '" << reading << "', meaning: '" << meaning << "', radicals: '" << radicals 
		<< "', strokes: " << strokes << " (" << strokeMode << ")");
	QLOGINC;

	bool readActive = !reading.isEmpty(), meanActive = !meaning.isEmpty(), 
		radActive = !radicals.isEmpty(), strokeActive = (strokes > 0);

	QList<int> result;

	// generate variants for reading in Japanese mode
	QStringList readingQuery;
	if (lang == LANG_JP)
	{
		// treat input string as hepburn romanization and convert to katakana and hiragana
		readingQuery.append(Util::hepburnToKana(reading, HIRAGANA));
		readingQuery.append(Util::hepburnToKana(reading, KATAKANA));
		// Add original string to search. This is for when input is already in pinyin, a kana or a hanzi character
		readingQuery.append(reading); 
	}

	// the following search algorithm sucks so bad I ought to get some sort of reward :/

	// determine the widest search matching set based on first active test
	enum Search { READ, MEAN, RAD, STROKE } firstSearch;
	if (readActive)
	{
		QLOG("First search to try is by READING" );
		firstSearch = READ;
		for (int i = 0; i < entries_.size(); ++i) 
		{
			const HanCharacter &entry = entries_.at(i);
			if ((lang == LANG_JP && entry.jpReadingsContain(readingQuery)) || 
				(lang == LANG_CN && entry.cnReadingsContain(reading))) result.append(i);
		}
	}
	else if (meanActive)
	{
		QLOG("First search to try is by MEANING" );
		firstSearch = MEAN;
		for (int i = 0; i < entries_.size(); ++i) 
		{
			const HanCharacter &entry = entries_.at(i);
			if ((lang == LANG_JP && entry.jpMeaningsContain(meaning)) || 
				(lang == LANG_CN && entry.cnMeaningsContain(meaning))) result.append(i);
		}
	}
	else if (radActive) // WTF? Radioactive? ;)
	{
		// The "radicals" string is not supposed to be the radicals themselves, but rather
		// a list of QChar kanjis which match the current radical criteria.
		QLOG("First search to try is by RADICAL" );
		firstSearch = RAD;
		int idx;
		// find the radical-matched characters in the kanji dictionary and store their indices
		for (int i = 0; i < radicals.size(); ++i)
		{
			idx = findCharacter(radicals.at(i));
			if (idx >= 0) result.append(idx);
		}
	}
	else if (strokeActive)
	{
		QLOG("First search to try is by STROKE" );
		firstSearch = STROKE;
		for (int i = 0; i < entries_.size(); ++i) 
		{
			const HanCharacter &entry = entries_.at(i);
			if (entry.strokesMatch(strokes, strokeMode)) result.append(i);
		}
	}
	else return result;

	if (result.empty()) 
	{ 
		QLOG("Search came up empty, done" ); 
		QLOGDEC;
		return result; 
	}
	QLOG("Preliminary search yielded " << result.count() << " characters");
	QList<int> removeIndices;

	// eliminate entries from matching set based on other tests
	// todo: this can be all put in one loop, just with many ifs inside
	if (meanActive && firstSearch != MEAN)
	{
		QLOG("Narrowing search using MEANING" );
		for (int i = 0; i < result.count(); ++i)
		{
			const HanCharacter &entry = entries_.at(result.at(i));
			if (((lang == LANG_JP && !entry.jpMeaningsContain(meaning)) ||
				(lang == LANG_CN && !entry.cnMeaningsContain(meaning))) && 
				!removeIndices.contains(i)) removeIndices.append(i);
		}
	}

	if (radActive && firstSearch != RAD)
	{
		QLOG("Narrowing search using RADICAL" );
		for (int i = 0; i < result.count(); ++i)
		{
			const HanCharacter &entry = entries_.at(result.at(i));
			if (!radicals.contains(entry.character()) && !removeIndices.contains(i)) removeIndices.append(i);
		}
	}

	if (strokeActive && firstSearch != STROKE)
	{
		QLOG("Narrowing search using STROKE" );
		for (int i = 0; i < result.count(); ++i) 
		{
			const HanCharacter &entry = entries_.at(result.at(i));
			if (!entry.strokesMatch(strokes, strokeMode) && !removeIndices.contains(i)) removeIndices.append(i);
		}
	}

	QLOG("Narrowing search using language");
	for (int i = 0; i < result.size(); ++i)
	{
		const HanCharacter &entry = entries_.at(result.at(i));

		// remove characters which don't have any reading and/or meaning in the current language
		if ((lang == LANG_JP && (entry.jpReadingsStr().isEmpty() || entry.jpMeaningsStr().isEmpty())) ||
		    (lang == LANG_CN && (entry.pinyinStr().isEmpty()     || entry.cnMeaningsStr().isEmpty()))) 
			removeIndices.append(i);
	}

	QLOG("Removing " << removeIndices.count() << " characters from result due to narrowing" );
	qSort(removeIndices);
	for (int i = removeIndices.size()-1; i >=0; --i)
	{
		QLOG("Result size: " << result.size() << ", removing index " << i << ": " << removeIndices.at(i));
		result.removeAt(removeIndices.at(i));
	}

	QLOG("Done, " << result.size() << " characters remaining" );
	QLOGDEC;

	return result;
}

// ==============================================================================================
// ============================== Radkfilex
// ==============================================================================================

Radkfilex::~Radkfilex()
{
	QLOGX("Destroying dictionary");
}

void Radkfilex::parse(const QString &dictPath)
{
	QFile file(dictPath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		error_ = true;
		return;
	}

	QTextStream stream(&file);
	QString line = "";
	QStringList tokens;
	QChar radical = '\0';
	QList<QChar> kanjis;
	int strokes = 0, curprogress, chars, globprogress = 0, lineno = 0;

	// read lines from radicals file
	while (!line.isNull())
	{
		line = stream.readLine();
		lineno++;

		curprogress = static_cast<int>((lineno / 1944) * 100);
		// update progress if rounded integer percentage changed value
		if (curprogress > globprogress)
		{
			globprogress = curprogress;
		}

		if (line.startsWith('#')) continue;	// comment
		else if (line.startsWith('$'))		// start of radical element
		{
			if (radical != '\0') 
			{
				addEntry(Radical(radical, kanjis, strokes));
				kanjis.clear();
			}

			tokens = line.split(' ');
			radical = tokens.at(1).at(0);
			strokes = tokens.at(2).toInt();
		}
		else								// line of kanjis for current radical
		{
			chars = line.length();
			for (int i = 0; i < chars; ++i)	kanjis.append(line.at(i));
		}
	}

	if (radical != '\0') 
	{
		addEntry(Radical(radical, kanjis, strokes));
		kanjis.clear();
	}

}

QString Radkfilex::matchingKanjis(const QList<int> &radicalIdxs) const
{
	QString result = "";
	if (radicalIdxs.empty()) return result;
	int lastidx = radicalIdxs.at(radicalIdxs.count() - 1);

	QString radicalNos = "";
	for (int j = 0; j < radicalIdxs.count(); ++j) radicalNos += QString::number(radicalIdxs.at(j)) + " ";

	QLOGX("Calculating set of kanjis from radicals intersection, count: " 
		<< radicalIdxs.count() << " [" << radicalNos << "] , last idx: " << lastidx );
	QLOGINC;

	// extract kanjis for last radical (smallest number)
	const QList<QChar> &leastChars = entries_.at(lastidx).kanjis();
	bool charOK;
	QLOG("First match (last radical) returned " << leastChars.count() << " items" );

	// iterate over characters from last radical
	for (int i = 0; i < leastChars.count(); ++i)
	{
		charOK = true;
		const QChar &curChar = leastChars.at(i);
		// iterate over remaining radicals
		for (int j = 0; j < radicalIdxs.count() - 1; ++j)
		{
			const QList<QChar> &radicalChars = entries_.at(radicalIdxs.at(j)).kanjis();
			if (!radicalChars.contains(curChar)) 
			{ 
				charOK = false; 
				break; 
			}
		}
		if (charOK) result += curChar;
	}

	QLOG("Ready, size: " << result.count() );
	QLOGDEC;

	return result;
}

QString Radkfilex::matchingRadicals(QChar kanji) const
{
	QString result = "";
	for (int i = 0; i < entries_.count(); ++i)
	{
		const QList<QChar> &kanjis = entries_.at(i).kanjis();
		if (kanjis.contains(kanji)) result += entries_.at(i).radical();
	}
	return result;
}

// ==============================================================================================
// ============================== Unihan
// ==============================================================================================

void Unihan::parse()
{
	parseDatabase("dictionaries/Unihan/Unihan_Readings.txt");
	parseDatabase("dictionaries/Unihan/Unihan_Variants.txt");
	parseDatabase("dictionaries/Unihan/Unihan_DictionaryLikeData.txt");
}

void Unihan::updateKanjidic(Kanjidic &dictionary)
{
	QLOGX("Updating " << dictionary.entryCount() << "-item Kanjidic with Unihan info");
	UnihanCharInfo uc;
	QChar c;
	int idx;
	int updated = 0, added = 0;

	QHashIterator<QChar, UnihanCharInfo> i(entriesHash_);
	while (i.hasNext())
	{
		i.next();
		c = i.key();
		uc = i.value();
		//QLOG("Unihan character: " << c 
		//	<< ", T: " << Util::charListMerge(uc.traditional_, "|") 
		//	<< ", S: " << Util::charListMerge(uc.simplified_, "|") 
		//	<< ", pinyin: " << Util::strListMerge(uc.pinyin_, "/") 
		//	<< ", def: " << Util::strListMerge(uc.definition_, ";"));
		QLOGINC;
		idx = dictionary.findCharacter(c);
		if (idx >= 0)
		{
			HanCharacter &kc = dictionary.entry(idx);
			//QLOG("Found in Kanjidic under " << idx << " (" << kc.character() << "), updating");
			kc.traditional_ = uc.traditional_;
			kc.simplified_ = uc.simplified_;
			kc.pinyins_ = uc.pinyin_;
			kc.cnMeanings_ = uc.definition_;
			//if (kc.strokes_ != uc.strokes_)
			//{
			//	QLOG("Different stroke count (" << uc.strokes_ << ") in UH for " << kc);
			//}
			updated++;
		}
		else if (!uc.definition_.isEmpty() && !uc.pinyin_.isEmpty() && uc.strokes_ > 0)
		{
			// todo: na pewno dodawac?
			//QLOG("Not found in Kanjidic, appending");
			HanCharacter h;
			h.character_ = c;
			h.strokes_ = uc.strokes_;
			h.traditional_ = uc.traditional_;
			h.simplified_ = uc.simplified_;
			h.pinyins_ = uc.pinyin_;
			h.cnMeanings_ = uc.definition_;
			dictionary.addEntry(h);
			QLOG("Added: " << h);
			added++;
		}
		QLOGDEC;
	}
	QLOG("Updated " << updated << " entries, added " << added << " new ones, KANJIDIC now has " 
		<< dictionary.entryCount() << " entries.");
}

void Unihan::parseDatabase(const QString &path)
{
	QLOGX("Parsing database '" << path << "'");
	QLOGINC;
	QFile f(path);
	if (!f.open(QIODevice::ReadOnly))
	{
		QLOG("Unable to open file for reading!");
		error_ = true;
		QLOGDEC;
		return;
	}
	QTextStream filestr(&f);
	filestr.setCodec(QTextCodec::codecForName("UTF-8"));

	QString line, token, code, key, data;
	bool ok = true;
	QList<QChar> chars;
	QChar c;
	int i = 0, linecnt = 0, strokes;

	// iterate over file lines
	for (i = 0; !filestr.atEnd(); ++i)
	{
		line = filestr.readLine();
		if (line.startsWith("#") || line.isEmpty()) continue;

		QTextStream linestr(&line, QIODevice::ReadOnly);
		linestr.setCodec(QTextCodec::codecForName("UTF-8"));

		linestr >> code >> key;

		//QLOG("Line " << i << " - code: '" << code << "', key: '" << key << "'");
		chars = str2char(code);
		// temporarily skip invalid characters from high codes, have to fix to use surogates in the future
		if (chars.isEmpty()) 
		{
			//QLOG("Skipping entry on line " << i << " (non-critical)");
			continue; 
		}
		if (chars.size() != 1) 
		{ 
			QLOG("Bad key character string at line " << i);
			error_ = true; 
			QLOGDEC;
			return; 
		}
		c = chars.at(0);

		data.clear();
		// read the rest of the string until EOL
		while (!linestr.atEnd())
		{
			linestr >> token;
			data.append(token + " ");
		}
		data.chop(1); // remove EOL

		// TODO: convert "U+..." strings inside definition into characters
		if (key == "kDefinition") 
		{
			entriesHash_[c].definition_ = data.split(';', QString::SkipEmptyParts);
			linecnt++;
		}
		else if (key == "kMandarin") 
		{
			entriesHash_[c].pinyin_ = data.toLower().split(' ', QString::SkipEmptyParts);
			linecnt++;
		}
		else if (key == "kSimplifiedVariant") 
		{
			chars = str2char(data);
			if (c.isNull())
			{
				QLOG("Bad simplified variant string at line " << i);
				error_ = true;
				QLOGDEC;
				return;
			}
			//if (chars.size() > 1) QLOGX("At least 2 chars in simplified variant: " << chars << ", line " << i);
			entriesHash_[c].simplified_ = chars;
			linecnt++;
		}
		else if (key == "kTraditionalVariant") 
		{
			chars = str2char(data);
			if (c.isNull())
			{
				QLOG("Bad traditional variant string at line " << i);
				error_ = true;
				QLOGDEC;
				return;
			}
			//if (chars.size() > 1) QLOGX("At least 2 chars in traditional variant: " << chars << ", line " << i);
			entriesHash_[c].traditional_ = chars;
			linecnt++;
		}
		else if (key == "kTotalStrokes")
		{
			strokes = data.toInt(&ok);
			if (ok)
			{
				entriesHash_[c].strokes_ = strokes;
			}
			else
			{
				QLOG("Bad value for stroke count at line " << i);
			}
			linecnt++;
		}
	}
	QLOG("Processed " << i << " lines, used info from " << linecnt);
	QLOGDEC;
}

QList<QChar> Unihan::str2char(const QString &str)
{
	QLOGINC;
	// split input string at spaces
	QStringList chars = str.split(' ', QString::SkipEmptyParts);

	QString buf;
	bool ok;
	uint unicode;
	QChar c;
	QList<QChar> ret;

	// iterate over string parts
	for (int i = 0; i < chars.size(); ++i)
	{
		buf = chars.at(i);
		buf.remove(0,2); // remove "U+"
		unicode = buf.toUInt(&ok, 16); // get numeric value of codepoint
		if (!ok)
		{
			QLOGX("Failed code-character conversion, string: '" << str << "'");
		}
		if (unicode > 0xFFFF)
		{
			//QLOGX("Codepoint " << unicode << " outside 16 bits, abandoning");
			continue;
		}
		c = QChar(unicode); // construct character from unicode codepoint
		if (c.isNull())
		{
			QLOGX("Failed constructing character from Unicode codepoint " << unicode);
		}
		else
		{
			ret.append(c);
		}
	}
	QLOGDEC;
	return ret;
}

QTextStream& operator <<(QTextStream &stream, const HanCharacter &item)
{
    return (stream << "[" << item.character() << "] - on: (" << item.onyomiStr() << "), kun: (" << item.kunyomiStr() << "), nan: ("
		<< item.nanoriStr() << "), pin: ("	<< item.pinyinStr() << "), S: (" << item.simplifiedStr() << "), T: (" 
		<< item.traditionalStr() << "), JP: (" << item.jpMeaningsStr() << "), CN: (" << item.cnMeaningsStr() << "), strokes: " 
        << item.strokes());
}

QTextStream& operator <<(QTextStream &stream, const Radical &/*item*/)
{
	qFatal("Not implemented");
    return stream;
}

QTextStream& operator <<(QTextStream &stream, const UnihanCharInfo &/*info*/)
{
	qFatal("Not implemented");
    return stream;
}

