#include "ttdelegate.h"
//#include "trkview.h"
#include "tqbase.h"
#include "modifypanel.h"
#include <ttglobal.h>
#include <tqdebug.h>

#include <QtGui>

#include <../tqgui/tqchoicearrayedit.h>

typedef QPair<const TQAbstractRecordTypeDef *,int> FieldKey;
typedef QMap<FieldKey, QItemEditorCreatorBase *> FieldMap;

class TQEditorFactoryPrivate
{
public:
    FieldMap simpleMap;
    FieldMap nativeMap;
    FieldMap fieldMap;
    QMap<const QWidget *,  QItemEditorCreatorBase *> editorMap;
};


/*!
 * \brief TQEditorFactory::TQEditorFactory
 */
TQEditorFactory::TQEditorFactory()
    : QItemEditorFactory(), d(new TQEditorFactoryPrivate())
{

}

TQEditorFactory::~TQEditorFactory()
{
    QSet<QItemEditorCreatorBase *> set;
    set = d->fieldMap.values().toSet();
    qDeleteAll(set);
    set = d->nativeMap.values().toSet();
    qDeleteAll(set);
    set = d->simpleMap.values().toSet();
    qDeleteAll(set);
    delete d;
}

QWidget *TQEditorFactory::createEditor(QVariant::Type type, QWidget *parent) const
{
    return QItemEditorFactory::createEditor(type, parent);
}

QWidget *TQEditorFactory::createSimpleEditor(const TQAbstractRecordTypeDef *recordDef, int simpleType, QWidget *parent) const
{
    FieldMap::iterator it = d->simpleMap.find(FieldKey(recordDef, simpleType));
    if(it == d->simpleMap.end())
        return 0;
    QWidget *w = it.value()->createWidget(parent);
    if(w)
        d->editorMap.insert(w, it.value());
    return w;
}

QWidget *TQEditorFactory::createNativeEditor(const TQAbstractRecordTypeDef *recordDef, int nativeType, QWidget *parent) const
{
    FieldMap::iterator it = d->nativeMap.find(FieldKey(recordDef, nativeType));
    if(it == d->nativeMap.end())
        return 0;
    QWidget *w = it.value()->createWidget(parent);
    if(w)
        d->editorMap.insert(w, it.value());
    return w;
}

QWidget *TQEditorFactory::createFieldEditor(const TQAbstractRecordTypeDef *recordDef, int vid, QWidget *parent) const
{
    FieldMap::iterator it = d->fieldMap.find(FieldKey(recordDef, vid));
    if(it == d->fieldMap.end())
        return 0;
    QWidget *w = it.value()->createWidget(parent);
    if(w)
        d->editorMap.insert(w, it.value());
    return w;
}

void TQEditorFactory::registerSimpleEditor(const TQAbstractRecordTypeDef *recordDef, int simpleType, QItemEditorCreatorBase *creator)
{
    FieldMap::iterator it = d->simpleMap.find(FieldKey(recordDef, simpleType));
    if(it != d->simpleMap.end())
    {
        QItemEditorCreatorBase *old = it.value();
        d->simpleMap.erase(it);
        delete old;
    }
    d->simpleMap.insert(FieldKey(recordDef, simpleType), creator);
}

void TQEditorFactory::registerNativeEditor(const TQAbstractRecordTypeDef *recordDef, int nativeType, QItemEditorCreatorBase *creator)
{
    FieldMap::iterator it = d->nativeMap.find(FieldKey(recordDef, nativeType));
    if(it != d->nativeMap.end())
    {
        QItemEditorCreatorBase *old = it.value();
        d->nativeMap.erase(it);
        delete old;
    }
    d->nativeMap.insert(FieldKey(recordDef, nativeType), creator);
}

void TQEditorFactory::registerFieldEditor(const TQAbstractRecordTypeDef *recordDef, int vid, QItemEditorCreatorBase *creator)
{
    FieldMap::iterator it = d->fieldMap.find(FieldKey(recordDef, vid));
    if(it != d->fieldMap.end())
    {
        QItemEditorCreatorBase *old = it.value();
        d->fieldMap.erase(it);
        delete old;
    }
    d->fieldMap.insert(FieldKey(recordDef, vid), creator);
}

QItemEditorCreatorBase *TQEditorFactory::editorCreator(QWidget *editor) const
{
    return d->editorMap.value(editor);
}

QByteArray TQEditorFactory::valuePropertyName(QWidget *editor) const
{
    QByteArray name;
    QItemEditorCreatorBase *creator = editorCreator(editor);
    if(creator)
        name = creator->valuePropertyName();
    if(name.isEmpty())
        name = editor->metaObject()->userProperty().name();
    return name;
}

void TQEditorFactory::editorDestroyed(QObject *editor)
{
    QWidget *w = qobject_cast<QWidget*>(editor);
    if(w)
        d->editorMap.remove(w);
}

/*!
  ====================================================================================
 * \brief TTDelegate::TTDelegate
 * \param parent
 * ===================================================================================
 */
TTDelegate::TTDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget *TTDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*
    QWidget *ed = QStyledItemDelegate::createEditor(parent, option, index);
    return ed;
    */


    TTItemEditor *te = (TTItemEditor*)(TTItemEditor::createEditor(parent, panel, option, index));
    connect(te,SIGNAL(resetItem(QString)),this,SLOT(onToolBtn()));
    connect(te,SIGNAL(clearItem(QString)),this,SLOT(onToolBtn()));
    QWidget *sub = createSubEditor(index, te);
    connect(sub,SIGNAL(destroyed(QObject*)),SLOT(editorDestroyed(QObject*)));
    te->setSubEditor(sub);
    return te;

    /*
    QComboBox *cb;
    QLineEdit *le;
    QSpinBox *sb;
    QDateTimeEdit *dt;
    QWidget *res;
    QStringList sl;
    bool ok;
    //if(!panel->fieldRow(index.row()).isEditable)
    //    return 0;
    const TrkFieldDef fdef = panel->fieldDef(index.row());
    int type = fdef.fType; // panel->fieldRow(index.row()).fieldType;
    switch((TRK_FIELD_TYPE)type)
    {
    case TRK_FIELD_TYPE_STATE:
    case TRK_FIELD_TYPE_CHOICE:
        cb = new QComboBox(parent);
        sl = fdef.choiceStringList(true);
        cb->addItems(sl);
        cb->setEditable(false);
        cb->setInsertPolicy(QComboBox::NoInsert);
        return cb;
    case TRK_FIELD_TYPE_STRING:
        le = new QLineEdit(parent);
        return le;

    case TRK_FIELD_TYPE_NUMBER:
        sb = new QSpinBox(parent);
        //sb->setMaximum(100);
        return sb;
    case TRK_FIELD_TYPE_DATE:
        dt = new QDateTimeEdit(parent);
        return dt;


    //case TRK_FIELD_TYPE_ELAPSED_TIME:
    }
    //return new QPlainTextEdit(parent);
    return new QLineEdit(parent);
    */
}

QWidget * TTDelegate::createSubEditor(const QModelIndex &index, QWidget *parent) const
{
    TQAbstractFieldType fdef = fieldDef(index);
    QWidget *res = 0;
    if(fdef.hasCustomFieldEditor())
    {
        res = fdef.createFieldEditor(parent);
        /*
        if(subeditor)
        {
            isCustomEditor = true;
            QHBoxLayout *lay = new QHBoxLayout(this);
            lay->setContentsMargins(0,0,0,0);
            lay->setSpacing(0);
            lay->addWidget(subeditor);
            return;
        }
        */
    }
    if(res)
        return res;
    TQEditorFactory *factory = editorFactory();
    res = factory->createFieldEditor(fdef.recordDef(), fdef.virtualID(), parent);
    if(res)
        return res;
    res = factory->createNativeEditor(fdef.recordDef(), fdef.nativeType(), parent);
    if(res)
        return res;
    res = factory->createSimpleEditor(fdef.recordDef(), fdef.simpleType(), parent);
    if(res)
        return res;
    TQChoiceEditor *ce;
    QLineEdit *le;
    QSpinBox *sb;
    TQDateTimeFieldEdit *dt;
    TQChoiceArrayEdit *mc;
    QStringList sl;
    int type = fdef.simpleType(); // panel->fieldRow(index.row()).fieldType;
    switch(type)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
        ce = new TQChoiceEditor(parent);
        ce->setField(fdef.recordDef(), fdef.virtualID());
//        cb = new QComboBox(parent);
//        sl = fdef.choiceStringList(true);
//        cb->addItems(sl);
//        cb->setEditable(false);
//        cb->setInsertPolicy(QComboBox::NoInsert);
        return ce;
    case TQ::TQ_FIELD_TYPE_STRING:
        le = new QLineEdit(parent);
        return le;

    case TQ::TQ_FIELD_TYPE_NUMBER:
        sb = new QSpinBox(parent);
        sb->setMinimum(fdef.minValueInt());
        sb->setMaximum(fdef.maxValueInt());
        //sb->setMaximum(100);
        return sb;
    case TQ::TQ_FIELD_TYPE_DATE:
        dt = new TQDateTimeFieldEdit(parent);
        dt->setDisplayFormat(fdef.recordDef()->dateTimeFormat());
        dt->setMinimumDateTime(fdef.minValue().toDateTime());
//        dt->setDisplayFormat(TT_DATETIME_FORMAT);
        return dt;
    case TQ::TQ_FIELD_TYPE_ARRAY:
        mc = new TQChoiceArrayEdit(parent);
        mc->setFieldDef(fdef);
        return mc;
//    case TRK_FIELD_TYPE_SUBMITTER:
//    case TRK_FIELD_TYPE_OWNER:
//    case TRK_FIELD_TYPE_USER:
//        cb = new QComboBox(parent);
//        cb->addItems(fdef->choiceStringList());
//        cb->setEditable(false);
//        return cb;
    //case TRK_FIELD_TYPE_ELAPSED_TIME:
    }
    QVariant::Type t = static_cast<QVariant::Type>(index.data(Qt::EditRole).userType());
    return factory->createEditor(t, parent);
}

TQAbstractFieldType TTDelegate::fieldDef(const QModelIndex &index) const
{
    return panel->fieldDef(index.row());

}



void TTDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    TTItemEditor* te = qobject_cast<TTItemEditor*>(editor);
    if(te)
    {
        QWidget *sub = te->subEditor();
        TQEditorFactory *factory = editorFactory();
        QByteArray name = factory->valuePropertyName(sub);
        QVariant value = index.data(Qt::UserRole);
        sub->setProperty(name.constData(), value);
//        te->setEditorData(index);
    }
    else
        QStyledItemDelegate::setEditorData(editor, index);
    /*
    QLineEdit *ed;
    QPlainTextEdit *pe;
    QSpinBox *sb;
    QComboBox *cb;
    QDateTimeEdit *dt;
    if(0!=(ed = qobject_cast<QLineEdit*>(editor)))
        ed->setText(index.data().toString());
    else if(0!=(pe = qobject_cast<QPlainTextEdit*>(editor)))
        pe->setPlainText(index.data().toString());
    else if(0!=(sb = qobject_cast<QSpinBox*>(editor)))
        sb->setValue(index.data().toInt());
    else if(0!=(cb = qobject_cast<QComboBox*>(editor)))
    {
        QString s = index.data().toString();
        int i = cb->findText(s);
        if(i>=0)
            cb->setCurrentIndex(i);
    }
    else if(0 != (dt = qobject_cast<QDateTimeEdit*>(editor)))
    {
        QDateTime value = index.data(Qt::EditRole).toDateTime();
        dt->setDateTime(value);
    }
    */
}

void TTDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    TTItemEditor* te = qobject_cast<TTItemEditor*>(editor);
    if(te)
    {
        TQAbstractFieldType fdef = fieldDef(index);
        if(!fdef.isValid())
            return;
        QString fieldName = fdef.name();
        QWidget *sub = te->subEditor();
        TQEditorFactory *factory = editorFactory();
        QByteArray name = factory->valuePropertyName(sub);
        panel->setFieldValue(fieldName, sub->property(name));
//        te->setModelData(index);
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
    /*
    QLineEdit *ed;
    QPlainTextEdit *pe;
    QSpinBox *sb;
    QComboBox *cb;
    QDateTimeEdit *dt;
    const TrkFieldDef fdef = panel->fieldDef(index.row());
    if(0!=(ed = qobject_cast<QLineEdit*>(editor)))
        panel->setRowValue(index.row(),ed->text());
        //model->setData(index,ed->text());
        //ed->setText(index.data().toString());

    else if(0!=(sb = qobject_cast<QSpinBox*>(editor)))
        panel->setRowValue(index.row(),sb->value());
    else if(0!=(cb = qobject_cast<QComboBox*>(editor)))
        panel->setRowValue(index.row(),cb->currentText());
    else if(0 != (dt = qobject_cast<QDateTimeEdit*>(editor)))
        panel->setRowValue(index.row(),dt->dateTime());
    */
}

void TTDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::updateEditorGeometry(editor,option,index);
}

void TTDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.row()<panel->rows.count() && panel->fieldRow(index.row()).isChanged)
    {
        QStyleOptionViewItem fieldOption(option);
        initStyleOption(&fieldOption,index);
        fieldOption.font.setBold(true);
        QStyledItemDelegate::paint(painter, fieldOption, index);
        /*
        const ModifyPanel::ModifyRow &row = panel->fieldRow(index.row());
        QRect rect = option.rect;
        rect.setLeft(rect.right()-16);
        row.resetBtn->setGeometry(rect);
        painter->dr
        row.resetBtn->p
        */
    }
    else
        QStyledItemDelegate::paint(painter, option, index);
}

QSize TTDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(panel->fieldRow(index.row()).isChanged)
    {
        QSize res = QStyledItemDelegate::sizeHint(option, index);
        res -= QSize(16,0);
        return res;
    }
    else
        return QStyledItemDelegate::sizeHint(option, index);
}

TQEditorFactory *TTDelegate::editorFactory() const
{
    return ttglobal()->mainProc()->fieldEditorFactory();
}

void TTDelegate::assignToPanel(ModifyPanel *apanel)
{
    panel = apanel;
    table = panel->tableWidget();
    table->setItemDelegateForColumn(1,this);
}

void TTDelegate::onToolBtn()
{
    QWidget *editor = qobject_cast<QWidget *>(sender());
    emit closeEditor(editor);
}

void TTDelegate::editorDestroyed(QObject *editor)
{
    editorFactory()->editorDestroyed(editor);
}

// ================= TTItemEditor ========================
TTItemEditor::TTItemEditor(QWidget *parent, ModifyPanel *apanel, const QString &itemName)
    : QWidget(parent), panel(apanel), subeditor(0), btnPlace(0)
{
    this->itemName = itemName;
    connect(this,SIGNAL(resetItem(QString)),panel,SLOT(resetField(QString)));
    connect(this,SIGNAL(clearItem(QString)),panel,SLOT(clearField(QString)));
    initInternal();
}

QWidget *TTItemEditor::createEditor(QWidget *parent, ModifyPanel *apanel,
                                    const QStyleOptionViewItem &option, const QModelIndex &index)
{
    TTItemEditor *editor = new TTItemEditor(parent, apanel, apanel->fieldName(index));
    return editor;
}

void TTItemEditor::setEditorData(const QModelIndex &index)
{
    QLineEdit *ed;
    QPlainTextEdit *pe;
    QSpinBox *sb;
    QComboBox *cb;
    TQDateTimeFieldEdit *dt;
//    TrkFieldType fdef = panel->fieldDef(index.row());
//    int type = fdef.fType();
    if(0!=(ed = qobject_cast<QLineEdit*>(subeditor)))
        ed->setText(index.data().toString());
    else if(0!=(pe = qobject_cast<QPlainTextEdit*>(subeditor)))
        pe->setPlainText(index.data().toString());
    else if(0!=(sb = qobject_cast<QSpinBox*>(subeditor)))
        sb->setValue(index.data().toInt());
    else if(0!=(cb = qobject_cast<QComboBox*>(subeditor)))
    {
        QString s = index.data().toString();
        int i = cb->findText(s);
        if(i>=0)
            cb->setCurrentIndex(i);
    }
    else if(0 != (dt = qobject_cast<TQDateTimeFieldEdit*>(subeditor)))
    {
        QVariant v = index.data(Qt::EditRole);
        QDateTime value = QDateTime::fromString(v.toString(), dt->displayFormat());
        dt->setDateTime(value);
    }
}

void TTItemEditor::setModelData(const QModelIndex &index)
{
    QLineEdit *ed;
    QPlainTextEdit *pe;
    QSpinBox *sb;
    QComboBox *cb;
    TQDateTimeFieldEdit *dt;
    QString fieldName = panel->fieldName(index);
    if(fieldName.isEmpty())
        return;
    TQAbstractFieldType fdef = panel->fieldDef(index.row());
    if(0!=(ed = qobject_cast<QLineEdit*>(subeditor)))
        panel->setFieldValue(fieldName, fdef.displayToValue(ed->text()));
        //model->setData(index,ed->text());
        //ed->setText(index.data().toString());

    else if(0!=(sb = qobject_cast<QSpinBox*>(subeditor)))
        panel->setFieldValue(fieldName,sb->value());
    else if(0!=(cb = qobject_cast<QComboBox*>(subeditor)))
    {
        panel->setFieldValue(fieldName, fdef.displayToValue(cb->currentText()));
        /*
        TQChoiceList ch = fdef.choiceList();
        int i = cb->currentIndex();
        if(i>=0 && i<ch.count())
            panel->setFieldValue(fieldName,ch.value(i).fieldValue);
        else
            panel->setFieldValue(fieldName,QVariant());
            */
    }
    else if(0 != (dt = qobject_cast<TQDateTimeFieldEdit*>(subeditor)))
    {
        panel->setFieldValue(fieldName, fdef.displayToValue(dt->text()));
        /*
        QString text = dt->text();
        QDateTime d = QDateTime::fromString(text,fdef.recordDef()->dateTimeFormat());
//        QDateTime d = QDateTime::fromString(text,TT_DATETIME_FORMAT);
        if(d.isValid())
            panel->setFieldValue(fieldName,dt->dateTime());
        else
            panel->setFieldValue(fieldName,QDateTime());
            */
    }
    else
    {
        QString displayValue = subeditor->property("text").toString();
        panel->setFieldValue(fieldName,  fdef.displayToValue(displayValue));
    }
}

void TTItemEditor::setSubEditor(QWidget *editor)
{
    if(subeditor == editor)
        return;
    if(subeditor)
        delete subeditor;
    subeditor = editor;
    if(subeditor)
    {
        lay->insertWidget(0, subeditor);
        setFocusProxy(subeditor);
    }
}

QWidget *TTItemEditor::subEditor()
{
    return subeditor;
}

bool TTItemEditor::event(QEvent *ev)
{
    return QWidget::event(ev);
}

bool TTItemEditor::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == win && ev->type() == QEvent::UpdateRequest && isVisible())
        showPanel();
    return QWidget::eventFilter(obj, ev);
}

void TTItemEditor::showEvent(QShowEvent *ev)
{
    showPanel();
    if(isToolBar())
        win->installEventFilter(this);
    return QWidget::showEvent(ev);
}

void TTItemEditor::hideEvent(QHideEvent *ev)
{
    btnPlace->hide();
    if(isToolBar())
        win->removeEventFilter(this);
    return QWidget::hideEvent(ev);
}

void TTItemEditor::showPanel()
{
    if(!btnPlace)
        return;
    if(isToolBar())
    {
        btnPlace->show();
        return;
    }
    QRect thisRect = QRect(QPoint(0,0),size());
//    QRect screen = d->popupGeometry(QApplication::desktop()->screenNumber(this));
    QPoint below = mapToGlobal(thisRect.bottomRight());
//    int belowHeight = screen.bottom() - below.y();
    QPoint above = mapToGlobal(thisRect.topRight());
//    int aboveHeight = above.y() - screen.y();
//    bool boundToScreen = !window()->testAttribute(Qt::WA_DontShowOnScreen);

    QSize s = btnPlace->size();
//    below -= QPoint(s.width(), 0);
    QPoint p = above;
    p -= QPoint(s.width(), s.height());
    QRect rec =  QRect(p, above);
//    QRect rec = btnPlace->geometry();
//    rec.setTopLeft(above);
//    rec.setWidth(1);
//    rec.setHeight(1);
//    btnPlace->setGeometry(rec);
    btnPlace->move(p);
    btnPlace->show();
    btnPlace->raise();
    QApplication::processEvents();

    s = btnPlace->size();
    p = above;
    p -= QPoint(s.width(), s.height());
    rec =  QRect(p, above);
    btnPlace->move(p);
    QApplication::processEvents();
}

/*
QWidget * TTItemEditor::createSubEditor(TQAbstractFieldType &fdef)
{
    TQChoiceEditor *cb;
    QLineEdit *le;
    QSpinBox *sb;
    TQDateTimeFieldEdit *dt;
    TQChoiceArrayEdit *mc;
    QStringList sl;
    int type = fdef.simpleType(); // panel->fieldRow(index.row()).fieldType;
    switch(type)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
        cb = new TQChoiceEditor(this);
//        sl = fdef.choiceStringList(true);
//        cb->addItems(sl);
//        cb->setEditable(false);
//        cb->setInsertPolicy(QComboBox::NoInsert);
        return cb;
    case TQ::TQ_FIELD_TYPE_STRING:
        le = new QLineEdit(this);
        return le;

    case TQ::TQ_FIELD_TYPE_NUMBER:
        sb = new QSpinBox(this);
        sb->setMinimum(fdef.minValueInt());
        sb->setMaximum(fdef.maxValueInt());
        //sb->setMaximum(100);
        return sb;
    case TQ::TQ_FIELD_TYPE_DATE:
        dt = new TQDateTimeFieldEdit(this);
        dt->setDisplayFormat(fdef.recordDef()->dateTimeFormat());
        dt->setMinimumDateTime(fdef.minValue().toDateTime());
//        dt->setDisplayFormat(TT_DATETIME_FORMAT);
        return dt;
    case TQ::TQ_FIELD_TYPE_ARRAY:
        mc = new TQChoiceArrayEdit(this, fdef.name());
        foreach(TQChoiceItem item, fdef.choiceList())
        {
            mc->addItem(item.displayText, false);
        }
        return mc;

//    case TRK_FIELD_TYPE_SUBMITTER:
//    case TRK_FIELD_TYPE_OWNER:
//    case TRK_FIELD_TYPE_USER:
//        cb = new QComboBox(parent);
//        cb->addItems(fdef->choiceStringList());
//        cb->setEditable(false);
//        return cb;


    //case TRK_FIELD_TYPE_ELAPSED_TIME:
    }
    return new QLineEdit(this);
}
*/

void TTItemEditor::doResetClick()
{
    emit resetItem(itemName);

}

void TTItemEditor::doClearClick()
{
    emit clearItem(itemName);
}


void TTItemEditor::initInternal()
{
    win = window();
    btnPlace = new QFrame(0/*, Qt::ToolTip*/);
    btnPlace->setGeometry(0,0,10,10);

    clearBtn = new QToolButton(btnPlace);
    clearBtn->setContentsMargins(0,0,0,0);
    clearBtn->setIcon(QIcon(":/images/cleartext.png"));
    resetBtn = new QToolButton(btnPlace);
    resetBtn->setContentsMargins(0,0,0,0);
    resetBtn->setIcon(QIcon(":/images/resetproperty.png"));
    QHBoxLayout *btnLay = new QHBoxLayout(btnPlace);
    btnLay->setMargin(0);
    btnLay->setSpacing(0);
    btnLay->setContentsMargins(0,0,0,0);
    btnLay->addWidget(clearBtn);
    btnLay->addWidget(resetBtn);

    lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);
    btnPlace->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    lay->addWidget(btnPlace);
//    setLayout(lay);
    //setVisible(true);
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(doClearClick()));
    connect(resetBtn,SIGNAL(clicked()),this,SLOT(doResetClick()));
}

bool TTItemEditor::isToolBar()
{
    return ((int)btnPlace->windowFlags() & Qt::Popup | Qt::Tool | Qt::ToolTip);
}

//===================================== TQDateTimeFieldEdit ======================================
TQDateTimeFieldEdit::TQDateTimeFieldEdit(QWidget *parent)
    : QDateTimeEdit(parent)
{
    setSpecialValueText(tr("пусто"));
    setCalendarPopup(true);
}

void TQDateTimeFieldEdit::clear()
{
    QDateTimeEdit::setDateTime(minimumDateTime());
}

void TQDateTimeFieldEdit::setDateTime(const QDateTime &dateTime)
{
    if(dateTime.isNull())
        clear();
    else
        QDateTimeEdit::setDateTime(dateTime);
}

QDateTime TQDateTimeFieldEdit::dateTime()
{
    QDateTime dt = QDateTimeEdit::dateTime();
    if(dt.isNull() || dt == minimumDateTime())
        return QDateTime();
    return dt;
}

class TQChoiceEditorPrivate
{
public:
    TQChoiceList list;
    const TQAbstractRecordTypeDef *recordDef;
    int vid;
};

TQChoiceEditor::TQChoiceEditor(QWidget *parent)
    : QComboBox(parent)
{
    d = new TQChoiceEditorPrivate();
    setEditable(false);
    setInsertPolicy(QComboBox::NoInsert);
}

TQChoiceEditor::~TQChoiceEditor()
{
    delete d;
}

QVariant TQChoiceEditor::currentValue() const
{
    int i = currentIndex();
    if(i<0 || i>=d->list.size())
        return d->recordDef->displayToValue(d->vid, currentText());
    tqDebug() << d->list[i].fieldValue.toString();
    return d->list[i].fieldValue;
}

void TQChoiceEditor::setCurrentValue(const QVariant &value)
{
    tqDebug() << "setCurrentValue" << value.toString();
    for(int i = 0; i<d->list.size(); i++)
    {
        const TQChoiceItem &item = d->list[i];
        if(value == item.fieldValue)
        {
            setCurrentIndex(i);
            return;
        }
    }
    setEditText(d->recordDef->valueToDisplay(d->vid, value));
}

void TQChoiceEditor::setField(const TQAbstractRecordTypeDef *recordDef, int vid)
{
    clear();
    clearEditText();
    QString table = recordDef->fieldChoiceTable(vid);
    d->recordDef = recordDef;
    d->vid = vid;
    d->list = recordDef->choiceTable(table);
    foreach(const TQChoiceItem &item, d->list)
        addItem(item.displayText);
}

