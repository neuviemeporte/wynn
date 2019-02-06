#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <random>

#include "quiz.h"
#include "database.h"
#include "ixplog_active.h"
extern class QTSLogger APP_LOG;
extern class QTSLogger *APP_LOGSTREAM;
#define QLOGSTREAM APP_LOGSTREAM
#define QLOGTAG "[WYNN_DB]"
#define QUIZDEBUG 1

namespace wynn {
namespace db {

static const QString EMPTY_STRING;

// todo: undo button in quiz dialog
Quiz::Quiz(DatabaseInterface *database, const QuizSettings &settings) 
  : dbase_(database), type_(settings.type) {
  QLOGX("Creating new quiz from database '" << dbase_->name() << "', items: " << dbase_->entryCount() << ", criteria-based"
        << ", type: " << type_);
  
  srand(time(NULL));
  if (settings.range) {
    QLOG("Range criterium active");
    addByRange(settings.rangeFrom, settings.rangeTo);
  }
  else {
    QLOG("Range criterium inactive");
    addAll();
  }
  
  if (settings.level) {
    QLOG("Level criterium active");
    eliminateByLevel(settings.levelFrom, settings.levelTo);
  }
  
  if (settings.take) {
    QLOG("Take criterium active");
    eliminateByTake(settings.takeCount, settings.takeMode);
  }
  
  randomizeOrder();
}

Quiz::Quiz(DatabaseInterface *database, const QuizDirection type, const QList<int> &idxs) 
  : dbase_(database), type_(type) {
  QLOGX("Creating new quiz from database '" << dbase_->name() << "', items: " << dbase_->entryCount()
        << ", from provided list of " << idxs.size() << " indices, type: " << type_);
  
  addByIndex(idxs);
  randomizeOrder();
}

int Quiz::questionLevel() const {
  if (finished()) 
    return -1;
  
  const auto &q = questions_.at(questionIndex());
  return q.entry().level(type_);
}

int Quiz::questionFails() const {
  if (finished()) 
    return -1;
  
  const auto &q = questions_.at(questionIndex());
  return q.entry().fails(type_);
}

const QString& Quiz::questionText() const {
  if (finished()) 
    return EMPTY_STRING;
  
  const auto &q = questions_.at(questionIndex());
  switch ( type_ ) {
  case db::DIR_SHOWITEM: return q.entry().item();
  case db::DIR_SHOWDESC: return q.entry().description();
  default: return DEFAULT_VAL;
  }
}

const QString& Quiz::answerText() const {
  if (finished()) 
    return EMPTY_STRING;
  
  const auto &q = questions_.at(questionIndex());
  switch ( type_ ) {
  case db::DIR_SHOWITEM: return q.entry().description();
  case db::DIR_SHOWDESC: return q.entry().item();
  default: return DEFAULT_VAL;
  }
}

void Quiz::answer(const QuizAnswer ans) {
  if (finished())
    return;
  
  QuizResult res = NOCHANGE;
  switch (ans) {
  case ANS_CORRECT:   res = SUCCESS;  stats_.correct++;   break;
  case ANS_INCORRECT: res = FAIL;     stats_.incorrect++; break;
  case ANS_UNSURE:    res = NOCHANGE; stats_.unsure++;    break;
  }
  questions_[stats_.curQuestion++].setResult(res);
}

void Quiz::saveResults() {
  QLOGX("Saving quiz results back to database");
  // update entries only up to the answered question index
  for ( auto &q : questions_ ) {
    if ( q.result() == NOCHANGE )
      continue;
    
    switch ( q.result() ) {
    case SUCCESS: dbase_->point(q.index(), type_); break;
    case FAIL:    dbase_->fail(q.index(), type_);  break;
    default: break;
    }
  }
}

void Quiz::logQuestions() {
#ifdef QUIZDEBUG
  QLOG("Questions (" << questionCount() << "):");
  for (auto &q : questions_){
    QLOG("entry: " << q.entry() << ", result: " << q.result());
  }
#endif
}

void Quiz::addByIndex(const QList<int> &idxs) {
  QLOGX("Adding  " << idxs.count() << " questions.");
  for (int i : idxs) {
    questions_.emplace_back(dbase_->entry(i), i);
  }
  logQuestions();
}

void Quiz::addByRange(int from, int to) {
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

void Quiz::randomizeOrder() {
  if ( questions_.empty() ) 
    return;
  QLOGX("Randomizing questions' order");
  std::random_device rd;
  std::mt19937 g(rd());
  std::shuffle( questions_.begin(), questions_.end(), g );
  logQuestions();
}

} // namespace db
} // namespace wynn
