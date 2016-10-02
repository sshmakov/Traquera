#include "tqrecordviewcontroller.h"
#include <ttglobal.h>
#include "ttrecwindow.h"

class TQRVCPrivate {
public:
    TTRecordWindow *view;
    QWidget *widget;
    QObject *object;
    QPointer<TQRecord> record;
    int modifyLocks;
    TQRVCPrivate()
    {
        modifyLocks = 0;
    }
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
    return qobject_cast<TTRecordWindow *>(view());
}

const TQAbstractRecordTypeDef *TQRecordViewController::recordDef() const
{
    TTRecordWindow *win = recView();
    if(!win)
        return 0;
    return win->recordTypeDef();
}

TQRecord *TQRecordViewController::currentRecord() const
{
    TTRecordWindow *win = recView();
    if(!win)
        return 0;
    return win->record();
}

QObjectList TQRecordViewController::selectedRecords() const
{
    TQRecord *record = currentRecord();
    if(record)
        return QObjectList() << record;
    return QObjectList();
}

void TQRecordViewController::onViewRecordChanged(TQRecord *record)
{
    if(d->record)
        disconnect(d->record.data());
    d->record = record;
    emit currentRecordChanged(record);
    emit selectedRecordsChanged();
    if(d->record)
        connect(d->record.data(), SIGNAL(changedState(int)), SIGNAL(recordModeChanged(int)));
}

bool TQRecordViewController::canInstantEdit() const
{
    return false;
}

bool TQRecordViewController::beginModifySection()
{
    TTRecordWindow *win = recView();
    if(!win)
        return false;
    return win->enableModify();
}

void TQRecordViewController::submitModifySection()
{
}

void TQRecordViewController::cancelModifySection()
{
}

