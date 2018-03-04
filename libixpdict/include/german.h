#ifndef IXP_DICT_GERMAN_H
#define IXP_DICT_GERMAN_H

#include "ixpdict_global.h"
#include "dictionary_store.h"
#include "util.h"

#include <QString>
#include <QTextStream>

enum BeoLanguage { BEO_NONE, BEO_DE, BEO_EN };

struct BeoResult
{
	int item, def, phrase;

	BeoResult() : item(-1), def(-1), phrase(-1) {}
	BeoResult(int itemVal, int defVal, int phraseVal) : item(itemVal), def(defVal), phrase(phraseVal) {}

	bool phraseFound() const { return (phrase >= 0); }
	bool defFound() const { return (def >= 0); }
	bool itemFound() const { return (item >= 0); }
	bool valid() const { return (phraseFound() && defFound() && itemFound()); }

	BeoResult allPhrases() const { return BeoResult(item, def, -1); }
};

LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const BeoResult &item);

class LIBIXPDICT_EXPORT BeoPhrase
{
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoPhrase &item);

private:
	QString text_, core_;

public:
	BeoPhrase() {}
	BeoPhrase(const BeoLanguage lang, const QString &text);

	const QString& text() const { return text_; }
	const QString& core() const { return core_; }
    bool ok() const { return !text_.isEmpty() && !core_.isEmpty(); }
	bool match(const QStringList &arg, const MatchType type) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const BeoPhrase &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoPhrase &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const BeoPhrase &item);

class LIBIXPDICT_EXPORT BeoDefinition
{
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoDefinition &item);

private:
	QList<BeoPhrase> de_, en_;

public:
	BeoDefinition() {}
	BeoDefinition(const QString &deText, const QString &enText);

    bool ok() const { return !de_.empty() && !en_.empty(); }
	const QList<BeoPhrase>& dePhrases() const { return de_; }
	const QList<BeoPhrase>& enPhrases() const { return en_; }
	int dePhraseCount() const { return de_.size(); }
	int enPhraseCount() const { return en_.size(); }
	const BeoPhrase& dePhrase(const int index) const { return de_.at(index); }
	const BeoPhrase& enPhrase(const int index) const { return en_.at(index); }
	QString deText() const;
	QString enText() const;

	BeoResult match(const BeoLanguage lang, const QStringList &arg, const MatchType type) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const BeoDefinition &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoDefinition &item);
//LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const BeoDefinition &item);

class LIBIXPDICT_EXPORT BeoItem
{
	friend LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoItem &item);

private:
	QList<BeoDefinition> defs_;

public:
	BeoItem() {}
	BeoItem(const QString &text);

	bool ok() const { return !defs_.isEmpty(); }
	const QList<BeoDefinition>& definitions() const { return defs_; }
	int definitionCount() const { return defs_.size(); }
	const BeoDefinition& definition(const int index) const { return defs_.at(index); }

	BeoResult match(const BeoLanguage lang, const QStringList &arg, const MatchType type) const;
};

LIBIXPDICT_EXPORT QDataStream& operator<<(QDataStream &stream, const BeoItem &item);
LIBIXPDICT_EXPORT QDataStream& operator>>(QDataStream &stream, BeoItem &item);
LIBIXPDICT_EXPORT QTextStream& operator<<(QTextStream &stream, const BeoItem &item);

class LIBIXPDICT_EXPORT BeoDict : public Dictionary<BeoItem>
{
public:
	BeoDict() : Dictionary<BeoItem>() {}

	QString resultText(const BeoLanguage lang, const BeoResult &result) const;
	QList<BeoResult> search(const BeoLanguage lang, const QStringList &query, const MatchType type) const;

	void parse(const QString &path);
};

#endif // IXP_DICT_GERMAN_H
