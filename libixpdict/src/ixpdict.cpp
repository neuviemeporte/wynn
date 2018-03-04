#include "ixpdict.h"
#include "ixplog_active.h"

QTSLogger
    IXPDICT_LOG("libcjdict_log.txt"),
    *IXPDICT_LOGSTREAM = &IXPDICT_LOG;

// make the functions of this library log to this entity instead of the deault one; swich the pointer to the log entity
void IXPDICT_LOGREDIR(class QTSLogger *log)
{
	IXPDICT_LOGSTREAM = log;
}
