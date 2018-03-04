#ifndef CN_PLUGIN_H
#define CN_PLUGIN_H

#include "dict_plugin.h"
#include "../jp-cn_base.h"
#include "mandarin.h"

// TODO:
// sth is screwed up with merging of traditional and simplified variants in the character dictionary

class CnPlugin: public JpCnPluginBase
{
	Q_OBJECT

#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "wynn.CnPlugin")
#endif

	Q_INTERFACES(DictionaryPlugin)

protected:
    static const QString DICT_WORD;
	Cedict wordDict_;

public:
	CnPlugin();
	virtual ~CnPlugin();

	virtual QString name() const { return QString("Wynn Mandarin plugin v1.0"); }
	virtual QString language() const { return QString("Mandarin"); }
	virtual QString description() const;

	virtual QString resultData(const int itemIndex, const int columnIndex) const;

protected:
	virtual void wordDictLoad();
	virtual int wordDictSize() const { return wordDict_.entryCount(); }
	virtual bool wordDictError() const { return wordDict_.error(); }
	virtual QString wordDictReading(const int itemIdx) const;
	virtual QString wordDictDefinition(const int itemIdx) const;
	virtual QString wordDictCategory(const int itemIdx) const;
	virtual QString wordDictEntry(const int itemIdx) const;
	virtual QString wordDictDesc(const int itemIdx) const;
	virtual QList<int> wordDictSearchLang(const QStringList &arg, const MatchType mode);
	virtual QList<int> wordDictSearchNative(const QString &arg, const MatchType mode);
	virtual QList<int> charSearch(const QString &read, const QString &mean, const QString &radical, const int strokeCount, const StrokeMode strokeMode) const;

	virtual void fillKanjiDetails(const HanCharacter &item);

	void alterInterface();
};

#endif // CN_PLUGIN_H
