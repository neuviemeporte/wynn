#ifndef DE_PLUGIN_GLOBAL_H
#define DE_PLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef DE_PLUGIN_LIB
# define DE_PLUGIN_EXPORT Q_DECL_EXPORT
#else
# define DE_PLUGIN_EXPORT Q_DECL_IMPORT
#endif

#endif // DE_PLUGIN_GLOBAL_H
