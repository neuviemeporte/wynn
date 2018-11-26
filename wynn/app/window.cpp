#include <stdexcept>
#include <QFile>
#include <QDir>
#include <QString>
#include <QDialog>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QShowEvent>
#include <QProgressBar>
#include <QToolButton>
#include <QModelIndex>
#include <QFont>
#include <QInputDialog>
#include <QLineEdit>
#include <QRegExp>
#include <QMessageBox>
#include <QFileDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QSettings>
#include <QClipboard>
#include <QSortFilterProxyModel>
#include <QProgressBar>
#include <QFileDialog>
#include <QPluginLoader>

#include "global.h"
#include "window.h"
#include "setup.h"
#include "dict_table.h"
#include "dict_plugin.h"
#include "database.h"
#include "database_model.h"
#include "quiz.h"
#include "ixplog_active.h"

QWidget *GUI;
QTSLogger *APP_LOGSTREAM = NULL;

const QString 
	MainForm::APPDIR      = QDir::homePath() + "/.wynn",
	MainForm::SETT_EXTDIR = "extdir", 
	MainForm::SETT_NODUPS = "nodbdups", 
	MainForm::SETT_CURDB  = "curdb";

namespace {
const QString PLUG_WILDCARD("wynnplugin-*");
#if defined(__linux__)
const QString PLUG_PREFIX("lib");
const QString PLUG_EXT(".so");
#elif defined(_WIN32)
const QString PLUG_PREFIX("");
const QString PLUG_EXT(".dll");
#else
#error Unrecognized target platform!
#endif
} // namespace

// ==============================================================================================
// ============================== MainForm: public
// ==============================================================================================

MainForm::MainForm(QWidget *parent) : QMainWindow(parent),
    curPlugin_(NULL),
    curDbase_(NULL), 
    dbaseModel_(NULL), 
    dbaseFindText_(""),
    dictModel_(NULL),
    dbaseDialog_(NULL), quizDialog_(NULL),
    setupThread_(NULL), 
    quiz_(NULL)
{
	// open debug log
	APP_LOGSTREAM = new QTSLogger("wynn_log.txt");
	QLOG("Main window constructor is active");
	QLOGINC;

	QLOG("Setting up interface");
	ui_.setupUi(this);
	GUI = this;

	QString build = ui_.buildLabel->text() + "\nBuild: " + __DATE__ + " " + __TIME__;
#ifdef _DEBUG
	build += " Debug";
#endif
#ifdef _WIN64
	build += " x86-64";
#else
	build += " x86";
#endif
	ui_.buildLabel->setText(build);

	// clear up any mock-up content of plugin panels, destroying contents
	clearPluginStacks(true);

	QLOG("Setting up database entry dialog");
	dbaseDialog_ = new QDialog(this);
	dbaseDialogUI_.setupUi(dbaseDialog_);

	QLOG("Setting up quiz dialog");
	quizDialog_ = new QDialog(this);
	quizDialogUI_.setupUi(quizDialog_);
	QuizDialogEventFilter *filter = new QuizDialogEventFilter(quizDialog_, &quizDialogUI_);
	quizDialog_->installEventFilter(filter);

	QLOG("Connecting signals");
	// Main stack widget page changing buttons
	connect(ui_.dictionaryButton, SIGNAL(clicked()),                           this, SLOT(slot_dictionaryButtonClicked()));
	connect(ui_.manageButton,     SIGNAL(clicked()),                           this, SLOT(slot_manageButtonClicked()));
	connect(ui_.settingsButton,   SIGNAL(clicked()),                           this, SLOT(slot_settingsButtonClicked()));
	connect(ui_.aboutButton,      SIGNAL(clicked()),                           this, SLOT(slot_aboutButtonClicked()));
	connect(ui_.createButton,     SIGNAL(clicked()),                           this, SLOT(slot_database_createClicked()));
	connect(ui_.deleteButton,     SIGNAL(clicked()),                           this, SLOT(slot_database_deleteClicked()));
	connect(ui_.databaseCombo,    SIGNAL(currentIndexChanged(const QString&)), this, SLOT(slot_database_changeCurrent(const QString&)));

	// dictionary page signals
	QHeaderView *dictTableHeader = ui_.dictionaryTable->horizontalHeader();
	connect(ui_.langCombo,          SIGNAL(currentIndexChanged(int)),      this, SLOT(slot_dict_switchPlugin(int)));
	connect(ui_.langAboutButton,    SIGNAL(clicked()),                     this, SLOT(slot_dict_langAboutClicked()));
	connect(ui_.dictDetailsButton,  SIGNAL(clicked()),                     this, SLOT(slot_dict_detailsClicked()));
	connect(ui_.dictDatabaseButton, SIGNAL(clicked()),                     this, SLOT(slot_dict_toDbaseClicked()));
	connect(ui_.dictionaryTable,    SIGNAL(activated(QModelIndex)),        this, SLOT(slot_dict_tableItemActivated(QModelIndex)));
	connect(dictTableHeader,        SIGNAL(sectionResized(int, int, int)), this, SLOT(slot_dict_columnResized(int, int, int)));

	// database functionality signals
	connect(ui_.dbaseAddButton,    SIGNAL(clicked()),                      this, SLOT(slot_database_addToClicked()));
	connect(ui_.dbaseRemoveButton, SIGNAL(clicked()),                      this, SLOT(slot_database_removeFromClicked()));
	connect(ui_.dbaseCopyButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_copyClicked()));
	connect(ui_.dbaseMoveButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_moveClicked()));
	connect(ui_.dbaseEditButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_editClicked()));
	connect(ui_.dbaseFindButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_findClicked()));
	connect(ui_.dbaseExportButton, SIGNAL(clicked()),                      this, SLOT(slot_database_exportClicked()));
	connect(ui_.dbaseResetButton,  SIGNAL(clicked()),                      this, SLOT(slot_database_resetClicked()));
	connect(ui_.quizButton,        SIGNAL(clicked()),                      this, SLOT(slot_database_quizClicked()));
	connect(ui_.quizShowDescRadio, SIGNAL(clicked()),                      this, SLOT(slot_database_quizTypeChanged()));
	connect(ui_.quizShowItemRadio, SIGNAL(clicked()),                      this, SLOT(slot_database_quizTypeChanged()));
    connect(ui_.databaseTable,     SIGNAL(activated(const QModelIndex &)), this, SLOT(slot_database_editClicked()));

	// settings' signals
	connect(ui_.settExtDirButton, SIGNAL(clicked()), this, SLOT(slot_settings_altdirButtonClicked()));
	connect(ui_.settNoDuplCheck,  SIGNAL(clicked()), this, SLOT(slot_settings_nodupCheckClicked()));
	connect(ui_.settPluginCombo,  SIGNAL(currentIndexChanged(int)), this, SLOT(slot_settings_switchPlugin(int)));

	// process stored settings
	loadSettings();

	// search for plugins and load them
	loadPlugins();

	// parse dictionaries and user databases in background thread
	setupThread_ = new SetupThread(this);
	setupThread_->start();
	QLOG("Main window constructor done");
	QLOGDEC;
}

MainForm::~MainForm()
{
	QLOG("MainForm destructor active");
	QLOGINC;

	saveSettings();

    // don't need to delete things like the dialogs which are created as children of the main window;
    // they will be destroyed automatically.
	delete dictModel_;
	delete dbaseModel_;
	delete quiz_;

	QLOG("Iterating over user databases and deleting their handles");
	for (int i = 0; i < databases_.size(); ++i)
	{
		Database *dbase = databases_.at(i);
		delete dbase;
	}

	// Need to remove all dictionary widgets from the dictionary panel manually before destroying plugins,
	// because plugins are supposed to dispose of these widgets themselves.
	clearPluginStacks(false);

	// Don't need to delete plugins manually here, they will be destroyed automatically when the application terminates.
	for (int i = 0; i < pluginCount(); ++i)
	{
		QPluginLoader *loader = pluginLoader(i);
		DictionaryPlugin *plug = qobject_cast<DictionaryPlugin*>(loader->instance());
		Q_ASSERT(plug);
		QLOG("Retrieved loader for plugin " << i << ": '" << plug->name() << "'");
		bool ok = loader->unload();
		QLOG("Unload OK: " << ok << ", destroying loader");
		delete loader;
	}

	delete APP_LOGSTREAM;
}

Database* MainForm::database(const QString &name) const
{
    if (name.isEmpty()) return NULL;
    QLOG("Getting database handle for name: '" << name << "'");
    Database *ret = NULL, *dbase;
    for (int i = 0; i < databases_.count(); ++i)
    {
        dbase = databases_.at(i);
        if (dbase->name() == name) { ret = dbase; break; }
    }
    return ret;
}

void MainForm::addDatabase(Database *dbase)
{
    Q_ASSERT(dbase);
    QLOGX("Adding handle for database '" << dbase->name() << "'");
    databases_.append(dbase);
}

// ==============================================================================================
// ============================== MainForm: public slots - top-level
// ==============================================================================================

void MainForm::slot_dictionaryButtonClicked()
{
	QLOGX("Switching to dictionary panel");
	QLOGINC;
	QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
	header->blockSignals(true);
	ui_.stackedWidget->setCurrentIndex(0);
	header->blockSignals(false);
	QLOGDEC;
}

void MainForm::slot_manageButtonClicked()
{
	QLOGX("Switching to database panel");
	QLOGINC;
	ui_.stackedWidget->setCurrentIndex(1);
	QLOGDEC;
}

void MainForm::slot_settingsButtonClicked()
{
	QLOGX("Switching to settings panel");
	QLOGINC;
	ui_.stackedWidget->setCurrentIndex(2);
	QLOGDEC;
}

void MainForm::slot_aboutButtonClicked()
{
	QLOGX("Switching to info panel");
	QLOGINC;
	ui_.stackedWidget->setCurrentIndex(3);
	QLOGDEC;
}

// ==============================================================================================
// ============================== MainForm: public slots - dictionary panel
// ==============================================================================================

void MainForm::slot_dict_switchPlugin(int plugIdx)
{
	QLOGX("Dictionary language changed to: " << plugIdx);
	QLOGINC;

	if (curPlugin_) curPlugin_->deactivate();
	curPlugin_ = plugin(plugIdx);
	Q_ASSERT(curPlugin_);
	QLOG("Current plugin: " << curPlugin_->name());
	const int stackIndex = curPlugin_->index();
	Q_ASSERT(stackIndex == plugIdx);
	Q_ASSERT(stackIndex >= 0 && stackIndex < ui_.dictPanelStack->count());

	QStackedWidget *stack = ui_.dictPanelStack;
	// set the size policy of the page being hidden to ignored so it doesn't influence the size
	// of the stacked widget anymore
	QWidget *curWidget = stack->currentWidget();
	if (curWidget)
	{
		curWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	}

	QLOG("Switching dictionary widget to index " << stackIndex);
	stack->setCurrentIndex(stackIndex);

	// set the size policy of the page that just became visible to make the widget adjust its size to it
	curWidget = stack->currentWidget();
	Q_ASSERT(curWidget);
	curWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	stack->adjustSize();

	// block the dictColumnResized signal from the main widget that is emitted after the model is changed,
	// don't want to overwrite the correct column widths in the plugin with a stupid value
	QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
	header->blockSignals(true);
	// todo: clear old results from plugins at switch
	dictModel_->setSource(curPlugin_);
	header->blockSignals(false);
	curPlugin_->activate();

	QLOGDEC;
}

void MainForm::slot_dict_langAboutClicked()
{
	QLOGX("Plugin about button clicked");
	QLOGINC;

	Q_ASSERT(curPlugin_);
	const QString
		plugName = curPlugin_->name(),
		plugDesc = curPlugin_->description();

	QMessageBox::information(this, plugName, plugDesc);

	QLOGDEC;
}

void MainForm::slot_dict_detailsClicked()
{
	QLOGX("Showing dictionary search result item details");
	QLOGINC;

	QModelIndex idx = ui_.dictionaryTable->currentIndex();
	QLOG("Table row: " << idx.row());
	if (idx.row() >= 0) curPlugin_->showResultDetails(idx.row());

	QLOGDEC;
}

// button clicked in a details dialog or the dictionary results table to add current dictionary
// search result entry to a database (selectable)
void MainForm::slot_dict_toDbaseClicked()
{
	QLOGX("Dictionary to database button clicked");
	// pop info and return if no database
	if (!curDbase_)
	{
		QLOG("Database pointer invalid, aborting");
		QMessageBox::information(this, tr("No database"), tr("No user database exists. You must first create one to be able to add items to it."));
		return;
	}
	if (curDbase_->locked()) 
	{ 
		QLOG("Database locked, aborting");
		popDbaseLocked(); 
		return; 
	}

	// return if nothing in table
	// get currently selected row
	QModelIndex index = ui_.dictionaryTable->currentIndex();
	// return if nothing selected in table
	if (!index.isValid()) 
	{ 
		QLOG("Noting selected in table, aborting");
		return; 
	}

	QStringList data = curPlugin_->getResultForDatabase(index.row());
	if (data.size() != 2)
	{
		QLOG("Invalid data from plugin");
		return;
	}

    // pop dialog asking letting user edit the item before adding to database
    addToDatabase(data.first(), data.last());
}

void MainForm::slot_dict_searchStart()
{
	QLOGX("Search start");
	dictModel_->beginReset();
}

void MainForm::slot_dict_searchDone()
{
	QLOGX("Search done");
	dictModel_->endReset();
}

void MainForm::slot_dict_tableItemActivated(const QModelIndex &index)
{
	QLOGX("Table item activated, row: " << index.row());
	QLOGINC;

	Q_ASSERT(curPlugin_ && index.row() >= 0);
	curPlugin_->showResultDetails(index.row());

	QLOGDEC;
}

void MainForm::slot_dict_columnResized(int index, int oldSize, int newSize)
{
	const int plugIdx = curPlugin_->index();
	QLOGX("Column " << index << " was resized from " << oldSize << " to " << newSize << ", current plugin: " << plugIdx);
	QLOGINC;
	emit dictColumnResized(plugIdx, index, oldSize, newSize);
	QLOGDEC;
}

void MainForm::slot_dict_resizeColumns(const QList<int> &widths)
{
	QLOGX("Requested column resize on " << widths.size() << " columns");
	QLOGINC;

	QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
	// prevent header from sending notifications about these resizes
	header->blockSignals(true);

	// check if all widths are larger than zero
	bool ok = true;
	for (int i = 0; i < widths.size(); ++i)
	{
		const int width = widths.at(i);
		if (width <= 0) { ok = false; break; }
	}

	QLOG("Widths ok: " << ok);

	// if so, proceed with resize
	if (ok) for (int i = 0; i < widths.size(); ++i)
	{
		header->resizeSection(i, widths.at(i));
	}
	// otherwise try an automatic resize
	else
	{
		ui_.dictionaryTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}

	header->blockSignals(false);

	QLOGDEC;
}

// ==============================================================================================
// ============================== MainForm: public slots - database functionality
// ==============================================================================================

// current selection in the combo box indicating current database has changed
void MainForm::slot_database_changeCurrent(const QString& text)
{
	if (text.isEmpty()) return;
	QLOGX("Current database changed to: " << text );
	QLOGINC;
    curDbase_ = database(text);
	if (!curDbase_) return; // no database

	dbaseModel_->setDatabase(curDbase_);
	//ui_.databaseTable->resizeColumnsToContents();
	slot_database_updateCount(); // update spinboxes' limits
	ui_.quizRangeToSpin->setValue(curDbase_->entryCount());
	QLOGDEC;
}

void MainForm::slot_database_updateCount()
{
	// neat trick
	//QObject *sender = QObject::sender();
	//const QMetaObject *meta = sender->metaObject();
	//QLOG("Sender: '" << sender->objectName() << "', class '" << meta->className() << "'");

	QLOG("Updating ranges of quiz spinboxes");
	// database was altered, element count could have changed, update range spinboxes
	Q_ASSERT(curDbase_ != NULL);
	int itemsmax = curDbase_->entryCount();
	int itemsmin = 0;
	if (itemsmax > 0) itemsmin = 1;
	ui_.quizRangeFromSpin->setMinimum(itemsmin);
	ui_.quizRangeFromSpin->setMaximum(itemsmax);
	ui_.quizRangeToSpin->setMinimum(itemsmin);
	ui_.quizRangeToSpin->setMaximum(itemsmax);
	ui_.quizTakeSpin->setMinimum(itemsmin);
	ui_.quizTakeSpin->setMaximum(itemsmax);
}

void MainForm::slot_database_createClicked()
{
	bool ok;
	QRegExp nameRegex("[a-zA-Z0-9_-]+");
	QString name = QInputDialog::getText(this, "Create database", "Enter name for new database:",
		QLineEdit::Normal, "", &ok);

	// discriminate against ill-formed database names
	if (ok && !name.isEmpty())
	{
		if (!nameRegex.exactMatch(name))
		{
			QMessageBox::warning(this, 	"Invalid name", 
				"database name may contain only alphanumeric, dash ('-') and underscore ('_') characters.", 
				QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
		QLOG("Creating database, name: '" << name << "'" );
		// create new database, update structures
		Database *dbase = new Database(this, name);
		databases_.append(dbase);
		ui_.databaseCombo->addItem(name);
		ui_.databaseCombo->setCurrentIndex(ui_.databaseCombo->count() - 1);
		ui_.databaseCombo->setEnabled(true);
		//ui_.databaseCombo->adjustSize();
	}
}

void MainForm::slot_database_deleteClicked()
{
	int idx = ui_.databaseCombo->currentIndex();
	Database *dbase = curDbase_;
	if (!dbase) return;

	int ans = QMessageBox::question(this, "Question", QString("Are you sure you want to delete the database '")
		+ dbase->name() + "'?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

	if (ans == QMessageBox::No) return;

	QLOG("Removing database, index: " << idx << ", name: " << dbase->name() 
		<< " (combo text: " << ui_.databaseCombo->currentText() << ")" );
	QLOGINC;

	const QString xmlpath = (dbase->external() ? extDir_ : APPDIR) + "/" + dbase->name() + Database::XML_EXT;
	QLOG("Removing data file '" << xmlpath << "'");
	QFile file(xmlpath);
	if (!file.remove())
	{
		QLOG("Unable to remove file! (Error: " << file.error() << ")");
	}

	delete dbase;
	dbaseModel_->setDatabase(NULL);
	curDbase_ = NULL;
	databases_.removeAt(idx);
	if (databases_.empty()) 
	{
		ui_.databaseCombo->setEnabled(false);
		ui_.quizRangeFromSpin->setMinimum(0);
		ui_.quizRangeFromSpin->setMaximum(0);
		ui_.quizRangeToSpin->setMinimum(0);
		ui_.quizRangeToSpin->setMaximum(0);
		ui_.quizTakeSpin->setMinimum(0);
		ui_.quizTakeSpin->setMaximum(0);
	}
	ui_.databaseCombo->removeItem(idx);
	QLOGDEC;
}

// add buton clicked in database panel
void MainForm::slot_database_addToClicked()
{
	if (!curDbase_)
	{
		QMessageBox::information(this, tr("No database"), tr("No user database exists. You must first create one\nto be able to add items to it."));
		return;
	}
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	QLOG("Database Add button clicked");
	QLOGINC;
	// setup database entry dialog, point to current database in combobox
	setupDbaseDialogCombo(true, true);
	dbaseDialogUI_.dbaseCombo->setEnabled(false);
	dbaseDialogUI_.entryEdit->clear();
	dbaseDialogUI_.descEdit->clear();
	dbaseDialogUI_.entryEdit->setEnabled(true);
	dbaseDialogUI_.entryEdit->setFocus(Qt::OtherFocusReason);
	dbaseDialogUI_.descEdit->setEnabled(true);
	dbaseDialog_->setWindowTitle("Add item to current database");
	// pop dialog and block, continue if accepted
	if (dbaseDialog_->exec() == QDialog::Accepted)
	{
		QString entry = dbaseDialogUI_.entryEdit->text().simplified();
		QString desc = dbaseDialogUI_.descEdit->text().simplified();
		Database *dbase = curDbase_;
		// empty input doesn't make sense
		if (entry.isEmpty() || desc.isEmpty()) 
		{
			QMessageBox::information(this, tr("Missing input"), tr("Database items can't be empty!"));
			QLOG("Missing data in user input, done");
			QLOGDEC;
			return;
		}
		QLOG("Adding entry to database '" << dbase->name() << ", text: '" << entry 
			<< "', description: '" << desc << "'" );
        DbError error = dbase->add(entry, desc);
        if (error == DbError::ERROR_DUPLI)
		{ 
			QMessageBox::StandardButton button;
            int dupidx = error.index();
			const DbEntry &dupEntry = dbase->entry(dupidx);
			button = QMessageBox::question(this, tr("Possible duplicate"), 
				tr("A similar entry was found to already exist in the database:\n'") 
				+ dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1) 
				+ tr(").\nWhile trying to add entry:\n'") + entry + "' / '" + desc 
				+ tr("'\nDo you want to add it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (button == QMessageBox::Yes)
			{
				QLOG("User wants to add anyway");
                error = dbase->add(entry, desc, true);
                QLOG("Result: " << QString::number(error.type()));
                Q_ASSERT(error == DbError::ERROR_OK);
			}
			else
			{
				QLOG("User doesn't want duplicate");
				QLOGDEC;
				return;
			}
		}
		ui_.databaseTable->setFocus(Qt::OtherFocusReason);
		ui_.databaseTable->selectRow(curDbase_->entryCount()-1);
	}
	QLOGDEC;
}

// remove button clicked in database panel
void MainForm::slot_database_removeFromClicked()
{
	if (!curDbase_) return;
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	QLOG("Database remove button clicked");
	QLOGINC;
	// retrieve selected indexes
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();

	// return if no selection
	if (dbIdxs.isEmpty()) 
	{
		QLOG("Nothing selected, done");
		QLOGDEC;
		return;
	}

	QLOGNL("Selected item indices (" << dbIdxs.size() << "): ");
	for (int i = 0; i < dbIdxs.size(); ++i)
	{
		QLOGRAW(dbIdxs.at(i) << " ");
	}
	QLOGRAW(endl);

	// ask for confirmation otherwise
	QMessageBox::StandardButton button;
	button = QMessageBox::question(this, "Remove item(s)", 
		"Are you sure you want to remove " 
			+ (dbIdxs.size() > 1 ? "these " + QString::number(dbIdxs.size()) 
			+ " items?" : "this item?"),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	// remove items
	if (button == QMessageBox::Yes)
	{
		Q_ASSERT(curDbase_ != NULL);
        curDbase_->remove(dbIdxs);
		ui_.databaseTable->setFocus(Qt::OtherFocusReason);
		//	TODO:
		//ui_.databaseTable->selectRow(Idxs.first());
		QLOG("Entries removed");
	}
	else
	{
		QLOG("Action cancelled");
	}
	QLOGDEC;
}

// copy button clicked in database panel
void MainForm::slot_database_copyClicked()
{
	if (!curDbase_) return;
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	QLOG("Database copy button clicked");
	QLOGINC;
	copyToAnotherDatabase(false);
	QLOGDEC;
}

// move button clicked in database panel
void MainForm::slot_database_moveClicked()
{
	if (!curDbase_) return;
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	QLOG("Database move button clicked");
	QLOGINC;
	copyToAnotherDatabase(true);
	QLOGDEC;
}

void MainForm::slot_database_editClicked()
{
	if (!curDbase_) return;
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	// setup database entry dialog, disable editing, enable combobox
	QLOG("Database edit button clicked");
	QLOGINC;
	// setup database entry dialog
	setupDbaseDialogCombo(true, true);
	dbaseDialogUI_.dbaseCombo->setEnabled(false);
	dbaseDialogUI_.entryEdit->setEnabled(true);
	dbaseDialogUI_.entryEdit->setFocus(Qt::OtherFocusReason);
	dbaseDialogUI_.descEdit->setEnabled(true);
	dbaseDialog_->setWindowTitle("Edit item");
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();

	if (dbIdxs.isEmpty())
	{
		QMessageBox::information(this, tr("Information"), tr("No entry selected in database for editing.\nPlease select something and try again"));
		QLOGDEC; return;
	}
	else if (dbIdxs.size() > 1)
	{
		QMessageBox::information(this, tr("Information"), tr("Cannot edit more than one entry at a time.\nPlease select single entry and try again"));
		QLOGDEC; return;
	}

	int idx = dbIdxs.first();
	QLOG("Selected index: " << idx);
    const DbEntry &e = curDbase_->entry(idx);

    dbaseDialogUI_.entryEdit->setText(e.item());
    dbaseDialogUI_.descEdit->setText(e.description());

    // pop dialog for user to enter changes and do stuff if ok pressed
	if (dbaseDialog_->exec() == QDialog::Accepted)
	{
        const QString
                newitem = dbaseDialogUI_.entryEdit->text(),
                newdesc = dbaseDialogUI_.descEdit->text();

		if (newitem.isEmpty() || newdesc.isEmpty())
		{
            QMessageBox::information(this, tr("Missing input"), tr("Database entries can't have empty fields."));
			return;
		}

		QLOG("User changes accepted");
        DbError error = curDbase_->alter(idx, newitem, newdesc);
        if (error == DbError::ERROR_DUPLI)
		{
			QMessageBox::StandardButton button;
            int dupidx = error.index();
			const DbEntry &dupEntry = curDbase_->entry(dupidx);
			button = QMessageBox::question(this, tr("Possible duplicate"), 
				tr("A similar entry was found to already exist in the database:\n'")
				+ dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1) 
				+ tr(").\nWhile trying to replace entry (") + QString::number(idx + 1) 
                + tr(") with:\n'") + newitem + "' / '" + newdesc
				+ tr("'\nDo you want to replace it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (button == QMessageBox::Yes)
			{
				QLOG("User wants to replace anyway");
                DbError error = curDbase_->alter(idx, newitem, newdesc, true);
                QLOG("Result: " << QString::number(error.type()));
                Q_ASSERT(error == DbError::ERROR_OK);
			}
			else
			{
				QLOG("User doesn't want duplicate");
				QLOGDEC;
				return;
			}
		}
	}
	else
	{
		QLOG("User changes rejected");
	}
	ui_.databaseTable->setFocus(Qt::OtherFocusReason);
	QLOGDEC;
}

void MainForm::slot_database_findClicked()
{
	if (!curDbase_) return;
	QLOG("Database find button clicked");
	QLOGINC;
	bool ok = false;
	QString text = QInputDialog::getText(this, tr("Find item in database"), tr("Please enter an expression to search for:"),
		QLineEdit::Normal, dbaseFindText_, &ok);

	if (ok && !text.isEmpty())
	{
		QLOG("Starting search for text: " << text);
		// store search text for next searches of the same item
		dbaseFindText_ = text;
		// get index of first selected row, or 0 (start of table) if none selected. search will start from this row.
		QList<int> dbIdxs = getSelectedDbaseTableIdxs();
		int idx = 0;
		if (!dbIdxs.empty()) idx = dbIdxs.first();
		// point to next row over selected one to enable for consecutive searches on the same term without getting stuck
		idx++;
		// if this will point to the last row in the table search from the top instead
		if (idx >= curDbase_->entryCount()) idx = 0;

        int found = curDbase_->findEntry(text, idx);

		// last resort to get a match if search started from somewhere in the table, not the beggining
		if (found < 0 && idx != 0)
		{
			QLOG("Last resort search from the top");
            found = curDbase_->findEntry(text, 0);
		}
		if (found < 0)
		{
			QLOG("Nothing found.");
			QMessageBox::information(this, tr("No results"), tr("Sorry, no results were found for '") + text + "'");

		}
		else
		{
			QModelIndex pidx = dbaseProxyModel_.mapFromSource(dbaseModel_->index(found, 0));
			QLOG("Found dbase idx: " << found << ", proxy: " << pidx.row());
			ui_.databaseTable->selectRow(pidx.row());
			ui_.databaseTable->setFocus(Qt::OtherFocusReason);
		}
	}
	QLOGDEC;
}

void MainForm::slot_database_exportClicked()
{
	QLOG("Database export button clicked");
	QLOGINC;
	Q_ASSERT(curDbase_ != NULL);
	QString fname = QFileDialog::getSaveFileName(this, tr("Export database"), QDir::homePath(), tr("HTML (*.htm *.html)"));
	if (fname.isEmpty())
	{
		QLOG("No file selected to save");
		return;
	}
	else
	{
		if (! (fname.endsWith(".htm", Qt::CaseInsensitive) || fname.endsWith(".html", Qt::CaseInsensitive))) fname.append(".html");
		QLOG("Exporting to file: '" << fname << "'");
		QList<int> idxs = getSelectedDbaseTableIdxs();
		bool ok = curDbase_->htmlExport(fname, idxs);
		if (!ok) QMessageBox::information(this, tr("Unable to export"), tr("Couldn't open file for writing: '") + fname + "'.");
	}
	QLOGDEC;
}

void MainForm::slot_database_resetClicked()
{
	QLOG("Database reset button clicked");
	QLOGINC;
	Q_ASSERT(curDbase_ != NULL);
	QMessageBox::StandardButton button = QMessageBox::warning(this, tr("Database reset"), tr("All quiz points, failure counters and test dates will be reset for database '") + curDbase_->name() 
		+ tr("'. Are you sure?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (button == QMessageBox::No)
	{
		QLOG("User canceled");
		return;
	}
	else 
	{
		QLOG("Proceeding to erase database contents");
		curDbase_->reset();
	}
	QLOGDEC;
}

void MainForm::slot_database_quizClicked()
{
	if (!curDbase_)
	{
		QMessageBox::information(this, tr("No database"), 
			tr("No user database exists. You must first create one\nand add items to it to run a quiz"));
		return;
	}
	if (curDbase_->locked()) { popDbaseLocked(); return; }
	QLOG("Database quiz button clicked");
	QLOGINC;

	QuizSettings qset;

	qset.type = curQuizType();

	qset.range = ui_.quizRangeCheck->isChecked();
	qset.rangeFrom = ui_.quizRangeFromSpin->value();
	qset.rangeTo = ui_.quizRangeToSpin->value();
	// range inside database index is taken care of automatically, can't select higher index in spinbox
	if (qset.rangeFrom >= qset.rangeTo) 
	{
		QMessageBox::information(this, tr("Bad range"), tr("Lower limit for range must be less than upper limit."));
		QLOGDEC;
		return;
	}

	qset.level = ui_.quizPointsCheck->isChecked();
	qset.levelFrom = ui_.quizPointsFromSpin->value();
	qset.levelTo = ui_.quizPointsToSpin->value();
	if (qset.levelFrom >= qset.levelTo) 
	{
		QMessageBox::information(this, tr("Bad range"), tr("Lower limit for points must be less than the upper limit."));
		QLOGDEC;
		return;
	}

	setQuizControlsEnabled(false);

	qset.take = ui_.quizTakeCheck->isChecked();
	qset.takeCount = ui_.quizTakeSpin->value();
	if      (ui_.quizTakeFailRadio->isChecked())   qset.takeMode = TAKE_FAILS;
	else if (ui_.quizTakeOldieRadio->isChecked())  qset.takeMode = TAKE_OLDIES;
	else if (ui_.quizTakeRandomRadio->isChecked()) qset.takeMode = TAKE_RANDOM;

	// retrieve selected indices from database table
	QList<int> selidxs = getSelectedDbaseTableIdxs();
	if (selidxs.count() > 1) 
	{
		QLOG("Adding quiz questions based on selection inside database table");
		quiz_ =  new Quiz(curDbase_, qset, selidxs, quizDialog_, &quizDialogUI_);
	}
	else
	{
		QLOG("Adding quiz questions based on selected criteria");
		quiz_ =  new Quiz(curDbase_, qset, quizDialog_, &quizDialogUI_);
	}
	connect(quiz_, SIGNAL(done()), this, SLOT(slot_quizDone()));
	quiz_->run();
	QLOGDEC;
}

void MainForm::slot_database_quizTypeChanged()
{
	QLOGX("Radio button selection changed in quiz type button group");
	QLOGINC;
	DbEntry::QuizDirection type = curQuizType();
	Q_ASSERT(dbaseModel_ != NULL);
	dbaseModel_->setQuizType(type);
	QLOGDEC;
}

// ==============================================================================================
// ============================== MainForm: public slots - settings
// ==============================================================================================

void MainForm::slot_settings_altdirCheckClicked(bool arg)
{
	QLOGX("Checkbox state changed to: " << arg);
	QLOGINC;
	if (!arg) 
	{
		QLOG("Clearing external directory path");
		extDir_.clear();
		ui_.settExtDirEdit->clear();
	}
	QLOGDEC;
}

void MainForm::slot_settings_altdirButtonClicked()
{
	QLOGX("User selecting directory");
	QLOGINC;
	QString dir = QFileDialog::getExistingDirectory(this, tr("Please select directory for databases"),
		(extDir_.isEmpty() ? QDir::homePath() : extDir_));
	if (dir.isEmpty())
	{
		QLOG("Dialog canceled, nothing done");
		QLOGDEC;
		return;
	}
	
	reloadExternalDbases(dir);

	QLOGDEC;
}

void MainForm::slot_settings_nodupCheckClicked()
{
	preventDuplicates_ = ui_.settNoDuplCheck->isChecked();
	applySettings();
}

void MainForm::slot_settings_switchPlugin(int plugIdx)
{
	QLOGX("Switching plugin in settings panel to: " << plugIdx);
	QLOGINC;

	DictionaryPlugin *plug = plugin(plugIdx);
	Q_ASSERT(plug);
	QLOG("Plugin: " << plug->name());
	const int stackIndex = plug->index();
	Q_ASSERT(stackIndex == plugIdx);
	Q_ASSERT(stackIndex >= 0 && stackIndex < ui_.settPluginStack->count());

	QStackedWidget *stack = ui_.settPluginStack;
	// set the size policy of the page being hidden to ignored so it doesn't influence the size
	// of the stacked widget anymore
	QWidget *curWidget = stack->currentWidget();
	if (curWidget)
	{
		curWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	}

	QLOG("Switching settings widget to index " << stackIndex);
	stack->setCurrentIndex(stackIndex);

	// set the size policy of the page that just became visible to make the widget adjust its size to it
	curWidget = stack->currentWidget();
	Q_ASSERT(curWidget);
	curWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	stack->adjustSize();

	QLOGDEC;
}

// ==============================================================================================
// ============================== MainForm: public slots - other
// ==============================================================================================

/// This slot is called by the background setup thread when it is done.
void MainForm::slot_setupDone()
{
    QLOGX("Setup done, will enable interface");
	QLOGINC;

	ui_.statusBar->clearMessage();

	applySettings();
	// connect here, because the state is changed earlier based on the value of altdir from settings
	// todo: use blockSignals?
	connect(ui_.settExtDirCheck, SIGNAL(clicked(bool)), this, SLOT(slot_settings_altdirCheckClicked(bool)));
	ui_.settExtDirEdit->setText(extDir_);
	if (!extDir_.isEmpty()) ui_.settExtDirCheck->setChecked(true);

	// delete the background thread, clear the status bar from progress information, 
	delete setupThread_;
	setupThread_ = NULL;

	verifyPlugins();

	// enable stack widget buttons
	ui_.mainFrame->setEnabled(true);
	ui_.databaseCombo->setEnabled(false);

	QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
	header->blockSignals(true);
	// create dictionary table model
	dictModel_ = new DictionaryModel();
	ui_.dictionaryTable->setModel(dictModel_);
	dictModel_->setSource(curPlugin_);
	header->blockSignals(false);
	if (curPlugin_)	curPlugin_->activate();

	// create database table model
	dbaseModel_ = new DatabaseModel();
	dbaseModel_->setQuizType(curQuizType());
	dbaseProxyModel_.setSourceModel(dbaseModel_);
	dbaseProxyModel_.setSortRole(Qt::UserRole);
	ui_.databaseTable->setModel(&dbaseProxyModel_);

    // reparent the database items to the main window, this causes events to get sent so it can't
    // be done from the setup thread
    for (auto dbase : databases_) {
        if (!dbase->parent()) dbase->setParent(this);
    }
    // fill database selection combo box with databases created from user xml files on startup
	updateDbaseCombo(true);

	QLOGDEC;
}

void MainForm::slot_quizDone()
{
	QLOGX("Quiz done");
	QLOGINC;

	setQuizControlsEnabled(true);

	delete quiz_;
	quiz_ = NULL;
	QLOGDEC;
}

void MainForm::slot_updateSetupMsg(const QString &msg)
{
	ui_.statusBar->showMessage(msg);
}

void MainForm::slot_testClicked()
{
	ui_.databaseTable->resizeColumnsToContents();
	ui_.databaseTable->resizeRowsToContents();

	ui_.dictionaryTable->resizeColumnsToContents();
	ui_.dictionaryTable->resizeRowsToContents();
}

// ==============================================================================================
// ============================== MainForm: protected
// ==============================================================================================

void MainForm::clearPluginStacks(bool destroy)
{
	QLOGX("Clearing contents of dictionary and settings stack widgets for all plugins, destroy: " << destroy);

	for (int i = ui_.dictPanelStack->count(); i >= 0; --i)
	{
		QWidget *dictWidget = ui_.dictPanelStack->widget(i);
		ui_.dictPanelStack->removeWidget(dictWidget);
		if (destroy) delete dictWidget;
	}

	for (int i = ui_.settPluginStack->count(); i >= 0; --i)
	{
		QWidget *settWidget = ui_.settPluginStack->widget(i);
		ui_.settPluginStack->removeWidget(settWidget);
		if (destroy) delete settWidget;
	}
}

void MainForm::loadPlugins()
{
	QLOGX("Loading dictionary plugins");
	QLOGINC;

	QDir curDir;
    const QStringList pluginFilter(PLUG_PREFIX + PLUG_WILDCARD + PLUG_EXT); 
	QStringList pluginFiles = curDir.entryList(pluginFilter, QDir::Files);
	QLOG("Found plugins: " << pluginFiles.size());

	Q_ASSERT(plugins_.isEmpty());
	for (int i = 0; i < pluginFiles.size(); ++i)
	{
		const QString &pluginFile = pluginFiles.at(i);
		QLOG("Loading: '" << pluginFile << "'");
		QPluginLoader *loader = new QPluginLoader(pluginFile);
		DictionaryPlugin *plug = qobject_cast<DictionaryPlugin*>(loader->instance());
		if (plug)
		{
			QLOG("Loaded OK, adding to list");
			plugins_.append(plug);
			pluginLoaders_.append(loader);
			QLOG("Performing pre-setup and intercepting log output");
			plug->preSetup(this, APP_LOGSTREAM);
		}
		else
		{
            QLOG("Loading failed: '" << loader->errorString() << "'");
			delete loader;
		}
	}

	QLOGDEC;
}

void MainForm::verifyPlugins()
{
	QLOGX("Verifying plugins, count: " << plugins_.size());
	QLOGINC;

	ui_.langCombo->blockSignals(true);
	ui_.langCombo->clear();
	ui_.settPluginCombo->blockSignals(true);
	ui_.settPluginCombo->clear();

	// iterate over plugins in reverse order
	for (int i = pluginCount() - 1; i >= 0; --i)
	{
		DictionaryPlugin *plug = plugin(i);
		QPluginLoader *loader = pluginLoader(i);
		Q_ASSERT(plug && loader);

		QLOG("Plugin " << i << " '" << plug->name() << "'");
        QLOGINC;

		// remove plugins which are invalid after their setup() was completed by the SetupThread
		if (!plug->ok()) 
        {
            QLOG("Plugin unusable, destroying");
			loader->unload();
			delete loader;
			plugins_.removeAt(i); 
			pluginLoaders_.removeAt(i); 
            QLOGDEC;
			continue; 
		}

		// perform optional additional setup steps
		QLOG("Plugin OK, performing post-setup");
		plug->postSetup();

		// remove plugins which are invalid after postSetup();
		if (!plug->ok()) 
        {
            QLOG("Post-setup failed, destroying after all");
			loader->unload();
			delete loader;
			plugins_.removeAt(i); 
			pluginLoaders_.removeAt(i); 
            QLOGDEC;
			continue; 
		}

        QLOGDEC;
	}

	// No language plugins available, hide dictionary panel
	if (!pluginCount())
	{
		ui_.dictPanelStack->hide();
		ui_.settPluginStack->hide();

		ui_.langCombo->setEnabled(false);
		ui_.langCombo->addItem("None available");
		ui_.langCombo->setCurrentIndex(0);

		ui_.settPluginCombo->setEnabled(false);
		ui_.settPluginCombo->addItem("None available");
		ui_.settPluginCombo->setCurrentIndex(0);

		ui_.langAboutButton->setEnabled(false);
	}
	// otherwise go through valid plugins in order, set their internal indices and add them to the GUI
	else 
	{
		for (int i = 0; i < pluginCount(); ++i)
		{
			DictionaryPlugin *plug = plugin(i);
			// if plugin still ok at this point, add it to the GUI.
			QLOG("Plugin '" << plug->name() << "' still OK, adding to interface");
			plug->setIndex(i);

			QWidget 
				*dictUI = plug->dictionaryUI(),
				*settUI = plug->settingsUI();

			if (dictUI)
			{
				dictUI->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
				ui_.langCombo->addItem(plug->language());
				ui_.dictPanelStack->addWidget(dictUI);
			}

			if (settUI)
			{
				dictUI->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
				ui_.settPluginCombo->addItem(plug->language());
				ui_.settPluginStack->addWidget(settUI);
			}
		}

		// force dictionary widget switch to first available language
		// todo: remember current plugin on exit and restore, not hard to 0
		ui_.langCombo->setCurrentIndex(0);
		ui_.settPluginCombo->setCurrentIndex(0);
		ui_.dictPanelStack->setCurrentIndex(0);
		ui_.settPluginStack->setCurrentIndex(0);

		// restore size policies of the currently visible widgets
		if (ui_.dictPanelStack->currentWidget())
		{
			ui_.dictPanelStack->currentWidget()->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		}

		if (ui_.settPluginStack->currentWidget())
		{
			ui_.settPluginStack->currentWidget()->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		}

		curPlugin_ = plugin(0);
		Q_ASSERT(curPlugin_);
		//curPlugin_->activate(); // done later, in setupDone()
	}

	ui_.langCombo->blockSignals(false);
	ui_.settPluginCombo->blockSignals(false);

	QLOGDEC;
}

void MainForm::loadSettings()
{
	QLOGX("Reading stored program settings");
	QLOGINC;

	QSettings settings;

	extDir_ = settings.value(SETT_EXTDIR, "").toString();
	QLOG("External directory for databases: '" << extDir_ << "'");

	preventDuplicates_ = settings.value(SETT_NODUPS, true).toBool();
	QLOG("Prevent database duplicates flag: " << preventDuplicates_);

	firstDbase_ = settings.value(SETT_CURDB, "").toString();
	QLOG("Current database to set: '" << firstDbase_ << "'");

	QLOGDEC;
}

void MainForm::saveSettings()
{
	QLOGX("Saving program settings to permanent storage");
	QSettings settings;
	settings.setValue(SETT_EXTDIR, extDir_);
	settings.setValue(SETT_NODUPS, preventDuplicates_);
	settings.setValue(SETT_CURDB, ui_.databaseCombo->currentText());
}

void MainForm::applySettings()
{
	QLOG("Applying settings");
	QLOGINC;

	ui_.settNoDuplCheck->setChecked(preventDuplicates_);

	QLOGDEC;
}

void MainForm::reloadExternalDbases(const QString &newdir)
{
	QLOGX("Scanning directory '" << newdir << "' for databases");
	QLOGINC;
	QLOG("Looking for databases from old directory ('" << extDir_ << "')");

	ui_.databaseCombo->setEnabled(false);
	ui_.createButton->setEnabled(false);
	ui_.deleteButton->setEnabled(false);
	QList<int> idxs;
	// iterate over databases, removing ones from old dir
	for (int i = 0; i < databases_.size(); ++i)
	{
		Database *dbase = databases_.at(i);
		// found database from old user dir
		if (dbase->external())
		{
			QLOG("Database '" << dbase->name() << "' needs to be removed");
			QLOGINC;
			idxs.append(i);
			QLOGDEC;
		} 
	}

	QLOG("Removing " << idxs.size() << " databases");
	ui_.databaseCombo->blockSignals(true);
	int idx;
	// iterate over indices of databases scheduled for deletion, backward
	for (int i = idxs.size()-1; i >= 0; --i)
	{
		idx = idxs.at(i);
		Database *dbase = databases_.at(idx);
		QLOG("Removing database: '" << dbase->name() << ", idx: " << idx);
		// need to save before removal
		if (dbase->dirty())
		{
			QLOG("Database needs saving");
			int answer = QMessageBox::question(this, tr("Question"), 
				tr("Do you want to save changes to the database '") + dbase->name() + "'?",
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
			
			if (answer == QMessageBox::Yes) 
			{
				const QString xmlpath = extDir_ + "/" + dbase->name() + Database::XML_EXT;
				QLOG("Saving to '" << xmlpath << "'");
				dbase->saveXML(xmlpath);
			}
			else if (answer == QMessageBox::Cancel) 
			{
				QLOG("Terminating by user's request");
				return;
			}
		}

		// remove database object, handle in list and entry in combobox
		databases_.removeAt(idx);
		int cidx = findComboIndex(ui_.databaseCombo, dbase->name());
		if (cidx >= 0) ui_.databaseCombo->removeItem(cidx);
		delete dbase;
	}

	ui_.statusBar->showMessage("Loading user databases...");
    QList<Database*> dbs = SetupThread::loadDbases(newdir, true);
    databases_.append(dbs);
	ui_.statusBar->clearMessage();
	updateDbaseCombo(false);
	extDir_ = newdir;
	ui_.settExtDirEdit->setText(newdir);

	ui_.databaseCombo->setEnabled(true);
	ui_.createButton->setEnabled(true);
	ui_.deleteButton->setEnabled(true);
	ui_.databaseCombo->blockSignals(false);
	QLOGDEC;
}

void MainForm::updateDbaseCombo(bool restore)
{
	int selidx = 0;
	ui_.databaseCombo->clear();
    if (databases_.empty())
    {
        QLOGX("Database combo is empty");
        ui_.databaseCombo->setEnabled(false);
        return;
    }

    QLOGX("Filling combo box with data for " << databases_.size() << " disk databases");
    QLOGINC;
    ui_.databaseCombo->blockSignals(true);
    for (int i = 0; i < databases_.size(); ++i)
    {
        Database *dbase = databases_[i];
        ui_.databaseCombo->addItem(dbase->name());
        if (dbase->name() == firstDbase_) selidx = i;
    }
    // restore stored default database index from settings if requested. Can't to this reliably when changing the user dir,
    // only on startup
    if (restore)
    {
        ui_.databaseCombo->setCurrentIndex(selidx);
        QLOG("Set current index to " << selidx << " (" << ui_.databaseCombo->currentText() << ")");
    }
    // have to invoke manually in case stored index was 0 (treated as no change and hence no signal emitted)
    slot_database_changeCurrent(ui_.databaseCombo->currentText());

    ui_.databaseCombo->setEnabled(true);
    ui_.databaseCombo->blockSignals(false);
    QLOGDEC;
}

/// Find index of a given text within a combo box. Returns -1 if not found.
int MainForm::findComboIndex(const QComboBox *combo, const QString &text)
{
	int ret = -1;
	for (int i = 0; i < combo->count(); ++i)
	{
		if (combo->itemText(i) == text) return i;
	}
	return ret;
}

void MainForm::setupDbaseDialogCombo(bool includeCurrent, bool selCurrent)
{
	dbaseDialogUI_.dbaseCombo->clear();
	if (!curDbase_) return;
	QString curname = curDbase_->name();
	// iterate over items in top-level database combo box
	for (int i = 0; i < ui_.databaseCombo->count(); ++i)
	{
		QString name = ui_.databaseCombo->itemText(i);
		// don't add current database name if flag set, otherwise add all
		if (includeCurrent || name != curname)
			dbaseDialogUI_.dbaseCombo->addItem(name);
	}

	// if flag set, point combo box to current database, otherwise remove selection
	if (includeCurrent && selCurrent)
	{
		dbaseDialogUI_.dbaseCombo->setCurrentIndex(ui_.databaseCombo->currentIndex());
	}
	else
	{
		dbaseDialogUI_.dbaseCombo->setCurrentIndex(0);
	}
}

QList<int> MainForm::getSelectedDbaseTableIdxs()
{
	QModelIndexList allIdxs = ui_.databaseTable->selectionModel()->selectedRows();
	QLOGX("all indexes: " << allIdxs.count());
	QLOGINC;

	QList<int> rowIdxs;
	int dbaseidx;

	for (int i = 0; i < allIdxs.size(); ++i)
	{
		const QModelIndex &idx = allIdxs.at(i);
		QLOG("row: " << idx.row() << ", col: " << idx.column());
		if (idx.column() == 0) 
		{
			dbaseidx = dbaseModel_->data(dbaseProxyModel_.mapToSource(idx), Qt::UserRole).toInt();
			QLOG("New dbase idx: " << dbaseidx);
			rowIdxs.append(dbaseidx);
		}
	}

	QLOGDEC;
	return rowIdxs;
}

// the move operation is almost identical to the copy operation, hence it makes sense to
// unite and export them here
void MainForm::copyToAnotherDatabase(bool move)
{
	// setup database entry dialog, disable editing, enable combobox
	setupDbaseDialogCombo(false, false);
	dbaseDialogUI_.dbaseCombo->setEnabled(true);
	dbaseDialogUI_.entryEdit->setEnabled(false);
	dbaseDialogUI_.descEdit->setEnabled(false);
	if (move)
	{
		dbaseDialog_->setWindowTitle("Move item(s) to another database");
	}
	else
	{
		dbaseDialog_->setWindowTitle("Copy item(s) to another database");
	}

	// retrieve selected indexes (may copy in bulk)
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();
	// return if no selection
	if (dbIdxs.isEmpty()) 
	{
		QLOG("Nothing selected, done");
		return;
	}

	QLOGNL("Selected rows (" << dbIdxs.size() << "): ");
	for (int i = 0; i < dbIdxs.size(); ++i)
	{
		QLOGRAW(dbIdxs.at(i) << " ");
	}
	QLOGRAW(endl);

	if (curDbase_ != NULL)
	{
		// if only one item selected, fill dialog with its data
		if (dbIdxs.size() == 1)
		{
			QLOG("Single item mode");
			const DbEntry &entry = curDbase_->entry(dbIdxs.at(0));
			dbaseDialogUI_.entryEdit->setText(entry.item());
			dbaseDialogUI_.descEdit->setText(entry.description());
		}
		// otherwise indicate multiple selections
		else
		{
			QLOG("Multiple items mode");
			dbaseDialogUI_.entryEdit->setText(tr(" -- multiple -- "));
			dbaseDialogUI_.descEdit->setText(tr(" -- multiple -- "));
		}
		// pop dialog and continue if ok clicked
		if (dbaseDialog_->exec() == QDialog::Accepted)
		{
			// find target database
            const QString comboText = dbaseDialogUI_.dbaseCombo->currentText();
            Database *target = database(comboText);
			QLOG("Copying " << dbIdxs.size() <<  " item(s) to database '" 
				<< target->name() << "'" );

            // bulk copy items to target database from current database
            // does it even make sense to have this function? perhaps, if DbError
            // held all the error indices, it would be possible to add in bulk and
            // then process the remainder as per user answer(s)
            //DbError error = target->add(curDbase_, dbIdxs);

            int answer = QMessageBox::NoButton;
			QList<int> okIdxs;
			for (int i = 0; i < dbIdxs.size(); ++i)
			{
				const DbEntry &entry = curDbase_->entry(dbIdxs.at(i));
                const QString
                        itemText = entry.item(),
                        descText = entry.description();

				// if go-ahead given, ignore duplicates
				if (answer == QMessageBox::YesToAll)
				{
                    DbError error = target->add(itemText, descText, true);
                    Q_ASSERT(error == DbError::ERROR_OK);
					if (move) okIdxs.append(dbIdxs.at(i));
				}
                // don't ignore duplicates (yet)
				else
				{
                    DbError error = target->add(itemText, descText, false);
					// check error type, 
                    if (error == DbError::ERROR_OK)
					{
						// in move mode, add index to successfully copied list for later removal from source
						if (move) okIdxs.append(dbIdxs.at(i));
						continue;
					}
					// if duplicate detected, warn
                    else if (error == DbError::ERROR_DUPLI && answer != QMessageBox::NoToAll)
					{
                        const DbEntry &dupEntry = target->entry(error.index());
						answer = QMessageBox::information(this, tr("Duplicate entry"), 
                            tr("Possible duplicate found for '") + entry.item() + "' (" + entry.description() +
                            tr(") in target database: '") + dupEntry.item() + "' (" + dupEntry.description() +
                            tr("). Continue?"),
							QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
					}
					// retry item add with duplicates ignored on "yes" or "yes to all"
					if (answer == QMessageBox::Yes || answer == QMessageBox::YesToAll)
					{
                        error = target->add(itemText, descText, true);
                        Q_ASSERT(error == DbError::ERROR_OK);
						if (move) okIdxs.append(dbIdxs.at(i));
					}
				}
			}


			if (move) 
			{
				QLOG("Removing item(s) from source database");
                curDbase_->remove(okIdxs);
			}
		}
		else
		{
			QLOG("User cancelled operation");
		}
	}
}

void MainForm::popDbaseLocked()
{
    QMessageBox::information(this, tr("Database locked"), tr("The current database is being used and cannot be altered right now."));
}

void MainForm::addToDatabase(const QString &item, const QString &desc)
{
    QLOG("Received database candidate, item: " << item << ", desc: " << desc);

    // fill add dialog's combo box with currently existing databases
    setupDbaseDialogCombo(true, true);
    dbaseDialogUI_.entryEdit->setText(item);
    dbaseDialogUI_.descEdit->setText(desc);
    dbaseDialogUI_.dbaseCombo->setEnabled(true);
    dbaseDialog_->setWindowTitle("Add item to database");

    QLOG("Showing dialog for adding entry to database (current: " << curDbase_->name() << ")" );

    // pop dialog and block waiting for answer, continue if OK pressed
    if (dbaseDialog_->exec() != QDialog::Accepted) {
        QLOG("Dialog was canceled!");
        return;
    }

    // retrieve input from dialog (entry, description, selected database)
    const QString
            userItem  = dbaseDialogUI_.entryEdit->text().simplified(),
            userDesc  = dbaseDialogUI_.descEdit->text().simplified(),
            comboText = dbaseDialogUI_.dbaseCombo->currentText();

    // guard against empty input on line edits
    if (userItem.isEmpty() || userDesc.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Entry and Description fields may not be empty!");
        return;
    }

    Database *dbase = database(comboText);
    if (dbase == nullptr) {
        QLOG("Could not find handle to database!");
        return;
    }

    QLOG("Adding entry to database '" << dbase->name() << "' (combo: " << comboText
        << "), item: '" << userItem << "', description: '" << userDesc << "'" );

    auto error = dbase->add(userItem, userDesc);

    if (error == DbError::ERROR_DUPLI)
    {
        // todo: could be duplicate of multiple items, show all of them in the dialog
        int dupidx = error.index();
        const DbEntry &dupEntry = dbase->entry(dupidx);
        const auto button = QMessageBox::question(this, tr("Possible duplicate"),
            tr("A similar entry was found to already exist in the database:\n'")
            + dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1)
            + tr(").\nWhile trying to add entry:\n'") + userItem + "' / '" + userDesc
            + tr("'\nDo you want to add it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            QLOG("User wants to add anyway");
            error = dbase->add(userItem, userDesc, true);
            QLOG("Result: " << QString::number(error.type()));
            // todo handle nested error
            if (error != DbError::ERROR_OK) {
                QLOG("Unable to add to database even with duplicates ignored?! (" << error.msg() << ")");
                return;
            }
        }
        else
        {
            QLOG("User doesn't want duplicate");
            return;
        }
    }
}

void MainForm::setQuizControlsEnabled(bool arg)
{
	QLOGX("Setting enabled state of quiz controls to: " << arg);

	ui_.databaseCombo->setEnabled(arg);
	ui_.createButton->setEnabled(arg);
	ui_.deleteButton->setEnabled(arg);

	ui_.quizDirectionGroupBox->setEnabled(arg);
	ui_.quizCriteriaGroupBox->setEnabled(arg);
	ui_.quizButton->setEnabled(arg);
}

DbEntry::QuizDirection MainForm::curQuizType() const
{
	return (ui_.quizShowDescRadio->isChecked() ? DbEntry::DIR_SHOWDESC : DbEntry::DIR_SHOWITEM);
}

void MainForm::closeEvent(QCloseEvent *event)
{
	QLOG("Main window close event intercepted");
	QLOGINC;
	// iterate over user databases
	if (setupThread_)
	{
		QLOG("Setup thread is active, can't exit");
		event->ignore();
		return;
	}

	for (int i = 0; i < databases_.size(); ++i)
	{
		Database *dbase = databases_[i];
		Q_ASSERT(dbase != NULL);
		// if a database has been altered, pop question dialog asking for save confirmation
		if (dbase->dirty())
		{
			QLOG("Database '" << dbase->name() 
				<< "' not synchronized, popping dialog for save confirmation" );
			int button = QMessageBox::question(this, "Question", 
				QString("Do you want to save changes to the database '") 
					+ dbase->name() + "'?", 
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
			// save if yes pushed
			if (button == QMessageBox::Yes) 
			{
				const QString xmlpath = (dbase->external() ? extDir_ : APPDIR) + "/" + dbase->name() + Database::XML_EXT;
				QLOG("Saving database to '" << xmlpath << "'");
				dbase->saveXML(xmlpath);
			}
			// if cancel pushed, ignore close event and return
			else if (button == QMessageBox::Cancel)
			{
				QLOG("Close interrupt requested, ignoring event");
				QLOGDEC;
				event->ignore();
				return;
			}
			else if (button == QMessageBox::No)
			{
				QLOG("User declined, database will not be saved");
			}
		}
	}
	// accept the event, close application
	QLOG("Accepting close event");
	QLOGDEC;
	event->accept();
}

// ==============================================================================================
// ============================== QuizDialogEventFilter
// ==============================================================================================

QuizDialogEventFilter::QuizDialogEventFilter(QDialog *parent, Ui::QuizDialog *dialogUI) : QObject(parent), 
	parent_(parent), dialogUI_(dialogUI)
{
}

bool QuizDialogEventFilter::eventFilter(QObject *obj, QEvent *event)
{
	// keyboard shortcuts for dialog buttons and ignore dialog close with escape
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		int key = keyEvent->key();
		if (key == Qt::Key_A) { dialogUI_->correctButton->animateClick(); return true; }
		else if (key == Qt::Key_S) { dialogUI_->incorrectButton->animateClick(); return true; }
		else if (key == Qt::Key_D) { dialogUI_->unsureButton->animateClick(); return true; }
		else if (key == Qt::Key_X) { dialogUI_->revealButton->animateClick(); return true; }
		else if (key == Qt::Key_Escape) { return true; }
	}
	// finish quiz on dialog close, unfortunately this can't be ignored
	else if (event->type() == QEvent::Close)
	{
		dialogUI_->closeButton->animateClick();
		return true;
	}
	return QObject::eventFilter(obj, event);
}
