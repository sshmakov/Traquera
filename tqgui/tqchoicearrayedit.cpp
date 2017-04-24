#include "tqchoicearrayedit.h"
#include <QtGui>
#include <tqplug.h>

//============================= TQChoiceArrayEdit ===============================
class TQChoiceArrayEditPrivate
{
public:
    QVariantList values;
    TQAbstractFieldType fieldDef;
    TQChoiceList choices;
//    TQChoiceArrayEditPrivate() {}
    QListWidget *list;
    QFrame *frame;
    TQArrayCompleter *comp;
};

TQChoiceArrayEdit::TQChoiceArrayEdit(QWidget *parent)
    : QComboBox(parent), d(new TQChoiceArrayEditPrivate())
{
//    setEditText("");

    // setup the popup list
    d->frame = new QFrame(0, Qt::Popup);
    d->list = new QListWidget();
    d->list->setSelectionMode(QAbstractItemView::MultiSelection);
    d->list->setSelectionBehavior(QAbstractItemView::SelectItems);
    d->list->clearSelection();
    d->frame->setLayout(new QVBoxLayout());
    d->frame->layout()->addWidget(d->list);
    d->frame->layout()->setContentsMargins(0,0,0,0);

    connect(d->list, SIGNAL(itemChanged(QListWidgetItem*)), SLOT(slotItemChanged(QListWidgetItem*)));
    d->list->installEventFilter(this);
    setEditable(true);
    d->comp = new TQArrayCompleter(this);
    setCompleter(d->comp);
    d->comp->setArrayEdit(this);
    connect(lineEdit(), SIGNAL(textEdited(QString)), SLOT(slotTextEdited(QString)));
    connect(lineEdit(), SIGNAL(editingFinished()), SLOT(slotEditFinished()));
}

TQChoiceArrayEdit::~TQChoiceArrayEdit()
{
    delete d->frame;
    delete d;
}

void TQChoiceArrayEdit::setFieldDef(const TQAbstractFieldType &fieldDef)
{
    Q_ASSERT(&fieldDef);
    d->fieldDef = fieldDef;
    d->choices = d->fieldDef.choiceList();
    foreach(TQChoiceItem item, d->choices)
        addItem(item.displayText, false);
    d->comp->setFieldDef(fieldDef);
}

void TQChoiceArrayEdit::showPopup()
{
    int screenBound = 50;

    QRect rec = QRect(geometry());

    //QPoint p = this->mapToGlobal(QPoint(0,rec.height()));
    //QRect rec2(p , p + QPoint(rec.width(), rec.height()));

    // get the two possible list points and height
    QRect screen = QApplication::desktop()->screenGeometry(this);
    QPoint above = this->mapToGlobal(QPoint(0,0));
    int aboveHeight = above.y() - screen.y();
    QPoint below = this->mapToGlobal(QPoint(0,rec.height()));
    int belowHeight = screen.bottom() - below.y();

    // first activate it with height 1px to get all the items initialized
    QRect rec2;
    rec2.setTopLeft(below);
    rec2.setWidth(rec.width());
    rec.setHeight(1);
    d->frame->setGeometry(rec2);
    d->frame->raise();
    d->frame->show();
    QCoreApplication::processEvents();

    // determine rect
    int contentHeight = d->list->count()*d->list->sizeHintForRow(0) + 4; // +4 - should be determined by margins?
    belowHeight = qMin(abs(belowHeight)-screenBound, contentHeight);
    aboveHeight = qMin(abs(aboveHeight)-screenBound, contentHeight);

    // do we use below or above
    if (belowHeight==contentHeight || belowHeight>aboveHeight)
    {
        rec2.setTopLeft(below);
        rec2.setHeight(belowHeight);
    }
    else
    {
        rec2.setTopLeft(above - QPoint(0,aboveHeight));
        rec2.setHeight(aboveHeight);
    }

    d->frame->setGeometry(rec2);
    d->frame->raise();
    d->frame->show();
    d->list->setFocus();
}

void TQChoiceArrayEdit::hidePopup()
{
    d->frame->hide();
}


void TQChoiceArrayEdit::clearValues()
{
    d->values.clear();
    QComboBox::clear();
}

void TQChoiceArrayEdit::setValues(const QVariantList &values)
{
    QString display = d->fieldDef.valueToDisplay(values);
    setEditText(display);
    for(int i = 0; i<d->choices.size(); i++)
    {
        TQChoiceItem item = d->choices[i];
        if(values.contains(item.fieldValue))
            setItemChecked(i, true);
        else
            setItemChecked(i, false);
    }
    d->values = values;
}

QVariantList TQChoiceArrayEdit::values() const
{

    return d->values;
}

int TQChoiceArrayEdit::addItem ( const QString & text, bool checked)
{
    QListWidgetItem* wi = new QListWidgetItem(text);
    wi->setFlags(wi->flags() | Qt::ItemIsUserCheckable);
    if (checked)
        wi->setCheckState(Qt::Checked);
    else
        wi->setCheckState(Qt::Unchecked);
    d->list->addItem(wi);
    return d->list->count()-1;
}

void TQChoiceArrayEdit::setReadOnly(bool readOnly)
{
    QComboBox::setEnabled(!readOnly);
}

void TQChoiceArrayEdit::setItemChecked(int index, bool checked)
{
    Qt::CheckState state = checked ? Qt::Checked : Qt::Unchecked;
    QListWidgetItem* item = d->list->item(index);
    if(item)
        item->setCheckState(state);
    setItemData(index, state, Qt::CheckStateRole);
}

bool TQChoiceArrayEdit::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == d->list)
    {
        if (event->type() == QEvent::KeyPress)
        {
             QKeyEvent *key = static_cast<QKeyEvent*>(event);
             if(key->modifiers() != Qt::NoModifier)
                 return false;
             switch(key->key())
             {
             case Qt::Key_Return:
             case Qt::Key_Enter:
                 hidePopup();
                 return true;
             }
        }
    }
    return QComboBox::eventFilter(obj, event);
}

void TQChoiceArrayEdit::slotItemChanged(QListWidgetItem *item)
{
    QVariantList values;
    for(int i = 0; i<d->list->count(); i++)
    {
        QListWidgetItem *it = d->list->item(i);
        if(it->checkState() == Qt::Checked)
        {
            QVariant v = d->choices[i].fieldValue;
            values.append(v);
        }
    }
    d->values = values;
    QString display = d->fieldDef.valueToDisplay(d->values);
    setEditText(display);
}

void TQChoiceArrayEdit::slotTextEdited(const QString &text)
{
    return;
}

void TQChoiceArrayEdit::slotEditFinished()
{
    QString text = lineEdit()->text();
    QVariant v = d->fieldDef.displayToValue(text);
    QVariantList vlist = v.toList();
    setValues(vlist);
//    QCompleter *comp = completer();
//    QStringList lines;
//    if(comp)
//        lines = comp->splitPath(lineEdit()->text());
//    foreach(TQChoiceItem &ch, d->choices)
//    {
//        if
//    }

//    d->values.clear();
//    return;
}

class TQArrayCompleterPrivate
{
public:
    TQChoiceArrayEdit *edit;
    TQArrayTreeModel *model;
};


TQArrayCompleter::TQArrayCompleter(QObject *parent)
    : QCompleter(parent),
      d(new TQArrayCompleterPrivate())
{
    setCompletionMode(UnfilteredPopupCompletion);
    setCaseSensitivity(Qt::CaseInsensitive);
    d->model = new TQArrayTreeModel(this);
    setModel(d->model);
}

TQArrayCompleter::~TQArrayCompleter()
{
    delete d;
}

void TQArrayCompleter::setArrayEdit(TQChoiceArrayEdit *edit)
{
    d->edit = edit;

}

void TQArrayCompleter::setFieldDef(const TQAbstractFieldType &fieldDef)
{
    d->model->setFieldDef(fieldDef);
}

QStringList TQArrayCompleter::splitPath(const QString &path) const
{
    QStringList res = path.split(QRegExp("\\s*;\\s*"),QString::KeepEmptyParts);
    return res;
    //return QCompleter::splitPath(path);
}

QString TQArrayCompleter::pathFromIndex(const QModelIndex &index) const
{
//    return index.data(Qt::EditRole).toString();

    QStringList res;
    QModelIndex i = index;
    while(i.isValid())
    {
        res.insert(0, i.data(Qt::EditRole).toString());
        i = i.parent();
    }
    return res.join("; ");

//    QString res = QCompleter::pathFromIndex(index);
//    return res;
}

struct TQArrayTreeItem
{
    QStringList parents;
    int parentId;
    QStringList childs;
    QList<int> childIds;
    bool isLoaded;
    TQChoiceItem cur;
    int row;
};

class TQArrayTreeModelPrivate
{
public:
    TQChoiceList choices;
    QList<TQArrayTreeItem> items;
};

TQArrayTreeModel::TQArrayTreeModel(QObject *parent)
    : QAbstractItemModel(parent), d(new TQArrayTreeModelPrivate())
{

}

TQArrayTreeModel::~TQArrayTreeModel()
{
    delete d;
}

void TQArrayTreeModel::setFieldDef(const TQAbstractFieldType &fieldDef)
{
    beginResetModel();
    d->choices = fieldDef.choiceList();
    d->items.clear();
    int row = 0;
    foreach(const TQChoiceItem &ch, d->choices)
    {
        TQArrayTreeItem p;
        p.parentId = -1;
        p.cur = ch;
        p.row = row++;
        foreach(const TQChoiceItem &cc, d->choices)
            if(cc.displayText != p.cur.displayText)
                p.childs.append(cc.displayText);
        p.isLoaded = false;
        d->items.append(p);
    }
    endResetModel();
}

int TQArrayTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant TQArrayTreeModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int id = index.internalId();
    if(id<0 || id>=d->items.size())
        return QVariant();
    const TQArrayTreeItem &it = d->items.at(id);
    if(role == Qt::DisplayRole)
        return it.cur.displayText;
    if(role == Qt::EditRole)
    {
        return it.cur.displayText;
//        QStringList plist = it.parents;
//        plist.append(it.cur.displayText);
//        QString res =  plist.join("; ");
//        return res;

//        return it.cur.fieldValue;
    }
    return QVariant();

}

QModelIndex TQArrayTreeModel::parent(const QModelIndex &index) const
{
    int id = index.internalId();
    if(id<0 || id>=d->items.size())
        return QModelIndex();
    const TQArrayTreeItem &item = d->items.at(id);
    int pid = item.parentId;
    if(pid<0)
        return QModelIndex();
    const TQArrayTreeItem &pitem = d->items.at(pid);
    return createIndex(pitem.row, 1, pid);
}

int TQArrayTreeModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return d->choices.size();
    int id = parent.internalId();
    if(id<0 || id>=d->items.size())
        return 0;
    const TQArrayTreeItem &item = d->items.at(id);
    return item.childs.size();
}

QModelIndex TQArrayTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(column != 0 || row < 0)
        return QModelIndex();
    if(!parent.isValid())
        return createIndex(row, column, row);
    int pid = parent.internalId();
    if(pid<0 || pid >= d->items.size())
        return QModelIndex();
    TQArrayTreeItem &pitem = d->items[pid];
    if(!pitem.isLoaded)
    {
        QStringList plist = pitem.parents;
        plist.append(pitem.cur.displayText);
        int childRow = 0;
        foreach (const TQChoiceItem &ch, d->choices)
        {
            if(pitem.childs.contains(ch.displayText))
            {
                TQArrayTreeItem citem;
                citem.cur = ch;
                citem.isLoaded = false;
                citem.parentId = pid;
                citem.parents = plist;
                citem.row = childRow++;
                foreach(const TQChoiceItem &cc, d->choices)
                    if(cc.displayText != citem.cur.displayText && !plist.contains(cc.displayText))
                        citem.childs.append(cc.displayText);
                d->items.append(citem);
                pitem.childIds.append(d->items.size()-1);
            }
        }
        pitem.isLoaded = true;
    }
    if(row >= pitem.childIds.size())
        return QModelIndex();
    int id = pitem.childIds.value(row);
    return createIndex(row, column, id);
}

bool TQArrayTreeModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return false;
    int pid = parent.internalId();
    if(pid<0 || pid >= d->items.size())
        return false;
    return !d->items.at(pid).childs.isEmpty();
}

Qt::ItemFlags TQArrayTreeModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
