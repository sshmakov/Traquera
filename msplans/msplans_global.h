#ifndef MSPLANS_GLOBAL_H
#define MSPLANS_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(MSPLANS_LIBRARY)
#  define MSPLANSSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MSPLANSSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // MSPLANS_GLOBAL_H
