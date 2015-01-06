#include "tqcolsdialog.h"
#include "ui_tqcolsdialog.h"

TQColsDialog::TQColsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TQColsDialog)
{
    ui->setupUi(this);
}

TQColsDialog::~TQColsDialog()
{
    delete ui;
}

int TQColsDialog::exec(QHeaderView *hv)
{
    ColList list;
    Qt::Orientation o = hv->orientation();
    for(int i=0; i<hv->count(); i++)
    {
        Col c;
        c.colName = hv->model()->headerData(i, o).toString();
        c.logicalIndex = i;
        c.pos = hv->visualIndex(c.logicalIndex);
        c.width = hv->sectionSize(c.logicalIndex);
        c.isHidden = hv->isSectionHidden(c.logicalIndex);
        list.insert(c.pos, c);
    }
    ui->listAvail->clear();
    ui->listShow->clear();
    foreach(const Col& c, list)
    {
        QListWidgetItem *item = new QListWidgetItem(c.colName);
        item->setData(Qt::UserRole, c.logicalIndex);
        item->setData(Qt::UserRole+1, c.width);
        item->setData(Qt::UserRole+2, c.pos);
        if(c.isHidden)
            ui->listAvail->addItem(item);
        else
            ui->listShow->addItem(item);
    }
    ui->listAvail->sortItems();
    int res = QDialog::exec();
    if(!res)
        return res;
    for(int r=0; r<ui->listShow->count(); r++)
    {
        QListWidgetItem *item = ui->listShow->item(r);
        int index = item->data(Qt::UserRole).toInt();
        int w = item->data(Qt::UserRole+1).toInt();
        int pos = item->data(Qt::UserRole+2).toInt();
        int oldpos =  hv->visualIndex(index);
        hv->moveSection(oldpos,r);
        hv->resizeSection(index, w);
        hv->setSectionHidden(index,false);
    }
    for(int r=0; r<ui->listAvail->count(); r++)
    {
        QListWidgetItem *item = ui->listAvail->item(r);
        int index = item->data(Qt::UserRole).toInt();
        int w = item->data(Qt::UserRole+1).toInt();
        int pos = item->data(Qt::UserRole+2).toInt();
//        hv->moveSection(pos,r);
        hv->resizeSection(index, w);
        hv->setSectionHidden(index,true);
    }
    return res;

}

void TQColsDialog::on_btnAdd_clicked()
{
    QList<QListWidgetItem *> items = ui->listAvail->selectedItems();
    foreach(QListWidgetItem *item, items)
    {
        int r = ui->listAvail->row(item);
        ui->listAvail->takeItem(r);
        int w = item->data(Qt::UserRole+1).toInt();
        if(w < 10)
            item->setData(Qt::UserRole+1, 50);
        ui->listShow->addItem(item);
    }
}

void TQColsDialog::on_btnDel_clicked()
{
    QList<QListWidgetItem *> items = ui->listShow->selectedItems();
    foreach(QListWidgetItem *item, items)
    {
        int r = ui->listShow->row(item);
        ui->listShow->takeItem(r);
        ui->listAvail->addItem(item);
    }
    ui->listAvail->sortItems();
}

void TQColsDialog::on_btnAllAdd_clicked()
{
    while( ui->listAvail->count())
    {
        QListWidgetItem *item = ui->listAvail->takeItem(0);
        int w = item->data(Qt::UserRole+1).toInt();
        if(w < 10)
            item->setData(Qt::UserRole+1, 50);
        ui->listShow->addItem(item);
    }
}

void TQColsDialog::on_btnAllDel_clicked()
{
    while( ui->listShow->count())
    {
        ui->listAvail->addItem(ui->listShow->takeItem(0));
    }
    ui->listAvail->sortItems();
}

void TQColsDialog::on_btnMoveUp_clicked()
{
    int fpos = -1;
    QList<QListWidgetItem *> items = ui->listShow->selectedItems();
    foreach(QListWidgetItem *item, items)
    {
        int r = ui->listShow->row(item);
        if(fpos <0 || fpos > r)
            fpos = r;
    }
    fpos--;
    if(fpos<0)
        return;
    foreach(QListWidgetItem *item, items)
    {
        int r = ui->listShow->row(item);
        ui->listShow->insertItem(fpos++, ui->listShow->takeItem(r));
    }
    ui->listShow->clearSelection();
    foreach(QListWidgetItem *item, items)
    {
        ui->listShow->setCurrentItem(item, QItemSelectionModel::Select);
    }
}

void TQColsDialog::on_btnMoveDown_clicked()
{
    int fpos = -1;
    QList<QListWidgetItem *> items = ui->listShow->selectedItems();
    foreach(QListWidgetItem *item, items)
    {
        int r = ui->listShow->row(item);
        if(fpos < r)
            fpos = r;
    }
    if(fpos<0)
        return;
    fpos++;
    for(int i=items.count()-1; i>=0; i--)
    {
        QListWidgetItem *item = items.value(i);
        int r = ui->listShow->row(item);
        ui->listShow->insertItem(fpos--, ui->listShow->takeItem(r));
    }
    ui->listShow->clearSelection();
    foreach(QListWidgetItem *item, items)
    {
        ui->listShow->setCurrentItem(item, QItemSelectionModel::Select);
    }
}

void TQColsDialog::on_listShow_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(current)
    {
        int w = current->data(Qt::UserRole+1).toInt();
        ui->editWidth->setValue(w);
        ui->editWidth->setEnabled(true);
    }
    else
    {
        ui->editWidth->setValue(0);
        ui->editWidth->setEnabled(false);
    }
}

void TQColsDialog::on_editWidth_editingFinished()
{
    QListWidgetItem *current = ui->listShow->currentItem();
    if(current)
        current->setData(Qt::UserRole+1, ui->editWidth->value());
}
