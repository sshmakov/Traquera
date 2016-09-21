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
    d->values = values;
    QString display = d->fieldDef.valueToDisplay(d->values);
    setEditText(display);
    for(int i = 0; i<d->choices.size(); i++)
    {
        TQChoiceItem item = d->choices[i];
        if(d->values.contains(item.fieldValue))
            setItemChecked(i, true);
        else
            setItemChecked(i, false);
    }
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


