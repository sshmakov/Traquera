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
    void loadGroupsDef(const TQAbstractRecordTypeDef *recDef);
    void saveGroupsDef(const TQAbstractRecordTypeDef *recDef);
    bool setDefaultGroupsDef(const TQAbstractRecordTypeDef *recDef);
    bool setGroupsDef(const TQAbstractRecordTypeDef *recDef, QByteArray &buf);
    QByteArray groupsDef() const;
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
    virtual void clearValues() = 0;
    virtual void setValues(const QVariant &values) = 0;
    virtual QVariant values() const = 0;
    virtual void setReadOnly(bool readOnly) = 0;
    void valueChanged(const QString &fieldName, const QVariant &newValue);
};

class TQStringEdit: public QLineEdit, public TQFieldEdit
{
    Q_OBJECT
    Q_INTERFACES(TQFieldEdit)
public:
    explicit TQStringEdit(QWidget *parent, const QString &fieldName);
    virtual void TQFieldEdit::clearValues();
    virtual void setValues(const QVariant &values);
    virtual QVariant values() const;
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
    virtual void clearValues();
    virtual void setValues(const QVariant &values);
    virtual QVariant values() const;
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
    virtual void clearValues();
    virtual void setValues(const QVariant &values);
    virtual QVariant values() const;
    virtual void setReadOnly(bool readOnly);
protected slots:
    void onDateTimeChanged(const QDateTime &values);
};

#endif // TRKDECORATOR_H
