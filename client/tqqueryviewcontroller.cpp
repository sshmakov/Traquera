#include "tqqueryviewcontroller.h"
#include "querypage.h"
#include <tqdebug.h>

class TQQVCPrivate
{
public:
    QWidget *widget;
};

TQQueryViewController::TQQueryViewController(QObject *parent) :
    TQViewController(parent), d(new TQQVCPrivate())
{
    qRegisterMetaType<TQQueryViewController*>("TQQueryViewController*");
    d->widget = qobject_cast<QWidget*>(parent);
}

TQQueryViewController::~TQQueryViewController()
{
    delete d;
}

void TQQueryViewController::emitCurrentRecordChanged(TQRecord *record)
{
//    tqProfile();
    emit currentRecordChanged(record);
}

/*
QWidget *TQQueryViewController::view() const
{
    return d->widget;
}

TQRecord *TQQueryViewController::currentRecord() const
{
    TQRecord *rec=0;
    QMetaObject::invokeMethod(d->widget, "currentRecord",
                              Q_RETURN_ARG(TQRecord *, rec));
    return rec;
}

QObjectList TQQueryViewController::selectionRecords() const
{
    QObjectList list;
    QMetaObject::invokeMethod(d->widget, "selectedRecords",
                              Q_RETURN_ARG(QObjectList, list));
    return list;
}

void TQQueryViewController::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
{
    QMetaObject::invokeMethod(d->widget, "addDetailTab",
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

void TQQueryViewController::addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon)
{
    QMetaObject::invokeMethod(d->widget, "addDetailWidgets",
                              Q_ARG(QWidget *, topWidget),
                              Q_ARG(QWidget *, pageWidget),
                              Q_ARG(const QString &, title),
                              Q_ARG(const QIcon &, icon));
}

void TQQueryViewController::onSelectedRecordsChanged()
{
    emit selectedRecordsChanged();
}

void TQQueryViewController::onDetailTitleChanged()
{
    QObject *obj = sender();
    QWidget *tab = qobject_cast<QWidget*>(obj);
    if(!tab)
        return;
    QString title = tab->property("title").toString();
    emit detailTabTitleChanged(tab, title);
}
*/
