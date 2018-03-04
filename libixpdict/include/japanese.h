#ifndef IXP_DICT_JAP_H
#define IXP_DICT_JAP_H

#include "ixpdict_global.h"
#include "dictionary_store.h"
#include "util.h"

#include <QStringList>
#include <QDataStream>
#include <QTextStream>

/// A Japanese word.
class LIBIXPDICT_EXPORT JpWord
{
	friend LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const JpWord &item);
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, JpWord &item);

private:
	QStringList expressions_, readings_, definitions_, categories_;

public:
	JpWord();
	JpWord(const QStringList &expression, const QStringList &reading, 
		   const QStringList &definition, const QStringList &category);

	const QStringList& expressions() const	{ return expressions_; }
	const QStringList& readings() const { return readings_;	}
	const QStringList& definitions() const	{ return definitions_; }
	const QStringList& categories() const { return categories_; }

	const QString expressionStr() const { return Util::strListMerge(expressions_, "; "); }
	const QString readingStr() const { return Util::strListMerge(readings_, ", "); }
	const QString definitionStr() const { return Util::strListMerge(definitions_, "; "); }
	const QString categoryStr() const { return Util::strListMerge(categories_, "; "); }

	bool expressionMatches(const QString &arg, MatchType match) const;
	bool readingMatches(const QString &arg, MatchType match) const;
	bool definitionMatches(const QString &arg, MatchType match) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const JpWord &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, JpWord &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const JpWord &item);

/// The Japanese JmDict dictionary
class LIBIXPDICT_EXPORT JmDict : public Dictionary<JpWord>
{
public:
	JmDict() : Dictionary<JpWord>() {}
	~JmDict();

	void parseXML(const QString &dictPath);

	QList<int> findJapanese(const QStringList &arg, MatchType match) const;
	QList<int> findEnglish(const QString &arg, MatchType match) const;
};

#endif // IXP_DICT_JAP_H
