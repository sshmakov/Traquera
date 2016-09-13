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
#include <TQMultiComboBox/TQMultiComboBox.h>

#define Settings_Grid "TrackerGrid3"

struct EditDef {
    QWidget *edit;
    QWidget *internalEditor;
    int fieldcol;
    int vid;
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
    Q_DECLARE_TR_FUNCTIONS(FieldGroupsDef)
public:
    QStringList groups;
    QList<QStringList> fieldsByGroup;
    QString other;
    FieldGroupsDef()
        : groups(), fieldsByGroup(), other()
    {
        groups.clear();
        fieldsByGroup.clear();
    }
    FieldGroupsDef(const FieldGroupsDef &src)
        : groups(src.groups), fieldsByGroup(src.fieldsByGroup), other(src.other)
    {}
    bool isValid()
    {
        return !fieldsByGroup.isEmpty();
    }
    FieldGroupsDef &operator=(const FieldGroupsDef &src)
    {
        groups = src.groups;
        fieldsByGroup = src.fieldsByGroup;
        return *this;
    }
    void clear();
    void setRecordType(const TQAbstractRecordTypeDef *recDef);
    bool loadDefault(const TQAbstractRecordTypeDef *recDef);
    bool loadXML(const TQAbstractRecordTypeDef *recDef, QByteArray &buf);
    QByteArray toXML() const;
};

class QueryPage;

class TQDecorator : public QObject
{
    Q_OBJECT
protected:
//    TQAbstractProject *prj;
public:
    explicit TQDecorator(QObject *parent = 0);
    void fillEditPanels(QTabWidget *tabs, const TQAbstractRecordTypeDef *recDef, EditDefList &def, bool onlyView = true);
    void readValues(TQRecord *record, EditDefList &def);
    void updateState(TQRecord *record, EditDefList &def);
    void loadViewDef(QueryPage *page);
    bool saveState(QueryPage *page);
    FieldGroupsDef loadGroups(const TQAbstractRecordTypeDef *recDef);
protected:
    FieldGroupsDef loadGroupsXML(const TQAbstractRecordTypeDef *recDef);
    void clearEdits(QTabWidget *tabs, EditDefList &def);
    
signals:
    
public slots:
    void fieldEditChanged();
    
};

extern TQDecorator *decorator;

class TQFieldEdit;

Q_DECLARE_INTERFACE(TQFieldEdit,
                     "allrecall.tracktasks.TrkFieldEditInterface/1.0")

class TQFieldEdit
{
protected:
    QString field;
    TQRecord *rec;
    int vid;
public:
    explicit TQFieldEdit(const QString &fieldName);
    virtual void connectToRecord(TQRecord *record, int fieldVid);
    virtual void clearValue() = 0;
    virtual void setValue(const QVariant &value) = 0;
    virtual QVariant value() const = 0;
    virtual void setReadOnly(bool readOnly) = 0;
    void valueChanged(const QString &fieldName, const QVariant &newValue);
};

class TQStringEdit: public QLineEdit, public TQFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TQFieldEdit)
public:
    explicit TQStringEdit(QWidget *parent, const QString &fieldName);
    virtual void TQFieldEdit::clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onTextChanged(const QString &text);
};

class TQChoiceBox: public QComboBox, public TQFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TQFieldEdit)
public:
    explicit TQChoiceBox(QWidget *parent, const QString &fieldName);
    virtual void clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onEditTextChanged(const QString &text);
};

class TQDateTimeEdit: public QDateTimeEdit, public TQFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TQFieldEdit)
public:
    explicit TQDateTimeEdit(QWidget *parent, const QString &fieldName);
    virtual void clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onDateTimeChanged(const QDateTime &value);
};

class TQChoiceArrayEdit: public TQMultiComboBox, public TQFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TQFieldEdit)
    Q_PROPERTY(QVariant value READ value WRITE setValue USER true)
public:
    explicit TQChoiceArrayEdit(QWidget *parent, const QString &fieldName);
    virtual void clearValue();
    virtual void setValue(const QVariant &value);
    virtual QVariant value() const;
    virtual void setReadOnly(bool readOnly);

};

#endif // TRKDECORATOR_H
