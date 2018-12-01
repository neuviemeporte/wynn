#ifndef QUIZ_H
#define QUIZ_H

#include "database.h"
#include <vector>

namespace wynn {
namespace db {

enum QuizDirection { DIR_UNDEF = -1, DIR_SHOWDESC = 101, DIR_SHOWITEM = 202 };
enum QuizTakeMode { TAKE_UNDEF = -1, TAKE_FAILS, TAKE_OLDIES, TAKE_RANDOM };
enum QuizResult { NOCHANGE, SUCCESS, FAIL };

struct QuizSettings {
    QuizDirection type;
	bool range, level, take;
	int rangeFrom, rangeTo, levelFrom, levelTo, takeCount;
	QuizTakeMode takeMode;

	QuizSettings() :
        type(DIR_UNDEF),
		range(false), level(false), take(false),
		rangeFrom(0), rangeTo(0), levelFrom(0), levelTo(0), takeCount(0),
		takeMode(TAKE_UNDEF) {}
};

struct QuizStats {
    int curQuestion, correct, incorrect, unsure;
    QuizStats() : curQuestion(0), correct(0), incorrect(0), unsure(0) {}
    int count(const QuizResult type) const {
        switch (type) {
        case NOCHANGE: return unsure;
        case SUCCESS:  return correct;
        case FAIL:     return incorrect;
        default: return -1;
        }
    }
    float percent(const QuizResult type) const { return static_cast<float>(count(type) * 100) / curQuestion; }
    float complete(const int count) const { return static_cast<float>(curQuestion * 100)/count; }
};

class QuizQuestion {
public:

private:
    Entry entry_;
    int entryIdx_;
    QuizResult result_;

public:
    QuizQuestion(const Entry &entry, const int entryIdx) :
        entry_(entry), entryIdx_(entryIdx), result_(NOCHANGE) {}

    const Entry& entry() const { return entry_; }
    int index() const { return entryIdx_; }
    QuizResult result() const { return result_; }
    void setResult(const QuizResult arg) { result_ = arg; }
};

class Quiz {
protected:
    Database *dbase_;
    QuizDirection type_;
    QuizStats stats_;
    std::vector<QuizQuestion> questions_;

public:
    Quiz(Database *database, const QuizSettings &settings);
    Quiz(Database *database, const QuizDirection type, const QList<int> &idxs);

    int questionCount() const { return static_cast<int>( questions_.size() ); }
    bool empty() const { return questions_.empty(); }
    int questionIndex() const { return stats_.curQuestion; }
    const QuizStats& stats() const { return stats_; }
    int questionLevel() const;
    int questionFails() const;
    const QString& questionText() const;
    const QString& answerText() const;

    void answerCorrect();
    void answerIncorrect();
    void answerUnsure();
    void saveResults();

protected:
	void addByIndex(const QList<int> &idxs);
	void addByRange(int from, int to);
	void addAll();

	void eliminateByLevel(int from, int to);
    void eliminateByTake(const int count, const QuizTakeMode mode);

private:
	void randomizeOrder();
	void logQuestions();
};

} // namespace db
} // namespace wynn

#endif // QUIZ_H

