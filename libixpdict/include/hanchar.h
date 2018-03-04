#ifndef IXP_DICT_HANCHAR_H
#define IXP_DICT_HANCHAR_H

#include "ixpdict_global.h"
#include "dictionary_store.h"
#include "util.h"

#include <QChar>
#include <QString>
#include <QStringList>
#include <QList>
#include <QHash>
#include <QTextStream>

enum HanCharLanguage { LANG_NONE, LANG_JP, LANG_CN };

/// A Chinese character.
class LIBIXPDICT_EXPORT HanCharacter
{
	friend class Unihan;
	friend LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const HanCharacter &kanji);
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, HanCharacter &kanji);

private:
	QChar character_;
	QStringList onyomis_, kunyomis_, nanoris_, pinyins_, jpMeanings_, cnMeanings_;
	QList<QChar> simplified_, traditional_;
	int strokes_;

public:
	HanCharacter();
	HanCharacter(QChar character, const QStringList &onyomi, const QStringList &kunyomi, const QStringList &nanori, 
				 const QStringList &pinyin, const QStringList &meanings, int strokeCount);

	const QChar& character() const { return character_; }
	QString characterStr() const { return QString(character_); }
	int strokes() const	{ return strokes_; }

	const QString onyomiStr() const { return Util::strListMerge(onyomis_, ", "); }
	const QString kunyomiStr() const { return Util::strListMerge(kunyomis_, ", "); }
	const QString nanoriStr() const { return Util::strListMerge(nanoris_, ", "); }
	const QString pinyinStr() const { return Util::strListMerge(pinyins_, ", "); }
	const QString jpReadingsStr() const;
	const QString jpMeaningsStr() const { return Util::strListMerge(jpMeanings_, "; "); }
	const QString cnMeaningsStr() const { return Util::strListMerge(cnMeanings_, "; "); }
	const QString simplifiedStr() const { return Util::charListMerge(simplified_, ", "); }
	const QString traditionalStr() const { return Util::charListMerge(traditional_, ", "); }

	bool jpReadingsContain(const QString &arg) const;
	bool jpReadingsContain(const QStringList &arg) const;
	bool jpMeaningsContain(const QString &arg) const;

	bool cnReadingsContain(const QString &arg) const;
	bool cnMeaningsContain(const QString &arg) const;

	bool strokesMatch(int strokes, StrokeMode mode) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const HanCharacter &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, HanCharacter &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const HanCharacter &item);

class LIBIXPDICT_EXPORT Radical
{
	friend LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const Radical &item);
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, Radical &item);

private:
	QChar radical_;
	int strokes_;
	QList<QChar> kanjis_;

public:
	Radical();
	Radical(QChar radical, const QList<QChar> &kanjis, int strokes);

	const QChar& radical() const { return radical_; }
	const QList<QChar>& kanjis() const { return kanjis_; }
	int strokes() const { return strokes_; }
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const Radical &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, Radical &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const Radical &item);

/// A dictionary for Kanji-s.
class LIBIXPDICT_EXPORT Kanjidic : public Dictionary<HanCharacter>
{
	friend class Unihan;

public:
	Kanjidic() : Dictionary<HanCharacter>() {}
	~Kanjidic();

	void parseXML(const QString &dictPath);

	int findCharacter(QChar character) const;
	QList<int> findCharacters(const HanCharLanguage lang, const QString &reading, const QString &meaning,
							   const QString &radicals, int strokes, StrokeMode strokeMode) const;
};

/// A dictionary for Radical-s.
class LIBIXPDICT_EXPORT Radkfilex : public Dictionary<Radical>
{
public:
	Radkfilex() : Dictionary<Radical>() {}
	~Radkfilex();

	void parse(const QString &dictPath);

	QString matchingKanjis(const QList<int> &radicalIdxs) const;
	QString matchingRadicals(QChar kanji) const;
};

/// Information about a character extracted from the Unihan database.
struct LIBIXPDICT_EXPORT UnihanCharInfo
{
	UnihanCharInfo() : strokes_(0) {}

	QStringList definition_, pinyin_;
	QList<QChar> simplified_, traditional_;
	int strokes_;
};

LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const UnihanCharInfo &info);

/// A dictionary of Unihan character entries. This doesn't inherit Dictionary<T>, because it doesn't keep its contents in 
/// a QList and all the inherited functions become useless.
class LIBIXPDICT_EXPORT Unihan
{
public:
	Unihan() : error_(false) {}

	const UnihanCharInfo entry(QChar c) const { return entriesHash_[c]; }
	int hashEntryCount() const { return entriesHash_.size(); }
	bool error() const { return error_; }
	void parse();
	void updateKanjidic(Kanjidic &dictionary);
	static QList<QChar> str2char(const QString &str);

protected:
	void parseDatabase(const QString &path);

	QHash<QChar, UnihanCharInfo> entriesHash_;
	bool error_;
};

#endif // IXP_DICT_HANCHAR_H
