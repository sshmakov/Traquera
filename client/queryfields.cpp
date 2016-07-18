#include "queryfields.h"
#include "ui_queryfields.h"
#include <modifypanel.h>
#include <tqviewcontroller.h>

QueryFields::QueryFields(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QueryFields),
    controller(0)
{
    ui->setupUi(this);
    modifyPanel = new ModifyPanel(this);
    ui->contentLayout->addWidget(modifyPanel);
//    QShortcut *s;
//    new QShortcut(QKeySequence("Ctrl+Enter"),this,SLOT(applyChanges()));
//    new QShortcut(QKeySequence("Ctrl+Return"),this,SLOT(applyChanges()));
    connect(modifyPanel, SIGNAL(applyButtonPressed()), SLOT(applyChanges()));
    new QShortcut(QKeySequence("Escape"),this,SLOT(cancelChanges()));
}

QueryFields::~QueryFields()
{
    delete ui;
}

void QueryFields::setViewController(TQViewController *viewController, int mode)
{
    if(controller)
        controller->disconnect(this);
    controller = viewController;
    if(controller)
    {
        connect(controller, SIGNAL(selectedRecordsChanged()), SLOT(selectedRecordsChanged()));
        connect(controller, SIGNAL(destroyed()), SLOT(controllerDestroyed()));
        modifyPanel->setRecordDef(controller->recordDef(),mode);
        modifyPanel->resetAll();
        modifyPanel->fillValues(controller->selectedRecords());
    }
    else
    {
        modifyPanel->setRecordDef(0,mode);
        modifyPanel->resetAll();
    }
}

void QueryFields::on_buttonBox_clicked(QAbstractButton *button)
{
    int role = ui->buttonBox->standardButton(button);
    if(role == QDialogButtonBox::Reset)
        cancelChanges();
    else if(role == QDialogButtonBox::Apply)
        applyChanges();
}

void QueryFields::controllerDestroyed()
{
    controller = 0;
}

void QueryFields::applyChanges()
{
    QVariantHash lastChanges = modifyPanel->changes();
    if(lastChanges.isEmpty())
        return;
    if(!controller)
        return;
    foreach(QObject *obj,controller->selectedRecords())
    {
        TQRecord *rec = qobject_cast<TQRecord *>(obj);
        if(!rec)
            continue;
        if(rec->updateBegin())
        {
            foreach(const QString &fieldName, lastChanges.keys())
            {
                int vid = rec->typeDef()->fieldVid(fieldName);
                rec->setValue(vid, lastChanges.value(fieldName));
            }
            if(!rec->commit())
                rec->cancel();
        }
    }
    modifyPanel->resetAll();
    modifyPanel->fillValues(controller->selectedRecords());
}

void QueryFields::cancelChanges()
{
    modifyPanel->resetAll();
    modifyPanel->fillValues(controller->selectedRecords());
}

void QueryFields::selectedRecordsChanged()
{
    if(!controller)
        return;
    QObjectList records = controller->selectedRecords();
    const TQAbstractRecordTypeDef *typeDef = controller->recordDef();
    modifyPanel->setRecordDef(typeDef, TQRecord::Edit);
    modifyPanel->fillValues(records);
}
