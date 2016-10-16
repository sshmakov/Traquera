#include "tqviewcontroller.h"
#include <tqdebug.h>

QMetaMethod notifyMethod(QObject *object, const char *property)
{
    const QMetaObject *meta = object->metaObject();
    int index = meta->indexOfProperty(property);
    QMetaProperty prop = meta->property(index);
    return prop.notifySignal();
}

QByteArray notifySignature(QObject *object, const char *property)
{
    QMetaMethod method = notifyMethod(object, property);
    return QByteArray(method.signature());
}

bool isMethodValid(const QMetaMethod &method)
{
    return !QByteArray(method.signature()).isEmpty();
}

TQViewController::TQViewController(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<TQViewController*>("TQViewController*");
    qRegisterMetaType<const TQAbstractRecordTypeDef *>("const TQAbstractRecordTypeDef *");
}

TQViewController::~TQViewController()
{
}

QWidget *TQViewController::view() const
{
    return qobject_cast<QWidget*>(parent());
}

TQRecord *TQViewController::currentRecord() const
{
    QObject *obj = view()->property("currentRecord").value<QObject*>();
    TQRecord *rec = qobject_cast<TQRecord *>(obj);
    return rec;
}

QObjectList TQViewController::selectedRecords() const
{
    QObjectList list;
    QMetaObject::invokeMethod(view(), "selectedRecords",
                              Q_RETURN_ARG(QObjectList, list));
    return list;
}

QList<int> TQViewController::selectedIds() const
{
    QList<int> ids;
    QObjectList list = selectedRecords();
    foreach(const QObject *obj, list)
    {
        const TQRecord *rec = qobject_cast<const TQRecord *>(obj);
        ids << rec->recordId();
    }
    return ids;
}

QAbstractItemView *TQViewController::tableView() const
{
    QWidget *widget = view();
    return qobject_cast<QAbstractItemView *>(widget->property("tableView").value<QObject*>());
}

bool TQViewController::flag(const QString &flagName) const
{
    return false;
}

const TQAbstractRecordTypeDef *TQViewController::recordDef() const
{
    const TQAbstractRecordTypeDef *def = 0;
    QVariant v = view()->property("recordTypeDef");
    def = v.value<const TQAbstractRecordTypeDef *>();
//    QMetaObject::invokeMethod(view(), "recordTypeDef", Q_RETURN_ARG(TQAbstractRecordTypeDef *, def));
    return def;
}

TQAbstractProject *TQViewController::project() const
{
    const TQAbstractRecordTypeDef *def = recordDef();
    if(!def)
        return 0;
    return def->project();
}

bool TQViewController::canInstantEdit() const
{
    return false;
}

bool TQViewController::beginModifySection()
{
    return false;
}

void TQViewController::submitModifySection()
{

}

void TQViewController::cancelModifySection()
{

}

void TQViewController::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
{
    QMetaObject::invokeMethod(view(), "addDetailTab",
                              Q_ARG(QWidget *, tab),
                              Q_ARG(const QString &, title),
                              Q_ARG(const QIcon &, icon));
    QMetaMethod titleSignal = notifyMethod(tab ,"title");
    if(isMethodValid(titleSignal))
    {
        int i = this->metaObject()->indexOfSlot("onDetailTitleChanged()");
        QMetaMethod slot = this->metaObject()->method(i);
        connect(tab, titleSignal, this, slot);
    }
}

void TQViewController::addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon)
{
    QMetaObject::invokeMethod(view(), "addDetailWidgets",
                              Q_ARG(QWidget *, topWidget),
                              Q_ARG(QWidget *, pageWidget),
                              Q_ARG(const QString &, title),
                              Q_ARG(const QIcon &, icon));
}

void TQViewController::onSelectedRecordsChanged()
{
    emit selectedRecordsChanged();
}

void TQViewController::onDetailTitleChanged()
{
    QObject *obj = sender();
    QWidget *tab = qobject_cast<QWidget*>(obj);
    if(!tab)
        return;
    QString title = tab->property("title").toString();
    emit detailTabTitleChanged(tab, title);
}
