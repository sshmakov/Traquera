#ifndef TRACKER_GLOBAL_H
#define TRACKER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TRACKER_LIBRARY)
#  define TRACKERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TRACKERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TRACKER_GLOBAL_H
