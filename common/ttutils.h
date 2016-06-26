#ifndef TTUTILS_H
#define TTUTILS_H

#include <QtCore>

extern QList<int> toIntList(const QString & ids);
//int parseToIntList(const char *text, QList<int> &arr);
extern bool isIntListOnly(const QString &text);
extern QList<int> uniqueIntList(const QList<int> &arr);
extern QString intListToString(const QList<int> &arr);

//QString intListToString(const QList<int> &list);
extern QList<int> stringToIntList(const QString &s);
extern QString onlyInts(const QString &text);
extern QString filterUtf16(const QString &src);
extern QByteArray stringToLocal8Bit(const QString &src);


#endif // TTUTILS_H
