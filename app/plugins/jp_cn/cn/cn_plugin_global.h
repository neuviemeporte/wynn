#ifndef CN_PLUGIN_GLOBAL_H
#define CN_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef CN_PLUGIN_LIB
# define CN_PLUGIN_EXPORT Q_DECL_EXPORT
#else
# define CN_PLUGIN_EXPORT Q_DECL_IMPORT
#endif

#endif // CN_PLUGIN_GLOBAL_H
