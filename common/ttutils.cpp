#include "ttutils.h"

QList<int> toIntList(const QString & ids)
{
    QStringList slist=ids.split(",",QString::SkipEmptyParts);
    QList<int> res;
    for(int i=0; i<slist.count(); i++)
    {
        bool ok;
        int v = slist[i].toInt(&ok);
        if(ok)
            res.append(v);
    }
    return res;
}


int parseToIntList(const char *text, QList<int> &arr)
{
    const char *p=text;
    bool digit=false;
    arr.clear();
    int cur=-1;
    while(*p)
    {
        if(*p >= '0' && *p <='9')
        {
            if(!digit)
            {
                ++cur;
                arr.append(0);
            }
            arr[cur]=arr[cur]*10+(*p-'0');
            digit = true;
        }
        else
        {
            digit = false;
        }
        p++;
    }
    return arr.count();
}

QList<int> stringToIntList(const QString &s)
{
    QList<int> ilist;
    parseToIntList(s.toLocal8Bit().constData(), ilist);
    return ilist;
    /*
    QStringList slist = s.split(',');
    foreach(const QString &si, slist)
    {
        bool ok;
        int i = si.toInt(&ok);
        if(ok)
            ilist << i;
    }
    return ilist;
    */
}


QList<int> uniqueIntList(const QList<int> &arr)
{
    QSet<int> s = arr.toSet();
    QList<int> unique = s.toList();
    qSort(unique);
    return unique;
}

/*
QList<int> minimizeIntList(const QList<int> &arr)
{
    QList<int> list = uniqueIntList(arr);
    QList<int>::iterator i = qUpperBound(list.begin(), list.end(), 10000);

    QSet<int> s = arr.toSet();
    QList<int> unique = s.toList();
    qSort(unique);
    return unique;
}
*/

QString intListToString(const QList<int> &arr)
{
    QStringList numbers;
    foreach(int i, arr)
        numbers.append(QString::number(i));
    return numbers.join(", ");
}

QString onlyInts(const QString &text)
{
    return intListToString(uniqueIntList(stringToIntList(text)));
}

/*
// Utils

QString intListToString(const QList<int> &list)
{
    QString res;
    foreach(int i, list)
    {
        if(!res.isEmpty())
            res += ",";
        res += QString::number(i);
    }
    return res;
}

*/
