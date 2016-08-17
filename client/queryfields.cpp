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
    connect(modifyPanel, SIGNAL(editButtonClicked()), SLOT(startChange()));
    connect(modifyPanel, SIGNAL(resetButtonClicked()), SLOT(cancelChanges()));
    connect(modifyPanel, SIGNAL(activatedField(QString)), SLOT(activatedField(QString)));
    new QShortcut(QKeySequence("Escape"),this,SLOT(cancelChanges()));
    ui->buttonBox->hide();
}

QueryFields::~QueryFields()
{
    delete ui;
}

void QueryFields::setViewController(TQViewController *viewController, int viewMode)
{
    if(controller)
        controller->disconnect(this);
    controller = viewController;
    if(viewMode == -1)
        mode = TQRecord::View;
    else
        mode = viewMode;
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

void QueryFields::activatedField(const QString &fieldName)
{
    if(modifyPanel->mode() != TQRecord::View)
        return;
    if(fieldName.isEmpty())
        return;
    if(!controller)
        return;
    TQRecord *rec = controller->currentRecord();
    if(!rec)
        return;
    const TQAbstractRecordTypeDef *editDef = rec->typeEditDef();
    modifyPanel->setRecordDef(editDef, TQRecord::Edit);
    modifyPanel->resetAll();
    modifyPanel->fillValues(controller->selectedRecords());
}

void QueryFields::applyChanges()
{
    QVariantHash lastChanges = modifyPanel->changes();
    if(controller)
    {
        if(!lastChanges.isEmpty())
        {
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
        }
        modifyPanel->setRecordDef(controller->recordDef(), mode);
        modifyPanel->resetAll();
        modifyPanel->fillValues(controller->selectedRecords());
    }
}

void QueryFields::cancelChanges()
{
    modifyPanel->setRecordDef(controller->recordDef(), mode);
    modifyPanel->resetAll();
    modifyPanel->fillValues(controller->selectedRecords());
}

void QueryFields::startChange()
{
    if(modifyPanel->mode() != TQRecord::View)
        return;
    if(!controller)
        return;
    TQRecord *rec = controller->currentRecord();
    if(!rec)
        return;
    const TQAbstractRecordTypeDef *editDef = rec->typeEditDef();
    modifyPanel->setRecordDef(editDef, TQRecord::Edit);
    modifyPanel->resetAll();
    modifyPanel->fillValues(controller->selectedRecords());
}

void QueryFields::selectedRecordsChanged()
{
    if(!controller)
        return;
    QObjectList records = controller->selectedRecords();
    const TQAbstractRecordTypeDef *typeDef = controller->recordDef();
    modifyPanel->setRecordDef(typeDef, TQRecord::View);
    modifyPanel->fillValues(records);
}
