#ifndef TQCHOICEARRAYEDIT_H
#define TQCHOICEARRAYEDIT_H

#include "tqgui_global.h"
#include <QComboBox>
#include <QCompleter>

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
    void slotTextEdited(const QString &text);
    void slotEditFinished();
};

class TQArrayCompleterPrivate;
class TQGUISHARED_EXPORT TQArrayCompleter: public QCompleter
{
    Q_OBJECT
private:
    TQArrayCompleterPrivate *d;
public:
    explicit TQArrayCompleter(QObject *parent = 0);
    ~TQArrayCompleter();
    void setArrayEdit(TQChoiceArrayEdit *edit);
    void setFieldDef(const TQAbstractFieldType &fieldDef);
    QStringList splitPath(const QString &path) const;
    QString	pathFromIndex ( const QModelIndex & index ) const;
};

class TQArrayTreeModelPrivate;
class TQGUISHARED_EXPORT TQArrayTreeModel: public QAbstractItemModel
{
    Q_OBJECT
private:
    TQArrayTreeModelPrivate *d;
public:
    explicit TQArrayTreeModel(QObject *parent = 0);
    ~TQArrayTreeModel();
    void setFieldDef(const TQAbstractFieldType &fieldDef);
    int	columnCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant	data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    QModelIndex	parent(const QModelIndex & index) const;
    int	rowCount(const QModelIndex & parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
    bool hasChildren(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

#endif // TQCHOICEARRAYEDIT_H
