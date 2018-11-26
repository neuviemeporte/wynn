#pragma once

#include "ui_quiz.h"
#include "database.h"
#include <QList>

class Database;
class DbEntry;

enum QuizTakeMode { TAKE_UNDEF = -1, TAKE_FAILS, TAKE_OLDIES, TAKE_RANDOM };

struct QuizSettings
{
	DbEntry::QuizDirection type;
	bool range, level, take;
	int rangeFrom, rangeTo, levelFrom, levelTo, takeCount;
	QuizTakeMode takeMode;

	QuizSettings() :
		type(DbEntry::DIR_UNDEF),
		range(false), level(false), take(false),
		rangeFrom(0), rangeTo(0), levelFrom(0), levelTo(0), takeCount(0),
		takeMode(TAKE_UNDEF) {}
};

class QuizQuestion
{
public:
    enum Result { NOCHANGE, SUCCESS, FAIL };

private:
    DbEntry entry_;
    Result result_;

public:
    QuizQuestion(const DbEntry entry) : entry_(entry), result_(NOCHANGE) {}

    const DbEntry& entry() const { return entry_; }
    Result result() const { return result_; }

    void setResult(const Result arg) { result_ = arg; }
};

bool showDescFailCompare(const QuizQuestion &arg1, const QuizQuestion &arg2);
bool showItemFailCompare(const QuizQuestion &arg1, const QuizQuestion &arg2);
bool dateCompare(const QuizQuestion &arg1, const QuizQuestion &arg2);

class Quiz : public QObject
{
	Q_OBJECT

protected:
	Database *dbase_;
	DbEntry::QuizDirection type_;
	int question_, correct_, incorrect_, unsure_;
    QList<QuizQuestion> questions_;
	QDialog *dialog_;
	Ui::QuizDialog *dialogUI_;

public:
	Quiz(Database *database, const QuizSettings &settings, QDialog *dialog, Ui::QuizDialog *dialogUI);
    Quiz(Database *database, const QuizSettings &settings, const QList<int> &idxs, QDialog *dialog, Ui::QuizDialog *dialogUI);
	~Quiz();

	int questionCount() const { return questions_.count(); }
	bool empty() const { return questions_.isEmpty(); }

	void run();

signals:
	void done();

public slots:
	void correctClicked();
	void incorrectClicked();
	void unsureClicked();
	void revealClicked();
	void closeClicked();
	void finish(bool keep);

protected:
	void addByIndex(const QList<int> &idxs);
	void addByRange(int from, int to);
	void addAll();

	void eliminateByLevel(int from, int to);
	void eliminateByTake(int count, QuizTakeMode mode);

private:
	void connectSignals();
	void randomizeOrder();
	void nextQuestion();
	void revealAnswer();
	void saveResults();
	void logQuestions();
};
