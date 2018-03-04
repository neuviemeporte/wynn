#ifndef LIBCJDICT_UTIL_H
#define LIBCJDICT_UTIL_H

#include <QString>
#include <QStringList>
#include <QChar>

#include "ixpdict_global.h"

/// Indicates a requested expression search match type in a dictionary word search.
enum MatchType { NO_MATCH = -1, EXACT, STARTS, ENDS, CONTAINS };
/// Indicates a requested stroke search criterium in a character search.
enum StrokeMode { NO_STROKE = -1, EQUALS, MORE, LESS };
/// Indicates the kana type.
enum KanaType { NO_KANA = -1, HIRAGANA, KATAKANA };

class LIBIXPDICT_EXPORT Util
{
protected:
	static const std::string ROMAJI_TABLE[];
	static const size_t ROMAJI_COUNT;
	static const std::wstring HIRAGANA_TABLE[];
	static const std::wstring KATAKANA_TABLE[];
	static const std::wstring PINYIN_TABLE[];

public:
	enum SearchMode { KANA, PINYIN, ROMAJI };

	static QString strListMerge(const QStringList &list, QString separator);
	static QString charListMerge(const QList<QChar> &list, QString separator);
	static bool strListContains(const QStringList &list, QString arg, SearchMode mode);
	static bool strMatch(const QString &arg, const QString &find, MatchType match);
	static int strListMatch(const QStringList &arg, const QString &find, MatchType match, const bool strInList = true);

	static QString removeFirst(const QString &input, const QString &before);

	static void convertRomaji(QStringList &output, const QString &input, const QString &accumulator, 
					   int foundIndex, KanaType type);
	static QStringList& hepburnToKana(const QString &input, KanaType kana);
	static QString pinyinToAccents(const QString& arg);
	static QString pinyinNoDigits(const QString& arg);

	static bool isHiragana(const QChar& character);
	static bool isKatakana(const QChar& character);
	static bool isKanji(const QChar& character);

	static QString makeUmlautsEszett(const QString &arg);

	static QString w2qstr(const std::wstring &str);
};


#endif
