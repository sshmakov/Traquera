#ifndef TTUTILS_H
#define TTUTILS_H

#include <QtCore>

QList<int> toIntList(const QString & ids);
int parseToIntList(const char *text, QList<int> &arr);
QList<int> uniqueIntList(const QList<int> &arr);
QString intListToString(const QList<int> &arr);

//QString intListToString(const QList<int> &list);
QList<int> stringToIntList(const QString &s);
QString onlyInts(const QString &text);



#endif // TTUTILS_H
