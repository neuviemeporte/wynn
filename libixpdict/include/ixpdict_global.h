#ifndef LIBIXPDICT_GLOBAL_H
#define LIBIXPDICT_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef IXPDICT_LIB
# define LIBIXPDICT_EXPORT Q_DECL_EXPORT
#else
# define LIBIXPDICT_EXPORT Q_DECL_IMPORT
#endif

extern class QTSLogger *IXPDICT_LOGSTREAM;

#endif // LIBIXPDICT_GLOBAL_H
