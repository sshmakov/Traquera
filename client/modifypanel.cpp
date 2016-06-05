#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QGroupBox>
#include <QToolButton>
#include <QComboBox>
#include <QStringList>

#include "modifypanel.h"
#include "ui_modifypanel.h"
#include "flowlayout.h"
#include "trkdecorator.h"
#include "ttdelegate.h"


ModifyPanel::ModifyPanel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModifyPanel),
    queryPage(0),
    //a_model(0),
    rdef(0),
    a_readOnly(false),
    curMode(TQRecord::View)
{
    ui->setupUi(this);
    TTDelegate *del = new TTDelegate(this);
    del->assignToPanel(this);
    ui->fieldsTableWidget->addAction(ui->actionApplyChanges);
    ui->fieldsTableWidget->addAction(ui->actionRevertChanges);
    ui->fieldsTableWidget->addAction(ui->actionRepeatChanges);
    ui->fieldsTableWidget->addAction(ui->actionRevertField);
    ui->fieldsTableWidget->addAction(ui->actionClearField);
    //loadDefinitions();
}

ModifyPanel::~ModifyPanel()
{
    delete ui;
}

/*
void ModifyPanel::setQueryPage(QueryPage *page)
{
    if(page == queryPage)
        return;
    if(queryPage)
        queryPage->disconnect(this);
    queryPage = page;
    if(queryPage)
    {
        //setModel(queryPage->tmodel);
        if(queryPage->tmodel)
            fillValues(queryPage->selectedRecords());
        connect(queryPage,SIGNAL(destroyed(QObject*)),this,SLOT(queryPageDestroyed(QObject*)),Qt::DirectConnection);
        //connect(queryPage,SIGNAL(modelChanged(TrkToolModel*)),this,SLOT(modelChanged(TrkToolModel*)));
    }
}
*/

/*
void ModifyPanel::setModel(TrkToolModel *newModel)
{
    if(a_model!=newModel)
    {
        if(a_model)
            a_model->disconnect(this);
        a_model = newModel;
        if(a_model)
        {
            loadDefinitions(a_model->typeDef());
            connect(a_model,SIGNAL(destroyed()),this,SLOT(modelDestroyed()));
        }
    }
}
*/

void ModifyPanel::setRecordDef(const TQAbstractRecordTypeDef *typeDef, int mode)
{
    if(rdef == typeDef && curMode == mode)
        return;
    rdef = typeDef;
    curMode = mode;
    rows.clear();
    if(rdef)
    {
        FieldGroupsDef def;
        def = decorator->loadGroups(typeDef);
        for(int p=0; p<def.groups.count(); p++)
        {
            QString pname = def.groups[p];
            ModifyRow mrow;
            mrow.isGroup = true;
            mrow.fieldName = pname;
            //mrow.editor = 0;
            mrow.displayValue = "";
            mrow.isChanged = false;
            mrow.isEditable = false;
            //mrow.resetBtn = 0;
            rows.append(mrow);
            const QStringList &flist = def.fieldsByGroup[p];
            for(int f=0; f<flist.count(); f++)
            {
                QString fname = flist[f];
                TQAbstractFieldType fType = typeDef->getFieldType(fname);
                ModifyRow frow;
                frow.isGroup = false;
                frow.fieldName = fname;
                frow.fieldType = fType.nativeType();
                //frow.editor = 0;
                frow.displayValue = "";
                frow.isChanged = false;
                if(mode == TQRecord::Insert)
                    frow.isEditable = fType.canSubmit();
                else
                    frow.isEditable = fType.canUpdate();
                //frow.resetBtn = 0;
                rows.append(frow);
            }
        }
    }
    fillTable();
}

void ModifyPanel::fillTable()
{
    int line=-1;
    ui->fieldsTableWidget->clearContents();
    if(!rows.count())
       ui->fieldsTableWidget->setRowCount(0);
    int wRows = ui->fieldsTableWidget->rowCount();
    QFont disabledFont = ui->fieldsTableWidget->font();
    disabledFont.setWeight(QFont::Light);
    disabledFont.setItalic(true);
    foreach(const ModifyRow &row, rows)
    {
        if(++line >= wRows)
            ui->fieldsTableWidget->insertRow(line);
        QTableWidgetItem *item;
        item = new QTableWidgetItem(row.fieldName,0);
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        if(!row.isEditable)
            item->setFont(disabledFont);
        ui->fieldsTableWidget->setItem(line,0,item);
        if(row.isGroup)
        {
            ui->fieldsTableWidget->setSpan(line,0,1,2);
            item->setBackground(QBrush(Qt::green));
        }
        else
        {
            item = new QTableWidgetItem(row.displayValue,row.fieldType);
            Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
            if(row.isEditable)
                flags |= Qt::ItemIsEditable;
            item->setFlags(flags);
            ui->fieldsTableWidget->setItem(line,1,item);
        }
    }
}

/*
void ModifyPanel::selectedRecordsChanged(const QObjectList &newSelection)
{
    fillValues(newSelection);
}
*/

/*
void ModifyPanel::fillValues( TrkToolRecord * rec)
{
    for(int r = 0; r<rows.count(); r++)
    {
        ModifyRow row = rows[r];
        if(row.isGroup)
            continue;
        if(row.isChanged)
            continue;
        row.displayValue = "";
        row.fieldValue = rec->value(row.fieldName, Qt::EditRole);
        //row.displayValue = fieldDef(r).valueToDisplay(row.fieldValue);
        row.displayValue = rec->value(row.fieldName, Qt::DisplayRole).toString();
        row.isDifferent = false;
        rows[r] = row;
        QTableWidgetItem *item = ui->fieldsTableWidget->item(r,1);
        item->setText(row.displayValue);
        //item->setData(Qt::EditRole, row.fieldValue);
    }
}
*/

void ModifyPanel::fillValues(const QObjectList &records)
{
    for(int r = 0; r<rows.count(); r++)
    {
        ModifyRow row = rows[r];
        if(row.isGroup)
            continue;
        if(row.isChanged)
            continue;
        row.displayValue = "";
        int i;
        for(i=0; i<records.count() && i < 20; i++)
        {
            const TQRecord *rec = qobject_cast<const TQRecord *>(records[i]);
            if(!rec || !rec->isValid())
                continue;
            if(!i)
            {
                row.fieldValue = rec->value(row.fieldName, Qt::EditRole);
                //row.displayValue = fieldDef(r).valueToDisplay(row.fieldValue);
                row.displayValue = rec->value(row.fieldName, Qt::DisplayRole).toString();
                row.isDifferent = false;
                //bool flag = !rec->isFieldReadOnly(row.fieldName);
                //row.isEditable = flag;
            }
            else
            {
                if(!row.isDifferent && row.displayValue != rec->value(row.fieldName, Qt::DisplayRole))
                {
                    row.isDifferent = true;
                    row.displayValue = "...";
                    row.fieldValue = QVariant();

                }
                //row.isEditable = row.isEditable && !rec->isFieldReadOnly(row.fieldName);
            }
        }
        if(i<records.count())
        {
            row.isDifferent = true;
            row.displayValue = "...";
            row.fieldValue = QVariant();
        }
        rows[r] = row;
        QTableWidgetItem *item = ui->fieldsTableWidget->item(r,1);
        item->setText(row.displayValue);
        //item->setData(Qt::EditRole, row.fieldValue);
    }
}

QTableWidget *ModifyPanel::tableWidget()
{
    return ui->fieldsTableWidget;
}

const TQAbstractFieldType ModifyPanel::fieldDef(int row) const
{
    return fieldDef(rows[row].fieldName);
    /*
    if(!model)
        return TrkFieldDef();
    const RecordTypeDef *rdef = model->typeDef();
    if(!rdef)
        return TrkFieldDef();
    return rdef->getFieldDef(rows[row].fieldName);
    */
}

const TQAbstractFieldType ModifyPanel::fieldDef(const QString &fieldName) const
{
    //if(!a_model)
    //    return TrkFieldDef();
    //const RecordTypeDef *rdef = a_model->typeDef();
    if(!rdef)
        return TQAbstractFieldType();
    return rdef->getFieldType(fieldName);
}

void ModifyPanel::setFieldValue(const QString &fieldName, const QVariant &value)
{
    for(int row=0; row<rows.count(); row++)
    {
        ModifyRow & frow = rows[row];
        if(frow.isGroup)
            continue;
        if(frow.fieldName != fieldName)
            continue;
        if(frow.isChanged || frow.fieldValue != value)
        {
            QTableWidgetItem *item = tableWidget()->item(row,1);
            frow.newValue = value;
            //item->setData(Qt::EditRole,value);
            frow.displayValue = fieldDef(row).valueToDisplay(value);
            item->setData(Qt::DisplayRole,frow.displayValue);
            frow.isChanged = true;
            QTableWidgetItem  *fitem = tableWidget()->item(row,0);
            QFont font = fitem->font();
            font.setBold(true);
            fitem->setFont(font);
            emit dataChanged();
        }
    }
}

QString ModifyPanel::fieldName(const QModelIndex &index)
{
    return rows[index.row()].fieldName;
}

QVariant ModifyPanel::fieldValue(const QString &fieldName)
{
    for(int row=0; row<rows.count(); row++)
    {
        ModifyRow & frow = rows[row];
        if(frow.isGroup)
            continue;
        if(frow.fieldName != fieldName)
            continue;
        if(frow.isChanged)
            return frow.newValue;
        else
            return frow.fieldValue;
    }
    return QVariant();
}

QVariantHash ModifyPanel::changes()
{
    QVariantHash res;
    foreach(const ModifyRow &f, rows)
    {
        if(f.isGroup)
            continue;
        if(!f.isChanged)
            continue;
        res.insert(f.fieldName,f.newValue);
    }
    return res;
}

void ModifyPanel::setChanges(const QVariantHash &newChanges)
{
    foreach(const QString &name, newChanges.keys())
        setFieldValue(name,newChanges.value(name));
}

void ModifyPanel::setButtonsVisible(bool visible)
{
    ui->buttonsPanel->setVisible(visible);
}

void ModifyPanel::resetField(const QString &fieldName)
{
    for(int row=0; row<rows.count(); row++)
    {
        ModifyRow & frow = rows[row];
        if(!frow.isGroup && frow.fieldName == fieldName)
        {
            frow.isChanged = false;
            QTableWidgetItem  *item = tableWidget()->item(row,1);
            //item->setData(Qt::EditRole, frow.fieldValue);
            frow.displayValue = fieldDef(row).valueToDisplay(frow.fieldValue);
            if(frow.isDifferent)
                frow.displayValue = "...";
            if(frow.fieldValue.isNull())
                item->setData(Qt::DisplayRole, "");
            else
                item->setData(Qt::DisplayRole, frow.displayValue);
            QTableWidgetItem  *fitem = tableWidget()->item(row,0);
            QFont font = fitem->font();
            font.setBold(false);
            fitem->setFont(font);
        }
    }

}

void ModifyPanel::resetAll()
{
    for(int row=0; row<rows.count(); row++)
    {
        ModifyRow & frow = rows[row];
        if(!frow.isGroup && frow.isChanged)
        {
            frow.isChanged = false;
            QTableWidgetItem  *item = tableWidget()->item(row,1);
            //item->setData(Qt::EditRole, frow.fieldValue);
            bool ok;
            TQAbstractFieldType fdef = rdef->getFieldType(frow.fieldName, &ok);
            if(ok)
                frow.displayValue = fdef.valueToDisplay(frow.fieldValue);
            else
                frow.displayValue = frow.fieldValue.toString();
            if(frow.isDifferent)
                frow.displayValue = "...";
            if(frow.fieldValue.isNull())

                item->setData(Qt::DisplayRole, "");
            else
                item->setData(Qt::DisplayRole, frow.displayValue);
            QTableWidgetItem  *fitem = tableWidget()->item(row,0);
            QFont font = fitem->font();
            font.setBold(false);
            fitem->setFont(font);
        }
    }
}

void ModifyPanel::clearField(const QString &fieldName)
{
    if(fieldDef(fieldName).isNullable())
        setFieldValue(fieldName, QVariant());
    else
        setFieldValue(fieldName, fieldDef(fieldName).defaultValue());
}

/*
void ModifyPanel::modelChanged(TrkToolModel *newmodel)
{
    setModel(newmodel);
}
*/

void ModifyPanel::queryPageDestroyed(QObject *page)
{
    if(queryPage && page == queryPage)
    {
        queryPage->disconnect(this);
        queryPage=0;
    }
}

/*
void ModifyPanel::modelDestroyed()
{
    a_model = 0;
}
*/

void ModifyPanel::on_saveChangesButton_clicked()
{
    emit applyButtonPressed();
}

void ModifyPanel::on_pushButton_clicked()
{
    resetAll();
}

void ModifyPanel::on_repeatButton_clicked()
{
    emit repeatButtonClicked();
}

void ModifyPanel::on_actionApplyChanges_triggered()
{
    emit applyButtonPressed();
}

void ModifyPanel::on_actionRevertChanges_triggered()
{
    resetAll();
}

void ModifyPanel::on_actionRepeatChanges_triggered()
{
    emit repeatButtonClicked();
}

void ModifyPanel::on_actionRevertField_triggered()
{
    QString field = fieldName(ui->fieldsTableWidget->currentIndex());
    if(!field.isEmpty())
        resetField(field);
}

void ModifyPanel::on_actionClearField_triggered()
{
    QString field = fieldName(ui->fieldsTableWidget->currentIndex());
    if(!field.isEmpty())
        clearField(field);

}


