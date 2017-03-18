#include "activefactory.h"
#include <ttglobal.h>
#include <Windows.h>
//#include <QAxFactory>

//#ifdef TQ_AXFACTORY
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

//#endif

/*
QT_BEGIN_NAMESPACE

QAxBase *qax_create_object_wrapper(QObject *object)
{
    IDispatch *dispatch = 0;
    QAxObject *wrapper = 0;
    qAxFactory()->createObjectWrapper(object, &dispatch);
    if (dispatch) {
        wrapper = new QAxObject(dispatch, object);
        wrapper->setObjectName(object->objectName());
        dispatch->Release();
    }
    return wrapper;
}

QT_END_NAMESPACE
*/

class ActiveXObjectPrivate
{
public:
    bool isWebKit;
    ActiveXObjectPrivate()
    {
        isWebKit = false;
    }
    ~ActiveXObjectPrivate()
    {

    }
};

ActiveXObject::ActiveXObject(QObject *parent)
    :QAxObject(parent), d(new ActiveXObjectPrivate)
{
}

ActiveXObject::ActiveXObject(const QString &c, QObject *parent)
    :QAxObject(c, parent), d(new ActiveXObjectPrivate)
{
}

ActiveXObject::ActiveXObject(IUnknown *iface, QObject *parent)
    :QAxObject(iface, parent), d(new ActiveXObjectPrivate)
{
}

ActiveXObject::~ActiveXObject()
{
    delete d;
}

int ActiveXObject::qt_metacall(QMetaObject::Call call, int id, void **v)
{
//    int res = QAxBase::qt_metacall(call, id, v);
    if(call == QMetaObject::ReadProperty)
    {
        const QMetaObject *mo = metaObject();
        QMetaProperty mprop = mo->property(id);
        QVariant::Type t = mprop.type();
        QString pname(mprop.name());
        QString ptype(mprop.typeName());
        if(t == QVariant::Int || t == QVariant::String || ptype == "IDispatch*" || t == QVariant::Invalid)
        {
            IDispatch *disp;
            queryInterface(QUuid(IID_IDispatch), (void**)&disp);
            LPOLESTR p = const_cast<LPOLESTR>(pname.utf16());
            DISPID dispid;
            disp->GetIDsOfNames(IID_NULL, &p, 1, LOCALE_USER_DEFAULT, &dispid);
            if (dispid != DISPID_UNKNOWN)
            {
                //                return index;

                // property found, so everthing that goes wrong now should not bother the caller
                //            index -= mo->propertyCount();

                VARIANTARG arg;
                VariantInit(&arg);
                DISPPARAMS params;
                EXCEPINFO excepinfo;
                memset(&excepinfo, 0, sizeof(excepinfo));
                HRESULT hres = E_FAIL;

                params.cArgs = 0;
                params.cNamedArgs = 0;
                params.rgdispidNamedArgs = 0;
                params.rgvarg = 0;

                hres = disp->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &arg, &excepinfo, 0);
                QVariant &var = *(QVariant *)v[0];
                switch(t) {
                case QVariant::Int:
                case QVariant::Invalid:
                    var = arg.intVal;
                    break;
                case QVariant::String:
                    var = QString::fromUtf16(arg.bstrVal);
                    break;
                case QVariant::UserType:
                    if(ptype == "IDispatch*")
                    {
                        v[0] = createComWrapper(arg.pdispVal);
//                        *(ActiveXObject **)v = new ActiveXObject(arg.pdispVal, this);
                    }
                    break;
                }

                disp->Release();
                return 1;
            }
            disp->Release();

        }
        int res = QAxBase::qt_metacall(call, id, v);
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
            v[0] = createComWrapper(disp);
//            if(disp)
//            {
//                ActiveXObject *object = new ActiveXObject(disp, this);
//                v[0] = object;
//            }
        }
        return res;
    }
    int res = QAxBase::qt_metacall(call, id, v);
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

const char *ActiveXObject::className() const
{
    return "dd";
}

QObject *ActiveXObject::createComWrapper(IDispatch *disp)
{
    if(!disp)
        return 0;
    ActiveXObject *object = new ActiveXObject(disp, this);
    return object;
}

