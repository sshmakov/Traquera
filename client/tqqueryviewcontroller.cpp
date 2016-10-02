#include "tqqueryviewcontroller.h"
#include "querypage.h"
#include <tqdebug.h>

class TQQVCPrivate
{
public:
    QWidget *widget;
    QPointer<TQRecord> record;
    int modifyLocks;
    TQQVCPrivate() : modifyLocks(0)
    {

    }
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
    tqProfile();
    if(d->record)
        disconnect(d->record);
    d->record = record;
    emit currentRecordChanged(record);
    if(d->record)
        connect(d->record.data(),SIGNAL(changedState(int)),SIGNAL(recordModeChanged(int)));
}

bool TQQueryViewController::canInstantEdit() const
{
    return true;
}

bool TQQueryViewController::beginModifySection()
{
    if(!d->record)
        return false;
    if(!d->record->isEditing() && !d->record->updateBegin())
        return false;
    d->modifyLocks++;
    return true;
}

void TQQueryViewController::submitModifySection()
{
    if(!--d->modifyLocks)
        d->record->commit();
}

void TQQueryViewController::cancelModifySection()
{
    if(!--d->modifyLocks)
        d->record->cancel();
}
