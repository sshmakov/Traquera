#include "fieldgroupdialog.h"
#include "ui_fieldgroupdialog.h"
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QtXml>


class FieldGroupDialogPrivate
{
public:
    QStandardItemModel *model;
    QSortFilterProxyModel *proxy;
    FieldGroupDialogPrivate()
    {
        model = new QStandardItemModel();
        proxy = new QSortFilterProxyModel();
        proxy->setSourceModel(model);
        proxy->setFilterRole(Qt::UserRole);
        proxy->setFilterKeyColumn(0);
        proxy->setFilterFixedString("false");
        proxy->setDynamicSortFilter(true);
        proxy->sort(0);
    }
    ~FieldGroupDialogPrivate()
    {
        delete proxy;
        delete model;
    }
    void setFields(const QStringList &fields)
    {
        foreach(QString field, fields)
            model->appendRow(new QStandardItem(field));
    }

    void setFiltered(bool enable)
    {
        proxy->setFilterFixedString(enable ? "false" : QString());
        proxy->invalidate();
    }
};

FieldGroupDialog::FieldGroupDialog(QWidget *parent) :
    QDialog(parent),
    d(new FieldGroupDialogPrivate()),
    ui(new Ui::FieldGroupDialog)
{
    ui->setupUi(this);
    ui->lvFields->setModel(d->proxy);
    ui->leNewGroup->installEventFilter(this);
    ui->btnAddGroup->installEventFilter(this);
    ui->lvFields->installEventFilter(this);
    ui->btnAdd->installEventFilter(this);

    QPushButton *ok = ui->buttonBox->button(QDialogButtonBox::Ok);
    if(ok)
    {
        ok->setDefault(false);
        ok->setAutoDefault(false);
    }

}

FieldGroupDialog::~FieldGroupDialog()
{
    delete ui;
    delete d;
}

void FieldGroupDialog::setFieldList(const QStringList &fields)
{
    d->setFields(fields);
//    d->model->setStringList(fields);
    /*
    ui->lwFields->clear();
    ui->lwFields->insertItems(0,fields);
    for(int i=0; i < ui->lwFields->count(); i++)
    {
        QListWidgetItem *item = ui->lwFields->item(i);
        item->setCheckState(Qt::Unchecked);
        Qt::ItemFlags f = item->flags();
        f &= ~Qt::ItemIsUserCheckable;
        item->setFlags(f);
    }
    */
    updateFieldsState();
}

void FieldGroupDialog::setGroupsDef(const QByteArray &buf)
{
    /*
      <panels>
        <panel title="Главное" cols="2">
          <field name="Id"/>
          <field name="Title"/>
          <field name="Request Type"/>
          <field name="Submitter"/>
        </panel>

        <panel title="Источник" cols="2">
          <field name="Submitter"/>
          <field name="Found In Build"/>
        </panel>
      </panels>

     */
    QDomDocument dom;
    if(!dom.setContent(buf,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return;
    QDomElement panels = doc.firstChildElement("panels");
    if(panels.isNull())
        return;
    ui->twGroups->clear();
    QDomElement panel = panels.firstChildElement("panel");
    QList<QTreeWidgetItem *> items;
    for(; !panel.isNull(); panel = panel.nextSiblingElement("panel"))
    {
        QString ptitle = panel.attribute("title");
        QTreeWidgetItem *panelItem = new QTreeWidgetItem();
        panelItem->setText(0,ptitle);
        items.append(panelItem);
        QDomElement field = panel.firstChildElement("field");
        for(; !field.isNull(); field = field.nextSiblingElement("field"))
        {
            QString fname = field.attribute("name");
//            int vid = field.attribute("vid").toInt();
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0,fname);
            panelItem->addChild(item);
        }
    }
    ui->twGroups->addTopLevelItems(items);
    ui->twGroups->expandAll();
    updateFieldsState();
}

QByteArray FieldGroupDialog::groupsDef() const
{
    QDomDocument dom("groups");
    QDomElement root = dom.createElement("groups");
    QDomElement panels = dom.createElement("panels");
    for(int g = 0; g < ui->twGroups->topLevelItemCount(); g++)
    {
        QTreeWidgetItem *top = ui->twGroups->topLevelItem(g);
        QDomElement panel = dom.createElement("panel");
        panel.setAttribute("title", top->text(0));
        for(int c = 0; c < top->childCount(); c++)
        {
            QTreeWidgetItem *child = top->child(c);
            QDomElement field = dom.createElement("field");
            field.setAttribute("name", child->text(0));
            panel.appendChild(field);
        }
        panels.appendChild(panel);
    }
    root.appendChild(panels);
    dom.appendChild(root);
    return dom.toByteArray();
}

void FieldGroupDialog::updateFieldsState()
{
    QStringList used;
    for(int g = 0; g < ui->twGroups->topLevelItemCount(); g++)
    {
        QTreeWidgetItem *top = ui->twGroups->topLevelItem(g);
        for(int c = 0; c < top->childCount(); c++)
        {
            QTreeWidgetItem *child = top->child(c);
            used.append(child->text(0));
        }
    }
    for(int r=0; r<d->model->rowCount(); r++)
    {
        QModelIndex index = d->model->index(r,0);
        if(used.contains(index.data().toString()))
            d->model->setData(index, "true", Qt::UserRole);
        else
            d->model->setData(index, "false", Qt::UserRole);
    }
    d->proxy->invalidate();
    /*
    for(int r=0; r<ui->lwFields->count(); r++)
    {
        QListWidgetItem *item = ui->lwFields->item(r);
        item->setCheckState(used.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }
    */
}

bool FieldGroupDialog::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj && obj == ui->leNewGroup)
    {
        if(ev && ev->type() == QEvent::FocusIn)
            ui->btnAddGroup->setDefault(true);
        if(ev && ev->type() == QEvent::FocusOut)
            ui->btnAddGroup->setDefault(false);
    }
    else if(obj && obj == ui->btnAddGroup)
    {
        if(ev && ev->type() == QEvent::FocusIn)
            ui->btnAddGroup->setDefault(true);
        if(ev && ev->type() == QEvent::FocusOut)
            ui->btnAddGroup->setDefault(false);
    }
    else if(obj && obj == ui->lvFields)
    {
        if(ev && ev->type() == QEvent::FocusIn)
            ui->btnAdd->setDefault(true);
        if(ev && ev->type() == QEvent::FocusOut)
            ui->btnAdd->setDefault(false);
    }
    else if(obj && obj == ui->btnAdd)
    {
        if(ev && ev->type() == QEvent::FocusIn)
            ui->btnAdd->setDefault(true);
        if(ev && ev->type() == QEvent::FocusOut)
            ui->btnAdd->setDefault(false);
    }
    return QDialog::eventFilter(obj, ev);
}

void FieldGroupDialog::on_btnAdd_clicked()
{
    QTreeWidgetItem * cur= ui->twGroups->currentItem();
    QTreeWidgetItem * par;
    int index = 0;
    if(!cur)
    {
        par = ui->twGroups->topLevelItem(ui->twGroups->topLevelItemCount()-1);
        if(par)
            index = par->childCount();
    }
    else if(cur->parent())
    {
        par = cur->parent();
        index = par->indexOfChild(cur)+1;
    }
    else
    {
        par = cur;
        cur = 0;
        index = 0;
    }
    if(!par)
        return;
    foreach(const QModelIndex sel, ui->lvFields->selectionModel()->selectedIndexes())
    {
        QString fname = sel.data(Qt::EditRole).toString();
        QTreeWidgetItem *treeItem = new QTreeWidgetItem();
        treeItem->setText(0, fname);
        par->insertChild(index++, treeItem);
    }
    /*
    foreach(QListWidgetItem *item, ui->lwFields->selectedItems())
    {
        QString fname = item->text();
        QTreeWidgetItem *treeItem = new QTreeWidgetItem();
        treeItem->setText(0, fname);
        par->insertChild(index++, treeItem);
    }
    */
    updateFieldsState();
}

void FieldGroupDialog::on_btnAddGroup_clicked()
{
    QString title = ui->leNewGroup->text().trimmed();
    if(title.isEmpty())
        return;
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, title);
    ui->twGroups->addTopLevelItem(item);
    item->setExpanded(true);
    ui->leNewGroup->clear();
}

void FieldGroupDialog::on_btnUp_clicked()
{
    QList<QTreeWidgetItem *> items = ui->twGroups->selectedItems();
    bool p=false, c=false;
    foreach(QTreeWidgetItem *i, items)
        if(i->parent())
            c |= true;
        else
            p |= true;
    if(!(p ^ c))
        return;
    if(p)
    {
        QList<int> indexes;
        foreach(QTreeWidgetItem *pi, items)
        {
            int i  = ui->twGroups->indexOfTopLevelItem(pi);
            indexes.append(i);
        }
        if(!indexes.size())
            return;
        qSort(indexes);
        int newIndex = indexes[0]-1;
        if(newIndex<0)
            return;
        ui->twGroups->clearSelection();
        foreach(int i, indexes)
        {
            QTreeWidgetItem *item = ui->twGroups->takeTopLevelItem(i);
            ui->twGroups->insertTopLevelItem(newIndex++, item);
            item->setSelected(true);
        }
        return;
    }
    else
    {
        QMultiMap<int, int> indexes;
        //int pIndex = ui->twGroups->topLevelItemCount(), cIndex;
        foreach(QTreeWidgetItem *item, items)
        {
            int pi  = ui->twGroups->indexOfTopLevelItem(item->parent());
            int ci = item->parent()->indexOfChild(item);
            indexes.insert(pi, ci);
        }
        int firstPIndex = indexes.constBegin().key();
        QList<int> firstCIndexes = indexes.values(firstPIndex);
        qSort(firstCIndexes);
        int newCIndex = firstCIndexes[0]-1;
        int newPIndex = firstPIndex;
        QTreeWidgetItem *newParent = ui->twGroups->topLevelItem(newPIndex);
        if(newCIndex < 0)
        {
            if(--newPIndex < 0)
                return;
            newParent = ui->twGroups->topLevelItem(newPIndex);
            newCIndex = newParent->childCount();
        }
        QList<QTreeWidgetItem *> buf;
        int pi = -1;
        foreach (int pIndex, indexes.keys())
        {
            if(pIndex == pi)
                continue;
            pi = pIndex;
            QList<int> cIndexes = indexes.values(pIndex);
            qSort(cIndexes.begin(), cIndexes.end(), qGreater<int>());
            QTreeWidgetItem *parent = ui->twGroups->topLevelItem(pIndex);
            foreach(int ci, cIndexes)
            {
                QTreeWidgetItem *item = parent->takeChild(ci);
                buf.insert(0,item);
            }
        }
        newParent->insertChildren(newCIndex, buf);
        ui->twGroups->clearSelection();
        foreach (QTreeWidgetItem *item, buf)
            item->setSelected(true);
        return;
    }
    /*
    QTreeWidgetItem * cur= ui->twGroups->currentItem();
    if(!cur)
        return;
    QTreeWidgetItem * par = cur->parent();
    if(!par) //top level
        return;
    int index = par->indexOfChild(cur);
    QTreeWidgetItem *newPar = 0;
    int newIndex = 0;
    if(index == 0)
    {
        int topIndex = ui->twGroups->indexOfTopLevelItem(par)-1;
        newPar = ui->twGroups->topLevelItem(topIndex);
        if(!newPar)
            return;
        newIndex = newPar->childCount();
    }
    else
    {
        newPar = par;
        newIndex = index-1;
    }
    if(par && newPar)
    {
        par->removeChild(cur);
        newPar->insertChild(newIndex,cur);
    }
    ui->twGroups->setCurrentItem(cur);
    */
}

void FieldGroupDialog::on_btnDown_clicked()
{
    QList<QTreeWidgetItem *> items = ui->twGroups->selectedItems();
    bool p=false, c=false;
    foreach(QTreeWidgetItem *i, items)
        if(i->parent())
            c |= true;
        else
            p |= true;
    if(!(p ^ c))
        return;
    if(p)
    {
        QList<int> indexes;
        foreach(QTreeWidgetItem *pi, items)
        {
            int i  = ui->twGroups->indexOfTopLevelItem(pi);
            indexes.append(i);
        }
        if(!indexes.size())
            return;
        qSort(indexes);
        int newIndex = indexes[indexes.size()-1]+1;
        if(newIndex >= ui->twGroups->topLevelItemCount())
            return;
        ui->twGroups->clearSelection();
        qSort(indexes.begin(), indexes.end(), qGreater<int>());
        foreach(int i, indexes)
        {
            QTreeWidgetItem *item = ui->twGroups->takeTopLevelItem(i);
            ui->twGroups->insertTopLevelItem(newIndex, item);
            item->setSelected(true);
        }
        return;
    }
    else
    {
        QMultiMap<int, int> indexes;
        //int pIndex = ui->twGroups->topLevelItemCount(), cIndex;
        foreach(QTreeWidgetItem *item, items)
        {
            int pi  = ui->twGroups->indexOfTopLevelItem(item->parent());
            int ci = item->parent()->indexOfChild(item);
            indexes.insert(pi, ci);
        }
        int lastPIndex = (indexes.constEnd()-1).key();
        int newPIndex = lastPIndex;
        QList<int> lastCIndexes = indexes.values(lastPIndex);
        QTreeWidgetItem *newParent = ui->twGroups->topLevelItem(newPIndex);
        qSort(lastCIndexes);
//        int otherCount = newParent->childCount() - lastCIndexes.count();
        int newCIndex = lastCIndexes[0]+1;
        int newLast = lastCIndexes[lastCIndexes.size()-1]+1;
        if(newLast >= newParent->childCount())
        {
            if(++newPIndex >= ui->twGroups->topLevelItemCount())
                return;
            newParent = ui->twGroups->topLevelItem(newPIndex);
            newCIndex = 0;
        }
        QList<QTreeWidgetItem *> buf;
        int pi = -1;
        foreach (int pIndex, indexes.keys())
        {
            if(pIndex == pi)
                continue;
            pi = pIndex;
            QList<int> cIndexes = indexes.values(pIndex);
            qSort(cIndexes.begin(), cIndexes.end(), qGreater<int>());
            QTreeWidgetItem *parent = ui->twGroups->topLevelItem(pIndex);
            foreach(int ci, cIndexes)
            {
                QTreeWidgetItem *item = parent->takeChild(ci);
                buf.insert(0,item);
            }
        }
        newParent->insertChildren(newCIndex, buf);
        ui->twGroups->clearSelection();
        foreach (QTreeWidgetItem *item, buf)
            item->setSelected(true);
        return;
    }

    /*
    QTreeWidgetItem * cur= ui->twGroups->currentItem();
    if(!cur)
        return;
    QTreeWidgetItem * par = cur->parent();
    if(!par) //top level
        return;
    int index = par->indexOfChild(cur);
    QTreeWidgetItem *newPar = 0;
    int newIndex = 0;
    if(index == par->childCount()-1)
    {
        int topIndex = ui->twGroups->indexOfTopLevelItem(par)+1;
        newPar = ui->twGroups->topLevelItem(topIndex);
        if(!newPar)
            return;
        newIndex = 0;
    }
    else
    {
        newPar = par;
        newIndex = index+1;
    }
    if(par && newPar)
    {
        par->removeChild(cur);
        newPar->insertChild(newIndex,cur);
    }
    ui->twGroups->setCurrentItem(cur);
    */
}

void FieldGroupDialog::on_btnDel_clicked()
{
    QList<QTreeWidgetItem *> items= ui->twGroups->selectedItems();
    QList<QTreeWidgetItem *> buf = items;
    foreach(QTreeWidgetItem *item, items)
    {
        if(!item->parent())
            continue;
        buf.removeAll(item);
        delete item;
    }
    foreach(QTreeWidgetItem *item, buf)
        delete item;
    updateFieldsState();
}

void FieldGroupDialog::on_cHideUsed_clicked()
{
    d->setFiltered(ui->cHideUsed->isChecked());
}
