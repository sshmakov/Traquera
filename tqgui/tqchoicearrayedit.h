#ifndef TQCHOICEARRAYEDIT_H
#define TQCHOICEARRAYEDIT_H

#include "tqgui_global.h"
#include <QComboBox>

class TQAbstractFieldType;
class QListWidgetItem;

class TQChoiceArrayEditPrivate;
class TQGUISHARED_EXPORT TQChoiceArrayEdit: public QComboBox
{
    Q_OBJECT
//    Q_INTERFACES(TQFieldEdit)
    Q_PROPERTY(QVariantList values READ values WRITE setValues USER true)
private:
    TQChoiceArrayEditPrivate *d;
public:
    explicit TQChoiceArrayEdit(QWidget *parent);
    ~TQChoiceArrayEdit();
    void setFieldDef(const TQAbstractFieldType &fieldDef);
    void showPopup();
    void hidePopup();
    virtual void clearValues();
    virtual void setValues(const QVariantList &values);
    virtual QVariantList values() const;
    virtual void setReadOnly(bool readOnly);
    void setItemChecked(int index, bool checked);
protected:
    int addItem(const QString &text, bool checked);
    bool eventFilter(QObject *obj, QEvent *ev);
private slots:
    void slotItemChanged(QListWidgetItem *item);
};


#endif // TQCHOICEARRAYEDIT_H
