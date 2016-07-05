#include "ttdelegate.h"
//#include "trkview.h"
#include "tqbase.h"
#include "modifypanel.h"

#include <QtGui>

TTDelegate::TTDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

QWidget *TTDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    TTItemEditor *te = (TTItemEditor*)(TTItemEditor::createEditor(parent, panel, option, index));
    connect(te,SIGNAL(resetItem(QString)),this,SLOT(onToolBtn()));
    connect(te,SIGNAL(clearItem(QString)),this,SLOT(onToolBtn()));
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

void TTDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    TTItemEditor* te = qobject_cast<TTItemEditor*>(editor);
    if(te)
    {
        te->setEditorData(index);
    }
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
        te->setModelData(index);
    }
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


TTItemEditor::TTItemEditor(QWidget *parent, ModifyPanel *apanel, const QString &itemName)
    : QWidget(parent), panel(apanel)
{
    this->itemName = itemName;
    connect(this,SIGNAL(resetItem(QString)),panel,SLOT(resetField(QString)));
    connect(this,SIGNAL(clearItem(QString)),panel,SLOT(clearField(QString)));
}

QWidget *TTItemEditor::createEditor(QWidget *parent, ModifyPanel *apanel, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    TTItemEditor *editor = new TTItemEditor(parent, apanel, apanel->fieldName(index));
    editor->initInternal(option, index);
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
}


QWidget * TTItemEditor::createSubEditor(TQAbstractFieldType &fdef)
{
    QComboBox *cb;
    QLineEdit *le;
    QSpinBox *sb;
    TQDateTimeFieldEdit *dt;
    TQMultiComboBox *mc;
//    QWidget *res;
    QStringList sl;
    int type = fdef.simpleType(); // panel->fieldRow(index.row()).fieldType;
    switch(type)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
        cb = new QComboBox(this);
        sl = fdef.choiceStringList(true);
        cb->addItems(sl);
        cb->setEditable(false);
        cb->setInsertPolicy(QComboBox::NoInsert);
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
        mc = new TQMultiComboBox(this);
        foreach(TQChoiceItem item, fdef.choiceList())
        {
            mc->addItem(item.displayText, false);
        }
        return mc;
        /*
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
        cb = new QComboBox(parent);
        cb->addItems(fdef->choiceStringList());
        cb->setEditable(false);
        return cb;
        */

    //case TRK_FIELD_TYPE_ELAPSED_TIME:
    }
    //return new QPlainTextEdit(parent);
    return new QLineEdit(this);
}

void TTItemEditor::doResetClick()
{
    emit resetItem(itemName);

}

void TTItemEditor::doClearClick()
{
    emit clearItem(itemName);
}


void TTItemEditor::initInternal(const QStyleOptionViewItem &option, const QModelIndex &index)
{
    TQAbstractFieldType fdef = panel->fieldDef(index.row());
    subeditor = 0;
    if(fdef.hasCustomFieldEditor())
    {
        subeditor = fdef.createFieldEditor(this);
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
    if(!subeditor)
        subeditor = createSubEditor(fdef);
    isCustomEditor = false;
    clearBtn = new QToolButton(this);
    clearBtn->setContentsMargins(0,0,0,0);
    clearBtn->setIcon(QIcon(":/images/cleartext.png"));
    resetBtn = new QToolButton(this);
    resetBtn->setContentsMargins(0,0,0,0);
    resetBtn->setIcon(QIcon(":/images/resetproperty.png"));
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);
    lay->addWidget(subeditor);
    lay->addWidget(clearBtn);
    lay->addWidget(resetBtn);
    setLayout(lay);
    //setVisible(true);
    setFocusProxy(subeditor);
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(doClearClick()));
    connect(resetBtn,SIGNAL(clicked()),this,SLOT(doResetClick()));
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


