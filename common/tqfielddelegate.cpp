#include "tqfielddelegate.h"
#include <QtGui>

TQFieldDelegate::TQFieldDelegate(QObject *parent) : QObject(parent)
{

}

QWidget *TQFieldDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const TQAbstractRecordTypeDef *recordDef, int vid) const
{
    /*
    QComboBox *cb;
    QLineEdit *le;
    QSpinBox *sb;
    TQDateTimeFieldEdit *dt;
    TQChoiceArrayEdit *mc;
    bool ok = false;
    TQAbstractFieldType fdef = recordDef->getFieldType(vid, &ok);
    if(!ok)
        return 0;
    QStringList sl;
    int type = fdef.simpleType(); // panel->fieldRow(index.row()).fieldType;
    switch(type)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
        cb = new QComboBox(parent);
        sl = fdef.choiceStringList(true);
        cb->addItems(sl);
        cb->setEditable(false);
        cb->setInsertPolicy(QComboBox::NoInsert);
        return cb;
    case TQ::TQ_FIELD_TYPE_STRING:
        le = new QLineEdit(parent);
        return le;

    case TQ::TQ_FIELD_TYPE_NUMBER:
        sb = new QSpinBox(parent);
        sb->setMinimum(fdef.minValueInt());
        sb->setMaximum(fdef.maxValueInt());
        return sb;
    case TQ::TQ_FIELD_TYPE_DATE:
        dt = new TQDateTimeFieldEdit(parent);
        dt->setDisplayFormat(fdef.recordDef()->dateTimeFormat());
        dt->setMinimumDateTime(fdef.minValue().toDateTime());
        return dt;
    case TQ::TQ_FIELD_TYPE_ARRAY:
        mc = new TQChoiceArrayEdit(parent, fdef.name());
        foreach(TQChoiceItem item, fdef.choiceList())
        {
            mc->addItem(item.displayText, false);
        }
        return mc;
    }
    */
    return new QLineEdit(parent);
}

void TQFieldDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const TQRecord *record, int vid) const
{
    painter->save();
    QStyle *style = QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &option, painter);
    painter->restore();
}

void TQFieldDelegate::setEditorData(QWidget *editor, const TQRecord *record, int vid) const
{

}

void TQFieldDelegate::setModelData(QWidget *editor, TQRecord *record, int vid) const
{

}

QSize TQFieldDelegate::sizeHint(const QStyleOptionViewItem &option, const TQRecord *record, int vid) const
{
    return QSize();
}

void TQFieldDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const TQRecord *record, int vid) const
{

}

