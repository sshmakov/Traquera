#ifndef TTDELEGATE_H
#define TTDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QWidget>
#include <QDateTimeEdit>

class QTableWidget;
class ModifyPanel;
class QToolButton;
class TQAbstractFieldType;

class TTItemEditor: public QWidget
{
    Q_OBJECT
protected:
    ModifyPanel *panel;
    QWidget *subeditor;
    bool isCustomEditor;
    QToolButton *clearBtn;
    QToolButton *resetBtn;
    QString itemName;
public:
    explicit TTItemEditor(QWidget *parent, ModifyPanel *apanel, const QString &itemName);
    static QWidget *createEditor(QWidget *parent, ModifyPanel *apanel, const QStyleOptionViewItem &option, const QModelIndex &index);
    void setEditorData(const QModelIndex &index);
    void setModelData(const QModelIndex &index);
signals:
    void resetItem(const QString &itemName);
    void clearItem(const QString &itemName);
private:
    void initInternal(const QStyleOptionViewItem &option, const QModelIndex &index);
    QWidget *createSubEditor(TQAbstractFieldType &fdef);
private slots:
    void doResetClick();
    void doClearClick();
};

class TTDelegate : public QStyledItemDelegate
{
    Q_OBJECT
private:
    ModifyPanel *panel;
    QTableWidget *table;
public:
    explicit TTDelegate(QObject *parent = 0);
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

    void assignToPanel(ModifyPanel *apanel);
private slots:
    void onToolBtn();
/*
It is possible for a custom delegate to provide editors without the use of an editor item factory. In this case, the following virtual functions must be reimplemented:

createEditor() returns the widget used to change data from the model and can be reimplemented to customize editing behavior.
setEditorData() provides the widget with data to manipulate.
updateEditorGeometry() ensures that the editor is displayed correctly with respect to the item view.
setModelData() returns updated data to the model.*/

signals:
    
public slots:
    
};

class TTAbstractEditor
{
public:

};

class TTUserEditor : public QComboBox
{
    Q_OBJECT
public:
};

class TQDateTimeFieldEdit: public QDateTimeEdit
{
    Q_OBJECT
public:
    explicit TQDateTimeFieldEdit(QWidget *parent = 0);
    void clear();
    void setDateTime(const QDateTime &dateTime);
    QDateTime dateTime();
};


#endif // TTDELEGATE_H
