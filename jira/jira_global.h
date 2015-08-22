#ifndef JIRA_GLOBAL_H
#define JIRA_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JIRA_LIBRARY)
#  define JIRASHARED_EXPORT Q_DECL_EXPORT
#else
#  define JIRASHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // JIRA_GLOBAL_H
