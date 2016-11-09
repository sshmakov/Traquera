#ifndef AXSCRIPTABLE_H
#define AXSCRIPTABLE_H
#include <QObject>
#include <QScriptable>
#include <QScriptClass>
#include <QAxObject>
#include "tqaxfactory_global.h"

class AxScriptablePrivate;

class TQAXFACTORYSHARED_EXPORT AxScriptable : public QScriptClass
{
    AxScriptablePrivate *d;
public:
    AxScriptable(QScriptEngine *engine);
    ~AxScriptable();
    void setAxObject(QAxObject *ax);
    QAxObject *axObject() const;
    QScriptValue property ( const QScriptValue & object, const QScriptString & name, uint id );
    void setProperty(QScriptValue & object, const QScriptString & name, uint id, const QScriptValue & value );
    QScriptValue::PropertyFlags propertyFlags ( const QScriptValue & object, const QScriptString & name, uint id );
    QScriptValue prototype() const;
    QueryFlags queryProperty ( const QScriptValue & object, const QScriptString & name, QueryFlags flags, uint * id );

    static void initEngine(QScriptEngine *engine);
    friend class AxScriptablePrivate;
};

#endif // AXSCRIPTABLE_H
