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
#include "backend.h"
#include "setup.h"
#include "dict_table.h"
#include "db/model.h"
#include "ixplog_active.h"

QWidget *GUI;
QTSLogger *APP_LOGSTREAM = nullptr;

using namespace wynn::db;

namespace wynn {
namespace app {

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

MainForm::MainForm(ExtBackend *backend) : QMainWindow(nullptr),
  backend_(backend),
  dbaseFindText_(""),
  dbaseDialog_(nullptr), 
  quizDialog_(nullptr),
  setupThread_(nullptr)
{
  Q_ASSERT(backend != nullptr);
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
  
  // TODO: simplify connection calls with functor-based connections, don't use reference args esp for strings
  QLOG("Setting up database entry dialog");
  dbaseDialog_ = new QDialog(this);
  dbaseDialogUI_.setupUi(dbaseDialog_);
  connect(dbaseDialog_, SIGNAL(accepted()), this, SLOT(slot_database_entryAccepted()));
  connect(dbaseDialog_, SIGNAL(rejected()), this, SLOT(slot_database_entryRejected()));
  
  QLOG("Setting up quiz dialog");
  quizDialog_ = new QDialog(this);
  quizDialogUI_.setupUi(quizDialog_);
  quizDialog_->installEventFilter(new QuizDialogEventFilter(quizDialog_, &quizDialogUI_));
  connect(quizDialogUI_.revealButton,    SIGNAL(clicked()), this, SLOT(slot_quiz_revealClicked()));
  connect(quizDialogUI_.correctButton,   SIGNAL(clicked()), this, SLOT(slot_quiz_correctClicked()));
  connect(quizDialogUI_.incorrectButton, SIGNAL(clicked()), this, SLOT(slot_quiz_incorrectClicked()));
  connect(quizDialogUI_.unsureButton,    SIGNAL(clicked()), this, SLOT(slot_quiz_unsureClicked()));
  connect(quizDialogUI_.closeButton,     SIGNAL(clicked()), this, SLOT(slot_quiz_closeClicked()));
  
  QLOG("Connecting signals");
  // Backend signals
  connect(backend_, SIGNAL(warning(QString, QString)), this, SLOT(slot_backend_warning(QString, QString)));
  connect(backend_, SIGNAL(error(QString, QString)), this, SLOT(slot_backend_error(QString, QString)));
  connect(backend_, SIGNAL(status(QString)), this, SLOT(slot_backend_status(QString));
  connect(backend_, SIGNAL(question(QString, QString, QVector<Backend::Answer>)), this, SLOT(slot_backend_question(QString, QString, QVector<Backend::Answer>)));
  connect(backend_, SIGNAL(getItem(QString, QString, QStringList)), this, SLOT(slot_backend_getItem(QString,QString,QStringList)));
  connect(backend_, SIGNAL(getText(QString, QString)), this, SLOT(slot_backend_getText(QString,QString)));
  connect(backend_, SIGNAL(dbaseItemCount(const int)), this, SLOT(slot_database_countUpdate(const int)));
  connect(backend_, SIGNAL(dbaseAdded(const QString &)), this, SLOT(slot_database_added(const QString&)));
  connect(backend_, SIGNAL(dbaseRemoved(const QString&)), this, SLOT(slot_database_removed(const QString&)));
  connect(backend_, SIGNAL(dbaseUpdated(const int)), this, SLOT(slot_database_updated(const int)));
  connect(backend_, SIGNAL(dbaseEntry(QString, QString, QString)), this, SLOT(slot_backend_dbaseEntry(QString, QString, QString)));
  connect(backend_, SIGNAL(quizQuestion(QString,QString,QString)), this, SLOT(slot_backend_quizQuestion(QString,QString)));
  connect(backend_, SIGNAL(quizFinished(QString,QString), this, SLOT(slot_backend_quizFinished(QString,QString)));
  connect(backend_, SIGNAL(dictResults()), this, SLOT(slot_dict_results()));
  
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
  connect(ui_.dbaseAddButton,    SIGNAL(clicked()),                      this, SLOT(slot_database_entryAddClicked()));
  connect(ui_.dbaseRemoveButton, SIGNAL(clicked()),                      this, SLOT(slot_database_entryRemoveClicked()));
  connect(ui_.dbaseCopyButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_copyClicked()));
  connect(ui_.dbaseMoveButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_moveClicked()));
  connect(ui_.dbaseEditButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_editClicked()));
  connect(ui_.dbaseFindButton,   SIGNAL(clicked()),                      this, SLOT(slot_database_findClicked()));
  connect(ui_.dbaseExportButton, SIGNAL(clicked()),                      this, SLOT(slot_database_exportClicked()));
  connect(ui_.dbaseResetButton,  SIGNAL(clicked()),                      this, SLOT(slot_database_resetClicked()));
  connect(ui_.quizButton,        SIGNAL(clicked()),                      this, SLOT(slot_database_quizClicked()));
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
  saveSettings();
  // Need to remove all dictionary widgets from the dictionary panel manually before destroying plugins,
  // because plugins are supposed to dispose of these widgets themselves.
  clearPluginStacks(false);
  delete APP_LOGSTREAM;
}

// ==============================================================================================
// ============================== Top level slots
// ==============================================================================================

void MainForm::slot_dictionaryButtonClicked()
{
  QLOGX("Switching to dictionary panel");
  QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
  header->blockSignals(true);
  ui_.stackedWidget->setCurrentIndex(0);
  header->blockSignals(false);
}

void MainForm::slot_manageButtonClicked()
{
  QLOGX("Switching to database panel");
  ui_.stackedWidget->setCurrentIndex(1);
}

void MainForm::slot_settingsButtonClicked()
{
  QLOGX("Switching to settings panel");
  ui_.stackedWidget->setCurrentIndex(2);
}

void MainForm::slot_aboutButtonClicked()
{
  QLOGX("Switching to info panel");
  ui_.stackedWidget->setCurrentIndex(3);
}

// ==============================================================================================
// ============================== Backend slots
// ==============================================================================================

void MainForm::slot_backend_warning(const QString &title, const QString &msg) {
  QMessageBox::warning(this, title, msg);
}

void MainForm::slot_backend_error(const QString &title, const QString &msg) {
  QMessageBox::critical(this, title, msg);
}

void MainForm::slot_backend_status(const QString &msg) {
  slot_statusMessage(msg);
}

void MainForm::slot_backend_question(const QString &title, const QString &msg, const QVector<Backend::Answer> &options)
{
  QMessageBox::StandardButtons buttons = QMessageBox::NoButton;
  for (auto &op : options) switch (op) {
    case Backend::ANS_YES:    buttons |= QMessageBox::Yes;    break;
    case Backend::ANS_NO:     buttons |= QMessageBox::No;     break;
    case Backend::ANS_YESALL: buttons |= QMessageBox::YesAll; break;
    case Backend::ANS_NOALL:  buttons |= QMessageBox::NoAll;  break;
    case Backend::ANS_CANCEL: buttons |= QMessageBox::Cancel; break;
  }
  Backend::Answer ans = Backend::ANS_NONE;
  switch (QMessageBox::question(this, title, msg, buttons)) {
    case QMessageBox::Yes   : ans = Backend::ANS_YES;    break;
    case QMessageBox::No    : ans = Backend::ANS_NO;     break;
    case QMessageBox::YesAll: ans = Backend::ANS_YESALL; break;
    case QMessageBox::NoAll : ans = Backend::ANS_NOALL;  break;
    case QMessageBox::Cancel: ans = Backend::ANS_CANCEL; break;
  }
  backend_->setAnswer(ans);
}

void MainForm::slot_backend_getItem(const QString &title, const QString &msg, const QStringList &options) {
  const auto item = QInputDialog::getItem(this, title, msg, options);
  backend_->setResponse(item);
}

void MainForm::slot_backend_getText(const QString &title, const QString &msg)
{
  static QString persistValue;
  persistValue = QInputDialog::getText(this, title, msg, QLineEdit::Normal, persistValue);
  backend_->setResponse(persistValue);
}

void MainForm::slot_backend_dbaseEntry(const QString &title, const QString &item, const QString &desc) {
  QLOGX("Received dbase entry from backend: " << item << "/" << desc);
  dbaseDialogUI_.entryEdit->setText(item);
  dbaseDialogUI_.descEdit->setText(desc);
  dbaseDialogUI_.entryEdit->setFocus(Qt::OtherFocusReason);
  dbaseDialog_->setWindowTitle(title);
  dbaseDialog_->open();
}

void MainForm::slot_backend_quizQuestion(const QString &qtext, const QString &atext, const QString &status) {
  QLOGX("New quiz question: " << qtext << "/" << atext << "/" << status);
  quizDialogUI_.questionLabel->setText(qtext);
  quizDialogUI_.answerLabel->setVisible(false);
  quizDialogUI_.answerLabel->setText(atext);
  quizDialogUI_.correctButton->setEnabled(false);
  quizDialogUI_.incorrectButton->setEnabled(false);
  quizDialogUI_.unsureButton->setEnabled(false);
  quizDialogUI_.revealButton->setEnabled(true);
  quizDialogUI_.progressLabel->setText(status);
  // quiz dialog is not visible yet, means we are just starting the quiz
  if (!quizDialog_->isVisible()) {
    QLOG("Initializing UI for quiz");
    setQuizControlsEnabled(false);
    quizDialog_->setWindowTitle(tr("Quiz (%1)").arg(dbaseName()));
    quizDialog_->show();
  }
}

void MainForm::slot_backend_quizFinished(const QString &title, const QString &stats) {
  QLOGX("Quiz finished");
  QMessageBox::information(this, title, stats);
  quizDialog_->hide();
  setQuizControlsEnabled(true);
}

// ==============================================================================================
// ============================== Dictionary slots
// ==============================================================================================

void MainForm::slot_dict_switchPlugin(int plugIdx) {
  QLOGX("Dictionary language changed to: " << plugIdx);
  // block the dictColumnResized signal from the main widget that is emitted after the model is changed,
  // don't want to overwrite the correct column widths in the plugin with a stupid value
  QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
  header->blockSignals(true);
  backend_->pluginChanged(plugIdx);
  QStackedWidget *stack = ui_.dictPanelStack;
  // set the size policy of the page being hidden to ignored so it doesn't influence the size
  // of the stacked widget anymore
  QWidget *curWidget = stack->currentWidget();
  if (curWidget)
    curWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  stack->setCurrentIndex(plugIdx);
  // set the size policy of the page that just became visible to make the widget adjust its size to it
  curWidget = stack->currentWidget();
  Q_ASSERT(curWidget);
  curWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  stack->adjustSize();
  header->blockSignals(false);
}

void MainForm::slot_dict_langAboutClicked() {
  backend_->pluginAbout();
}

void MainForm::slot_dict_detailsClicked() {
  backend_->dictDetails();
}

void MainForm::slot_dict_toDbaseClicked() {
  backend_->dictStore();
}

void MainForm::slot_dict_results() {
  // when results present, shift keyboard focus to results table
  ui_.dictionaryTable->selectRow(0);
  ui_.dictionaryTable->setFocus(Qt::OtherFocusReason);
}

void MainForm::slot_dict_tableItemActivated(const QModelIndex &index) {
  QLOGX("Table item activated, row: " << index.row());
  backend_->dictDetails();
}

void MainForm::slot_dict_columnResized(int index, int oldSize, int newSize) {
  QLOGX("Column " << index << " was resized from " << oldSize << " to " << newSize);
  // TODO: cleanup processing for this in plugins, come up with something better on UI side
}

// TODO: remove if not necessary anymore
void MainForm::slot_dict_resizeColumns(const QList<int> &widths) {
  QLOGX("Requested column resize on " << widths.size() << " columns");
  QHeaderView *header = ui_.dictionaryTable->horizontalHeader();
  // prevent header from sending notifications about these resizes
  header->blockSignals(true);
  
  // check if all widths are larger than zero
  bool ok = true;
  for (int i = 0; i < widths.size(); ++i) {
    const int width = widths.at(i);
    if (width <= 0) { ok = false; break; }
  }
  
  QLOG("Widths ok: " << ok);
  // if so, proceed with resize
  if (ok) for (int i = 0; i < widths.size(); ++i)
    header->resizeSection(i, widths.at(i));
  // otherwise try an automatic resize
  else
    ui_.dictionaryTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
  
  header->blockSignals(false);
}

// ==============================================================================================
// ============================== Database slots
// ==============================================================================================

// current selection in the combo box indicating current database has changed
void MainForm::slot_database_changeCurrent(const QString& text) {
  QLOGX("Current database changed to: '" << text << "'");
  // enable or disable ui elements depending on whether the dbase combo box is empty or not
  const bool haveItem = !text.isEmpty();
  for (QWidget *widget : { ui_.databaseCombo, ui_.saveButton, ui_.deleteButton, ui_.dbaseAddButton, ui_.dbaseRemoveButton, 
        ui_.dbaseCopyButton, ui_.dbaseMoveButton, ui_.dbaseEditButton, ui_.dbaseFindButton, ui_.dbaseExportButton, ui_.dbaseResetButton, 
        ui_.quizButton }) widget->setEnabled( haveItem );
  backend_->dbaseSwitch(text);
}

void MainForm::slot_database_countUpdate(const int count) {
  QLOG("Updating database item count");
  const int max = count;
  const int min = (max > 0 ? 1 : 0);
  QLOG("Determined min = " << min << ", max = " << max);
  
  // update range spinboxes in quiz panel
  ui_.quizRangeFromSpin->setMinimum(min);
  ui_.quizRangeFromSpin->setMaximum(max);
  ui_.quizRangeToSpin->setMinimum(min);
  ui_.quizRangeToSpin->setMaximum(max);
  ui_.quizTakeSpin->setMinimum(min);
  ui_.quizTakeSpin->setMaximum(max);
}

void MainForm::slot_database_createClicked() {
  bool accepted = false;
  QRegExp nameRegex("[a-zA-Z0-9_-]+");
  const QString name = QInputDialog::getText(this, tr("Create database"), tr("Enter name for new database:"),
                                             QLineEdit::Normal, "", &accepted);
  
  if (!accepted || name.isEmpty()) return;
  // discriminate against ill-formed database names
  if (!nameRegex.exactMatch(name)) {
    QMessageBox::warning(this, tr("Invalid name"), 
                         tr("Database name may contain only alphanumeric, dash ('-') and underscore ('_') characters."), 
                         QMessageBox::Ok, QMessageBox::Ok);
    return;
  }
  QLOG("Creating database, name: '" << name << "'" );
  backend_->dbaseCreate(name);
}

void MainForm::slot_database_added(const QString &name) {
  ui_.databaseCombo->addItem(name);
  ui_.databaseCombo->setCurrentText(name);
  ui_.databaseCombo->setEnabled(true);
  slot_database_countUpdate(0);
}

void MainForm::slot_database_saveClicked() {
  QLOG("Database save button clicked");
  backend_->dbaseSave();
}

void MainForm::slot_database_deleteClicked() {
  QLOG("Database delete button clicked");
  const QString dbName = ui_.databaseCombo->currentText();
  if (dbName.isEmpty()) {
    QLOGX("No current database!");
    return;
  }
  
  if (QMessageBox::question(this, "Question", tr("Are you sure you want to delete the database '%1'?").arg(dbName), 
                            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes) {
    QLOGX("User declined database deletion");
    return;
  } 
  
  QLOG("Removing database: " << dbName);
  backend_->dbaseDelete();
}

void MainForm::slot_database_removed(const QString &name) {
  QLOGX("Database " << name << " was removed");
  bool removed = false;
  for (int i = 0; i < ui_.databaseCombo->count(); ++i) 
    if ( ui_.databaseCombo->itemText(i) == name ) {
      ui_.databaseCombo->removeItem(i);
      removed = true;
    }
  if (!removed) {
    QLOG("Unable to find dbase in combobox!");
  }
}

// add buton clicked in database panel
void MainForm::slot_database_entryAddClicked() {
  QLOG("Database Add button clicked");
  backend_->dbaseEntryAdd();
}

void MainForm::slot_database_entryAccepted() {
  QLOGX("Database entry accepted");
  const QString 
      &item = dbaseDialogUI_.entryEdit->text().simplified(),
      &desc = dbaseDialogUI_.descEdit->text().simplified();
  if (item.isEmpty() || desc.isEmpty()) {
    // TODO: unify naming of item/description/entry through code and UI
    QMessageBox::information(this, tr("Empty input"), tr("Entry and Description fields may not be empty when adding item to database!"));
    return;
  }
  backend_->setEntry(item, desc);
}

void MainForm::slot_database_entryRejected() {
  QLOGX("Database entry rejected");
  backend_->setAnswer(Backend::ANS_NO);
}

// TODO: come up with better name, too similar to slot_database_countUpdate
void MainForm::slot_database_updated(const int where) {
  QLOGX("Database updated, focus: " << where);
  ui_.databaseTable->setFocus(Qt::OtherFocusReason);
  ui_.databaseTable->selectRow(where);
  slot_database_countUpdate(entryCount);
}

// remove button clicked in database panel
void MainForm::slot_database_entryRemoveClicked() {
  QLOG("Database remove button clicked");
  const auto selection = dbaseSelection();
  if (selection.empty()) {
    QLOG("Nothing selected in database table!");
    QMessageBox::information(this, tr("Nothing selected"), tr("No entries selected in database table for removal!"));
    return;
  }
  backend_->dbaseEntryRemove(selection);
}

// copy button clicked in database panel
void MainForm::slot_database_copyClicked() {
  QLOG("Database copy button clicked");
  const auto selection = dbaseSelection();
  if (selection.empty()) {
    QLOG("Nothing selected in database table!");
    QMessageBox::information(this, tr("Nothing selected"), tr("No entries selected in database table for copying!"));
    return;
  }
  backend_->dbaseEntryCopy(selection, false);
}

// move button clicked in database panel
void MainForm::slot_database_moveClicked() {
  QLOG("Database move button clicked");
  const auto selection = dbaseSelection();
  if (selection.empty()) {
    QLOG("Nothing selected in database table!");
    QMessageBox::information(this, tr("Nothing selected"), tr("No entries selected in database table for moving!"));
    return;
  }
  backend_->dbaseEntryCopy(selection, true);
}

void MainForm::slot_database_editClicked() {
  QLOGX("Database edit button clicked");
  const auto selection = dbaseSelection();
  if (selection.empty()) {
    QLOG("Nothing selected in database table!");
    QMessageBox::information(this, tr("Nothing selected"), tr("No entries selected in database table for editing!"));
    return;
  }  
  backend_->dbaseEntryEdit(selection);
}
  
void MainForm::slot_database_findClicked() {
  QLOGX("Database find button clicked");
  backend_->dbaseEntryFind(dbaseSelection());
}

void MainForm::slot_database_exportClicked() {
  QLOGX("Database export button clicked");
  const QString path = QFileDialog::getSaveFileName(this, tr("Export database"), QDir::homePath(), "HTML (*.htm *.html)");
  if ( path.isEmpty() ) {
    QLOG("No file selected to save");
    return;
  }
  
  if (! (path.endsWith(".htm", Qt::CaseInsensitive) || path.endsWith(".html", Qt::CaseInsensitive))) 
    path.append(".html");
  
  QLOG("Exporting to file: '" << path << "'");
  
  const auto selection = ui_.databaseTable->selectionModel()->selectedRows();
  backend_->dbaseExport(selection, path);
}

void MainForm::slot_database_resetClicked() {
  QLOGX("Database reset button clicked");
  const auto button = QMessageBox::warning(this, 
    tr("Database reset"), 
    tr("All quiz points, failure counters and test dates will be reset for database '%1'. Are you sure?").arg(dbaseName()), 
    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
  
  if (button != QMessageBox::Yes) {
    QLOG("User canceled");
    return;
  }
  
  backend_->dbaseReset();
}

void MainForm::slot_database_quizClicked() {
  QLOGX("Quiz start button clicked");
  const auto selection = dbaseSelection();
  // TODO: figure out better way to pass quiz settings than to expose DB internals to UI
  db::QuizSettings settings;
  settings.type = (ui_.quizShowDescRadio->isChecked() ? DIR_SHOWDESC : DIR_SHOWITEM);
  // TODO: remove UI controls in database panel, pop special dialog for quiz settings before starting
  // - will also make unnecessary updating of the quiz spinboxes every time there is a database update,
  // and remove ambiguity between row selection and quiz criteria - we won't show the criteria options
  // in the settings dialog if rows were selected.
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
  backend_->quizStart(selection, settings);
}

// TODO: remove after moving quiz controls to separate dialog, display both directions' points in table view
void MainForm::slot_quiz_typeChanged() {
  QLOGX("Radio button selection changed in quiz type button group");
  QuizDirection type = curQuizType();
  Q_ASSERT(dbaseModel_ != nullptr);
  dbaseModel_->setQuizType(type);
}

void MainForm::slot_quiz_revealClicked() {
  QLOGX("Reveal button clicked");
  quizDialogUI_.answerLabel->setVisible(true);
  quizDialogUI_.correctButton->setEnabled(true);
  quizDialogUI_.incorrectButton->setEnabled(true);
  quizDialogUI_.unsureButton->setEnabled(true);
  quizDialogUI_.revealButton->setEnabled(false);
}

void MainForm::slot_quiz_correctClicked() {
  QLOGX("Correct answer in quiz");
  backend_->quizAnswer(Backend::CORRECT);
}

void MainForm::slot_quiz_incorrectClicked() {
  QLOGX("Incorrect answer in quiz");
  backend_->quizAnswer(Backend::INCORRECT);
}

void MainForm::slot_quiz_unsureClicked() {
  QLOGX("Unsure answer in quiz");
  backend_->quizAnswer(Backend::UNSURE);
}

void MainForm::slot_quiz_closeClicked() {
  QLOGX("Close button clicked on quiz dialog");
  
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
  slot_statusMessage(msg);
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

QString MainForm::dbaseName() const {
  return ui_.databaseCombo->currentText();
}

QModelIndexList MainForm::dbaseSelection() const {
  return ui_.databaseTable->selectionModel()->selectedRows();
}

void MainForm::slot_statusMessage(const QString &text) {
  ui_.statusBar->showMessage(text, MSG_TIMEOUT);
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

void MainForm::setQuizControlsEnabled(bool arg) {
  QLOGX("Setting enabled state of quiz controls to: " << arg);
  ui_.databaseCombo->setEnabled(arg);
  ui_.createButton->setEnabled(arg);
  ui_.deleteButton->setEnabled(arg);
  ui_.quizDirectionGroupBox->setEnabled(arg);
  ui_.quizCriteriaGroupBox->setEnabled(arg);
  ui_.quizButton->setEnabled(arg);
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
