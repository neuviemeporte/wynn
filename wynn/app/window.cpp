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
#include "db/database.h"
#include "db/model.h"
#include "db/quiz.h"
#include "ixplog_active.h"

QWidget *GUI;
QTSLogger *APP_LOGSTREAM = nullptr;

using namespace wynn::db;

namespace wynn {
namespace app {

const QString 
    MainForm::VERSION     = "0.9.6",
	MainForm::APPDIR      = QDir::homePath() + "/.wynn",
	MainForm::SETT_EXTDIR = "extdir", 
	MainForm::SETT_NODUPS = "nodbdups", 
	MainForm::SETT_CURDB  = "curdb";

namespace {
const QString PLUG_WILDCARD = "wynnplugin-*";
#if defined(__linux__)
const QString PLUG_PREFIX = "lib";
const QString PLUG_EXT = ".so";
#elif defined(_WIN32)
const QString PLUG_PREFIX = "";
const QString PLUG_EXT = ".dll";
#else
#error Unrecognized target platform!
#endif
// for showing transient messages in the status bar
const int MSG_TIMEOUT = 2000;
} // namespace

// ==============================================================================================
// ============================== MainForm: public
// ==============================================================================================

MainForm::MainForm(QWidget *parent) : QMainWindow(parent),
    curPlugin_(nullptr),
    curDbase_(nullptr), 
    dbaseModel_(nullptr), 
    dbaseFindText_(""),
    dictModel_(nullptr),
    dbaseDialog_(nullptr), 
    quizDialog_(nullptr),
    setupThread_(nullptr), 
    quiz_(nullptr)
{
	// open debug log
	APP_LOGSTREAM = new QTSLogger("wynn_log.txt");
	QLOG("Main window constructor is active");
	QLOGINC;

	QLOG("Setting up interface");
	ui_.setupUi(this);
    ui_.stackedWidget->setCurrentIndex(3);
	GUI = this;
    
#ifdef _WIN32
#elif defined (__linux__) 
    // remove text captions on database manipulation buttons on Linux, use theme icons only
    ui_.createButton->setText("");
    ui_.saveButton->setText("");
    ui_.deleteButton->setText("");
#endif

    ui_.versionLabel->setText(ui_.versionLabel->text().arg(VERSION));

	QString build = ui_.buildLabel->text() + "\nBuild: " + __DATE__ + " " + __TIME__;
#ifdef DEBUG
	build += " Debug";
#endif
#if defined(_WIN64) || defined(__x86_64__)
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
    quizDialog_->installEventFilter(new QuizDialogEventFilter(quizDialog_, &quizDialogUI_));
    connect(quizDialogUI_.correctButton,   SIGNAL(clicked()), this, SLOT(slot_quiz_correctClicked()));
    connect(quizDialogUI_.incorrectButton, SIGNAL(clicked()), this, SLOT(slot_quiz_incorrectClicked()));
    connect(quizDialogUI_.unsureButton,    SIGNAL(clicked()), this, SLOT(slot_quiz_unsureClicked()));
    connect(quizDialogUI_.revealButton,    SIGNAL(clicked()), this, SLOT(slot_quiz_revealClicked()));
    connect(quizDialogUI_.closeButton,     SIGNAL(clicked()), this, SLOT(slot_quiz_closeClicked()));

	QLOG("Connecting signals");
	// Main stack widget page changing buttons
	connect(ui_.dictionaryButton, SIGNAL(clicked()),                           this, SLOT(slot_dictionaryButtonClicked()));
	connect(ui_.manageButton,     SIGNAL(clicked()),                           this, SLOT(slot_manageButtonClicked()));
	connect(ui_.settingsButton,   SIGNAL(clicked()),                           this, SLOT(slot_settingsButtonClicked()));
	connect(ui_.aboutButton,      SIGNAL(clicked()),                           this, SLOT(slot_aboutButtonClicked()));
	connect(ui_.createButton,     SIGNAL(clicked()),                           this, SLOT(slot_database_createClicked()));
    connect(ui_.saveButton,       SIGNAL(clicked()),                           this, SLOT(slot_database_saveClicked()));
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
    connect(ui_.quizButton,        SIGNAL(clicked()),                      this, SLOT(slot_quiz_clicked()));
    connect(ui_.quizShowDescRadio, SIGNAL(clicked()),                      this, SLOT(slot_quiz_typeChanged()));
    connect(ui_.quizShowItemRadio, SIGNAL(clicked()),                      this, SLOT(slot_quiz_typeChanged()));
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
    if ( name.isEmpty() ) return nullptr;
    QLOG("Getting database handle for name: '" << name << "'");
    for ( auto db : databases_ ) 
        if ( db->name() == name ) return db;
    return nullptr;
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
    // when results present, shift keyboard focus to results table
    if ( dictModel_->rowCount(QModelIndex()) ) {
        ui_.dictionaryTable->selectRow(0);
        ui_.dictionaryTable->setFocus(Qt::OtherFocusReason);
    }
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
    QLOGX("Current database changed to: '" << text << "'");
    
    // enable or disable ui elements depending on whether the dbase combo box is empty or not
    const bool haveItem = !text.isEmpty();
    for ( auto widget : std::vector<QWidget*>{ 
          ui_.databaseCombo, ui_.saveButton, ui_.deleteButton, ui_.dbaseAddButton, ui_.dbaseRemoveButton, ui_.dbaseCopyButton, 
          ui_.dbaseMoveButton, ui_.dbaseEditButton, ui_.dbaseFindButton, ui_.dbaseExportButton, ui_.dbaseResetButton, ui_.quizButton })
    {
        widget->setEnabled( haveItem );
    }
            
    // in case the selection became empty (last database removed),
    // set current dbase handle to null and disconnect signals from model to database
    curDbase_ = database(text);
	dbaseModel_->setDatabase(curDbase_);
    
    // update range spinboxes in quiz panel
	dbaseCountUpdate();
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

void MainForm::slot_database_saveClicked() {
    QLOG("Database save button clicked");
    if (!curDbase_ ) {
        QLOGX("Current database is null!");
        return;
    }
    
    auto path = dbaseSavePath(curDbase_);
    curDbase_->saveXML(path);
    const auto msg = tr("Database saved to ") + path 
            + " (" + QString::number(curDbase_->entryCount()) + tr(" items)");
    statusMessage(msg, MSG_TIMEOUT);
}

void MainForm::slot_database_deleteClicked()
{
    QLOG("Database delete button clicked");
	Database *dbase = curDbase_;
	if (!dbase) {
        QLOGX("Current database is nullptr!");
        return;
    }

	if (QMessageBox::question(this, "Question", 
            tr("Are you sure you want to delete the database '") + dbase->name() + "'?", 
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) 
    {
        QLOGX("User declined database deletion");
        return;
    } 

    const int idx = ui_.databaseCombo->currentIndex();
	QLOG("Removing database, index: " << idx << ", name: " << dbase->name() << " (combo text: " << ui_.databaseCombo->currentText() << ")");
    Q_ASSERT(ui_.databaseCombo->currentText() == dbase->name());

	const QString xmlpath = dbaseSavePath(dbase);
	QLOG("Removing data file '" << xmlpath << "'");
	QFile file(xmlpath);
	if (!file.remove())	{
		QLOG("Unable to remove file! (Error: " << file.error() << ")");
	}

    // remove item from dbase selection combobox, triggers slot_database_changeCurrent()
	ui_.databaseCombo->removeItem(idx);

    // finally, destroy database object    
	databases_.removeAt(idx);
    delete dbase;
}

// add buton clicked in database panel
void MainForm::slot_database_addToClicked() {
    QLOG("Database Add button clicked");
	if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    }
    
	// setup database entry dialog, point to current database in combobox
	setupDbaseDialogCombo(true, true);
	dbaseDialogUI_.dbaseCombo->setEnabled(false);
	dbaseDialogUI_.entryEdit->clear();
	dbaseDialogUI_.descEdit->clear();
	dbaseDialogUI_.entryEdit->setEnabled(true);
	dbaseDialogUI_.entryEdit->setFocus(Qt::OtherFocusReason);
	dbaseDialogUI_.descEdit->setEnabled(true);
	dbaseDialog_->setWindowTitle(tr("Add item to current database"));
    
	// pop dialog and block, continue if accepted
    if ( dbaseDialog_->exec() != QDialog::Accepted ) {
        QLOG("User declined entry add dialog");
        return;
    }
        
    QString entry = dbaseDialogUI_.entryEdit->text().simplified();
    QString desc = dbaseDialogUI_.descEdit->text().simplified();
    Database *dbase = curDbase_;
    
    // empty input doesn't make sense
    if (entry.isEmpty() || desc.isEmpty()) {
        QMessageBox::information(this, tr("Missing input"), tr("Database items can't be empty!"));
        QLOG("Missing data in user input, done");
        return;
    }
    
    QLOG("Adding entry to database '" << dbase->name() << ", text: '" << entry << "', description: '" << desc << "'" );
    Error error = dbase->add(entry, desc);
    
    if (error == Error::DUPLI) { 
        int dupidx = error.index();
        const Entry &dupEntry = dbase->entry(dupidx);
        const auto button = QMessageBox::question(this, tr("Possible duplicate"), 
            tr("A similar entry was found to already exist in the database:\n'") 
            + dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1) 
            + tr(").\nWhile trying to add entry:\n'") + entry + "' / '" + desc 
            + tr("'\nDo you want to add it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        
        if ( button == QMessageBox::Yes ) {
            QLOG("User wants to add anyway");
            error = dbase->add(entry, desc, {}, true);
            QLOG("Result: " << QString::number(error.type()));
        }
        else {
            QLOG("User doesn't want duplicate");
            return;
        }
    }
    
    if ( error == Error::OK ) {
        ui_.databaseTable->setFocus(Qt::OtherFocusReason);
        ui_.databaseTable->selectRow(curDbase_->entryCount()-1);
        dbaseCountUpdate();
    }
}

// remove button clicked in database panel
void MainForm::slot_database_removeFromClicked()
{
    QLOG("Database remove button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    }
    
	// retrieve selected indexes
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();

	// return if no selection
	if (dbIdxs.isEmpty()) {
		QLOG("Nothing selected, done");
		return;
	}

	QLOG("Selected item indices (" << dbIdxs.size() << "): " << dbIdxs);

	// ask for confirmation
	const auto button = QMessageBox::question(this, 
        tr("Remove item(s)"), 
		tr("Are you sure you want to remove ") 
			+ (dbIdxs.size() > 1 ? tr("these ") + QString::number(dbIdxs.size()) + tr(" items?") : tr("this item?")),
		QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
	// remove items
	if (button == QMessageBox::Yes)	{
		Q_ASSERT(curDbase_ != nullptr);
        curDbase_->remove(dbIdxs);
		ui_.databaseTable->setFocus(Qt::OtherFocusReason);
		// TODO:
		//ui_.databaseTable->selectRow(Idxs.first());
		QLOG("Entries removed");
        dbaseCountUpdate();
	}
	else {
		QLOG("Action cancelled");
	}
}

// copy button clicked in database panel
void MainForm::slot_database_copyClicked()
{
    QLOG("Database copy button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    }
    
	copyToAnotherDatabase(false);
}

// move button clicked in database panel
void MainForm::slot_database_moveClicked()
{
    QLOG("Database move button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    }

	copyToAnotherDatabase(true);
}

void MainForm::slot_database_editClicked()
{
    QLOG("Database edit button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	if ( curDbase_->locked() )  { 
        popDbaseLocked(); 
        return; 
    }
    
	// setup database entry dialog, disable editing, enable combobox
	setupDbaseDialogCombo(true, true);
	dbaseDialogUI_.dbaseCombo->setEnabled(false);
	dbaseDialogUI_.entryEdit->setEnabled(true);
	dbaseDialogUI_.entryEdit->setFocus(Qt::OtherFocusReason);
	dbaseDialogUI_.descEdit->setEnabled(true);
	dbaseDialog_->setWindowTitle("Edit item");
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();

    if (dbIdxs.isEmpty()) {
		QMessageBox::information(this, tr("Information"), tr("No entry selected in database for editing.\nPlease select something and try again"));
        return;
	}
	else if (dbIdxs.size() > 1)	{
		QMessageBox::information(this, tr("Information"), tr("Cannot edit more than one entry at a time.\nPlease select single entry and try again"));
        return;
	}

	int idx = dbIdxs.first();
	QLOG("Selected index: " << idx);
    const Entry &e = curDbase_->entry(idx);

    dbaseDialogUI_.entryEdit->setText(e.item());
    dbaseDialogUI_.descEdit->setText(e.description());

    // pop dialog for user to enter changes and do stuff if ok pressed
	if (dbaseDialog_->exec() != QDialog::Accepted) {
        QLOG("User changes rejected");
        return;
    }
    
    const QString
            newitem = dbaseDialogUI_.entryEdit->text(),
            newdesc = dbaseDialogUI_.descEdit->text();

    if ( newitem.isEmpty() || newdesc.isEmpty() ) {
        QMessageBox::information(this, tr("Missing input"), tr("Database entries can't have empty fields."));
        return;
    }

    QLOG("User changes accepted");
    Error error = curDbase_->alter(idx, newitem, newdesc);
    if (error == Error::DUPLI)
    {
        QMessageBox::StandardButton button;
        int dupidx = error.index();
        const Entry &dupEntry = curDbase_->entry(dupidx);
        button = QMessageBox::question(this, tr("Possible duplicate"), 
            tr("A similar entry was found to already exist in the database:\n'")
            + dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1) 
            + tr(").\nWhile trying to replace entry (") + QString::number(idx + 1) 
            + tr(") with:\n'") + newitem + "' / '" + newdesc
            + tr("'\nDo you want to replace it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (button == QMessageBox::Yes)
        {
            QLOG("User wants to replace anyway");
            Error error = curDbase_->alter(idx, newitem, newdesc, true);
            QLOG("Result: " << QString::number(error.type()));
            Q_ASSERT(error == Error::OK);
        }
        else
        {
            QLOG("User doesn't want duplicate");
            return;
        }
    }

    ui_.databaseTable->setFocus(Qt::OtherFocusReason);
}

void MainForm::slot_database_findClicked()
{
    QLOG("Database find button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	bool ok = false;
	const auto text = QInputDialog::getText(
                this, tr("Find item in database"), tr("Please enter an expression to search for:"),
                QLineEdit::Normal, dbaseFindText_, &ok);

    if ( !ok || text.isEmpty() ) {
        QLOG("Find dialog not accepted");
        return;
    }
    
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
    if (found < 0 && idx != 0) {
        QLOG("Last resort search from the top");
        found = curDbase_->findEntry(text, 0);
    }
    if (found < 0) {
        QLOG("Nothing found.");
        QMessageBox::information(this, tr("No results"), tr("Sorry, no results were found for '") + text + "'");

    }
    else {
        QModelIndex pidx = dbaseProxyModel_.mapFromSource(dbaseModel_->index(found, 0));
        QLOG("Found dbase idx: " << found << ", proxy: " << pidx.row());
        ui_.databaseTable->selectRow(pidx.row());
        ui_.databaseTable->setFocus(Qt::OtherFocusReason);
    }
}

void MainForm::slot_database_exportClicked()
{
	QLOG("Database export button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	QString fname = QFileDialog::getSaveFileName(this, tr("Export database"), QDir::homePath(), tr("HTML (*.htm *.html)"));
	if ( fname.isEmpty() ) {
		QLOG("No file selected to save");
		return;
	}
	else {
		if (! (fname.endsWith(".htm", Qt::CaseInsensitive) || fname.endsWith(".html", Qt::CaseInsensitive))) 
            fname.append(".html");
        
		QLOG("Exporting to file: '" << fname << "'");
		QList<int> idxs = getSelectedDbaseTableIdxs();
		bool ok = curDbase_->htmlExport(fname, idxs);
		if (!ok) QMessageBox::information(this, tr("Unable to export"), tr("Couldn't open file for writing: '") + fname + "'.");
	}
}

void MainForm::slot_database_resetClicked() {
	QLOG("Database reset button clicked");
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
	const auto button = QMessageBox::warning(this, 
        tr("Database reset"), 
        tr("All quiz points, failure counters and test dates will be reset for database '") + curDbase_->name() 
            + tr("'. Are you sure?"), 
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    
	if (button == QMessageBox::No) {
		QLOG("User canceled");
		return;
	}
	else {
		QLOG("Proceeding to erase database contents");
		curDbase_->reset();
	}
}

void MainForm::slot_quiz_clicked() {
    if ( !curDbase_ ) {
        popDbaseMissing();
		return;
	}
    
    if ( quiz_ ) {
        QLOGX("Previous quiz instance not cleaned up before trying to start new one!");
        return;
    }
    
    if ( curDbase_->locked() ) {
        popDbaseLocked(); 
        return; 
    }

    const auto quizType = curQuizType();
	// retrieve selected indices from database table
    QList<int> idxs = getSelectedDbaseTableIdxs();
    if (idxs.count() > 1) {
		QLOG("Adding quiz questions based on selection inside database table");
        quiz_ = new Quiz(curDbase_, quizType, idxs);
	}
    else {
		QLOG("Adding quiz questions based on selected criteria");
        QuizSettings settings;
        settings.type = quizType;
        QLOG("Database quiz button clicked, type = " << settings.type);

        settings.range     = ui_.quizRangeCheck->isChecked();
        settings.rangeFrom = ui_.quizRangeFromSpin->value();
        settings.rangeTo   = ui_.quizRangeToSpin->value();
        // range inside database index is taken care of automatically, can't select higher index in spinbox
        if ( settings.range && settings.rangeFrom >= settings.rangeTo ) {
            QMessageBox::information(this, tr("Bad range"), tr("\"From\" value for index must be less than \"To\" value."));
            return;
        }

        settings.level     = ui_.quizPointsCheck->isChecked();
        settings.levelFrom = ui_.quizPointsFromSpin->value();
        settings.levelTo   = ui_.quizPointsToSpin->value();
        if ( settings.level && settings.levelFrom >= settings.levelTo ) {
            QMessageBox::information(this, tr("Bad range"), tr("\"From\" value for level must be less than \"To\" value."));
            return;
        }

        settings.take      = ui_.quizTakeCheck->isChecked();
        settings.takeCount = ui_.quizTakeSpin->value();
        if      ( ui_.quizTakeFailRadio->isChecked()   ) settings.takeMode = TAKE_FAILS;
        else if ( ui_.quizTakeOldieRadio->isChecked()  ) settings.takeMode = TAKE_OLDIES;
        else if ( ui_.quizTakeRandomRadio->isChecked() ) settings.takeMode = TAKE_RANDOM;
        quiz_ = new Quiz(curDbase_, settings);
	}
    
    if ( quiz_->empty() ) {
		QMessageBox::information(this, tr("No questions"), 
			tr("The current combination of quiz settings yielded no questions to be asked. "
               "Change the settings or select some entries from the table by hand and try again."));
		return;
	}
    
    setQuizControlsEnabled( false );
    quizDialog_->setWindowTitle(tr("Quiz") + " (" + curDbase_->name() + ")");
    // quiz dialog is not modal, lets user use dictionary while quiz in progress,
    // but database modifications are locked out
    curDbase_->setLocked( true );
    displayQuizQuestion();
    quizDialog_->show();
}

void MainForm::slot_quiz_typeChanged()
{
	QLOGX("Radio button selection changed in quiz type button group");
	QLOGINC;
    QuizDirection type = curQuizType();
	Q_ASSERT(dbaseModel_ != nullptr);
	dbaseModel_->setQuizType(type);
	QLOGDEC;
}

void MainForm::slot_quiz_correctClicked() {
    QLOGX("CORRECT answer in quiz");
    Q_ASSERT(quiz_);
    quiz_->answerCorrect();
    displayQuizQuestion();
}

void MainForm::slot_quiz_incorrectClicked() {
    QLOGX("INCORRECT answer in quiz");
    Q_ASSERT(quiz_);
    quiz_->answerIncorrect();
    displayQuizQuestion();
}

void MainForm::slot_quiz_unsureClicked() {
    QLOGX("UNSURE answer in quiz");
    Q_ASSERT(quiz_);
    quiz_->answerUnsure();
    displayQuizQuestion();
}

void MainForm::slot_quiz_revealClicked() {
    QLOGX("Reveal button clicked");
    reveaQuizAnswer();
}

void MainForm::slot_quiz_closeClicked() {
    QLOGX("Close button clicked");

    const int qIdx = quiz_->questionIndex();
    // nothing answered yet, no results so close without further questions
    if ( qIdx == 0 ) {
        finishQuiz(false);
        return;
    }

    QMessageBox::StandardButtons buttons;
    QString msg;
    // dialog not yet closed, still visible and quiz can be resumed
    if ( quizDialog_->isVisible() ) {
        buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        msg = tr("Quiz will be interrupted. ");
    }
    // dialog already closed by [x] button with mouse
    else {
        buttons = QMessageBox::Yes | QMessageBox::No;
        msg = tr("Quiz was interrupted. ");
    }

    const auto button = QMessageBox::question(
                GUI, tr("Quiz not complete"),
                msg + tr("Do you want to save the partial results?"), buttons
    );

    switch( button ) {
    case QMessageBox::Yes: finishQuiz(true); break;
    case QMessageBox::No: finishQuiz(false); break;
    // don't finish quiz if cancel pressed
    default: break;
    }
}

void MainForm::finishQuiz(const bool saveResults) {
    Q_ASSERT(curDbase_ && quiz_);
    if ( saveResults ) {
        const int count = quiz_->questionCount();
        auto stats = quiz_->stats();

        QMessageBox::information(GUI, tr("Done"), tr("Quiz completed.\n\nQuestions answered: ")
            + QString::number( stats.curQuestion ) + " of " + QString::number( count )
            + " (" + QString::number( stats.complete( count ),'f',2) + "%)\n"
            + tr("Correct answers: ")   + QString::number(stats.count(SUCCESS))
            + " ("+ QString::number(stats.percent(SUCCESS),'f',2) + "%)\n"
            + tr("Incorrect answers: ") + QString::number(stats.count(FAIL))
            + " (" + QString::number(stats.percent(FAIL),'f',2) + "%)\n"
            + tr("Unsure answers: ")    + QString::number(stats.count(NOCHANGE))
            + " (" + QString::number(stats.percent(NOCHANGE),'f',2) + "%)\n");

        quiz_->saveResults();
    }

    QLOGX("Quiz done");
    curDbase_->setLocked(false);
    quizDialog_->hide();
    delete quiz_;
    quiz_ = nullptr;
    setQuizControlsEnabled(true);
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
	ui_.statusBar->clearMessage();

	applySettings();
    
	// connect here, because the state is changed earlier based on the value of altdir from settings
	// TODO: use blockSignals instead?
	connect(ui_.settExtDirCheck, SIGNAL(clicked(bool)), this, SLOT(slot_settings_altdirCheckClicked(bool)));
	ui_.settExtDirEdit->setText(extDir_);
	if (!extDir_.isEmpty()) ui_.settExtDirCheck->setChecked(true);

	// delete the background thread, clear the status bar from progress information, 
	delete setupThread_;
	setupThread_ = nullptr;

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
    
	if ( curPlugin_ ) 
        curPlugin_->activate();

	// create database table model
    dbaseModel_ = new Model();
	dbaseModel_->setQuizType(curQuizType());
	dbaseProxyModel_.setSourceModel(dbaseModel_);
	dbaseProxyModel_.setSortRole(Qt::UserRole);
	ui_.databaseTable->setModel(&dbaseProxyModel_);

    // reparent the database items to the main window, this causes events to get sent so it can't
    // be done from the setup thread
    for (auto dbase : databases_)
        if ( !dbase->parent() ) dbase->setParent(this);
    
    // fill database selection combo box with databases created from user xml files on startup
	reloadDbaseCombo(true);
}

void MainForm::slot_updateSetupMsg(const QString &msg) {
	statusMessage(msg);
}

void MainForm::slot_testClicked() {
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

void MainForm::statusMessage(const QString &text, const int timeoutMs) {
    ui_.statusBar->showMessage(text, timeoutMs);
}

void MainForm::dbaseCountUpdate()
{
	QLOG("Updating database item count");
    int max = 0 , min = 0;
    if (curDbase_) {
        QLOG("Current database is '" << curDbase_->name() << "'");
        max = curDbase_->entryCount();
        if (max > 0) min = 1;
    }
    QLOG("Determined min = " << min << ", max = " << max);
    
	// update range spinboxes in quiz panel
	ui_.quizRangeFromSpin->setMinimum(min);
	ui_.quizRangeFromSpin->setMaximum(max);
	ui_.quizRangeToSpin->setMinimum(min);
	ui_.quizRangeToSpin->setMaximum(max);
	ui_.quizTakeSpin->setMinimum(min);
	ui_.quizTakeSpin->setMaximum(max);
}

QString MainForm::dbaseSavePath(const Database *db) const {
    if (!db) return {};
    
    return (db->external() ? extDir_ : APPDIR) + QDir::separator() + db->name() + Database::XML_EXT;
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
	reloadDbaseCombo(false);
	extDir_ = newdir;
	ui_.settExtDirEdit->setText(newdir);

	ui_.databaseCombo->setEnabled(true);
	ui_.createButton->setEnabled(true);
	ui_.deleteButton->setEnabled(true);
	ui_.databaseCombo->blockSignals(false);
	QLOGDEC;
}

void MainForm::reloadDbaseCombo(const bool restoreSelection) {
    QLOGX("Filling combo box with data for " << databases_.size() << " databases");
    // prevent firing of slot_database_changeCurrent() while we add the items
    ui_.databaseCombo->blockSignals(true);
    ui_.databaseCombo->clear();
    
    for ( auto dbase : databases_ )
        ui_.databaseCombo->addItem(dbase->name());
    
    // restore stored default database index from settings if requested
    // can't do this reliably when changing the user dir, only on startup
    if ( restoreSelection ) {
        const int idx = ui_.databaseCombo->findText( firstDbase_ );
        if (idx >= 0) {
            QLOG("Restored current dbase to " << idx << " (" << ui_.databaseCombo->currentText() << ")");
            ui_.databaseCombo->setCurrentIndex( idx );
        }
        else {
            QLOG("Unable to find index for restore dbase '" + firstDbase_ + "'");
        }
    }
    
    // have to trigger manually in case stored index was 0 (treated as no change and hence no signal emitted)
    slot_database_changeCurrent(ui_.databaseCombo->currentText());

    ui_.databaseCombo->setEnabled(true);
    // restore normal firing of slot_database_changeCurrent()
    ui_.databaseCombo->blockSignals(false);
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

QList<int> MainForm::getSelectedDbaseTableIdxs() {
	auto allIdxs = ui_.databaseTable->selectionModel()->selectedRows();
	QLOGX("Selected indexes in database table: " << allIdxs.count());

	QList<int> rowIdxs;
	for (const auto &idx : allIdxs)	{
		if (idx.column() != 0) 
            continue;
        
        QLOG("row: " << idx.row() << ", col: " << idx.column());
        int dbaseidx = dbaseModel_->data( dbaseProxyModel_.mapToSource( idx ), Qt::UserRole).toInt();
        QLOG("New dbase idx: " << dbaseidx);
        rowIdxs.append( dbaseidx );
	}

	return rowIdxs;
}

void MainForm::copyToAnotherDatabase(const bool move)
{
    // setup database entry dialog: disable item/desc editing, enable database selection combobox
	setupDbaseDialogCombo(false, false);
    if (dbaseDialogUI_.dbaseCombo->count() == 0) {
        QLOG("Target database selection combo is empty after setup, unable to proceed");
        QMessageBox::information(this, tr("Information"), tr("Unable to perform operation. There are no databases to copy/move to."));
        return;
    }

	dbaseDialogUI_.dbaseCombo->setEnabled(true);
	dbaseDialogUI_.entryEdit->setEnabled(false);
	dbaseDialogUI_.descEdit->setEnabled(false);
    
    const QString dialogTitle = (move ? tr("Move item(s) to another database") : 
                                        tr("Copy item(s) to another database"));
    dbaseDialog_->setWindowTitle( dialogTitle );

    // retrieve indexes of selected database items (may copy in bulk)
	QList<int> dbIdxs = getSelectedDbaseTableIdxs();
	// return if no selection
	if (dbIdxs.isEmpty()) {
		QLOG("Nothing selected, done");
		return;
	}

	QLOG("Selected rows (" << dbIdxs.size() << "): " << dbIdxs);

    if (curDbase_ == nullptr) {
        QLOG("Current database handle is nullptr!");
        return;
    }

    // if only one item selected, fill dialog with its data
    if (dbIdxs.size() == 1) {
        QLOG("Single item mode");
        const Entry &entry = curDbase_->entry(dbIdxs.at(0));
        dbaseDialogUI_.entryEdit->setText(entry.item());
        dbaseDialogUI_.descEdit->setText(entry.description());
    }
    // otherwise indicate multiple selections
    else {
        QLOG("Multiple items mode");
        dbaseDialogUI_.entryEdit->setText(tr(" -- multiple -- "));
        dbaseDialogUI_.descEdit->setText(tr(" -- multiple -- "));
    }

    // pop dialog and continue if ok clicked
    if (dbaseDialog_->exec() != QDialog::Accepted) {
        QLOG("User cancelled operation");
        return;
    }

    // find target database
    Database *target = database(dbaseDialogUI_.dbaseCombo->currentText());
    if (target == nullptr) {
        QLOG("Target database handle is nullptr!");
        return;
    }

    QLOG("Copying " << dbIdxs.size() <<  " item(s) to database '" << target->name() << "'" );

    // bulk copy items to target database from current database
    // TODO: does it even make sense to have this function? perhaps, if DbError
    // held all the error indices, it would be possible to add in bulk and
    // then process the remainder as per user answer(s)
    int answer = QMessageBox::NoButton;
    QList<int> moveIdxs;
    for (int curIdx : dbIdxs) {
        const Entry &entry = curDbase_->entry( curIdx );
        auto itemText = entry.item();
        auto descText = entry.description();

        // if go-ahead given, ignore duplicates
        if ( answer == QMessageBox::YesToAll ) {
            Error error = target->add(itemText, descText, {}, true);
            Q_ASSERT(error == Error::OK);
            if ( move ) moveIdxs.append( curIdx );
        }
        else {
            // don't ignore duplicates (yet)
            Error error = target->add(itemText, descText, {}, false);
            // check error type,
            if (error == Error::OK) {
                // in move mode, add index to successfully copied list for later removal from source
                if ( move ) moveIdxs.append( curIdx );
                continue;
            }
            // if duplicate detected, warn
            else if ( error == Error::DUPLI && answer != QMessageBox::NoToAll ) {
                const Entry &dupEntry = target->entry(error.index());
                answer = QMessageBox::information(this, tr("Duplicate entry"),
                                                  tr("Possible duplicate found for '") + entry.item() + "' (" + entry.description() +
                                                  tr(") in target database: '") + dupEntry.item() + "' (" + dupEntry.description() +
                                                  tr("). Continue?"),
                                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll);
            }
            
            // retry item add with duplicates ignored on "yes" or "yes to all"
            if ( answer == QMessageBox::Yes || answer == QMessageBox::YesToAll ) {
                error = target->add(itemText, descText, {}, true);
                if (error != Error::OK) {
                    QLOG("Unable to add to database despite ignoring duplicates?! (" << error.msg() << ")");
                    return;
                }
                if ( move ) moveIdxs.append( curIdx );
            }
        } // not YesToAll
    } // iterate over input indexes


    if ( move && !moveIdxs.isEmpty() ) {
        QLOG("Removing item(s) from source database");
        curDbase_->remove(moveIdxs);
        dbaseCountUpdate();
    }
}

void MainForm::popDbaseMissing() {
    QMessageBox::information(this, 
                             tr("No database"), 
                             tr("No user database exists. You must first create one\nto be able to add items to it."));    
}

void MainForm::popDbaseLocked()
{
    QMessageBox::information(this, 
                             tr("Database locked"), 
                             tr("The current database is being used and cannot be altered right now."));
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

    if (error == Error::DUPLI)
    {
        // todo: could be duplicate of multiple items, show all of them in the dialog
        int dupidx = error.index();
        const Entry &dupEntry = dbase->entry(dupidx);
        const auto button = QMessageBox::question(this, tr("Possible duplicate"),
            tr("A similar entry was found to already exist in the database:\n'")
            + dupEntry.item() + "' / '" + dupEntry.description() + "' (" + QString::number(dupidx + 1)
            + tr(").\nWhile trying to add entry:\n'") + userItem + "' / '" + userDesc
            + tr("'\nDo you want to add it anyway?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

        if (button == QMessageBox::Yes)
        {
            QLOG("User wants to add anyway");
            error = dbase->add(userItem, userDesc, {}, true);
            QLOG("Result: " << QString::number(error.type()));
            // todo handle nested error
            if (error != Error::OK) {
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

QuizDirection MainForm::curQuizType() const
{
    return (ui_.quizShowDescRadio->isChecked() ? DIR_SHOWDESC : DIR_SHOWITEM);
}

void MainForm::displayQuizQuestion() {
    Q_ASSERT(quiz_);
    const int qIdx = quiz_->questionIndex(), qCnt = quiz_->questionCount();
    auto qTxt = quiz_->questionText();
    QLOGX("Showing next question, number: " << qIdx << " of " << qCnt << ": " << qTxt);

    if ( qIdx >= qCnt ) {
        QLOGX("Out of questions, quiz done");
        finishQuiz(true);
        return;
    }

    quizDialogUI_.progressLabel->setText( QString::number( qIdx + 1 ) + " / " + QString::number( qCnt ) );
    quizDialogUI_.correctButton->setEnabled(false);
    quizDialogUI_.incorrectButton->setEnabled(false);
    quizDialogUI_.unsureButton->setEnabled(false);
    quizDialogUI_.revealButton->setEnabled(true);
    quizDialogUI_.questionLabel->setText(qTxt);
    quizDialogUI_.answerLabel->clear();
}

void MainForm::reveaQuizAnswer() {
    Q_ASSERT(quiz_);
    const int qIdx = quiz_->questionIndex(), qCnt = quiz_->questionCount(), qLvl = quiz_->questionLevel(), qFail = quiz_->questionFails();
    auto qAns = quiz_->answerText();
    QLOGX("Revealing answer, question number: " << qIdx << " of " << qCnt << ": " << qAns);

    if ( qIdx >= qCnt ) {
        QLOGX("Question index out of range!");
        finishQuiz(true);
        return;
    }

    quizDialogUI_.correctButton->setEnabled(true);
    quizDialogUI_.incorrectButton->setEnabled(true);
    quizDialogUI_.unsureButton->setEnabled(true);
    quizDialogUI_.revealButton->setEnabled(false);
    quizDialogUI_.answerLabel->setText(qAns);
    quizDialogUI_.progressLabel->setText(
                quizDialogUI_.progressLabel->text() +
                tr(" (Index: ") + QString::number(qIdx + 1) +
                tr(", Level: ") + QString::number(qLvl) +
                tr(", Failures: ") + QString::number(qFail) + ")"
    );
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
		Q_ASSERT(dbase != nullptr);
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
				const auto xmlpath = dbaseSavePath(dbase);
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

bool QuizDialogEventFilter::eventFilter(QObject *obj, QEvent *event) {
    // keyboard shortcuts for dialog buttons
    if ( event->type() == QEvent::KeyPress ) {
        switch ( static_cast<QKeyEvent*>(event)->key() ) {
        case Qt::Key_A: dialogUI_->correctButton->animateClick(); return true;
        case Qt::Key_S: dialogUI_->incorrectButton->animateClick(); return true;
        case Qt::Key_D: dialogUI_->unsureButton->animateClick(); return true;
        case Qt::Key_X: dialogUI_->revealButton->animateClick(); return true;
        // ignore escape key, normally would close dialog
        case Qt::Key_Escape: return true;
        }
	}
	// finish quiz on dialog close, unfortunately this can't be ignored
    else if ( event->type() == QEvent::Close ) {
		dialogUI_->closeButton->animateClick();
		return true;
	}
	return QObject::eventFilter(obj, event);
}

} // namespace app
} // namespace wynn
