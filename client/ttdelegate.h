#ifndef TTDELEGATE_H
#define TTDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QWidget>
#include <QDateTimeEdit>
#include <QItemEditorCreator>
#include <tqplug.h>

class QTableWidget;
class QToolButton;
class QHBoxLayout;

class ModifyPanel;
class TQAbstractFieldType;
class TTDelegate;
class TQEditorFactoryPrivate;

/*!
  \class TQEditorFactory
 * \brief The TQEditorFactory class
 */
class TQEditorFactory: public QItemEditorFactory
{
    TQEditorFactoryPrivate *d;
public:
    TQEditorFactory();
    ~TQEditorFactory();
    QWidget *	createEditor ( QVariant::Type type, QWidget * parent ) const;
    QWidget *	createSimpleEditor (const TQAbstractRecordTypeDef *recordDef, int simpleType, QWidget * parent ) const;
    QWidget *	createNativeEditor (const TQAbstractRecordTypeDef *recordDef, int nativeType, QWidget * parent ) const;
    QWidget *	createFieldEditor (const TQAbstractRecordTypeDef *recordDef, int vid,  QWidget * parent ) const;
//    void	registerEditor ( QVariant::Type type, QItemEditorCreatorBase * creator );
    void registerSimpleEditor(const TQAbstractRecordTypeDef *recordDef, int simpleType,  QItemEditorCreatorBase * creator);
    void registerNativeEditor(const TQAbstractRecordTypeDef *recordDef, int nativeType,  QItemEditorCreatorBase * creator);
    void registerFieldEditor(const TQAbstractRecordTypeDef *recordDef, int vid,  QItemEditorCreatorBase * creator);
//    QByteArray	valuePropertyName ( QVariant::Type type) const;
    QItemEditorCreatorBase *editorCreator(QWidget *editor) const;
    QByteArray valuePropertyName(QWidget *editor) const;
    void editorDestroyed(QObject *editor);
};

class TTItemEditor: public QWidget
{
    Q_OBJECT
protected:
    ModifyPanel *panel;
    QWidget *subeditor;
    QToolButton *clearBtn;
    QToolButton *resetBtn;
    QWidget *btnPlace;
    QHBoxLayout *lay;
    QString itemName;
    QWidget *win;
public:
    explicit TTItemEditor(QWidget *parent, ModifyPanel *apanel, const QString &itemName);
    static QWidget *createEditor(QWidget *parent, ModifyPanel *apanel,
                                 const QStyleOptionViewItem &option, const QModelIndex &index);
    void setEditorData(const QModelIndex &index);
    void setModelData(const QModelIndex &index);
    void setSubEditor(QWidget *editor);
    QWidget *subEditor();
signals:
    void resetItem(const QString &itemName);
    void clearItem(const QString &itemName);
protected:
    bool event(QEvent *ev);
    bool eventFilter(QObject *obj, QEvent *ev);
    void showEvent(QShowEvent *ev);
    void hideEvent(QHideEvent *ev);
    void showPanel();
private:
    void initInternal();
    bool isToolBar();
//    QWidget *createSubEditor(TQAbstractFieldType &fdef);
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
    TQEditorFactory *editorFactory() const;

    void assignToPanel(ModifyPanel *apanel);
private slots:
    void onToolBtn();
    void editorDestroyed(QObject *editor);
/*
It is possible for a custom delegate to provide editors without the use of an editor item factory. In this case, the following virtual functions must be reimplemented:

createEditor() returns the widget used to change data from the model and can be reimplemented to customize editing behavior.
setEditorData() provides the widget with data to manipulate.
updateEditorGeometry() ensures that the editor is displayed correctly with respect to the item view.
setModelData() returns updated data to the model.*/

signals:
    
public slots:
    
protected:
    QWidget *createSubEditor(const QModelIndex &index, QWidget *parent) const;
    TQAbstractFieldType fieldDef(const QModelIndex &index) const;
};


//class TQFieldEditorCreatorBase : public QItemEditorCreator
//{
//public:
//    TQFieldEditorCreatorBase();
//    QWidget *	createWidget(QWidget * parent) const;
//    QByteArray	valuePropertyName() const;
//};

class TTAbstractEditor
{
public:

};


class TQUserEditor : public QComboBox
{
    Q_OBJECT
public:
};



class TQDateTimeFieldEdit: public QDateTimeEdit
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime USER true)
public:
    explicit TQDateTimeFieldEdit(QWidget *parent = 0);
    void clear();
    void setDateTime(const QDateTime &dateTime);
    QDateTime dateTime();
};

class TQChoiceEditorPrivate;
class TQChoiceEditor: public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant currentValue READ currentValue WRITE setCurrentValue USER true)
protected:
    TQChoiceEditorPrivate *d;
public:
    TQChoiceEditor(QWidget *parent=0);
    ~TQChoiceEditor();
    QVariant currentValue() const;
    void setCurrentValue(const QVariant &value);
    void setField(const TQAbstractRecordTypeDef *recordDef, int vid);
};

#endif // TTDELEGATE_H
