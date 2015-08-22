#ifndef TQPLUGIN_GLOBAL_H
#define TQPLUGIN_GLOBAL_H

#include <QtCore/QtGlobal>

#ifdef TQ_PLUGIN_API
#  define TQPLUGIN_EXPORT Q_DECL_EXPORT
#else
#  define TQPLUGIN_EXPORT Q_DECL_IMPORT

#endif


#endif // TQPLUGIN_GLOBAL_H
