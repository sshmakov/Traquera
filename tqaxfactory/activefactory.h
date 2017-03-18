#ifndef ACTIVEFACTORY_H
#define ACTIVEFACTORY_H

#include <QAxObject>
#include "tqaxfactory_global.h"

//#ifdef TQ_AXFACTORY
#include <QAxFactory>

class TQAXFACTORYSHARED_EXPORT ActiveFactory : public QAxFactory
{
    Q_OBJECT
    Q_CLASSINFO("ClassID", "{D2CCDBD9-E436-46a7-A3A3-BCFEED3A54C3}")
    Q_CLASSINFO("InterfaceID", "{9454D024-6B37-43f2-A529-93A9FB06F75B}")
    Q_CLASSINFO("EventsID", "{EF02C7F1-9804-4544-BDF5-ABD8CAE0B876}")
public:
    explicit //ActiveFactory(QObject *parent = 0);
    ActiveFactory(const QUuid &libid, const QUuid &appid);

    QStringList featureList() const;
    QObject *createObject(const QString &key);
    const QMetaObject *metaObject(const QString &key) const;
    QUuid classID(const QString &key) const;
    QUuid interfaceID(const QString &key) const;
    QUuid eventsID(const QString &key) const;
signals:
    
public slots:
    
};
//#endif

class ActiveXObjectPrivate;

class TQAXFACTORYSHARED_EXPORT ActiveXObject :public QAxObject
{
private:
    ActiveXObjectPrivate *d;
public:
     ActiveXObject( QObject * parent = 0 );
     ActiveXObject( const QString & c, QObject * parent = 0 );
     ActiveXObject( IUnknown * iface, QObject * parent = 0 );
     ~ActiveXObject();
     int qt_metacall(QMetaObject::Call call, int id, void **v);
     const char *className() const;
protected:
     virtual QObject *createComWrapper(IDispatch *disp);
};

class TQAXFACTORYSHARED_EXPORT AXWebKitObject :public QAxObject
{
public:
    AXWebKitObject(const QString & c, QObject * parent);
    int qt_metacall(QMetaObject::Call call, int id, void **v);
protected:
    QObject *createObjectWrapper(IDispatch *disp);
};


#endif // ACTIVEFACTORY_H
