#ifndef TQDEBUG_H
#define TQDEBUG_H

#include <QDebug>
#include <QUrl>
#include <tqplugin_global.h>

typedef void (*TQMsgWriter)(int level, const QString &string);
enum TQMsgLevel {
    TQVerboseLevel = 4,
    TQDebugLevel = 3,
    TQInfoLevel = 2,
    TQWarningLevel = 2,
    TQErrorLevel = 1,
    TQCriticalLevel = 1,
    TQFatalLevel = 0};


class TQPLUGIN_SHARED TQDebug
{
private:
    int lvl;
    static int curVerboseLevel;
    QTextStream ts;
    QString buffer;
    bool needSpace;
public:
    inline TQDebug() :
        lvl(TQDebugLevel),
        ts(&buffer, QIODevice::WriteOnly),
        needSpace(false)
    {}
    inline TQDebug(int level) :
        lvl(level),
        ts(&buffer, QIODevice::WriteOnly),
        needSpace(false)
    {}
    inline ~TQDebug()
    {
        if(lvl <= curVerboseLevel)
            flush();
    }
    inline TQDebug &operator <<(const QString &string)
    {
        if(lvl <= curVerboseLevel)
        {
            ts << space() << string;
        }
        return *this;
    }
    inline TQDebug &operator <<(const QUrl &url)
    {
        if(lvl <= curVerboseLevel)
            ts << space() << QString("URL(%1)").arg(url.toString());
        return *this;
    }

    inline QString space()
    {
        if(needSpace)
            return QString(' ');
        else
        {
            needSpace = true;
            return QString();
        }
    }

    static void setVerboseLevel(int level);
    static int verboseLevel();
    static TQMsgWriter registerMsgWriter(TQMsgWriter writer);
protected:
    void flush();
};

inline TQDebug tqDebug()
{
    return TQDebug();
}

inline TQDebug tqDebug(int initLevel)
{
    return TQDebug(initLevel);
}

inline TQDebug tqInfo()
{
    return TQDebug((int)TQInfoLevel);
}

inline TQDebug tqError()
{
    return TQDebug((int)TQErrorLevel);
}

#endif // TQDEBUG_H
