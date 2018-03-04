#include "util.h"

#include <ixplog_inactive.h>
#include <stdexcept>

const std::string Util::ROMAJI_TABLE[] = { 
    "a",    "i", "u", "e", "o", 
    "ka",  "ki", "ku", "ke", "ko", 
    "sa", "shi", "su", "se", "so",
    "ta", "chi", "tsu", "te", "to",
    "na",  "ni", "nu", "ne", "no",
    "ha", "hi", "fu", "he", "ho",
    "ma", "mi", "mu", "me", "mo",
    "ya", "yu", "yo",
    "ra", "ri", "ru", "re", "ro",
    "wa", "wo", "n", 
    "ga", "gi", "gu", "ge", "go",
    "za", "ji", "zu", "ze", "zo",
    "da", "ji", "zu", "de", "do",
    "ba", "bi", "bu", "be", "bo",
    "pa", "pi", "pu", "pe", "po",
    "kya", "kyu", "kyo", 
    "sha", "shu", "sho",
    "cha", "chu", "cho",
    "nya", "nyu", "nyo",
    "hya", "hyu", "hyo",
    "mya", "myu", "myo",
    "rya", "ryu", "ryo",
    "gya", "gyu", "gyo",
    "ja", "ju", "jo",
    "ja", "ju", "jo",
    "bya", "byu", "byo",
    "pya", "pyu", "pyo",
    "q", "x"           // 'q' is a substitute for 'small -tsu', 'x' for katakana chouon
};

const size_t Util::ROMAJI_COUNT = sizeof(ROMAJI_TABLE) / sizeof(std::string);

const std::wstring Util::HIRAGANA_TABLE[] = {
    L"\u3042", L"\u3044", L"\u3046", L"\u3048", L"\u304a", 
    L"\u304b", L"\u304d", L"\u304f", L"\u3051", L"\u3053", 
    L"\u3055", L"\u3057", L"\u3059", L"\u305b", L"\u305d",
    L"\u305f", L"\u3061", L"\u3064", L"\u3066", L"\u3068",
    L"\u306a", L"\u306b", L"\u306c", L"\u306d", L"\u306e",
    L"\u306f", L"\u3072", L"\u3075", L"\u3078", L"\u307b",
    L"\u307e", L"\u307f", L"\u3080", L"\u3081", L"\u3082",
    L"\u3084", L"\u3086", L"\u3088",
    L"\u3089", L"\u308a", L"\u308b", L"\u308c", L"\u308d",
    L"\u308f", L"\u3092", L"\u3093",
    L"\u304c", L"\u304e", L"\u3050", L"\u3052", L"\u3054",
    L"\u3056", L"\u3058", L"\u305a", L"\u305c", L"\u305e",
    L"\u3060", L"\u3062", L"\u3065", L"\u3067", L"\u3069",
    L"\u3070", L"\u3073", L"\u3076", L"\u3079", L"\u307c",
    L"\u3071", L"\u3074", L"\u3077", L"\u307a", L"\u307d",
    L"\u304d\u3083", L"\u304d\u3085", L"\u304d\u3087", 
    L"\u3057\u3083", L"\u3057\u3085", L"\u3057\u3087",
    L"\u3061\u3083", L"\u3061\u3085", L"\u3061\u3087",
    L"\u306b\u3083", L"\u306b\u3085", L"\u306b\u3087",
    L"\u3072\u3083", L"\u3072\u3085", L"\u3072\u3087",
    L"\u307f\u3083", L"\u307f\u3085", L"\u307f\u3087",
    L"\u308a\u3083", L"\u308a\u3085", L"\u308a\u3087",
    L"\u304e\u3083", L"\u304e\u3085", L"\u304e\u3087",
    L"\u3058\u3083", L"\u3058\u3085", L"\u3058\u3087", 
    L"\u3062\u3083", L"\u3062\u3085", L"\u3062\u3087",
    L"\u3073\u3083", L"\u3073\u3085", L"\u3073\u3087",
    L"\u3074\u3083", L"\u3074\u3085", L"\u3074\u3087",
    L"\u3063", L""
};

const std::wstring Util::KATAKANA_TABLE[] = {
    L"\u30a2", L"\u30a4", L"\u30a6", L"\u30a8", L"\u30aa", 
    L"\u30ab", L"\u30ad", L"\u30af", L"\u30b1", L"\u30b3", 
    L"\u30b5", L"\u30b7", L"\u30b9", L"\u30bb", L"\u30bd",
    L"\u30bf", L"\u30c1", L"\u30c4", L"\u30c6", L"\u30c8",
    L"\u30ca", L"\u30cb", L"\u30cc", L"\u30cd", L"\u30ce",
    L"\u30cf", L"\u30d2", L"\u30d5", L"\u30d8", L"\u30db",
    L"\u30de", L"\u30df", L"\u30e0", L"\u30e1", L"\u30e2",
    L"\u30e4", L"\u30e6", L"\u30e8",
    L"\u30e9", L"\u30ea", L"\u30eb", L"\u30ec", L"\u30ed",
    L"\u30ef", L"\u30f2", L"\u30f3", 
    L"\u30ac", L"\u30ae", L"\u30b0", L"\u30b2", L"\u30b4",
    L"\u30b6", L"\u30b8", L"\u30ba", L"\u30bc", L"\u30be",
    L"\u30c0", L"\u30c2", L"\u30c5", L"\u30c7", L"\u30c9",
    L"\u30d0", L"\u30d3", L"\u30d6", L"\u30d9", L"\u30dc",
    L"\u30d1", L"\u30d4", L"\u30d7", L"\u30da", L"\u30dd",
    L"\u30ad\u30e3", L"\u30ad\u30e5", L"\u30ad\u30e7", 
    L"\u30b7\u30e3", L"\u30b7\u30e5", L"\u30b7\u30e7",
    L"\u30c1\u30e3", L"\u30c1\u30e5", L"\u30c1\u30e7",
    L"\u30cb\u30e3", L"\u30cb\u30e5", L"\u30cb\u30e7",
    L"\u30d2\u30e3", L"\u30d2\u30e5", L"\u30d2\u30e7",
    L"\u30df\u30e3", L"\u30df\u30e5", L"\u30df\u30e7",
    L"\u30ea\u30e3", L"\u30ea\u30e5", L"\u30ea\u30e7",
    L"\u30ae\u30e3", L"\u30ae\u30e5", L"\u30ae\u30e7",
    L"\u30b8\u30e3", L"\u30b8\u30e5", L"\u30b8\u30e7",
    L"\u30c2\u30e3", L"\u30c2\u30e5", L"\u30c2\u30e7",
    L"\u30d3\u30e3", L"\u30d3\u30e5", L"\u30d3\u30e7",
    L"\u30d4\u30e3", L"\u30d4\u30e5", L"\u30d4\u30e7",
    L"\u30c3", L"\u30fc"
};

// order:
//    macron, acute, caron, grave
// a
// e
// i
// o
// u

const std::wstring Util::PINYIN_TABLE[] = {
	L"\u0101", L"\u00E1", L"\u0103", L"\u00E0",
	L"\u0113", L"\u00E9", L"\u0115", L"\u00E8",
	L"\u012B", L"\u00ED", L"\u012D", L"\u00EC",
	L"\u014D", L"\u00F3", L"\u014F", L"\u00F2",
	L"\u016B", L"\u00FA", L"\u016D", L"\u00F9"
};

// ==============================================================================================
// ============================== various global helper functions
// ==============================================================================================

/// Concatenate strings with a provided separator between elements.
QString Util::strListMerge(const QStringList &list, QString separator)
{
	QString ret = "";
	QStringListIterator i(list);

	while (i.hasNext())
	{
		ret.append(i.next());
		if (i.hasNext()) ret.append(separator);
	}
	return ret;
}

QString Util::charListMerge(const QList<QChar> &list, QString separator)
{
	QString ret = "";
	QListIterator<QChar> i(list);

	while (i.hasNext())
	{
		ret.append(QString(i.next()));
		if (i.hasNext()) ret.append(separator);
	}
	return ret;
}

/// Test string list for match against a template.
/// This function is to be used by Kanji-objects and is optionally okurigana-aware.
/// Searching within the readings is exact, inexact within definitions.
bool Util::strListContains(const QStringList &list, QString arg, SearchMode mode)
{
	bool ret = false;

	// reading entry list, may contain okurigana-separating dot
	if (mode == KANA) 
	{
		QString entry;
		for (int i = 0; i < list.size(); ++i)
		{
			entry = list.at(i);
			entry.remove(QChar('.'), Qt::CaseInsensitive);
			if (entry == arg) { ret = true; break; }
		}
	}
	// pinyin list, no dot, convert to lowercase and try:
	// first, exact matches (when query contains tone numbers like the dictionary);
	// second, trim tone numbers from dictionary entry and compare (user searches for query without tone numbers);
	// third, convert numbers to accents and compare (user entered an accented pinyin string).
	else if (mode == PINYIN)
	{
		QString entry;
		arg = arg.toLower();
		for (int i = 0; i < list.size(); ++i)
		{
			entry = list.at(i).toLower();
			if (entry == arg || 
				Util::pinyinNoDigits(entry) == arg ||
				Util::pinyinToAccents(entry) == arg) { ret = true; break; }
		}
	}
	// romanization list, no dot, lowercase search and inexact matches
	else if (mode == ROMAJI)
	{
		QString entry;
		arg = arg.toLower();
		for (int i = 0; i < list.size(); ++i)
		{
			entry = list.at(i).toLower();
			if (entry.contains(arg)) { ret = true; break; }
		}
	}

	return ret;
}

/// Test string for match (exact or otherwise) against a template.
bool Util::strMatch(const QString &arg, const QString &find, MatchType match)
{
	bool ret = false;
	switch (match)
	{
		case EXACT:    if (arg.compare(find, Qt::CaseInsensitive) == 0) ret = true; break;
		case STARTS:   if (arg.startsWith(find, Qt::CaseInsensitive)) ret = true; break;
		case ENDS:     if (arg.endsWith(find, Qt::CaseInsensitive)) ret = true; break;
		case CONTAINS: if (arg.contains(find, Qt::CaseInsensitive)) ret = true; break;
		default: break;
	}
	return ret;
}

/// Test list of strings for match (exact or otherwise) against a template.
/// The last argment specifies whether we are matching the string ("find") against all elements of the list (default),
/// or if we are matching all elements of the list agains the string in succession.
int Util::strListMatch(const QStringList &arg, const QString &find, MatchType match, const bool strInList)
{
	if (strInList) for (int i = 0; i < arg.size(); ++i)
	{
		if (strMatch(arg.at(i), find, match)) return i;
	}
	else for (int i = 0; i < arg.size(); ++i)
	{
		if (strMatch(find, arg.at(i), match)) return i;
	}

	return -1;
}

/// Remove first ocurrence of template within input.
QString Util::removeFirst(const QString &input, const QString &before)
{
	QString ret(input);
	int idx = ret.indexOf(before);
	if (idx < 0) return ret;
	else
	{
		return ret.remove(idx, before.length());
	}
}

/// Generates a list of kana strings for a given preprocessed romaji string.
/// WARNING: the input string is consumed!
void Util::convertRomaji(QStringList &output, const QString &input, const QString &accumulator, 
				   int foundIndex, KanaType type)
{
	const std::wstring *kanaTable = NULL;

	switch (type)
	{
		case HIRAGANA: kanaTable = HIRAGANA_TABLE; break;
		case KATAKANA: kanaTable = KATAKANA_TABLE; break;
		default: return;
	}

	// Determine the kana character found in the previous pass
	QString suffix;
	if (foundIndex >=0 && foundIndex < 109) // 109 == length of tables, no access due to pointer redirection
	{
		suffix = w2qstr(kanaTable[foundIndex]);
	}

	// Append the found character to the accumulator, which will be passed to the next call to the function
	QString buffer = accumulator + suffix;

	// Input is an empty string - end of branch reached: add accumulated buffer to the output list and 
	// start return sequence
	if (input.isEmpty())
	{
		output << buffer;
		return;
	}

	// Determine which kana characters match the beginning of the current input and call this function 
	// recursively for each of them. Upon the call, the current buffer becomes the new accumulator, and
	// the current input with the matched sequence cut out becomes the new input.
    for (int i = 0; i < ROMAJI_COUNT; ++i)
	{
		const QString romaji = QString::fromStdString(ROMAJI_TABLE[i]);
		if (input.startsWith(romaji))
		{
			convertRomaji(output, removeFirst(input, romaji), buffer, i, type);
		}
	}        

	// If no kana character is matched at this point (input isn't empty), the function starts a return 
	// sequence up the tree.
	// Nothing has been added to the output list, this branch is considered invalid and rejected.
}

// Generates a list of kana strings for a user-provided unprepared romaji string.
QStringList& Util::hepburnToKana(const QString &input, KanaType kana)
{
	QString romaji(input);
	QStringList *output = new QStringList;

	if (romaji.isEmpty()) return *output;

	//debug << "Parsing romaji:" << romaji << "hiragana:" << sizeof(hiraganaTable)/sizeof(QString)
	//	<< "katakana:" << sizeof(katakanaTable)/sizeof(QString) << "romaji: "
	//	<< sizeof(romajiTable)/sizeof(QString) << "type: " << kana );

	romaji = romaji.toLower();

	// rough elimination of ill-formed strings by letters allowed in Hepburn romanization
	if (QRegExp("[aiueokstcnhfmyrwngzdjbp]+").exactMatch(romaji))
	{
		//debug << "input accepted:" << romaji );

		QRegExp rx;
		int pos;
		QString group, after;

		// replace all geminate consonants with 'q' placeholder for small 'tsu'
		rx.setPattern("kk|ss|tt|cc|hh|ff|rr|gg|zz|jj|dd|bb|pp");
		pos = 0;
		after = "q";
		while (pos >= 0)
		{
			pos = rx.indexIn(romaji, pos);
			if (pos >= 0)
			{
				group = rx.cap();
				//debug << "geminate group:" << group );
				romaji.replace(pos, 2, after + group.at(0));
			}
		}

		// replace 'm' before m,b,p consonants with 'n'
		rx.setPattern("m[mbp]");
		pos = 0;
		after = "n";
		while (pos >= 0)
		{
			pos = rx.indexIn(romaji, pos);
			if (pos >= 0)
			{
				group = rx.cap();
				//debug << "final-m group:" << group );
				romaji.replace(pos, 2, after + group.at(1));
			}
		}

		// For katakana conversion, generate additional input variant with long vowels replaced 
		// with 'x' placeholder for chouon mark
		QString romaji2 = "";
		if (kana == KATAKANA)
		{
			romaji2 = romaji;

			rx.setPattern("aa|ii|uu|ee|ou");
			pos = 0;
			after = "x";
			while (pos >= 0)
			{
				pos = rx.indexIn(romaji2, pos);
				if (pos >= 0)
				{
					group = rx.cap();
					//debug << "katakana long vovel group:" << group );
					romaji2.replace(pos, 2, QString(group.at(0)) + after);
				}
			}
		}

		//debug << "preprocessing done:"<< romaji << "(" << romaji2 << ")" );

		// parse the input using a recursive function to traverse it as a tree
		convertRomaji(*output, romaji, QString(""), -1, kana);
		if (kana == KATAKANA && romaji2 != romaji)
		{
			convertRomaji(*output, romaji2, QString(""), -1, kana);
		}

	}

	return *output;
}

// Returns a tonal diacritic equivalent of a pinyin string with numbers (e.g. "zhong1guo2").
QString Util::pinyinToAccents(const QString &arg)
{
	QLOGX("Converting '" << arg << "' to accent notation");
	QLOGINC;
	QString ret, syl, acc;
	int sylStart = 0, sylLen = 0, accPos = 0, letter;
	int tone;
	// go over the input, character by character;
	for (int i = 0; i < arg.size(); ++i)
	{
		const QChar &c = arg[i];
		// found tone number
		if (c.isDigit())
		{
			// extract current sylable, "sylLen" characters long from "sylStart" (without the tone number)
			// some syllables contain capitals, not sure why but will convert to lower to avoid the trouble of adding capital
			// variants to the accent table
			syl = arg.mid(sylStart, sylLen).toLower(); 
			tone = QChar::digitValue(c.unicode());
			QLOG("Tonal mark, value: " << tone << ", sylable: '" << syl << "'");
			QLOGINC;
			// skip bad tones silently
			if (tone < 1 || tone > 5) 
			{
				QLOGDEC;
				continue; 
			}

			// on tone 5 (no mark), append sylable as is and continue with next (if present)
			if (tone == 5)
			{
				ret+= syl;
				QLOGDEC;
				continue;
			}

			// The algorithm is borrowed from Wikipedia (en) entry on "Pinyin"
			// 1. 'a' or 'e' in sylable, grab index and replace with accent variant;
			// 2. 'ou' in syllable, put accent over 'o'
			// 3. otherwise place accent over second vowel (??)
			if (((accPos = syl.indexOf('a', 0, Qt::CaseInsensitive)) >= 0) ||
				((accPos = syl.indexOf('e', 0, Qt::CaseInsensitive)) >= 0) ||
				((accPos = syl.indexOf('o', 0, Qt::CaseInsensitive)) >= 0) ||
				((accPos = syl.indexOf('i', 0, Qt::CaseInsensitive)) >= 0) ||
				((accPos = syl.indexOf('u', 0, Qt::CaseInsensitive)) >= 0))
			{
				// start from a-row, e-row or o-row in table?
				if		(syl[accPos] == 'a') letter = 0;
				else if (syl[accPos] == 'e') letter = 4;
				else if (syl[accPos] == 'o') letter = 12;
				else if (syl[accPos] == 'i') letter = 8;
				else if (syl[accPos] == 'u') letter = 16;
                else throw std::logic_error("Unexpected pinyin vowel");
				QLOG("Found '" << syl[accPos] << "' in sylable, substituting with " << letter << " + " << tone << " from table");
				acc = w2qstr(PINYIN_TABLE[letter + tone - 1]);
				QLOG("Retrieved: '" << acc << "'");
				// replace old character with accent character
				syl[accPos] = acc.at(0);
			}
			ret += syl; // append processed sylable to output

			sylStart = i+1; // start next sylable at character after the tone number
			sylLen = 0;
			QLOGDEC;
			continue;
		}
		sylLen++;
	}
	QLOGDEC;
	return ret;
}

/// Eliminate tone numbers from pinyin string.
QString Util::pinyinNoDigits(const QString& arg)
{
	QString ret;

	for (int i = 0; i < arg.size(); ++i)
	{
		if (arg[i].isDigit()) continue;
		ret.append(arg[i]);
	}

	return ret;
}

//U+3040 - 309f: hiragana
//U+30a0 - 30ff: katakana
//U+4e00 - 9fff: cjk unified ideographs
bool Util::isHiragana(const QChar& character)
{
	return (character.unicode() >= 0x3040 && character.unicode() <= 0x309f);
}

bool Util::isKatakana(const QChar& character)
{
	return (character.unicode() >= 0x30a0 && character.unicode() <= 0x30ff);
}

bool Util::isKanji(const QChar& character)
{
	ushort code = character.unicode();
	return ((code >= 0x4e00 && code <= 0x9fff) || // CJK unified ideographs
			(code >= 0x3400 && code <= 0x4dbf));	  // CJK extension A
}

// replace 'ae', 'ue', 'oe' with valid umlauts, also replace the number '3' with eszett (scharfes s)
QString Util::makeUmlautsEszett(const QString &arg)
{
	QString ret = arg;
	ret.replace("ae", w2qstr(L"\u00E4"));
	ret.replace("ue", w2qstr(L"\u00FC"));
	ret.replace("oe", w2qstr(L"\u00F6"));
	ret.replace("3",  w2qstr(L"\u00DF"));
	return ret;
}

QString Util::w2qstr(const std::wstring &str)
{
#ifdef _MSC_VER
    return QString::fromUtf16((const ushort *)str.c_str());
#else
    return QString::fromStdWString(str);
#endif
}
