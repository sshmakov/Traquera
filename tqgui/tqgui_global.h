#ifndef TQGUI_GLOBAL_H
#define TQGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TQGUI_LIBRARY)
#  define TQGUISHARED_EXPORT Q_DECL_EXPORT
#else
#  define TQGUISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TQGUI_GLOBAL_H
