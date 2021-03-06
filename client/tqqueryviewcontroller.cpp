#include "tqqueryviewcontroller.h"
#include "querypage.h"
#include <tqdebug.h>
#include <asprotect.h>

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
    if(d->record == record)
        return;
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
    REG_CRYPT_BEGIN1

    if(d->record->isEditing())
    {
        d->modifyLocks++;
        return true;
    }
    if(d->record->updateBegin())
    {
        d->modifyLocks++;
        return true;
    }
    REG_CRYPT_END1
    return false;
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
