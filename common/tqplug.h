#ifndef TQPLUG_H
#define TQPLUG_H

#include "QtCore"

class TQ
{
  Q_ENUMS(TQFieldTypes)
public:
  enum TQFieldTypes {
      TQ_FIELD_TYPE_NONE			= 0,
      TQ_FIELD_TYPE_CHOICE		= 1,
      TQ_FIELD_TYPE_STRING		= 2,
      TQ_FIELD_TYPE_NUMBER		= 3,
      TQ_FIELD_TYPE_DATE			= 4,
      TQ_FIELD_TYPE_SUBMITTER	= 5,
      TQ_FIELD_TYPE_OWNER		= 6,
      TQ_FIELD_TYPE_USER			= 7,
      TQ_FIELD_TYPE_ELAPSED_TIME = 8,
      TQ_FIELD_TYPE_STATE		= 9,
      TQ_FIELD_TYPE_PERCENT     = 101
  };
};

class TrkToolChoice {
public:
    QString displayText;
    QVariant fieldValue;
    TrkToolChoice() {}
    TrkToolChoice(const TrkToolChoice &src)
        :displayText(src.displayText), fieldValue(src.fieldValue)
    {}
    ~TrkToolChoice() {}
    //TRK_UINT weight;
    //TRK_UINT order;
    //TRK_UINT res;
};

typedef QHash<QString, QVariant> FieldValues;

typedef QList<TrkToolChoice> ChoiceList;

class AbstractRecordTypeDef;

class AbstractFieldDef {
protected:
    mutable ChoiceList *p_choiceList;
    AbstractRecordTypeDef *recDef;
    //static ChoiceList emptyChoices;
public:
    QString name;
    int fType;
    bool nullable;
    int minValue;
    int maxValue;
    QVariant defaultValue;
    //const ChoiceList *userList();

    AbstractFieldDef(AbstractRecordTypeDef *recordDef=0)
        : recDef(recordDef), name(), fType(TQ::TQ_FIELD_TYPE_NONE), p_choiceList(new ChoiceList()),
          nullable(false), minValue(0), maxValue(INT_MAX)
    {

    }
    AbstractFieldDef(const AbstractFieldDef& src)
        : recDef(src.recDef), name(src.name), fType(src.fType), p_choiceList(new ChoiceList(*src.p_choiceList)),
          nullable(src.nullable), minValue(src.minValue), maxValue(src.maxValue), defaultValue(src.defaultValue)
    {

    }
    ~AbstractFieldDef()
    {
        if(p_choiceList)
        {
            delete p_choiceList;
            p_choiceList = 0;
        }
    }

    AbstractFieldDef& operator =(const AbstractFieldDef& src)
    {
        recDef = src.recDef;
        name = src.name;
        fType = src.fType;
        nullable = src.nullable;
        p_choiceList = src.p_choiceList;
        minValue = src.minValue;
        maxValue = src.maxValue;
        defaultValue = src.defaultValue;
        return *this;
    }

    bool isChoice() const
    {
        return (fType == TQ::TQ_FIELD_TYPE_CHOICE)
                || (fType == TQ::TQ_FIELD_TYPE_SUBMITTER)
                || (fType == TQ::TQ_FIELD_TYPE_OWNER)
                || (fType == TQ::TQ_FIELD_TYPE_USER)
                //|| (type == TRK_FIELD_TYPE_ELAPSED_TIME)
                || (fType == TQ::TQ_FIELD_TYPE_STATE)
                ;
    }
    bool isUser() const
    {
        return (fType == TQ::TQ_FIELD_TYPE_SUBMITTER)
                || (fType == TQ::TQ_FIELD_TYPE_OWNER)
                || (fType == TQ::TQ_FIELD_TYPE_USER)
                ;
    }
    inline bool isValid() const
    {
        return recDef && !name.isEmpty();
    }
    bool isNullable() const
    {
        return nullable;
    }
    virtual ChoiceList choiceList() const = 0;
    virtual QStringList choiceStringList(bool isDisplayText = true) const = 0;
    virtual bool canSubmit() = 0;
    virtual bool canUpdate() = 0;
    virtual QString valueToDisplay(const QVariant &value) const = 0;
    virtual QVariant displayToValue(const QString &text) const = 0;

    //friend class TrkToolProject;
    friend class AbstractFieldType;
};

class AbstractFieldType {
protected:
    AbstractFieldDef *def;
public:
    AbstractFieldType(AbstractFieldDef *srcDef):def(srcDef) {}
    AbstractFieldType(AbstractFieldDef &srcDef):def(&srcDef) {}
    AbstractFieldType():def(0) {}
    AbstractFieldType(const AbstractFieldType &src)
        :def(src.def)
    {}

    inline bool isValid() const { return def!=0; }
    int fType() const
    {
        if(isValid())
            return def->fType;
        return TQ::TQ_FIELD_TYPE_NONE;
    }
    ChoiceList choiceList() const
    {
        if(isValid())
            return def->choiceList();
        return QList<TrkToolChoice>(); //&AbstractFieldDef::emptyChoices;
    }
    QStringList choiceStringList(bool isDisplayText = true) const
    {
        if(isValid())
            return def->choiceStringList(isDisplayText);
        return QStringList();
    }
    inline bool isChoice() const
    {
        if(isValid())
            return def->isChoice();
        return false;
    }
    inline bool isUser() const
    {
        if(isValid())
            return def->isUser();
        return false;
    }
    inline bool isNullable() const
    {
        if(isValid())
            return def->isNullable();
        return false;
    }
    inline bool canSubmit() const
    {
        if(isValid())
            return def->canSubmit();
        return false;
    }
    inline bool canUpdate() const
    {
        if(isValid())
            return def->canUpdate();
        return false;
    }

    inline QString valueToDisplay(const QVariant &value) const
    {
        if(isValid())
            return def->valueToDisplay(value);
        return "";
    }
    inline QVariant displayToValue(const QString &text) const
    {
        if(isValid())
            return def->displayToValue(text);
        return QVariant();
    }
    inline QVariant defaultValue() const
    {
        if(isValid())
            return def->defaultValue;
        return QVariant();
    }
    inline int minValue() const
    {
        if(isValid())
            return def->minValue;
        return 0;
    }
    inline int maxValue() const
    {
        if(isValid())
            return def->maxValue;
        return 0;
    }

    friend class AbstractRecordTypeDef;
};


class AbstractRecordTypeDef
{
public:
    virtual QStringList fieldNames() const = 0;
    virtual AbstractFieldType getFieldDef(int vid, bool *ok = 0) const = 0;
    virtual AbstractFieldType getFieldDef(const QString &name, bool *ok = 0) const = 0;
    virtual int fieldType(const QString &name) const = 0;
    //virtual TRK_FIELD_TYPE fieldType(TRK_VID vid) const = 0;
    virtual bool canFieldSubmit(const QString &name) const = 0;
    virtual bool canFieldUpdate(const QString &name) const = 0;
    virtual ChoiceList choiceList(const QString &fieldName) = 0;
    //QList<TRK_VID> fieldVids() const ;
    //TRK_VID fieldVid(const QString &name);
    virtual QList<int> fieldVids() const = 0;
    //TRK_RECORD_TYPE recordType() const;
    virtual QString fieldName(int vid) const = 0;
    virtual QIODevice *defineSource() const = 0;
};

Q_DECLARE_INTERFACE(AbstractFieldType, "com.allrecall.traquera.AbstractFieldType")
Q_DECLARE_INTERFACE(AbstractFieldDef, "com.allrecall.traquera.AbstractFieldDef")
Q_DECLARE_INTERFACE(AbstractRecordTypeDef, "com.allrecall.traquera.AbstractRecordTypeDef")


#endif // TQPLUG_H
