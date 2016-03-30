#include "tqrecordviewcontroller.h"
#include <ttglobal.h>
#include "ttrecwindow.h"

class TQRVCPrivate {
public:
    TTRecordWindow *view;
    QWidget *widget;
    QObject *object;
    TQRecord *record;
};

TQRecordViewController::TQRecordViewController(QObject *parent) :
    TQViewController(parent),
    d(new TQRVCPrivate)
{
    qRegisterMetaType<TQRecordViewController*>("TQRecordViewController*");
    d->view = (TTRecordWindow *)parent;
    d->object = parent;
    d->widget = qobject_cast<QWidget*>(d->object);
    connect(d->view,SIGNAL(recordChanged(TQRecord*)),SLOT(onViewRecordChanged(TQRecord*)));
    connect(this,SIGNAL(detailTabTitleChanged(QWidget*,QString)),d->view,SLOT(setDetailTabTitle(QWidget*,QString)));
}

TQRecordViewController::~TQRecordViewController()
{
    delete d;
}

TTRecordWindow *TQRecordViewController::recView() const
{
    return (TTRecordWindow *)view();
}

const TQAbstractRecordTypeDef *TQRecordViewController::recordDef() const
{
    return recView()->recordTypeDef();
}

TQRecord *TQRecordViewController::currentRecord() const
{
    return recView()->record();
}

void TQRecordViewController::onViewRecordChanged(TQRecord *record)
{
    emit currentRecordChanged(record);
}

/*
QWidget *TQRecordViewController::view() const
{
    return d->widget;
}

TQRecord *TQRecordViewController::currentRecord() const
{
    return d->record;
}

void TQRecordViewController::setView(QWidget *view)
{
    d->widget = view;


}

void TQRecordViewController::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
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

void TQRecordViewController::addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon)
{
    QMetaObject::invokeMethod(d->widget, "addDetailWidgets",
                              Q_ARG(QWidget *, topWidget),
                              Q_ARG(QWidget *, pageWidget),
                              Q_ARG(const QString &, title),
                              Q_ARG(const QIcon &, icon));
}

void TQRecordViewController::onViewRecordChanged(TQRecord *record)
{
    d->record = record;
    emit recordChanged(record);
}

void TQRecordViewController::onDetailTitleChanged()
{
    QObject *obj = sender();
    QWidget *tab = qobject_cast<QWidget*>(obj);
    if(!tab)
        return;
    QString title = tab->property("title").toString();
    emit detailTabTitleChanged(tab, title);
}
*/
