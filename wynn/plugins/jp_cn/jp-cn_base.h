#ifndef JP_CN_BASE_H
#define JP_CN_BASE_H

#include <QObject>
#include <QtPlugin>
#include <QFont>
#include <QAbstractTableModel>

#include "ui_panel.h"
#include "ui_settings.h"
#include "ui_kanji_dialog.h"
#include "ui_word_dialog.h"
#include "ui_radical_dialog.h"

#include "dict_plugin.h"
#include "japanese.h"
#include "hanchar.h"

// dispose of entries in kanji dictionary, for which there is no definition (look under pinyin guo2 for example)
// settings: display nanori in table readings?
// kanji reading type in table by color? in future color to settings
// disable radical table resize - not sure how yet
// omit no-definition characters from dictionaries - default-on setting in the future [DONE?]
// individual middle/big font sizes in Settings
// categories from edict displayed as short entity names, additional info on floating tooltips

// The base class for the Japanese plugin. Separated for the Mandarin plugin to inherit easily.
class JpCnPluginBase : public QObject, public DictionaryPlugin
{
	Q_OBJECT
	Q_INTERFACES(DictionaryPlugin)

protected:
	static const QString SETT_FONT, SETT_KANJI_SZ, SETT_WORD_SZ, SETT_COLW_0, SETT_COLW_1, SETT_COLW_2;
    static const QString DICT_CHARACTER, DICT_RADICAL;

	QWidget *app_;
	Ui::PanelUI dictUI_;
	Ui::SettingsUI settUI_;
	Ui::KanjiDialogUI kanjiUI_;
	Ui::WordDialogUI wordUI_;
	Ui::RadicalDialogUI radicalUI_;

	QDialog *kanjiDialog_, *wordDialog_, *radicalDialog_;
	QFont baseFont_, middleFont_, bigFont_;
	QSize kanjiDialogSize_, wordDialogSize_;
	
	Kanjidic charDict_;
	QList<int> resultIndices_;
	QStringList kanjiHeader_, wordHeader_;
    QList<QLabel*> wordLabels_;

	Radkfilex radicalDict_;
	QList<QPushButton*> radicalButtons_;
	QList<bool> radicalStates_;

	bool kanjiInTable_, wordsInTable_;
	int col0Width_, col1Width_, col2Width_;

public:
	JpCnPluginBase();
	virtual ~JpCnPluginBase();

	virtual QString name() const { return QString("Wynn Japanese plugin v1.0"); }
	virtual QString language() const { return QString("Japanese"); }
	virtual QString description() const;
	virtual QFont font() const { return baseFont_; }

	virtual int resultItemCount() const { return resultIndices_.size(); }
	virtual int resultColumnCount() const { return 3; }
	virtual QString resultHeaderData(const int column) const;

	virtual void showResultDetails(const int resultIndex);
	virtual QStringList getResultForDatabase(const int resultIndex);

	virtual void preSetup(QWidget *parent, class QTSLogger *debugLog);
	virtual void setup();
	virtual void postSetup();
	virtual void activate();

signals:
	void searchStart();
	void searchDone();
	void message(const QString &text);
	void columnWidthsNotify(const QList<int> &widths);

public slots:
	void showKanjiDetails(const QString &kanji);
	void columnResize(int curPlugIdx, int colIdx, int oldSize, int newSize);

private slots:
	void charEditCheckToggled(bool arg);
	void strokeModeButtonClicked();
	void charSearchButtonClicked();
	void wordSearchButtonClicked();

	void kanjiAddClicked();
	void wordAddClicked();
	void kanjiCopyClicked();
	void wordCopyClicked();
	
	void radTableButtonClicked();
	void radTableClearButtonClicked();
	void radTableAccepted();
	void radTableRejected();

	void selFontButtonClicked();

protected:
	virtual void wordDictLoad() = 0;
	virtual int wordDictSize() const = 0;
	virtual bool wordDictError() const = 0;
	virtual QString wordDictReading(const int itemIdx) const = 0;
	virtual QString wordDictDefinition(const int itemIdx) const = 0;
	virtual QString wordDictCategory(const int itemIdx) const = 0;
	virtual QString wordDictEntry(const int itemIdx) const = 0;
	virtual QString wordDictDesc(const int itemIdx) const = 0;
	virtual QList<int> wordDictSearchLang(const QStringList &arg, const MatchType mode) = 0;
	virtual QList<int> wordDictSearchNative(const QString &arg, const MatchType mode) = 0;
	virtual QStringList wordSearchQuery(const QString &arg) const { return QStringList(arg); }
	virtual QList<int> charSearch(const QString &read, const QString &mean, const QString &radical, const int strokeCount, const StrokeMode strokeMode) const;

	void postMessage(const QString &msg);
	void setupDictionaries();
	void setupRadicalsDialog();

	virtual void saveSettings();
	virtual void loadSettings();
	void applyFontChange();

	void popKanjiDetailsDialog(const int itemIdx);
	void popWordDetailsDialog(const int itemIdx);
	virtual void fillKanjiDetails(const HanCharacter &item);

	QHBoxLayout* hanStringToLayout(const QString &han);
};

class WordLabel : public QLabel
{
	Q_OBJECT
public:
	WordLabel(const QFont &font);
	virtual ~WordLabel();
};

class KanjiLabel : public WordLabel
{
	Q_OBJECT
public:
	KanjiLabel(QObject *parent, const QFont &font);
	virtual ~KanjiLabel();

	virtual void enterEvent(QEvent *e);
	virtual void leaveEvent(QEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

signals:
	void clicked(const QString &contents);
};

#endif // JP_CN_BASE_H
