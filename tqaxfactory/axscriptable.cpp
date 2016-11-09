#include "axscriptable.h"

#include <QtCore>
#include <QMetaMethod>
#include <QScriptEngine>
#include <QDebug>
#include <QScriptContextInfo>

#include <oaidl.h>

struct AxFuncDef
{
    AxScriptable *obj;
    QString funcName;
    QString signature;
    int argCount;
};

class AxScriptablePrivate
{
public:
    QScriptEngine *engine;
    QAxObject *ax;
    const QMetaObject *meta;
    QScriptValue proto;
    QStringList props;
    QMultiMap<QString, AxFuncDef*> funcs;

    QScriptValue variantToValue(QVariant &v);
    static QScriptValue callFunction(QScriptContext *context, QScriptEngine *engine, void *arg);
};

AxScriptable::AxScriptable(QScriptEngine *engine)
    : QScriptClass(engine), d(new AxScriptablePrivate())
{
    d->engine = engine;
    d->ax = 0;
    d->meta = 0;
}

AxScriptable::~AxScriptable()
{
    foreach(AxFuncDef* def, d->funcs)
        delete def;
}

static QScriptValue testFunction(QScriptContext *context, QScriptEngine *engine, void *arg)
{
    if(context->isCalledAsConstructor())
        return context->throwError(QScriptContext::TypeError, "not constructor");
    AxFuncDef *def = (AxFuncDef *)arg;
    AxScriptable *obj = def->obj;
    QString fName = def->funcName;
    QVariantList args;
    for(int i = 0; i < context->argumentCount(); i++)
    {
        QVariant v = context->argument(i).toVariant();
        args.append(v);
    }
    QString method = def->signature;
    QVariant v = obj->axObject()->dynamicCall(method.toLatin1().constData(), args);
//    return obj->d->variantToValue(v);
    return engine->newVariant(v);
}

void AxScriptable::setAxObject(QAxObject *ax)
{
    d->ax = ax;
    if(!ax)
    {
        d->meta = 0;
        return;
    }
    d->meta = ax->metaObject();
    d->proto = d->engine->newObject();
    for(int i = 0; i<d->meta->propertyCount(); i++)
    {
        d->proto.setProperty(d->meta->property(i).name(), d->engine->newObject());
    }
    for(int i = 0; i<d->meta->methodCount(); i++)
    {
        QMetaMethod method = d->meta->method(i);
        AxFuncDef *def = new AxFuncDef;
        def->signature = method.signature();
        def->funcName = def->signature.left(def->signature.indexOf('('));
        def->argCount = method.parameterNames().size();
        def->obj = this;
        d->funcs.insert(def->funcName, def);
    }
    QStringList funcNames = d->funcs.keys() ;
    funcNames.removeDuplicates();
    foreach(const QString func, funcNames)
    {
        QList<AxFuncDef*> list = d->funcs.values(func);
        d->proto.setProperty(func, d->engine->newFunction(AxScriptablePrivate::callFunction, list.first()));
    }
//    d->proto.setProperty("FileOpenEx", d->engine->newFunction(testFunction, this));
}

QAxObject *AxScriptable::axObject() const
{
    return d->ax;
}

QScriptValue AxScriptable::property(const QScriptValue &object, const QScriptString &name, uint id)
{
    qDebug() << "Property" << name << id;
    QVariant v = d->ax->property(name.toString().toLatin1().constData());
    qDebug() << "- returned value type" << v.typeName();
    return d->variantToValue(v);
}

void AxScriptable::setProperty(QScriptValue &object, const QScriptString &name, uint id, const QScriptValue &value)
{
    d->ax->setProperty(name.toString().toLatin1().constData(), value.toVariant());
}

QScriptValue::PropertyFlags AxScriptable::propertyFlags(const QScriptValue &object, const QScriptString &name, uint id)
{
    qDebug() << "Property flags" << name << id;
    return QScriptValue::ReadOnly;
}

QScriptValue AxScriptable::prototype() const
{
    return d->proto;
//    return d->engine->toScriptValue<QObject*>(d->ax);
//    QString proto = "{ Visible: false, FileOpenEx: function() }";
//    return proto;
}

QScriptClass::QueryFlags AxScriptable::queryProperty(const QScriptValue &object, const QScriptString &name, QScriptClass::QueryFlags flags, uint *id)
{
    qDebug() << "Query property" << name.toString() << flags << id;
    QString propName = name.toString();
    int i;
    if((i = d->meta->indexOfProperty(propName.toLatin1().constData())) != -1)
    {
        return (QScriptClass::HandlesWriteAccess | QScriptClass::HandlesReadAccess) & flags;
    }
//    if(name.toString() ==  "Visible")
//        return (QScriptClass::HandlesWriteAccess | QScriptClass::HandlesReadAccess) & flags;
    return QScriptClass::queryProperty(object, name, flags, id);
}

static QScriptValue newAxObject(QScriptContext *context, QScriptEngine *engine)
{
    if(!context->isCalledAsConstructor())
    {
        qDebug() << "Called isn't as constructor";
        return context->throwError("Called isn't as constructor");
    }
    if(!context->argumentCount())
    {
        qDebug() << "Class name not presented";
        return context->throwError("Class name not presented");
    }
    QString className = context->argument(0).toString();
    qDebug() << className;
    AxScriptable *obj = new AxScriptable(engine);
    QAxObject *ax = new QAxObject();
    ax->setControl(className);
    obj->setAxObject(ax);
    return engine->newObject(obj);
}

void AxScriptable::initEngine(QScriptEngine *engine)
{
    QScriptValue fooProto = engine->newObject();
    QScriptValue func;
    func = engine->newFunction(newAxObject, fooProto, 1);
    engine->globalObject().setProperty("ActiveXObject", func);
}

QScriptValue AxScriptablePrivate::variantToValue(QVariant &v)
{
    if(v.canConvert<IDispatch*>())
    {
        IDispatch *disp = v.value<IDispatch*>();
        if(disp)
        {
            IUnknown *unk;
            void *p;
            disp->QueryInterface(IID_IUnknown, &p);
            unk = (IUnknown *)p;
            if(unk)
            {
                QAxObject *obj = new QAxObject(unk);
                AxScriptable *value = new AxScriptable(engine);
                value->setAxObject(obj);
                return engine->newObject(value);
            }
        }
    }
    return engine->newVariant(v);
}

QScriptValue AxScriptablePrivate::callFunction(QScriptContext *context, QScriptEngine *engine, void *arg)
{
    if(context->isCalledAsConstructor())
        return context->throwError(QScriptContext::TypeError, "not constructor");
    AxFuncDef *def = (AxFuncDef *)arg;
    AxScriptable *obj = def->obj;
    QString fName = def->funcName;
    QVariantList args;
    for(int i = 0; i < context->argumentCount(); i++)
    {
        QVariant v = context->argument(i).toVariant();
        args.append(v);
    }
    QString method = def->signature;
    QVariant v = obj->axObject()->dynamicCall(method.toLatin1().constData(), args);
    return obj->d->variantToValue(v);
//    return engine->newVariant(v);
}
