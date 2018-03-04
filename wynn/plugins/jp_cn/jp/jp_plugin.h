#ifndef JP_PLUGIN_H
#define JP_PLUGIN_H

#include "dict_plugin.h"
#include "../jp-cn_base.h"

#include <QObject>
#include <QtPlugin>
#include <QFont>
#include <QAbstractTableModel>

class JpPlugin : public JpCnPluginBase
{
	Q_OBJECT

	// Q_PLUGIN_METADATA not supported before Qt5
#if QT_VERSION >= 0x050000 
	Q_PLUGIN_METADATA(IID "wynn.JpPlugin")
#endif

	Q_INTERFACES(DictionaryPlugin)

protected:
    static const QString DICT_WORD;
	JmDict wordDict_;

public:
	JpPlugin();
	virtual ~JpPlugin();

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
	virtual QStringList wordSearchQuery(const QString &arg) const;
};

#endif // JP_PLUGIN_H
