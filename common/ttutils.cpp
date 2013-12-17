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

QString filterUtf16(const QString &src)
{
    static const ushort chars[32] = {
        0x0000, // 00
        0x263A, // 01
        0x263B, // 02
        0x2665, // 03
        0x2666, // 04
        0x2663, // 05
        0x2660, // 06
        0x2022, // 07
        0x25D8, // 08
        0x0009, // 0x25CB, // 09
        0x000A, // 0x25D9, // 10
        0x2642, // 11
        0x2640, // 12
        0x000D, // 0x266A, // 13
        0x266B, // 14
        0x263C, // 15
        0x25BA, // 16
        0x25C4, // 17
        0x2195, // 18
        0x203C, // 19
        0x00B6, // 20
        0x00A7, // 21
        0x25AC, // 22
        0x21A8, // 23
        0x2191, // 24
        0x2193, // 25
        0x2192, // 26
        0x2190, // 27
        0x221F, // 28
        0x2194, // 29
        0x25B2, // 30
        0x25BC  // 31
    };
    int len = src.length();
    ushort *p,*buf = (ushort*)malloc(sizeof(ushort)*(len+1));
    qMemCopy(buf,src.utf16(),sizeof(ushort)*len);
    p=buf;
    for(int i=0; i<len; i++, p++)
        if(*p >= 0x0001 && *p < 0x0020)
            *p = chars[*p];
    QString res;
    res.setUtf16(buf,len);
    free(buf);
    return res;
}

QByteArray stringToLocal8Bit(const QString &src)
{
    int len = src.length();
    ushort *p,*buf = (ushort*)malloc(sizeof(ushort)*(len+1));
    qMemCopy(buf,src.utf16(),sizeof(ushort)*len);
    p=buf;
    for(int i=0; i<len; i++, p++)
        switch(*p) {
        case 0x263A: *p = 1; break;
        case 0x263B: *p = 2; break;
        case 0x2665: *p = 3; break;
        case 0x2666: *p = 4; break;
        case 0x2663: *p = 5; break;
        case 0x2660: *p = 6; break;
        case 0x2022: *p = 7; break;
        case 0x25D8: *p = 8; break;
        case 0x2642: *p = 11; break;
        case 0x2640: *p = 12; break;
        case 0x266B: *p = 14; break;
        case 0x263C: *p = 15; break;
        case 0x25BA: *p = 16; break;
        case 0x25C4: *p = 17; break;
        case 0x2195: *p = 18; break;
        case 0x203C: *p = 19; break;
        case 0x00B6: *p = 20; break;
        case 0x00A7: *p = 21; break;
        case 0x25AC: *p = 22; break;
        case 0x21A8: *p = 23; break;
        case 0x2191: *p = 24; break;
        case 0x2193: *p = 25; break;
        case 0x2192: *p = 26; break;
        case 0x2190: *p = 27; break;
        case 0x221F: *p = 28; break;
        case 0x2194: *p = 29; break;
        case 0x25B2: *p = 30; break;
        case 0x25BC: *p = 31; break;
        }
    QString res;
    res.setUtf16(buf,len);
    free(buf);
    return res.toLocal8Bit();
}
