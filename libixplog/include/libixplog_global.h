#ifndef LIBIXPLOG_GLOBAL_H
#define LIBIXPLOG_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef IXPLOG_LIB
#define LIBQTLOG_EXPORT Q_DECL_EXPORT
#else
#define LIBQTLOG_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBIXPLOG_GLOBAL_H
