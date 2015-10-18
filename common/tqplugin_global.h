#ifndef TQPLUGIN_GLOBAL_H
#define TQPLUGIN_GLOBAL_H

#include <QtCore/QtGlobal>

#ifdef TQ_PLUGIN_STATIC
#define TQPLUGIN_SHARED
#else
#ifdef TQ_PLUGIN_API
#  define TQPLUGIN_SHARED Q_DECL_EXPORT
#else
#  define TQPLUGIN_SHARED Q_DECL_IMPORT

#endif
#endif

#endif // TQPLUGIN_GLOBAL_H
