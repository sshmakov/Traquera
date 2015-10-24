#include "tqjson.h"
#include <QtScript>

class TQJsonPrivate
{
public:
    QScriptEngine *engine;
    QTextCodec *codec;
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
    if(!d->codec)
        return QString();
    return d->codec->name();
}

void TQJson::setCharset(const QString &charset)
{
    d->codec = QTextCodec::codecForName(charset.toLocal8Bit().constData());
}

void TQJson::reset()
{
    if(d->engine)
    {
        delete d->engine;
        d->engine = 0;
    }
    d->engine = new QScriptEngine(this);
    d->engine->evaluate("function toString() { return JSON.stringify(this) }");
    d->codec = QTextCodec::codecForLocale();
}

QString TQJson::toString(const QVariant &value)
{
    QScriptValue toString = d->engine->globalObject().property("toString");
    QScriptValue obj = d->engine->toScriptValue(value);
    return toString.call(obj).toString();
}

QByteArray TQJson::toByteArray(const QVariant &value)
{
    QString string = toString(value);
    QByteArray buf = d->codec->fromUnicode(string);
    return buf;
}

QVariant TQJson::toVariant(const QString &string)
{
    QScriptValue val = d->engine->evaluate("("+string+")");
    return val.toVariant();
}

QVariant TQJson::toVariant(const QByteArray &buf)
{
    QString string = d->codec->toUnicode(buf);
    QScriptValue val = d->engine->evaluate("("+string+")");
    return val.toVariant();

}

bool TQJson::isError() const
{
    return d->engine->hasUncaughtException();
}

QString TQJson::errorString() const
{
    QScriptValue v = d->engine->uncaughtException();
    return v.toString();
}
