#ifndef IXP_DICT_MANDARIN_H
#define IXP_DICT_MANDARIN_H

#include <QStringList>
#include <QDataStream>
#include <QTextStream>

#include "ixpdict_global.h"
#include "dictionary_store.h"
#include "util.h"

/// A Chinesese (Mandarin) word.
class LIBIXPDICT_EXPORT CnWord
{
	friend LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const CnWord &kanji);
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, CnWord &kanji);

private:
	QString traditional_, simplified_, pinyin_;
	QStringList definitions_;

public:
	CnWord();
	CnWord(const QString &traditional, const QString &simplified, const QString &pinyin, 
		   const QStringList &definitions);

	const QString& traditional() const { return traditional_; }
	const QString& simplified() const { return simplified_; }
	const QString expressionStr() const { return simplified_ 
		+ (traditional_.isEmpty()  || (traditional_ == simplified_)? "" : " (" + traditional_ + ")"); }
	const QString& pinyin() const { return pinyin_; }
	const QString definitionStr() const { return Util::strListMerge(definitions_, "; "); }

	bool expressionMatches(const QString &arg, MatchType match) const;
	bool pinyinMatches(const QString &arg, MatchType match) const;
	bool definitionMatches(const QString &arg, MatchType match) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const CnWord &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, CnWord &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const CnWord &item);


/// The Cedict Mandarin Chinese dictionary
class LIBIXPDICT_EXPORT Cedict : public Dictionary<CnWord>
{
public:
	Cedict() : Dictionary<CnWord>() {}
	~Cedict();

	void parse(const QString &path);

	QList<int> findMandarin(const QString &arg, MatchType match) const;
	QList<int> findEnglish(const QString &arg, MatchType match) const;
};

#endif // IXP_DICT_MANDARIN_H