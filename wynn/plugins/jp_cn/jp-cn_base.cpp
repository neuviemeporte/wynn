#include <QClipboard>
#include <QSettings>
#include <QClipboard>
#include <QMouseEvent>
#include <QFontDialog>

#include "jp-cn_base.h"
#include "ixpdict.h"
#include "ixplog_active.h"

extern QTSLogger *PLUGIN_LOGSTREAM;
#define QLOGSTREAM PLUGIN_LOGSTREAM
#define QLOGTAG "[PLUG_JPCN]"

// ==============================================================================================
// ============================== JpCnPluginBase: public
// ==============================================================================================

const QString 
	JpCnPluginBase::SETT_FONT = "font",
	JpCnPluginBase::SETT_KANJI_SZ = "kanjiDialogSize", 
	JpCnPluginBase::SETT_WORD_SZ = "wordDialogSize",
	JpCnPluginBase::SETT_COLW_0 = "colWidth0", 
	JpCnPluginBase::SETT_COLW_1 = "colWidth1", 
	JpCnPluginBase::SETT_COLW_2 = "colWidth2";

const QString
    JpCnPluginBase::DICT_CHARACTER = "dictionaries/kanjidic-uh.dat",
    JpCnPluginBase::DICT_RADICAL   = "dictionaries/radkfilex.dat";

JpCnPluginBase::JpCnPluginBase() : QObject(), DictionaryPlugin(),
	app_(NULL),
	kanjiDialog_(NULL), wordDialog_(NULL), radicalDialog_(NULL),
	kanjiInTable_(false), wordsInTable_(false),
	col0Width_(0), col1Width_(0), col2Width_(0)
{
	QLOGX("Constructing instance");
	QLOGINC;

	kanjiHeader_ << tr("Character") << tr("Readings") << tr("Meanings");
	wordHeader_  << tr("Word")      << tr("Reading")  << tr("Definition");

	// setup interface for all the widgets and dialog windows
	// need to do this here and not in the constructor because widgets need to be created on the main thread
	dictWidget_ = new QWidget();
	settWidget_ = new QWidget();
	kanjiDialog_ = new QDialog();
	wordDialog_ = new QDialog();
	radicalDialog_ = new QDialog();

	QLOG("Setting up dictionary panel");
	dictUI_.setupUi(dictWidget_);

	connect(dictUI_.charReadingCheck,       SIGNAL(toggled(bool)),   this, SLOT(charEditCheckToggled(bool)));
	connect(dictUI_.charMeaningCheck,       SIGNAL(toggled(bool)),   this, SLOT(charEditCheckToggled(bool)));
	connect(dictUI_.charStrokeButton,       SIGNAL(clicked()),       this, SLOT(strokeModeButtonClicked()));
	connect(dictUI_.charRadicalsButton,     SIGNAL(clicked()),       this, SLOT(radTableButtonClicked()));
	connect(dictUI_.charReadingEdit,        SIGNAL(returnPressed()), this, SLOT(charSearchButtonClicked()));
	connect(dictUI_.charMeaningEdit,        SIGNAL(returnPressed()), this, SLOT(charSearchButtonClicked()));
	connect(dictUI_.charSearchButton,       SIGNAL(clicked()),       this, SLOT(charSearchButtonClicked()));

	connect(dictUI_.wordSearchLangRadio,    SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchEnglishRadio, SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchMatchRadio,   SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchContainRadio, SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchStartRadio,   SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchEndRadio,     SIGNAL(clicked()),       dictUI_.wordSearchQueryEdit, SLOT(setFocus()));
	connect(dictUI_.wordSearchQueryEdit,    SIGNAL(returnPressed()), this,                        SLOT(wordSearchButtonClicked()));
	connect(dictUI_.wordSearchButton,       SIGNAL(clicked()),       this,                        SLOT(wordSearchButtonClicked()));

	QLOG("Setting up settings panel");
	settUI_.setupUi(settWidget_);
	connect(settUI_.settFontButton, SIGNAL(clicked()), this, SLOT(selFontButtonClicked()));

	QLOG("Setting up kanji details dialog");
	kanjiUI_.setupUi(kanjiDialog_);
    connect(kanjiUI_.detCopyButton, SIGNAL(clicked()), this, SLOT(kanjiCopyClicked()));

	QLOG("Setting up word details dialog");
	wordUI_.setupUi(wordDialog_);
	connect(wordUI_.detCopyButton, SIGNAL(clicked()), this, SLOT(wordCopyClicked()));

	QLOGDEC;
}

JpCnPluginBase::~JpCnPluginBase()
{
	QLOGX("Destroying instance");
	QLOGINC;

	saveSettings();

	QLOG("Disposing of dialogs");
	delete kanjiDialog_;
	delete wordDialog_;
	delete radicalDialog_;

	QLOGDEC;
}

QString JpCnPluginBase::description() const
{
	return QString("This is the original Japanese language plugin for Wynn.<br><br>") +
		"Dictionary content was compiled from the following sources:" +
		"<ul>" +
		"<li>KANJIDIC2 dictionary (1)</li>" +
		"<li>JMDict dictionary (1)</li>" +
		"<li>RADKFILE dictionary (1)</li>" +
		"<li>Unihan database (2)</li>" +
		"</ul>" +
		"(1): Copyright by <a href=\"http://www.edrdg.org/\">Electronic Dictionary Research Group</a><br>" +
		"(2): Copyright by <a href=\"http://unicode.org/\">Unicode Inc.</a><br><br>" +
		"Plugin (C) 2013 Darek Szczepanek <a href=\"mailto:szczepanek.d@gmail.com\">&lt;szczepanek.d@gmail.com&gt;</a><br>" +
		"Plugin code released under the terms of <a href=\"http://www.gnu.org/licenses/gpl-2.0-standalone.html\">GPLv2</a>.<br><br>" +
		"Search info: word searches from English are performed inside the definitions provided by the JMDict dictionary using the provided " +
		"search criteria. This may sometimes cause confusion, e.g. searching for \"eat\" will yield no results when searching for English " +
		"matches using \"exact\" mode. That's because the definition text for the dictionary text for \"taberu\" is \"to eat\" and not \"eat\". " +
		"This would have been found using the \"contains\" mode (but would also find many unrelated results). Keep this in mind while searching, " +
        "prepend verbs with \"to\", and generally take a moment to think when a search comes up empty. This is a limitation stemming from the " +
        "format of the dictionary data and I have not yet figured out a way to do something about it.";
}

QString JpCnPluginBase::resultHeaderData(const int column) const
{
	Q_ASSERT(column < kanjiHeader_.size() && column < wordHeader_.size());

	if (kanjiInTable_) return kanjiHeader_.at(column);
	else if (wordsInTable_) return wordHeader_.at(column);

	return QString();
}

void JpCnPluginBase::showResultDetails(const int resultIndex)
{
	QLOGX("Showing details dialog");
	QLOGINC;

	Q_ASSERT(resultIndex >= 0 && resultIndex < resultIndices_.size());
	const int resultDictIndex = resultIndices_.at(resultIndex);
	QLOG("Dictionary index: " << resultDictIndex);
	if (kanjiInTable_)
	{
		popKanjiDetailsDialog(resultDictIndex);
	}
	else if (wordsInTable_)
	{
		popWordDetailsDialog(resultDictIndex);
	}
	QLOGDEC;
}

QStringList JpCnPluginBase::getResultForDatabase(const int resultIndex)
{
	QLOGX("Requested dictionary search result index " << resultIndex << " for database");
	QLOGINC;

	Q_ASSERT(resultIndex < resultIndices_.size());
	const int resultDictIndex = resultIndices_.at(resultIndex);
	
	QString entry, desc;

	if (kanjiInTable_)
	{
		QLOG("Table shows kanji" ); 
		const HanCharacter &item = charDict_.entry(resultDictIndex);
		entry = item.character();
		desc = item.jpMeaningsStr() + (item.jpReadingsStr().isEmpty() ? "" : " (" + item.jpReadingsStr() + ")");
	}
	// current dictionary search result shows words
	else if (wordsInTable_)
	{
		QLOG("Table shows words" ); 
		entry = wordDictEntry(resultDictIndex);
		desc = wordDictDesc(resultDictIndex);
	}

	QLOG("Established entry: " << entry << ", desc: " << desc);
	QStringList ret;
	ret << entry << desc;

	QLOGDEC;
	return ret;
}

void JpCnPluginBase::preSetup(QWidget *parent, class QTSLogger *debugLog)
{
	Q_ASSERT(ok_ && parent);

    if (debugLog)
    {
        QLOG("Redirecting debugging output to application log");
        PLUGIN_LOGSTREAM = debugLog;
        IXPDICT_LOGREDIR(debugLog); // todo: mutex?
    }

	QLOGX("Performing plugin pre-setup");
    QLOGINC;

	app_ = parent;

	// need to do it here rather than the constructor, because we need the vfptr table initialized 
	// for it to work for CnPlugin. Also, easier to have its output in the main log.
	loadSettings();

	// reparent dialogs to the main app window, otherwise they won't inherit the icon
	// todo: still need to do sth about hanzi labels not receiving mouse events at the beggining
	Qt::WindowFlags flags = parent->windowFlags();
	kanjiDialog_->setParent(parent, flags);
	wordDialog_->setParent(parent, flags);

	// connect signals to and from application
    connect(this,                  SIGNAL(searchStart()),                         parent, SLOT(slot_dict_searchStart()));
    connect(this,                  SIGNAL(searchDone()),                          parent, SLOT(slot_dict_searchDone()));
    connect(this,                  SIGNAL(message(const QString &)),              parent, SLOT(slot_updateSetupMsg(const QString &)));
    connect(this,                  SIGNAL(columnWidthsNotify(const QList<int>&)), parent, SLOT(slot_dict_resizeColumns(const QList<int>&)));
    connect(parent,                SIGNAL(dictColumnResized(int, int, int, int)), this,   SLOT(columnResize(int, int, int, int)));
    connect(kanjiUI_.detAddButton, SIGNAL(clicked()),                             parent, SLOT(slot_dict_toDbaseClicked()));
    connect(wordUI_.detAddButton,  SIGNAL(clicked()),                             parent, SLOT(slot_dict_toDbaseClicked()));

    QLOGDEC;
}

void JpCnPluginBase::setup()
{
	if (!ok()) return;

	QLOGX("Setting up plugin");
	QLOGINC;

	setupDictionaries();

	QLOGDEC;
}

void JpCnPluginBase::postSetup()
{
	if (!ok()) return;

	QLOGX("Carrying out additional setup tasks");
	QLOGINC;

	if (charDict_.error())
	{
		QLOG("Character dictionary error");
		dictUI_.kanjiGroupBox->setEnabled(false);
	}
	else
	{
		// figure out highest stroke count in kanji dictionary and fill appropriate combobox in gui
		int s = 0;
		for (int i = 0; i < charDict_.entryCount(); ++i)
		{
			const HanCharacter &han = charDict_.entry(i);
			if (han.strokes() > s) s = han.strokes();
		}
		QLOG("Found highest stroke count: " << s);
		dictUI_.charStrokeSpin->setMinimum(1);
		dictUI_.charStrokeSpin->setMaximum(s);
	}

	if (radicalDict_.error())
	{
		QLOG("Radical dictionary error");
		dictUI_.charRadicalsCheck->setEnabled(false);
		dictUI_.charRadicalsButton->setEnabled(false);
	}
	else setupRadicalsDialog();

	if (wordDictError())
	{
		QLOG("Word dictionary error");
		dictUI_.wordGroupBox->setEnabled(false);
	}

	QLOGDEC;
}

void JpCnPluginBase::activate()
{
	QLOGX("Activating plugin");
	QLOGINC;

	QLOG("Sending column widths: " << col0Width_ << "/" << col1Width_ << "/" << col2Width_);
	QList<int> widths;
	widths << col0Width_ << col1Width_ << col2Width_;
	emit columnWidthsNotify(widths);

	QLOGDEC;
}

// ==============================================================================================
// ============================== JpCnPluginBase: public slots
// ==============================================================================================

void JpCnPluginBase::showKanjiDetails(const QString &character)
{
	if (character.isEmpty()) return;
	QLOGX("HanCharacter details requested slot invoked, contents: '" << character << "'" );
	QLOGINC;
	Q_ASSERT(character.size() == 1);
	QChar ch = character.at(0);
	const int idx = charDict_.findCharacter(ch);
	QLOG("Found index: " << idx );
	if (idx >= 0)
	{
		popKanjiDetailsDialog(idx);
	}
	QLOGDEC;
}

void JpCnPluginBase::columnResize(int curPlugIdx, int colIdx, int oldSize, int newSize)
{
	if (curPlugIdx != index_) return;

	QLOGX("Resizing column " << colIdx << " from " << oldSize << " to " << newSize);
	switch(colIdx)
	{
		case 0: col0Width_ = newSize; break;
		case 1: col1Width_ = newSize; break;
		case 2: col2Width_ = newSize; break;
	}
}

// ==============================================================================================
// ============================== JpCnPluginBase: private slots
// ==============================================================================================

void JpCnPluginBase::charEditCheckToggled(bool arg)
{
	if (!arg) return;

	QCheckBox *sender = static_cast<QCheckBox *>(QObject::sender());
	if (sender->objectName() == "charReadingCheck")
	{
		dictUI_.charReadingEdit->setFocus();
	}
	else if (sender->objectName() == "charMeaningCheck")
	{
		dictUI_.charMeaningEdit->setFocus();
	}
}

void JpCnPluginBase::strokeModeButtonClicked()
{
	const QString text = dictUI_.charStrokeButton->text();

	if      (text == "=") dictUI_.charStrokeButton->setText(">");
	else if (text == ">") dictUI_.charStrokeButton->setText("<");
	else if (text == "<") dictUI_.charStrokeButton->setText("=");
}

void JpCnPluginBase::charSearchButtonClicked()
{
	QLOGX("Character dictionary search");
	QLOGINC;

	if (!dictUI_.charReadingCheck->isChecked()  && !dictUI_.charMeaningCheck->isChecked() &&
		!dictUI_.charRadicalsCheck->isChecked() && !dictUI_.charStrokeCheck->isChecked())
	{
		QLOG("No active search criteria, aborting");
		QLOGDEC;
		return;
	}

	// extract data for reading and meaning criteria
	QString read = dictUI_.charReadingCheck->isChecked() ? dictUI_.charReadingEdit->text() : "";
	QString mean = dictUI_.charMeaningCheck->isChecked() ? dictUI_.charMeaningEdit->text() : "";
	
	// obtain matching characters for radical search criterium
	QString radi = "";
	if (dictUI_.charRadicalsCheck->isChecked())
	{
		// create a list of indices of enabled radicals
		QList<int> radidx;
		for (int i = 0; i < radicalStates_.count(); ++i)
		{
			if (radicalStates_.at(i)) radidx.append(i);
		}
		radi = radicalDict_.matchingKanjis(radidx);
	}

	// extract data for stroke count criterium
	QString stroketext = dictUI_.charStrokeButton->text();
    StrokeMode stroke = NO_STROKE;
	if (stroketext == "=") stroke = EQUALS;
	else if (stroketext == ">") stroke = MORE;
	else if (stroketext == "<") stroke = LESS;
	int strokeno = dictUI_.charStrokeCheck->isChecked() ? dictUI_.charStrokeSpin->value() : -1;

	if (read.isEmpty() && mean.isEmpty() && radi.isEmpty() && strokeno < 0)
	{
		QLOG("Search criteria invalid, aborting");
		QLOGDEC;
		return;
	}

	// indicate busy status
	// todo: search on seaprate thread; ditto for words
	dictUI_.charSearchButton->setEnabled(false);
	dictUI_.wordSearchButton->setEnabled(false);
	postMessage(tr("Searching..."));
	emit searchStart();

	QLOG("Search started, invalidating previous results");
	wordsInTable_ = false;
	kanjiInTable_ = false;
	resultIndices_.clear();

	// perform dictionary search
	resultIndices_ = charSearch(read, mean, radi, strokeno, stroke);
	const int itemcount = resultIndices_.size();
	QLOG("Result list has " << itemcount << " entries" );

	if (itemcount)
	{
		kanjiInTable_ = true;
	}

	// show status message
	const QString msg = (itemcount ? tr("Done, ") + QString::number(itemcount) + tr(" results") : tr("No results"));
	postMessage(msg);

	QLOG("Ending search");
	emit searchDone();
	dictUI_.charSearchButton->setEnabled(true);
	dictUI_.wordSearchButton->setEnabled(true);

	QLOGDEC;
}

void JpCnPluginBase::wordSearchButtonClicked()
{
	QLOGX("Word dictionary search");
	QLOGINC;

	// retrieve search parameters from controls, preprocess
	QString query = dictUI_.wordSearchQueryEdit->text();
	if (query.isEmpty()) 
	{
		QLOG("Query string empty, aborting");
		QLOGDEC;
		return;
	}

	query = query.toLower().simplified();

	bool searchEnglish = dictUI_.wordSearchEnglishRadio->isChecked();
	MatchType searchMode = EXACT;
	if      (dictUI_.wordSearchContainRadio->isChecked()) searchMode = CONTAINS;
	else if (dictUI_.wordSearchStartRadio->isChecked())   searchMode = STARTS;
	else if (dictUI_.wordSearchEndRadio->isChecked())     searchMode = ENDS;

	// indicate busy status
	dictUI_.charSearchButton->setEnabled(false);
	dictUI_.wordSearchButton->setEnabled(false);
	postMessage(tr("Searching..."));
	emit searchStart();
	resultIndices_.clear();

	QLOG("Query: '" << query << "', searchEnglish: " << searchEnglish << ", mode: " << searchMode);

	// expand query with possible variants for searches from the target language, if necessary
	QStringList queryEx;
	if (!searchEnglish) queryEx = wordSearchQuery(query);

	int itemcount, pass = 4; // maximum number of tries in persistent mode
	bool complete = false, persistent = dictUI_.wordSearchPersistentCheck->isChecked();

	// generate search results, search up to 4 times in persistent mode until results are 
	// not empty, search once in normal mode
	while (!complete)
	{
		QLOG("Running search, searchMode: " << searchMode << ", pass: " << pass);
		
		if (searchEnglish)
			resultIndices_ = wordDictSearchNative(query, searchMode);
		else
			resultIndices_ = wordDictSearchLang(queryEx, searchMode);

		itemcount = resultIndices_.size();
		QLOG("Got " << itemcount << " results");

		// results present, we're done
		if (itemcount > 0) 
		{
			complete = true; 
		}
		else if (persistent) // no results but persistent search requested
		{
			// select next search mode based on completed search
			if (searchMode == EXACT) // completed EXACT, can try 3 more, start with STARTS
			{
				searchMode = STARTS; pass = 3;
			}
			else if (searchMode == STARTS) // completed STARTS, can try CONTAINS and possibly ENDS
			{
				if (pass > 2) { searchMode = ENDS;     pass = 2; }
				else          { searchMode = CONTAINS; pass = 1; }
			}
			else if (searchMode == ENDS) // completed ENDS, can try CONTAINS and possibly STARTS
			{
				if (pass > 2) { searchMode = STARTS;   pass = 2; }
				else          { searchMode = CONTAINS; pass = 1; }
			}
			else if (searchMode == CONTAINS) // completed CONTAINS, can't do more, sorry
			{
				pass = 0;
			}

			if (!pass) complete = true; // break loop when no more tries allowed
		}
		// empty results but not persistent, done
		else
		{
			complete = true; 
		}
	}

	// select the button which generated results in persistent mode
	if (persistent && itemcount > 0)
	{
		QLOG("Selecting button from persistent mode: " << searchMode);
		QRadioButton *radio = NULL;
		switch(searchMode)
		{
			case EXACT:    radio = dictUI_.wordSearchMatchRadio;   break;
			case STARTS:   radio = dictUI_.wordSearchStartRadio;   break;
			case ENDS:     radio = dictUI_.wordSearchEndRadio;     break;
			case CONTAINS: radio = dictUI_.wordSearchContainRadio; break;
			default: break;
		}
		if (radio) radio->setChecked(true);
	}

	QLOG("Result list has " + QString::number(itemcount) + " entries");

	// update the table with new data
	wordsInTable_ = true;
	kanjiInTable_ = false;
	
	// show status message
	QString msg = (itemcount ? tr("Done, ") + QString::number(itemcount) + tr(" results") : tr("No results. See About info for additional help on searching."));
	postMessage(msg);
	
	dictUI_.wordSearchButton->setEnabled(true);
	dictUI_.charSearchButton->setEnabled(true);

	emit searchDone();

	QLOGDEC;
}

void JpCnPluginBase::kanjiCopyClicked()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = kanjiUI_.detCharLabel->text();
	QLOGX("Copying to clipboard: '" << text << "'");
	clipboard->setText(text);
}

void JpCnPluginBase::wordCopyClicked()
{
	QLOGX("Copy to clipboard requested");
	QLOGINC;
	QClipboard *clipboard = QApplication::clipboard();
	QString text;

	// join text from all word details labels
	QLOG("Need to process " << wordLabels_.size() << " labels from container");
	for (int i = 0; i < wordLabels_.size(); ++i)
	{
		QLOG("Item: " << i);
		const QLabel *label = wordLabels_.at(i);
		Q_ASSERT(label);
		text += label->text();
		QLOG("Text: " << text);
	}
	text += " (" + wordUI_.detReadLabel->text() + ")";
	QLOG("Text: '" << text << "'");
	clipboard->setText(text);
	QLOGDEC;
}

void JpCnPluginBase::radTableButtonClicked()
{
	radicalDialog_->exec();
}

void JpCnPluginBase::radTableClearButtonClicked()
{
	QPushButton *button;
	for (int i = 0; i < radicalButtons_.count(); ++i)
	{
		button = radicalButtons_.at(i);
		button->setChecked(false);
	}
}

void JpCnPluginBase::radTableAccepted()
{
	QPushButton *button = NULL;
	QString radicals = "";
	Q_ASSERT(radicalButtons_.size() == radicalStates_.size());
	for (int i = 0; i < radicalButtons_.count(); ++i)
	{
		button = radicalButtons_.at(i);
		if (button->isChecked())
		{
			const Radical &radical = radicalDict_.entry(i);
			radicals += radical.radical();
			radicalStates_[i] = true;
		}
		else radicalStates_[i] = false;
	}
	dictUI_.charRadicalsEdit->setText(radicals);
}

void JpCnPluginBase::radTableRejected()
{
	QPushButton *button = NULL;
	Q_ASSERT(radicalButtons_.size() == radicalStates_.size());
	for (int i = 0; i < radicalStates_.count(); ++i)
	{
		button = radicalButtons_.at(i);
		button->setChecked(radicalStates_.at(i));
	}
}

void JpCnPluginBase::selFontButtonClicked()
{
	bool ok;
	baseFont_ = QFontDialog::getFont(&ok, baseFont_, app_, tr("Select base font"));
	applyFontChange();
}

// ==============================================================================================
// ============================== JpCnPluginBase: protected
// ==============================================================================================

QList<int> JpCnPluginBase::charSearch(const QString &read, const QString &mean, const QString &radical, const int strokeCount, const StrokeMode strokeMode) const
{
	return charDict_.findCharacters(LANG_JP, read, mean, radical, strokeCount, strokeMode);
}

// send a message to the GUI
void JpCnPluginBase::postMessage(const QString &msg)
{
	emit message(name() + ": " + msg);
}

void JpCnPluginBase::setupDictionaries()
{
	QLOGX("Setting up dictionaries");
	QLOGINC;

	postMessage("Loading KANJIDIC dictionary...");
    QLOG("Loading kanji dictionary from " << DICT_CHARACTER);
    charDict_.loadData(DICT_CHARACTER);

	postMessage("Loading Radkfilex dictionary...");
    QLOG("Loading radicals database from " << DICT_RADICAL);
    radicalDict_.loadData(DICT_RADICAL);

	QLOG("Kanji dictionary: " << charDict_.entryCount() << ", Radical dictionary: " << radicalDict_.entryCount());

	wordDictLoad();

	if (charDict_.error() && wordDictError()) 
	{
		QLOG("Both dictionaries unusable");
		ok_ = false;
	}


	QLOGDEC;
}

void JpCnPluginBase::setupRadicalsDialog()
{
	QLOGX("Setting up radicals dialog");
	QLOGINC;

	if (radicalDict_.error())
	{
		QLOG("Radical dictionary empty, skipping");
		QLOGDEC;
		return;
	}

	Q_ASSERT(radicalDialog_);
	radicalUI_.setupUi(radicalDialog_);
	QFrame *frame = radicalUI_.frame;
	QGridLayout *layout = new QGridLayout;
	// mock label for determining the width of buttons
	QLabel *label = new QLabel;
	label->setFont(baseFont_);
	int curstrokes = 0, strokes;
	int row=0, col=0;
	const int wrap = 15;

	Q_ASSERT(radicalButtons_.empty() && radicalStates_.empty());
	QLOG("Radical count: " << radicalDict_.entryCount());
	for (int i = 0; i < radicalDict_.entryCount(); ++i)
	{
		const Radical &radical = radicalDict_.entry(i);
		strokes = radical.strokes();

		// add a new stroke group label
		if (strokes > curstrokes)
		{
			curstrokes = strokes;
			QLabel *label = new QLabel();
			label->setText("<b>" + QString::number(strokes) + "</b>");
			label->setAlignment(Qt::AlignRight);
			layout->addWidget(label, row, col);
			col++;
			if (col > wrap) { col = 0; row++; }
		}

		// figure out the smallest size in which the current radical character fits
		// for the selected font
		label->setText(radical.radical());
		label->adjustSize();
		QSize textSize = label->size();

		// create a radical button
		QPushButton *button = new QPushButton();
		button->setCheckable(true);
		button->setFont(baseFont_);
		button->setText(radical.radical());
		// force the width of the button to not exceed the minimum size plus a margin
		button->setMaximumWidth(textSize.width() + 10);
		button->adjustSize();
		layout->addWidget(button, row, col);

		// remember a pointer to the buttons and their states
		radicalButtons_.append(button);
		radicalStates_.append(false);

		col++;
		if (col > wrap) { col = 0; row++; }
	}

	delete label;
	Q_ASSERT(radicalButtons_.count() == radicalStates_.count());

	frame->setLayout(layout);

	connect(radicalUI_.clearButton, SIGNAL(clicked()),  this, SLOT(radTableClearButtonClicked()));
	connect(radicalDialog_,         SIGNAL(accepted()), this, SLOT(radTableAccepted()));
	connect(radicalDialog_,         SIGNAL(rejected()), this, SLOT(radTableRejected()));
	QLOGDEC;
}

void JpCnPluginBase::saveSettings()
{
	QLOGX("Saving plugin settings, name: " << name());
	QLOGINC;

	QSettings settings("IXP", name());

	const QString fontStr = baseFont_.toString();
	settUI_.settFontEdit->setText(fontStr);

	const QSize
		kdSize = kanjiDialog_->size(),
		wdSize = wordDialog_->size();

	QLOG("Font: " << fontStr << ", kanji dialog: " << kdSize << ", word dialog: " << wdSize);
	settings.setValue(SETT_FONT, fontStr);
	settings.setValue(SETT_KANJI_SZ, kdSize);
	settings.setValue(SETT_WORD_SZ, wdSize);
	QLOG("Columns: " << col0Width_ << ", " << col1Width_ << ", " << col2Width_);
	settings.setValue(SETT_COLW_0, col0Width_);
	settings.setValue(SETT_COLW_1, col1Width_);
	settings.setValue(SETT_COLW_2, col2Width_);

	QLOGDEC;
}

void JpCnPluginBase::loadSettings()
{
	QLOGX("Loading plugin settings");
	QLOGINC;

	QSettings settings("IXP", name());

	const QString fontstr = settings.value(SETT_FONT, "MS Gothic").toString();
	if (!fontstr.isEmpty())	
	{
		QLOG("Japanese font: '" << fontstr << "'");
		baseFont_.fromString(fontstr);
		applyFontChange();
	}

	const QSize ksize = settings.value(SETT_KANJI_SZ).toSize();
	if (ksize.isValid()) 
	{
		QLOG("Kanji dialog size: " << ksize);
		kanjiDialogSize_ = ksize;
	}

	const QSize wsize = settings.value(SETT_WORD_SZ).toSize();
	if (wsize.isValid()) 
	{
		QLOG("Word dialog size: " << wsize);
		wordDialogSize_ = wsize;
	}

	bool ok0 = true, ok1 = true, ok2 = true;
	const int 
		col0 = settings.value(SETT_COLW_0).toInt(&ok0),
		col1 = settings.value(SETT_COLW_1).toInt(&ok1),
		col2 = settings.value(SETT_COLW_2).toInt(&ok2);
	if (ok0 && ok1 && ok2)
	{
		QLOG("Columns = " << col0 << ", " << col1 << ", " << col2);
		col0Width_ = col0;
		col1Width_ = col1;
		col2Width_ = col2;
	}

	QLOGDEC;
}

void JpCnPluginBase::applyFontChange()
{
	QLOGX("Applying font change");

	//jpBaseFont.setWeight(QFont::Normal);
	middleFont_ = baseFont_;
	middleFont_.setPointSize(baseFont_.pointSize() + 4);
	//jpMiddleFont.setWeight(QFont::Normal);
	bigFont_ = baseFont_;
	bigFont_.setPointSize(baseFont_.pointSize() + 12);
	//jpBigFont.setWeight(QFont::Normal);

	dictUI_.charReadingEdit->setFont(baseFont_);
	dictUI_.charRadicalsEdit->setFont(baseFont_);
	dictUI_.wordSearchQueryEdit->setFont(baseFont_);

	settUI_.settFontEdit->setText(baseFont_.family() + " " + QString::number(baseFont_.pointSize()));

	kanjiUI_.detRadLabel->setFont(baseFont_);
	kanjiUI_.detReadLabel->setFont(middleFont_);
	kanjiUI_.detCharLabel->setFont(bigFont_);
	kanjiUI_.simpleLabel->setFont(bigFont_);
	kanjiUI_.traditionalLabel->setFont(bigFont_);

	wordUI_.detReadLabel->setFont(middleFont_);

	for (int i = 0; i < radicalButtons_.count(); ++i)
	{
		QPushButton *button = radicalButtons_.at(i);
		button->setFont(baseFont_);
	}
}

// display dialog containing details for a HanCharacter search result
void JpCnPluginBase::popKanjiDetailsDialog(const int itemIdx)
{
	QLOGX("Popping kanji dialog for dictionary item " << itemIdx);
	QLOGINC;

	const HanCharacter &item = charDict_.entry(itemIdx);
	kanjiUI_.detCharLabel->setText(item.character());
	kanjiUI_.detStrokeLabel->setText(QString::number(item.strokes()));

	QString radicals = radicalDict_.matchingRadicals(item.character());
	kanjiUI_.detRadLabel->setText(radicals.isEmpty() ? tr("N/A") : radicals);

	fillKanjiDetails(item);

	if (kanjiDialogSize_.isValid()) kanjiDialog_->resize(kanjiDialogSize_);
	else kanjiDialog_->adjustSize();

	kanjiDialog_->exec();

	kanjiDialogSize_ = kanjiDialog_->size();

	QLOGDEC;
}

// display dialog containing details for a JpWord search result
void JpCnPluginBase::popWordDetailsDialog(const int itemIdx)
{
	QLOGX("Popping word dialog for dictionary item " << itemIdx);
	QLOGINC;

	const QString expression = wordDictEntry(itemIdx);

	// convert string to a series of labels inside a layout, old labels also get destroyed here
	QHBoxLayout *newlayout = hanStringToLayout(expression);
	Q_ASSERT(newlayout != NULL);
	QLOG("Received layout with " << newlayout->count() << " items");

	// destroy old layout if it exists, can't install new over old
	const QLayout *oldlayout = wordUI_.wordFrame->layout();
	delete oldlayout;

	// set new layout
	wordUI_.wordFrame->setLayout(newlayout);
	// fill rest of fields on dialog
	wordUI_.detReadLabel->setText(wordDictReading(itemIdx));
	wordUI_.detDefLabel->setText(wordDictDefinition(itemIdx));
	wordUI_.detCatLabel->setText(wordDictCategory(itemIdx));

	if (wordDialogSize_.isValid()) wordDialog_->resize(wordDialogSize_);
	else wordDialog_->adjustSize();

	wordDialog_->exec();

	wordDialogSize_ = wordDialog_->size();

	QLOGDEC;
}

void JpCnPluginBase::fillKanjiDetails(const HanCharacter &item)
{
	kanjiUI_.detReadLabel->setText(item.jpReadingsStr());
	kanjiUI_.detDefLabel->setText(item.jpMeaningsStr());
}

// this also deletes all old labels created on the previous run
QHBoxLayout* JpCnPluginBase::hanStringToLayout(const QString &han)
{
	if (han.isEmpty()) return NULL;

	QLOGX("Running with input '" << han << "' and " << wordLabels_.size() << " old labels to destroy");
	QLOGINC;

	// delete all old labels and clear their handles
	// todo: necessary? won't these get destroyed along with the parent layout?
	for (int i = 0; i < wordLabels_.size(); ++i) delete wordLabels_.at(i);
	wordLabels_.clear();

	// create new layout
	QHBoxLayout *layout = new QHBoxLayout();
	QLabel *tempLabel = NULL;
	// iterate through expression string, character by character
	QLOG("Need to process " << han.size() << " characters, layout has " << layout->count() << " elements");
	QLOGINC;
	for (int i = 0; i < han.size(); ++i)
	{
		const QChar &c = han.at(i);
		QLOG("Character " << i << ": '" << c << "'");
		QLOGINC;
		// character is kanji, create separate custom labels for each character
		if (Util::isKanji(c))
		{
			QLOG("Character is han");
			// previous label exists, add to layout and label list
			if (tempLabel != NULL) 
			{ 
				QLOG("ADDING TO LAYOUT: existing non-han label");
				layout->addWidget(tempLabel); 
			}
			// create new custom label, set it up and add to layout and label list
			QLOG("Creating new kanji label and adding character");
			tempLabel = new KanjiLabel(this, baseFont_);
			wordLabels_.append(tempLabel);
			tempLabel->setText(c);
			QLOG("ADDING TO LAYOUT: kanji label");
			layout->addWidget(tempLabel);
			// forget reference to label; it should hold a single character only
			tempLabel = NULL;
		}
		// character isn't kanji, append to a new or a pre-existing regular label
		else
		{
			QLOG("Character isn't han");
			// label already exists
			if (tempLabel) 
			{
				QLOG("Appending to existing label");
				tempLabel->setText(tempLabel->text() + c);
			}
			else
			{
				QLOG("Creating new label for non-han characters");
				tempLabel = new WordLabel(baseFont_);
				wordLabels_.append(tempLabel);
				tempLabel->setText(c);
			}
		}
		QLOGDEC;
	}
	QLOGDEC;
	// process a remaining label if exists
	if (tempLabel)
	{
		QLOG("ADDING TO LAYOUT: remaining label");
		layout->addWidget(tempLabel);
	}

	// setup the layout for display
	layout->addStretch();
	layout->setSpacing(0);

	QLOG("Layout generated, has " << layout->count() << " elements");
	QLOGDEC;
	return layout;
}

// ==============================================================================================
// ============================== WordLabel
// ==============================================================================================

WordLabel::WordLabel(const QFont &font) : QLabel()
{
	QLOGX("Creating label");
	//setStyleSheet("border: 1px solid black;");
	QFont labelfont(font);
	labelfont.setPointSize(font.pointSize() + 12);
	setFont(labelfont);
}

WordLabel::~WordLabel()
{
	QLOGX("Destroying word label, text: '" << text() << "'");
}

// ==============================================================================================
// ============================== KanjiLabel
// ==============================================================================================

KanjiLabel::KanjiLabel(QObject *parent, const QFont &font) : WordLabel(font)
{
	QLOGX("Creating label");
	connect(this, SIGNAL(clicked(const QString &)), parent, SLOT(showKanjiDetails(const QString &)));
	setStyleSheet("color: blue;");
}

KanjiLabel::~KanjiLabel()
{
	QLOGX("Destroying kanji label, text: '" << text() << "'");
}

// todo: when dialog is exec'd, for some reason KanjiLabel-s don't receive these events - at least not until the
// dialog is clicked. it's as if the mouse focus still belonged to the parent window. Perhaps it's necessary to
// subclass QDialog for and grabFocus() or something in its showEvent()?
void KanjiLabel::enterEvent(QEvent *e)
{
	setStyleSheet("color: red;");
	e->accept();
}

void KanjiLabel::leaveEvent(QEvent *e)
{
	setStyleSheet("color: blue;");
	e->accept();
}

void KanjiLabel::mouseReleaseEvent(QMouseEvent *e)
{
	QLOG("Mouse press in KanjiLabel handler, text: '" << text() << "'" );
    e->accept();
	emit clicked(text());
}
