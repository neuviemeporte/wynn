#ifndef QUIZ_H
#define QUIZ_H

#include "ui_quiz.h"
#include "database.h"
#include <vector>

namespace wynn {
namespace app {

enum QuizTakeMode { TAKE_UNDEF = -1, TAKE_FAILS, TAKE_OLDIES, TAKE_RANDOM };

struct QuizSettings {
    db::QuizDirection type;
	bool range, level, take;
	int rangeFrom, rangeTo, levelFrom, levelTo, takeCount;
	QuizTakeMode takeMode;

	QuizSettings() :
        type(db::DIR_UNDEF),
		range(false), level(false), take(false),
		rangeFrom(0), rangeTo(0), levelFrom(0), levelTo(0), takeCount(0),
		takeMode(TAKE_UNDEF) {}
};

class QuizQuestion {
public:
    enum Result { NOCHANGE, SUCCESS, FAIL };

private:
    db::Entry entry_;
    int entryIdx_;
    Result result_;

public:
    QuizQuestion(const db::Entry &entry, const int entryIdx) :
        entry_(entry), entryIdx_(entryIdx), result_(NOCHANGE) {}

    const db::Entry& entry() const { return entry_; }
    int index() const { return entryIdx_; }
    Result result() const { return result_; }
    void setResult(const Result arg) { result_ = arg; }
};

class Quiz : public QObject {
	Q_OBJECT

protected:
    db::Database *dbase_;
    db::QuizDirection type_;
    int questionIndex_, correct_, incorrect_, unsure_;
    std::vector<QuizQuestion> questions_;
	QDialog *dialog_;
	Ui::QuizDialog *dialogUI_;

public:
    Quiz(db::Database *database, const QuizSettings &settings, QDialog *dialog, Ui::QuizDialog *dialogUI);
    Quiz(db::Database *database, const QuizSettings &settings, const QList<int> &idxs, QDialog *dialog, Ui::QuizDialog *dialogUI);
	~Quiz();

    int questionCount() const { return static_cast<int>( questions_.size() ); }
    bool empty() const { return questions_.empty(); }

	void run();

signals:
	void done();

public slots:
	void correctClicked();
	void incorrectClicked();
	void unsureClicked();
	void revealClicked();
	void closeClicked();
    void finish(const bool keep);

protected:
	void addByIndex(const QList<int> &idxs);
	void addByRange(int from, int to);
	void addAll();

	void eliminateByLevel(int from, int to);
    void eliminateByTake(const int count, const QuizTakeMode mode);

private:
	void connectSignals();
	void randomizeOrder();
	void nextQuestion();
	void revealAnswer();
	void saveResults();
	void logQuestions();
};

} // namespace app
} // namespace wynn

#endif // QUIZ_H

