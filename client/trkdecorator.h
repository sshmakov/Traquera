#ifndef TRKDECORATOR_H
#define TRKDECORATOR_H

#include <QObject>
#include <QValidator>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QDateTimeEdit>
//#include "trkview.h"
#include "tqbase.h"
#include <QTableView>

#define Settings_Grid "TrackerGrid3"

struct EditDef {
    QWidget *edit;
    QWidget *internalEditor;
    int fieldcol;
    int fieldid;
    QString fieldName;
    QString title;
};

class EditDefList: public QObject
{
    Q_OBJECT
protected:
    TQRecord *rec;
public:
    QList<EditDef> edits;
    QList<QWidget *> panels;
    bool isFilled;
    bool onlyView;
    int recType;
    EditDefList()
        :isFilled(0), recType(-1), rec(0)
    {
    }
    ~EditDefList();
    void connectToRecord(TQRecord *record);
    void readValues();
    void updateState();
    void clear();
protected slots:
    void recordChanged();
    void recordDestroyed();
};

class FieldGroupsDef
{
public:
    QStringList groups;
    QList<QStringList> fieldsByGroup;
    FieldGroupsDef() : groups(), fieldsByGroup() { groups.clear(); fieldsByGroup.clear(); }
    FieldGroupsDef(const FieldGroupsDef &src) : groups(src.groups), fieldsByGroup(src.fieldsByGroup) {}
    bool isValid() { return !fieldsByGroup.isEmpty(); }
    FieldGroupsDef &operator=(const FieldGroupsDef &src)
    {
        groups = src.groups;
        fieldsByGroup = src.fieldsByGroup;
        return *this;
    }
};

class TrkDecorator : public QObject
{
    Q_OBJECT
public:
    explicit TrkDecorator(QObject *parent = 0);
    void fillEditPanels(QTabWidget *tabs, const TQAbstractRecordTypeDef *recDef, EditDefList &def, bool onlyView = true);
    void readValues(TQRecord *record, EditDefList &def);
    void updateState(TQRecord *record, EditDefList &def);
    void loadViewDef(QTableView *view);
    FieldGroupsDef loadGroups(const TQAbstractRecordTypeDef *recDef);
protected:
    void clearEdits(QTabWidget *tabs, EditDefList &def);
    
signals:
    
public slots:
    void fieldEditChanged();
    
};

extern TrkDecorator *decorator;

class TrkFieldEdit;

Q_DECLARE_INTERFACE(TrkFieldEdit,
                     "allrecall.tracktasks.TrkFieldEditInterface/1.0")

class TrkFieldEdit
{
protected:
    QString field;
    TQRecord *rec;
public:
    explicit TrkFieldEdit(const QString &fieldName);
    void connectToRecord(TQRecord *record);
    virtual void clearValue() = 0;
    virtual void setValue(const QVariant &value) = 0;
    virtual QVariant value() const = 0;
    virtual void setReadOnly(bool readOnly) = 0;
    void valueChanged(const QString &fieldName, const QVariant &newValue);
};

class TrkStringEdit: public QLineEdit, public TrkFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TrkFieldEdit)
public:
    explicit TrkStringEdit(QWidget *parent, const QString &fieldName);
    virtual void TrkFieldEdit::clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onTextChanged(const QString &text);
};

class TrkChoiceBox: public QComboBox, public TrkFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TrkFieldEdit)
public:
    explicit TrkChoiceBox(QWidget *parent, const QString &fieldName);
    virtual void clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onEditTextChanged(const QString &text);
};

class TrkDateTimeEdit: public QDateTimeEdit, public TrkFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TrkFieldEdit)
public:
    explicit TrkDateTimeEdit(QWidget *parent, const QString &fieldName);
    virtual void clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onDateTimeChanged(const QDateTime &value);
};

#endif // TRKDECORATOR_H
