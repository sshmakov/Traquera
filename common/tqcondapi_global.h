#ifndef TQCONDAPI_GLOBAL_H
#define TQCONDAPI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TQCONDAPI_LIBRARY)
#  define TQCONDAPISHARED_EXPORT Q_DECL_EXPORT
#else
#  define TQCONDAPISHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TQCONDAPI_GLOBAL_H
