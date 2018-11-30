#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <QMessageBox>

#include "quiz.h"
#include "database.h"
#include "global.h"
#include "ixplog_active.h"
#define QUIZDEBUG 1

using namespace wynn::db;

namespace wynn {
namespace app {

// todo: undo button in quiz dialog
Quiz::Quiz(Database *database, const QuizSettings &settings, QDialog *dialog, Ui::QuizDialog *dialogUI) : 
    QObject(GUI), dbase_(database),
    type_(settings.type),
    dialog_(dialog), dialogUI_(dialogUI)
{
    QLOGX("Creating new quiz from database '" << dbase_->name() << "', items: " << dbase_->entryCount() << ", criteria-based"
          << ", type: " << type_);
    QLOGINC;

    // TODO: override dialog's close event to finish quiz silently
    srand(time(NULL));
    connectSignals();
    dbase_->setLocked(true);

    if (settings.range)
    {
        QLOG("Range criterium active");
        addByRange(settings.rangeFrom, settings.rangeTo);
    }
    else
    {
        QLOG("Range criterium inactive");
        addAll();
    }

    if (settings.level)
    {
        QLOG("Level criterium active");
        eliminateByLevel(settings.levelFrom, settings.levelTo);
    }

    if (settings.take)
    {
        QLOG("Take criterium active");
        eliminateByTake(settings.takeCount, settings.takeMode);
    }

    QLOGDEC;
}

Quiz::Quiz(Database *database, const QuizSettings &settings, const QList<int> &idxs, QDialog *dialog, Ui::QuizDialog *dialogUI) :
    QObject(GUI), dbase_(database),
    type_(settings.type),
    dialog_(dialog), dialogUI_(dialogUI)
{
    QLOGX("Creating new quiz from database '" << dbase_->name() << "', items: " << dbase_->entryCount()
          << ", from provided list of " << idxs.size() << " indices, type: " << type_);
    QLOGINC;

    connectSignals();
    dbase_->setLocked(true);
    addByIndex(idxs);

    QLOGDEC;
}

Quiz::~Quiz() { 
    dbase_->setLocked(false);
}

void Quiz::logQuestions() {
#ifdef QUIZDEBUG
    QLOG("Questions (" << questionCount() << "):");
    for (auto &q : questions_){
        QLOG("entry: " << q.entry() << ", result: " << q.result());
    }
#endif
}

void Quiz::addByIndex(const QList<int> &idxs)
{
    QLOGX("Adding  " << idxs.count() << " questions.");
    for (int i : idxs) {
        questions_.emplace_back(dbase_->entry(i), i);
    }
    logQuestions();
}

void Quiz::addByRange(int from, int to)
{
    Q_ASSERT(to > from);
    // TODO: why?
    from--;
    to--;
    QLOGX("Range from: " << from << ", to: " << to);
    for (int i = from; i <= to; ++i) {
        questions_.emplace_back(dbase_->entry(i), i);
    }
    logQuestions();
}

void Quiz::addAll() {
    QLOGX("Adding  " << dbase_->entryCount() << " questions.");
    for (int i = 0; i < dbase_->entryCount(); ++i) {
        questions_.emplace_back(dbase_->entry(i), i);
    }
    logQuestions();
}

void Quiz::eliminateByLevel(int from, int to) {
    Q_ASSERT(to > from);
    QLOGX("Eliminating quiz questions by level, from: " << from << ", to: " << to);
    for (auto it = questions_.begin(); it != questions_.end();)
    {
        const Entry& entry = it->entry();
        const int level = entry.level(type_);

        if (level < from || level > to)
            it = questions_.erase(it);
        else
            ++it;
    }
    logQuestions();
    QLOGDEC;
}

void Quiz::eliminateByTake(const int takeCount, const QuizTakeMode mode) {
    if ( questionCount() <= takeCount ) {
        QLOGX("Question count (" << questionCount() << " ) already less than criterium count (" << takeCount << ")");
        return;
    }

    QLOGX("Eliminating questions by take criterium, mode: " << mode
          << " from current " << questionCount() << " to target count: " << takeCount);

    if ( mode == TAKE_FAILS ) {
        QLOG("Sorting by fail count, type: " << type_);
        std::sort(questions_.begin(), questions_.end(), [&](auto &arg1, auto &arg2){
            return (arg1.entry().fails(type_) < arg2.entry().fails(type_));
        });

        // remove fail count-sorted questions from list until desired number left
        questions_.erase( questions_.begin(), questions_.begin() + std::min( questionCount(), takeCount ) );
        // don't include 0-fails questions in quiz, remove further
        std::remove_if(questions_.begin(), questions_.end(), [&](auto &q){
            return q.entry().fails( type_ ) == 0;
        });
    }
    else if ( mode == TAKE_OLDIES ) {
        QLOG("Sorting by update date");
        std::sort(questions_.begin(), questions_.end(), [](auto &arg1, auto &arg2){
            return arg1.entry().updateStamp() > arg2.entry().updateStamp();
        });

        questions_.erase( questions_.begin(), questions_.begin() + std::min( questionCount(), takeCount ) );
    }
    else if ( mode == TAKE_RANDOM ) {
        QLOG("Eliminating randomly");
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle( questions_.begin(), questions_.end(), g );
        questions_.erase( questions_.begin(), questions_.begin() + std::min( questionCount(), takeCount ) );
    }

    QLOG("Trimmed to size, question count: " << questionCount());
    Q_ASSERT(questionCount() <= takeCount);
    logQuestions();
    QLOGDEC;
}

void Quiz::connectSignals() {
    connect(dialogUI_->correctButton,   SIGNAL(clicked()), this, SLOT(correctClicked()));
    connect(dialogUI_->incorrectButton, SIGNAL(clicked()), this, SLOT(incorrectClicked()));
    connect(dialogUI_->unsureButton,    SIGNAL(clicked()), this, SLOT(unsureClicked()));
    connect(dialogUI_->revealButton,    SIGNAL(clicked()), this, SLOT(revealClicked()));
    connect(dialogUI_->closeButton,     SIGNAL(clicked()), this, SLOT(closeClicked()));
}

void Quiz::randomizeOrder()
{
    QLOGX("Randomizing questions' order");
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle( questions_.begin(), questions_.end(), g );
    logQuestions();
}

void Quiz::run()
{
    QLOGX("Starting quiz, type: " << type_);
    randomizeOrder();
    questionIndex_ = -1;
    correct_ = incorrect_ = unsure_ = 0;
    nextQuestion();
    dialog_->setWindowTitle(tr("Quiz") + " (" + dbase_->name() + ")");
    dialog_->show();
}

void Quiz::finish(const bool keep) {
    dbase_->setLocked(false);
    if (keep) {
        const float
                correctPt = float(correct_*100)/questionIndex_,
                incorrectPt = float(incorrect_*100)/questionIndex_,
                unsurePt = float(unsure_*100)/questionIndex_;

        QMessageBox::information(GUI, tr("Done"), tr("Quiz completed.\n\nQuestions asked: ")
                                 + QString::number(questionIndex_) + " of " + QString::number(questionCount())
                                 + " (" + QString::number(float(questionIndex_*100)/questionCount(),'f',2) + "%)\n"
                                 + tr("Correct answers: ") + QString::number(correct_) + " (" + QString::number(correctPt,'f',2) + "%)\n"
                                 + tr("Incorrect answers: ") + QString::number(incorrect_) + " (" + QString::number(incorrectPt,'f',2) + "%)\n"
                                 + tr("Unsure answers: ") + QString::number(unsure_) + " (" + QString::number(unsurePt,'f',2) + "%)\n");
        saveResults();
    }
    dialog_->hide();
    emit done();
}

void Quiz::saveResults()
{
    QLOGX("Saving quiz results back to database");
    // update entries only up to the answered question index
    for (int i = 0; i < questionIndex_; ++i)
    {
        const QuizQuestion &q = questions_.at(i);

        if ( q.result() == QuizQuestion::NOCHANGE )
            continue;

        switch ( q.result() ) {
        case QuizQuestion::SUCCESS: dbase_->point(q.index(), type_); break;
        case QuizQuestion::FAIL:    dbase_->fail(q.index(), type_);  break;
        default: break;
        }
    }
}

void Quiz::nextQuestion()
{
    questionIndex_++;
    if (questionIndex_ >= questionCount()) { finish(true); return; }

    QLOGX("Showing next question, number: " << questionIndex_);
    dialogUI_->progressLabel->setText(QString::number(questionIndex_ + 1) + " / " + QString::number(questionCount()));
    dialogUI_->correctButton->setEnabled(false);
    dialogUI_->incorrectButton->setEnabled(false);
    dialogUI_->unsureButton->setEnabled(false);
    dialogUI_->revealButton->setEnabled(true);

    const QuizQuestion &question = questions_.at(questionIndex_);
    Q_ASSERT(type_ == db::DIR_SHOWITEM || type_ == db::DIR_SHOWDESC);

    QString questionText;
    switch(type_)
    {
    case db::DIR_SHOWITEM: questionText = question.entry().item(); break;
    case db::DIR_SHOWDESC: questionText = question.entry().description(); break;
    default: break;
    }
    dialogUI_->questionLabel->setText(questionText);
    dialogUI_->answerLabel->setText("");
}

void Quiz::revealAnswer() {
    QLOGX("Revealing answer for question number " << questionIndex_);
    dialogUI_->correctButton->setEnabled(true);
    dialogUI_->incorrectButton->setEnabled(true);
    dialogUI_->unsureButton->setEnabled(true);
    dialogUI_->revealButton->setEnabled(false);

    Q_ASSERT(questionIndex_ >= 0 && questionIndex_ < questionCount());
    const QuizQuestion &question = questions_.at(questionIndex_);
    Q_ASSERT(type_ == db::DIR_SHOWITEM || type_ == db::DIR_SHOWDESC);

    QString answerText;
    switch(type_) {
    case db::DIR_SHOWITEM: answerText = question.entry().description(); break;
    case db::DIR_SHOWDESC: answerText = question.entry().item(); break;
    default: break;
    }
    dialogUI_->answerLabel->setText(answerText);

    const QString progressText = dialogUI_->progressLabel->text() +
            tr(" (Index: ") + QString::number(question.index() + 1) +
            tr(", Level: ") + QString::number(question.entry().level(type_)) +
            tr(", Failures: ") + QString::number(question.entry().fails(type_)) + ")";
    dialogUI_->progressLabel->setText(progressText);
}

void Quiz::correctClicked() {
    QLOGX("CORRECT answer in quiz");
    correct_++;
    questions_[questionIndex_].setResult(QuizQuestion::SUCCESS);
    nextQuestion();
}

void Quiz::incorrectClicked() {
    QLOGX("INCORRECT answer in quiz");
    incorrect_++;
    questions_[questionIndex_].setResult(QuizQuestion::FAIL);
    nextQuestion();
}

void Quiz::unsureClicked() {
    QLOGX("UNSURE answer in quiz");
    unsure_++;
    questions_[questionIndex_].setResult(QuizQuestion::NOCHANGE);
    nextQuestion();
}

void Quiz::revealClicked() {
    QLOGX("Reveal button clicked");
    revealAnswer();
}

void Quiz::closeClicked() {
    QLOGX("Close button clicked");

    // nothing answered yet, no results so close without further questions
    if ( questionIndex_ == 0 ) {
        finish(false);
        return;
    }

    QMessageBox::StandardButtons buttons;
    QString msg;

    // dialog not yet closed, still visible and quiz can be resumed
    if (dialog_->isVisible()) {
        buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        msg = tr("Quiz will be interrupted. ");
    }
    // dialog already closed by [x] button with mouse
    else {
        buttons = QMessageBox::Yes | QMessageBox::No;
        msg = tr("Quiz was interrupted. ");
    }

    const auto button = QMessageBox::question(GUI, tr("Quiz not complete"),
                                              msg + tr("Do you want to save the partial results?"), buttons);

    switch(button){
    case QMessageBox::Yes: finish(true); break;
    case QMessageBox::No: finish(false); break;
    default: break;
    }
}

} // namespace app
} // namespace wynn
