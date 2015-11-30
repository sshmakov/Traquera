#include "activefactory.h"
#include <ttglobal.h>
#include <Windows.h>

ActiveFactory::ActiveFactory( const QUuid & libid, const QUuid & appid ) :
    QAxFactory(libid, appid)
{
}

QStringList ActiveFactory::featureList() const
{
    QStringList list;
//    list << "ActiveX1";
//    list << "ActiveX2";
    return list;
}

QObject *ActiveFactory::createObject(const QString &key)
{
//    if (key == "ActiveX1")
//        return new ActiveX1(parent);
//    if (key == "ActiveX2")
//        return new ActiveX2(parent);
    return 0;
}

const QMetaObject *ActiveFactory::metaObject(const QString &key) const
{
//    if (key == "ActiveX1")
//        return &ActiveX1::staticMetaObject;
//    if (key == "ActiveX2")
//        return &ActiveX2::staticMetaObject;
    return 0;
}

QUuid ActiveFactory::classID(const QString &key) const
{
//    if (key == "ActiveX1")
//        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
//    ...
    return QUuid();
}

QUuid ActiveFactory::interfaceID(const QString &key) const
{
//    if (key == "ActiveX1")
//        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
//    ...
    return QUuid();
}

QUuid ActiveFactory::eventsID(const QString &key) const
{
//    if (key == "ActiveX1")
//        return "{01234567-89AB-CDEF-0123-456789ABCDEF}";
//    ...
    return QUuid();
}

//QAXFACTORY_EXPORT(
//    ActiveFactory,                          // factory class
//    "{01234567-89AB-CDEF-0123-456789ABCDEF}", // type library ID
//    "{01234567-89AB-CDEF-0123-456789ABCDEF}"  // application ID
//)

QAXFACTORY_BEGIN
(
     "{D9A71614-5EC2-4e97-A39A-6B765AA0FFDE}", // type library ID
     "{E35822C3-06E7-475a-B26F-7187D64F1F2A}"  // application ID
)
    QAXCLASS(TTGlobal)
//     QAXCLASS(Class2)
QAXFACTORY_END()

ActiveXObject::ActiveXObject(QObject *parent)
    :QAxObject(parent)
{
}

ActiveXObject::ActiveXObject(const QString &c, QObject *parent)
    :QAxObject(c, parent)
{
}

ActiveXObject::ActiveXObject(IUnknown *iface, QObject *parent)
    :QAxObject(iface, parent)
{
}

ActiveXObject::~ActiveXObject()
{
}

int ActiveXObject::qt_metacall(QMetaObject::Call call, int id, void **v)
{
    int res = QAxBase::qt_metacall(call, id, v);
    if(call == QMetaObject::ReadProperty)
    {
        const QMetaObject *mo = metaObject();
        QMetaProperty mprop = mo->property(id);
        QString ptype(mprop.typeName());
        if(ptype == "IDispatch*")
        {
//            VARIANT *var = (VARIANT *)v[0];
//            IDispatch *disp = *var->ppdispVal;
            IDispatch *disp = *(IDispatch **)v[0];
            *(ActiveXObject **)v = new ActiveXObject(disp, this);
        }
        return res;
        QVariant &var = *(QVariant *)v[0];
        if(var.isValid())
        {
            IDispatch *disp = var.value<IDispatch*>();
            if(disp)
            {
                ActiveXObject *object = new ActiveXObject(disp, this);
                v[0] = object;
            }
        }
        return res;
    }
    if(call != QMetaObject::InvokeMetaMethod)
        return res;
    const QMetaObject *mo = metaObject();
    int index = id + mo->methodOffset();
    QMetaMethod::MethodType mtype = mo->method(index).methodType();
    if(mtype != QMetaMethod::Method && mtype != QMetaMethod::Slot)
        return res;
    int pCount = mo->method(index).parameterTypes().size();
    char *p=0;
    if(pCount)
    {
        p = (char*)v[0];
    }
    return res;
}

