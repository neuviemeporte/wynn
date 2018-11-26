#include "quiz.h"
#include "database.h"
#include "global.h"

#include <ctime>
#include <cstdlib>
#include <QMessageBox>
#include <ixplog_active.h>

#define QUIZDEBUG 1

// comparison functions for sorting the questions according to level or number of failures for the appropriate quiz type
bool showDescFailCompare(const QuizQuestion &arg1, const QuizQuestion &arg2)
{
    return (arg1.entry().fails(DbEntry::DIR_SHOWDESC) < arg2.entry().fails(DbEntry::DIR_SHOWDESC));
}

bool showItemFailCompare(const QuizQuestion &arg1, const QuizQuestion &arg2)
{
    return (arg1.entry().fails(DbEntry::DIR_SHOWITEM) < arg2.entry().fails(DbEntry::DIR_SHOWITEM));
}

bool dateCompare(const QuizQuestion &arg1, const QuizQuestion &arg2)
{
    return (arg1.entry().updateStamp() < arg2.entry().updateStamp());
}

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

Quiz::~Quiz() 
{ 
	dbase_->setLocked(false); 
}

void Quiz::logQuestions()
{
#ifdef QUIZDEBUG
	QLOG("Questions (" << questions_.count() << "):");
	QLOGINC;

	for (int i = 0; i < questions_.count(); ++i)
	{
        const QuizQuestion &q = questions_.at(i);
        QLOG("entry: " << q.entry() << ", result: " << q.result());
	}
	QLOGDEC;
#endif
}

void Quiz::addByIndex(const QList<int> &idxs)
{
	QLOGX("Adding  " << idxs.count() << " questions.");
	for (int i = 0; i < idxs.count(); ++i)
	{	
        QuizQuestion q(dbase_->entry(idxs.at(i)));
        questions_.append(q);
	}
	logQuestions();
}

void Quiz::addByRange(int from, int to)
{
	Q_ASSERT(to > from);
	from--;
	to--;
	QLOGX("Range from: " << from << ", to: " << to);
	QLOGINC;
	QList<int> idxs;
	for (int i = from; i <= to; ++i)
	{
        QuizQuestion q(dbase_->entry(i));
        questions_.append(q);
	}
	logQuestions();
	QLOGDEC;
}

void Quiz::addAll()
{
	QLOGX("Adding  " << dbase_->entryCount() << " questions.");
    QLOGINC;

	for (int i = 0; i < dbase_->entryCount(); ++i)
	{	
        QuizQuestion q(dbase_->entry(i));
        questions_.append(q);
	}

	logQuestions();
    QLOGDEC;
}

void Quiz::eliminateByLevel(int from, int to)
{
	Q_ASSERT(to > from);
	QLOGX("Eliminating quiz questions by level, from: " << from << ", to: " << to);
	QLOGINC;
	QList<int> idxs;
	for (int i = 0; i < questions_.count(); ++i)
	{
        const DbEntry& entry = questions_.at(i).entry();
		int level = 0;

        level = entry.level(type_);

		if (level < from || level > to)
		{
			idxs.append(i);
		}
	}
	for (int i = idxs.size() - 1; i >= 0; --i)
	{
		questions_.removeAt(idxs.at(i));
	}

	logQuestions();
	QLOGDEC;
}

void Quiz::eliminateByTake(int count, QuizTakeMode mode)
{
	if (questions_.count() <= count) return;
	QLOGX("Eliminating questions by take criterion, mode: " << mode << " from current " << questionCount() << " to target count: " << count);
	QLOGINC;

	if (mode == TAKE_FAILS)
	{
		QLOG("Sorting by fail count, type: " << type_);
		if (type_ == DbEntry::DIR_SHOWDESC)
		{
			qSort(questions_.begin(), questions_.end(), showDescFailCompare);
		}
		else if (type_ == DbEntry::DIR_SHOWITEM)
		{
			qSort(questions_.begin(), questions_.end(), showItemFailCompare);
		}
		// remove fail count-sorted questions from list until desired number left
		while (questions_.count() > count) questions_.removeFirst();
		QLOG("Trimmed to size, question count: " << questionCount());
		// don't include 0-fails questions in quiz, remove further
        while (!questions_.empty() && questions_.first().entry().fails(type_) == 0) questions_.removeFirst();
	}
	else if (mode == TAKE_OLDIES)
	{
		QLOG("Sorting by update date");
		qSort(questions_.begin(), questions_.end(), dateCompare);
		while (questions_.count() > count) questions_.removeLast();
	}
	else if (mode == TAKE_RANDOM)
	{
		QLOG("Eliminating randomly");
		srand(time(NULL));
		while (questions_.count() > count)
		{
			questions_.removeAt(rand() % questions_.count());
		}
	}

	logQuestions();
	QLOGDEC;
}

void Quiz::connectSignals()
{
	connect(dialogUI_->correctButton, SIGNAL(clicked()), this, SLOT(correctClicked()));
	connect(dialogUI_->incorrectButton, SIGNAL(clicked()), this, SLOT(incorrectClicked()));
	connect(dialogUI_->unsureButton, SIGNAL(clicked()), this, SLOT(unsureClicked()));
	connect(dialogUI_->revealButton, SIGNAL(clicked()), this, SLOT(revealClicked()));
	connect(dialogUI_->closeButton, SIGNAL(clicked()), this, SLOT(closeClicked()));
}

void Quiz::randomizeOrder()
{
	QLOGX("Randomizing questions' order");
	srand(time(NULL));
	for (int i = 0; i < questions_.count(); ++i)
	{
		questions_.swap(i, (rand() % questions_.count())); // random number from 0 to (count - 1)
	}
	logQuestions();
}

void Quiz::run()
{
	if (questions_.empty())
	{
		QMessageBox::information(GUI, tr("No questions"), 
			tr("The current combination of quiz settings yielded no questions to be asked.\n") 
			+ tr("Change the settings or select some entries from the table by hand and try again."));
		emit done();
		return;
	}
	QLOGX("Starting quiz, type: " << type_);
	QLOGINC;
	randomizeOrder();
	question_ = -1;
	correct_ = incorrect_ = unsure_ = 0;
	nextQuestion();
	dialog_->setWindowTitle(tr("Quiz") + " (" + dbase_->name() + ")");
	dialog_->show();
	QLOGDEC;
}

void Quiz::finish(bool keep)
{
	dbase_->setLocked(false);
	if (keep)
	{
        const float
            correctPt = float(correct_*100)/question_,
            incorrectPt = float(incorrect_*100)/question_,
            unsurePt = float(unsure_*100)/question_;

		QMessageBox::information(GUI, tr("Done"), tr("Quiz completed.\n\nQuestions asked: ") 
			+ QString::number(question_) + " of " + QString::number(questions_.count()) 
			+ " (" + QString::number(float(question_*100)/questions_.count(),'f',2) + "%)\n"
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
	for (int i = 0; i < question_; ++i)
	{
        const QuizQuestion &q = questions_.at(i);

        if (q.result() != QuizQuestion::NOCHANGE)
        {
            // todo: make questions_ into list of structs holding DbEntries and their database indices, avoid uuid lookup?
            // otoh, this is cheap once dbase::entries_ is made into QHash<QUuid,DbEntry>
            const int idx = dbase_->entryIndex(q.entry().uuid());

            switch(q.result())
            {
                case QuizQuestion::SUCCESS: dbase_->point(idx, type_); break;
                case QuizQuestion::FAIL:    dbase_->fail(idx, type_);  break;
                default: break;
            }
        }
	}
}

void Quiz::nextQuestion()
{
	question_++;
	if (question_ >= questions_.count()) { finish(true); return; }

	QLOGX("Showing next question, number: " << question_);
	dialogUI_->progressLabel->setText(QString::number(question_ + 1) + " / " + QString::number(questions_.count()));
	dialogUI_->correctButton->setEnabled(false);
	dialogUI_->incorrectButton->setEnabled(false);
	dialogUI_->unsureButton->setEnabled(false);
	dialogUI_->revealButton->setEnabled(true);

    const QuizQuestion &question = questions_.at(question_);
	Q_ASSERT(type_ == DbEntry::DIR_SHOWITEM || type_ == DbEntry::DIR_SHOWDESC);

    QString questionText;
    switch(type_)
    {
        case DbEntry::DIR_SHOWITEM: questionText = question.entry().item(); break;
        case DbEntry::DIR_SHOWDESC: questionText = question.entry().description(); break;
        default: break;
    }
    dialogUI_->questionLabel->setText(questionText);
    dialogUI_->answerLabel->setText("");
}

void Quiz::revealAnswer()
{
	QLOGX("Revealing answer for question number " << question_);
	dialogUI_->correctButton->setEnabled(true);
	dialogUI_->incorrectButton->setEnabled(true);
	dialogUI_->unsureButton->setEnabled(true);
	dialogUI_->revealButton->setEnabled(false);

    Q_ASSERT(question_ >= 0 && question_ < questions_.count());
    const QuizQuestion &question = questions_.at(question_);
	Q_ASSERT(type_ == DbEntry::DIR_SHOWITEM || type_ == DbEntry::DIR_SHOWDESC);

    QString answerText;
    switch(type_)
    {
        case DbEntry::DIR_SHOWITEM: answerText = question.entry().description(); break;
        case DbEntry::DIR_SHOWDESC: answerText = question.entry().item(); break;
        default: break;
    }
    dialogUI_->answerLabel->setText(answerText);

    const int idx = dbase_->entryIndex(question.entry().uuid());
    const QString progressText = dialogUI_->progressLabel->text() +
            tr(" (Index: ") + QString::number(idx + 1) +
            tr(", Level: ") + QString::number(question.entry().level(type_)) +
            tr(", Failures: ") + QString::number(question.entry().fails(type_)) + ")";
	dialogUI_->progressLabel->setText(progressText);
}

void Quiz::correctClicked()
{
	QLOGX("CORRECT answer in quiz");
	QLOGINC;
	correct_++;
    QuizQuestion &question = questions_[question_];
    question.setResult(QuizQuestion::SUCCESS);
	nextQuestion();
	QLOGDEC;
}

void Quiz::incorrectClicked()
{
	QLOGX("INCORRECT answer in quiz");
	QLOGINC;
	incorrect_++;
    QuizQuestion &question = questions_[question_];
    question.setResult(QuizQuestion::FAIL);
	nextQuestion();
	QLOGDEC;
}

void Quiz::unsureClicked()
{
	QLOGX("UNSURE answer in quiz");
	QLOGINC;
	unsure_++;
    QuizQuestion &question = questions_[question_];
    question.setResult(QuizQuestion::NOCHANGE);
	nextQuestion();
	QLOGDEC;
}

void Quiz::revealClicked()
{
	QLOGX("Reveal button clicked");
	QLOGINC;
	revealAnswer();
	QLOGDEC;
}

void Quiz::closeClicked()
{
	QLOGX("Close button clicked");
	QLOGINC;

    // nothing answered yet, no results so close without further questions
	if (question_ == 0) { finish(false); QLOGDEC; return; }

    QMessageBox::StandardButtons buttons;
    QString msg;

    // dialog not yet closed, still visible and quiz can be resumed
	if (dialog_->isVisible()) 
	{
        buttons = QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
		msg = tr("Quiz will be interrupted. "); 
	}
    // dialog already closed by [x] button with mouse
    else
    {
        buttons = QMessageBox::Yes | QMessageBox::No;
        msg = tr("Quiz was interrupted. ");
    }

    const QMessageBox::StandardButton button = QMessageBox::question(GUI, tr("Quiz not complete"),
        msg + tr("Do you want to save the partial results?"), buttons);

    switch(button)
    {
        case QMessageBox::Yes: finish(true); break;
        case QMessageBox::No: finish(false); break;
        default: break;
	}
	QLOGDEC;
}
