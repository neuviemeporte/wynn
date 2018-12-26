#include <QString>
#include <QDialog>
#include <QSettings>

#include "de_plugin.h"
#include "ixpdict.h"
#include "ixplog_active.h"

QTSLogger *PLUGIN_LOGSTREAM = NULL;
#define QLOGSTREAM PLUGIN_LOGSTREAM
#define QLOGTAG "[PLUG_DE]"

// need to export plugin class like this for Qt4
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(de_plugin, DePlugin)
#endif

const QString 
	DePlugin::SETT_COLW_DE = "colWidthDE", 
    DePlugin::SETT_COLW_EN = "colWidthEN",
    DePlugin::DICT_BEO = "dictionaries/de_en.dat";

DePlugin::DePlugin() :
	detailsDialog_(NULL),
	deQuery_(false), enQuery_(false),
 	colGermanWidth_(0), colEnglishWidth_(0)
{
	PLUGIN_LOGSTREAM = new QTSLogger("de-plugin_log.txt");
	QLOGX("Constructing instance");
	QLOGINC;

	dictWidget_ = new QWidget();
	panelUI_.setupUi(dictWidget_);

	connect(panelUI_.searchButton, SIGNAL(clicked()),		this,				   SLOT(doSearch()));
	connect(panelUI_.queryEdit,    SIGNAL(returnPressed()), panelUI_.searchButton, SIGNAL(clicked()));

	QLOGDEC;
}

DePlugin::~DePlugin()
{
	saveSettings();
	delete detailsDialog_;
	
    // TODO: crash FIXME!!!
    // remove multiple logfiles would be best, add logfile object management to QTSLogger
    // singleton-style, with diverse IDs for logs? just an idea...
	//delete PLUGIN_LOGSTREAM;
}

QString DePlugin::description() const
{
	return QString("This is the original German language plugin for Wynn.<br><br>") +
		"Dictionary content was generated from the BEOLINGUS wordlist, released under the GPLv2 license.<br>" +
        "Available at <a href=\"http://dict.tu-chemnitz.de/\">http://dict.tu-chemnitz.de/</a><br>" +
		"(C) 1995-2013 by Frank Richter<br><br>" +
		"Plugin (C) 2013 Darek Szczepanek <a href=\"mailto:szczepanek.d@gmail.com\">&lt;szczepanek.d@gmail.com&gt;</a><br>" +
		"Plugin code released under the terms of <a href=\"http://www.gnu.org/licenses/gpl-2.0-standalone.html\">GPLv2</a>.";
}

int DePlugin::resultItemCount() const
{
	return results_.size();
}

int DePlugin::resultColumnCount() const
{
	return 2;
}

QString DePlugin::resultData(const int itemIndex, const int columnIndex) const
{
	const BeoResult &result = results_.at(itemIndex);

	if (deQuery_) switch(columnIndex)
	{
		case 0: return dict_.resultText(BEO_DE, result); break;
		case 1: return dict_.resultText(BEO_EN, result.allPhrases()); break;
	}
	else if (enQuery_) switch(columnIndex)
	{
		case 0: return dict_.resultText(BEO_DE, result.allPhrases()); break;
		case 1: return dict_.resultText(BEO_EN, result); break;
	}

	return QString();
}

QString DePlugin::resultHeaderData(const int column) const
{
	switch(column)
	{
		case 0: return QString("German"); break;
		case 1: return QString("English"); break;
	}

	return QString();
}

void DePlugin::showResultDetails(const int resultIndex)
{
	QLOGX("Need to show details for result " << resultIndex);
	QLOGINC;

	// clean old contents of dialog first
	QWidget *oldWidget = detailsUI_.scrollArea->widget();
	delete oldWidget;

	// grab BeoItem whose details will be used to fill the dialog
	const BeoResult &result = results_.at(resultIndex);
	QLOG("Result: " << result);
	Q_ASSERT(result.itemFound());
	const BeoItem &resultItem = dict_.entry(result.item);

	// create new layout containing labels and separators for all definitions
	// of the BeoItem
	QVBoxLayout *newLayout = new QVBoxLayout;
	for (int defIdx = 0; defIdx < resultItem.definitionCount(); ++defIdx)
	{
		const BeoDefinition &def = resultItem.definition(defIdx);

		// create labels containing text of the german end english parts
		// of the definition and set their properties
		QLabel
			*deLabel = new QLabel(def.deText()),
			*enLabel = new QLabel(def.enText());
		deLabel->setWordWrap(true);
		enLabel->setWordWrap(true);
		deLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		enLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
		// labels except the first ones should be indented
		if (defIdx > 0)
		{
			deLabel->setIndent(20);
			enLabel->setIndent(20);
		}

		// arrange the two labels in a horizontal box and add it to the new layout
		QHBoxLayout *defLayout = new QHBoxLayout;
		defLayout->addWidget(deLabel);
		defLayout->addWidget(enLabel);
		newLayout->addLayout(defLayout);

		// create separator item and add it to the layout below the two boxed labels
		QFrame *separator = new QFrame;
		separator->setGeometry(0, 0, 100, 3);
		separator->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
		separator->setFrameShape(QFrame::HLine);
		separator->setFrameShadow(QFrame::Sunken);
		newLayout->addWidget(separator);
	}

	// add final item (a vertical spacer) below the labels and separators, 
	// set layout on details widget and exec dialog
	newLayout->addStretch();
	QWidget *newWidget = new QWidget;
	newWidget->setLayout(newLayout);
	detailsUI_.scrollArea->setWidget(newWidget);
	detailsDialog_->exec();

	QLOGDEC;
}

QStringList DePlugin::getResultForDatabase(const int resultIndex)
{
	QStringList ret;
	Q_ASSERT(resultIndex >= 0 && resultIndex < results_.size());
	ret << resultData(resultIndex, 0) << resultData(resultIndex, 1);
	return ret;	
}

void DePlugin::preSetup(QWidget *parent, class QTSLogger *debugLog)
{
	Q_ASSERT(ok_ && parent);	
	if (debugLog) 
	{
		QLOG("Redirecting log output");
		PLUGIN_LOGSTREAM = debugLog;
		IXPDICT_LOGREDIR(debugLog);
	}

	QLOGX("Performing plugin pre-setup");
    QLOGINC;

	detailsDialog_ = new QDialog(parent);
	detailsUI_.setupUi(detailsDialog_);
	// connect signals to and from application
	connect(this,   SIGNAL(searchStart()),                         parent, SLOT(slot_dict_searchStart()));
	connect(this,   SIGNAL(searchDone()),                          parent, SLOT(slot_dict_searchDone()));
	connect(this,   SIGNAL(message(const QString &)),              parent, SLOT(slot_updateSetupMsg(const QString &)));
	
	connect(parent, SIGNAL(dictColumnResized(int, int, int, int)), this,   SLOT(columnResize(int, int, int, int)));
	connect(this,   SIGNAL(columnWidthsNotify(const QList<int>&)), parent, SLOT(slot_dict_resizeColumns(const QList<int>&)));

	loadSettings();

	QLOGDEC;
}

void DePlugin::setup()
{
    QLOGX("Loading Beolinugs dictionary from " << DICT_BEO);
	QLOGINC;

	postMessage("Loading BEOLINGUS dictionary...");
    dict_.loadData(DICT_BEO);

	if (dict_.error())
	{
		QLOG("Error loading dictionary");
		ok_ = false;
	}

	QLOGDEC;
}

void DePlugin::activate()
{
	QLOGX("Activating plugin");
	QLOGINC;

	QLOG("Sending column widths: " << colGermanWidth_ << "/" << colEnglishWidth_);
	QList<int> widths;
	widths << colGermanWidth_ << colEnglishWidth_;
	emit columnWidthsNotify(widths);

	QLOGDEC;
}

void DePlugin::columnResize(int curPlugIdx, int colIdx, int oldSize, int newSize)
{
	if (curPlugIdx != index_) return;

	QLOGX("Resizing column " << colIdx << " from " << oldSize << " to " << newSize);
	switch(colIdx)
	{
		case 0: colGermanWidth_ = newSize; break;
		case 1: colEnglishWidth_ = newSize; break;
	}
}

// TODO: abstract the search algorithm in a function, preferably a member of libixpdict's Util::, avoid boilerplate
// TODO: search on a separate thread for all plugins?
void DePlugin::doSearch()
{
	QLOGX("Word dictionary search");
	QLOGINC;

	// retrieve search parameters from controls, preprocess
	QString query = panelUI_.queryEdit->text();
	if (query.isEmpty()) 
	{
		QLOG("Query string empty, aborting");
		QLOGDEC;
		return;
	}

	query = query.toLower().simplified();

	BeoLanguage lang = (panelUI_.englishRadio->isChecked() ? BEO_EN : BEO_DE);
	MatchType searchMode = EXACT;
	if      (panelUI_.containRadio->isChecked()) searchMode = CONTAINS;
	else if (panelUI_.startRadio->isChecked())   searchMode = STARTS;
	else if (panelUI_.endRadio->isChecked())     searchMode = ENDS;

	// indicate busy status
	panelUI_.searchButton->setEnabled(false);
	postMessage(tr("Searching..."));
	emit searchStart();
	deQuery_ = false;
	enQuery_ = false;
	results_.clear();

	QLOG("Query: '" << query << "', lang: " << lang << ", mode: " << searchMode);

	// expand query with possible variants for searches from the target language, if necessary
	QStringList queryEx(query);
	if (lang == BEO_DE) 
	{
		const QString deExtend = Util::makeUmlautsEszett(query);
		if (deExtend != query) queryEx << deExtend;
	}
	QLOG("Extended query: '" << queryEx << "'");

	int itemcount = 0, pass = 4; // maximum number of tries in persistent mode
	bool complete = false, persistent = panelUI_.persistentCheck->isChecked();

	// generate search results, search up to 4 times in persistent mode until results are 
	// not empty, search once in normal mode
	while (!complete)
	{
		QLOG("Running search, searchMode: " << searchMode << ", pass: " << pass);
		
		results_ = dict_.search(lang, queryEx, searchMode);

		itemcount = results_.size();
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
			case EXACT:    radio = panelUI_.matchRadio;   break;
			case STARTS:   radio = panelUI_.startRadio;   break;
			case ENDS:     radio = panelUI_.endRadio;     break;
			case CONTAINS: radio = panelUI_.containRadio; break;
			default: break;
		}
		if (radio) radio->setChecked(true);
	}

	QLOG("Result list has " + QString::number(itemcount) + " entries");

	// show status message
	QString msg = (itemcount ? tr("Done, ") + QString::number(itemcount) + tr(" results") : tr("No results"));
	postMessage(msg);
	panelUI_.searchButton->setEnabled(true);

	switch(lang)
	{
		case BEO_DE: deQuery_ = true; break;
		case BEO_EN: enQuery_ = true; break;
		case BEO_NONE: break;
	}

	emit searchDone();

	QLOGDEC;
}

void DePlugin::postMessage(const QString &msg)
{
	emit message(name() + ": " + msg);
}

void DePlugin::saveSettings()
{
	QLOGX("Saving plugin settings");
	QLOGINC;

	QSettings settings("IXP", name());

	QLOG("widthDE = " << colGermanWidth_ << ", widthEN = " << colEnglishWidth_);
	settings.setValue(SETT_COLW_DE, colGermanWidth_);
	settings.setValue(SETT_COLW_EN, colEnglishWidth_);

	QLOGDEC;
}

void DePlugin::loadSettings()
{
	QLOGX("Loading plugin settings");
	QLOGINC;

	QSettings settings("IXP", name());

	bool okDE = true, okEN = true;
	const int 
		widthDE = settings.value(SETT_COLW_DE).toInt(&okDE),
		widthEN = settings.value(SETT_COLW_EN).toInt(&okEN);
	if (okDE && okEN)
	{
		QLOG("widthDE = " << widthDE << ", widthEN = " << widthEN);
		colGermanWidth_ = widthDE;
		colEnglishWidth_ = widthEN;
	}

	QLOGDEC;
}
