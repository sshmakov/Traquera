#ifndef TQSERVICE_GLOBAL_H
#define TQSERVICE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(TQSERVICE_LIBRARY)
#  define TQSERVICESHARED_EXPORT Q_DECL_EXPORT
#else
#  define TQSERVICESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // TQSERVICE_GLOBAL_H
