#include <QtCore>
#include "tqdebug.h"

#ifdef QT_DEBUG
int TQDebug::curVerboseLevel = TQDebugLevel;
#else
int TQDebug::curVerboseLevel = TQInfoLevel;
#endif

static TQMsgWriter curWriter;

void TQDebug::setVerboseLevel(int level)
{
    curVerboseLevel = level;
}

int TQDebug::verboseLevel()
{
    return curVerboseLevel;
}

TQMsgWriter TQDebug::registerMsgWriter(TQMsgWriter writer)
{
    TQMsgWriter old = curWriter;
    curWriter = writer;
    return old;
}

void TQDebug::flush()
{
    if(curWriter)
        curWriter(lvl, buffer+"\n");
    else if(lvl >= TQDebugLevel)
        qDebug(buffer.toLocal8Bit());
    else if(lvl >= TQInfoLevel)
        qWarning(buffer.toLocal8Bit());
    else if(lvl >= TQCriticalLevel)
        qCritical(buffer.toLocal8Bit());
    else if(lvl >= TQFatalLevel)
        qFatal(buffer.toLocal8Bit());
}

