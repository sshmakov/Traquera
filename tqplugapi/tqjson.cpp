#include "tqjson.h"
#include <QtScript>
#include <QMutex>
#include <QMutexLocker>

class TQJsonPrivate
{
public:
    QScriptEngine *engine;
    QTextCodec *codec;
    QMutex mutex;
};

TQJson::TQJson(QObject *parent) :
    QObject(parent), d(new TQJsonPrivate())
{
    d->engine = 0;
    reset();
}

TQJson::~TQJson()
{
}

QString TQJson::charset() const
{
    QMutexLocker locker(&d->mutex);
    if(!d->codec)
        return QString();
    return d->codec->name();
}

void TQJson::setCharset(const QString &charset)
{
    QMutexLocker locker(&d->mutex);
    d->codec = QTextCodec::codecForName(charset.toLocal8Bit().constData());
}

void TQJson::reset()
{
    QMutexLocker locker(&d->mutex);
    if(d->engine)
    {
        delete d->engine;
        d->engine = 0;
    }
    try {
        d->engine = new QScriptEngine(this);
        d->engine->evaluate("function toString() { return JSON.stringify(this) }");
        d->codec = QTextCodec::codecForLocale();
    }
    catch(...) {
        d->engine = 0;
    }
}

QString TQJson::toString(const QVariant &value)
{
    QMutexLocker locker(&d->mutex);
    if(!d->engine)
        return QString();
    QScriptValue toString = d->engine->globalObject().property("toString");
    QScriptValue obj = d->engine->toScriptValue(value);
    return toString.call(obj).toString();
}

QByteArray TQJson::toByteArray(const QVariant &value)
{
    //QMutexLocker locker(&d->mutex);
    QString string = toString(value);
    QByteArray buf = d->codec->fromUnicode(string);
    return buf;
}

QVariant TQJson::toVariant(const QString &string)
{
    QMutexLocker locker(&d->mutex);
    QScriptValue val = d->engine->evaluate("("+string+")");
    return val.toVariant();
}

QVariant TQJson::toVariant(const QByteArray &buf)
{
    QMutexLocker locker(&d->mutex);
    if(!d->engine)
        return QVariant();
    QString string = d->codec->toUnicode(buf);
    QScriptValue val = d->engine->evaluate("("+string+")");
    return val.toVariant();

}

bool TQJson::isError() const
{
    QMutexLocker locker(&d->mutex);
    if(!d->engine)
        return true;
    return d->engine->hasUncaughtException();
}

QString TQJson::errorString() const
{
    QMutexLocker locker(&d->mutex);
    if(!d->engine)
        return tr("Not initialized");
    QScriptValue v = d->engine->uncaughtException();
    return v.toString();
}
