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
      //TQ_FIELD_TYPE_SUBMITTER	= 5,
      //TQ_FIELD_TYPE_OWNER		= 6,
      TQ_FIELD_TYPE_USER			= 7,
      TQ_FIELD_TYPE_ELAPSED_TIME = 8,
      //TQ_FIELD_TYPE_STATE		= 9,
      TQ_FIELD_TYPE_PERCENT     = 101
  };
};

/*
enum TQFieldType {
    TQ_FIELD_TYPE_NONE			= 0,
    TQ_FIELD_TYPE_CHOICE		= 1,
    TQ_FIELD_TYPE_STRING		= 2,
    TQ_FIELD_TYPE_NUMBER		= 3,
    TQ_FIELD_TYPE_DATE			= 4,
    TQ_FIELD_TYPE_USER			= 7};
*/

class TQChoiceItem {
public:
    QString displayText;
    QVariant fieldValue;
    int id;
    int weight;
    int order;
    TQChoiceItem()
        :order(0), weight(0), id(0)
    {}
    TQChoiceItem(const TQChoiceItem &src)
        :displayText(src.displayText), fieldValue(src.fieldValue), order(src.order), weight(src.weight), id(src.id)
    {}
    ~TQChoiceItem() {}
    bool operator == (const TQChoiceItem &src)
    {
        return displayText == src.displayText
                && fieldValue == src.fieldValue
                && order == src.order
                && weight == src.weight
                && id == src.id;
    }

    //TRK_UINT res;
};

typedef QHash<QString, QVariant> FieldValues;

class TQChoiceList: public QList<TQChoiceItem>
{
public:
    inline QStringList displayLines() const
    {
        QStringList lines;
        for(int i=0; i<count(); i++)
            lines.append(operator [](i).displayText);
        return lines;
    }
    inline TQChoiceList &operator =(const QList<TQChoiceItem> &src)
    {
        QList<TQChoiceItem>::operator =(src);
        return *this;
    }
};

class TQAbstractFieldType;
class TQAbstractProject;

class TQAbstractRecordTypeDef
{
public:
    enum TQFieldRole {
        IdField,
        TitleField,
        DescriptionField,
        StateField,
        SubmitDateTimeField,
        SubmitterField,
        OwnerField
    };

    virtual QStringList fieldNames() const = 0;
    virtual TQAbstractFieldType getFieldType(int vid, bool *ok = 0) const = 0;
    virtual TQAbstractFieldType getFieldType(const QString &name, bool *ok = 0) const = 0;
    virtual int fieldNativeType(int vid) const = 0;
    virtual int fieldSimpleType(int vid) const = 0;
    virtual bool canFieldSubmit(int vid) const = 0;
    virtual bool canFieldUpdate(int vid) const = 0;
    virtual bool isNullable(int vid) const = 0;
    virtual bool hasChoiceList(int vid) const = 0;
    //virtual ChoiceList choiceList(int vid) const = 0;
    //virtual ChoiceList choiceList(const QString &fieldName) const = 0;
    virtual TQChoiceList choiceTable(const QString &tableName) const = 0;
    virtual bool containFieldVid(int vid) const = 0;
    virtual int fieldVid(const QString &name) const = 0;
    virtual QList<int> fieldVids() const = 0;
    virtual QString fieldName(int vid) const = 0;
    virtual QIODevice *defineSource() const = 0;
    virtual int recordType() const = 0;
    virtual QString valueToDisplay(int vid, const QVariant &value) const = 0;
    virtual QVariant displayToValue(int vid, const QString &text) const = 0;
    virtual QVariant fieldDefaultValue(int vid) const = 0;
    virtual QVariant fieldMinValue(int vid) const = 0;
    virtual QVariant fieldMaxValue(int vid) const = 0;
    virtual QString fieldChoiceTable(int vid) const = 0;
    virtual int roleVid(int role) const = 0;
    virtual QString dateTimeFormat() const = 0;
    virtual QStringList noteTitleList() const = 0;
    virtual TQAbstractProject *project() const = 0;
};

/*class AbstractFieldDef {
protected:
    //mutable ChoiceList *p_choiceList;
    AbstractRecordTypeDef *recDef;
    //QString name;
    int vid;
public:
//    int fType;
//    bool nullable;
//    int minValue;
//    int maxValue;
//    QVariant defaultValue;
    //const ChoiceList *userList();

    AbstractFieldDef(AbstractRecordTypeDef *recordDef, int fieldVID)
        : recDef(recordDef),
          //name(),
          vid(fieldVID)
          //, fType(TQ::TQ_FIELD_TYPE_NONE), p_choiceList(new ChoiceList()),
          //nullable(false), minValue(0), maxValue(INT_MAX),
    {

    }
    AbstractFieldDef(const AbstractFieldDef& src)
        : recDef(src.recDef),
          //name(src.name),
          vid(src.vid)
        //, fType(src.fType), p_choiceList(new ChoiceList(*src.p_choiceList)),
        //  nullable(src.nullable), minValue(src.minValue), maxValue(src.maxValue), defaultValue(src.defaultValue)
    {

    }
    ~AbstractFieldDef()
    {

//        if(p_choiceList)
//        {
//            delete p_choiceList;
//            p_choiceList = 0;
//        }

    }

    AbstractFieldDef& operator =(const AbstractFieldDef& src)
    {
        recDef = src.recDef;
        //name = src.name;
        vid = src.vid;
//        fType = src.fType;
//        nullable = src.nullable;
//        p_choiceList = src.p_choiceList;
//        minValue = src.minValue;
//        maxValue = src.maxValue;
//        defaultValue = src.defaultValue;
        return *this;
    }

    int nativeType() const
    {
        return recDef->fieldNativeType(vid);
    }

    int simpleType() const
    {
        return recDef->fieldSimpleType(vid);
    }

    bool isChoice() const
    {
        int fType = simpleType();
        return (fType == TQ::TQ_FIELD_TYPE_CHOICE)
                || (fType == TQ::TQ_FIELD_TYPE_USER)
                ;
    }
    bool isUser() const
    {
        int fType = simpleType();
        return (fType == TQ::TQ_FIELD_TYPE_USER);
    }
    inline bool isValid() const
    {
        return recDef && vid;
    }
    bool isNullable() const
    {
        return false;
    }
    virtual ChoiceList choiceList() const
    {
        if(!isValid())
            return ChoiceList();
        return recDef->choiceList(vid);
    }
    virtual QStringList choiceStringList(bool isDisplayText = true) const = 0;
    virtual bool canSubmit() = 0;
    virtual bool canUpdate() = 0;
    virtual QString valueToDisplay(const QVariant &value) const = 0;
    virtual QVariant displayToValue(const QString &text) const = 0;
    int fieldVid() const
    {
        return vid;
    }

    //friend class TrkToolProject;
    friend class AbstractFieldType;
};
*/


/*class AbstractFieldType {
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
*/

class TQAbstractFieldType {
protected:
    const TQAbstractRecordTypeDef *def;
    int vid;
public:
    TQAbstractFieldType()
        : def(0), vid(0)
    {}

    TQAbstractFieldType(const TQAbstractRecordTypeDef *recDef, int fieldVid)
        : def(recDef), vid(fieldVid)
    {}
    TQAbstractFieldType(const TQAbstractFieldType &src)
        : def(src.def), vid(src.vid)
    {}
    inline bool isValid() const { return def && vid; }
    inline QString name()
    {
        if(isValid())
           return def->fieldName(vid);
        return QString();
    }

    int nativeType() const
    {
        if(isValid())
            return def->fieldNativeType(vid);
        return TQ::TQ_FIELD_TYPE_NONE;
    }
    int simpleType() const
    {
        if(isValid())
            return def->fieldSimpleType(vid);
        return TQ::TQ_FIELD_TYPE_NONE;
    }

    virtual QString choiceTableName() const
    {
        if(isValid())
            return def->fieldChoiceTable(vid);
        return QString();
    }

    virtual TQChoiceList choiceList() const
    {
        if(isValid())
        {
            QString table = choiceTableName();
            if(!table.isEmpty())
            {
                TQChoiceList res;
                if(isNullable())
                {
                    TQChoiceItem ch;
                    ch.displayText="";
                    ch.fieldValue=QVariant();
                    ch.order = 0;
                    ch.weight = 0;
                    res.append(ch);
                }
                res.append(def->choiceTable(table));
                return res;
            }
        }
        return TQChoiceList(); //&AbstractFieldDef::emptyChoices;
    }

    virtual QStringList choiceStringList(bool isDisplayText = true) const
    {
        QStringList res;
        if(isValid())
        {
            foreach(const TQChoiceItem &ch, choiceList())
                if(isDisplayText)
                    res.append(ch.displayText);
                else
                    res.append(ch.fieldValue.toString());
        }
        return res;
    }
    inline bool isChoice() const
    {
        if(isValid())
            return def->hasChoiceList(vid);
        return false;
    }
    inline bool isUser() const
    {
        if(isValid())
            return def->fieldSimpleType(vid) == TQ::TQ_FIELD_TYPE_USER;
        return false;
    }
    inline bool isNullable() const
    {
        if(isValid())
            return def->isNullable(vid);
        return false;
    }
    inline bool canSubmit() const
    {
        if(isValid())
            return def->canFieldSubmit(vid);
        return false;
    }
    inline bool canUpdate() const
    {
        if(isValid())
            return def->canFieldUpdate(vid);
        return false;
    }

    inline QString valueToDisplay(const QVariant &value) const
    {
        if(isValid())
            return def->valueToDisplay(vid,value);
        return "";
    }
    inline QVariant displayToValue(const QString &text) const
    {
        if(isValid())
            return def->displayToValue(vid,text);
        return QVariant();
    }
    inline QVariant defaultValue() const
    {
        if(isValid())
            return def->fieldDefaultValue(vid);
        return QVariant();
    }
    inline int minValueInt() const
    {
        return minValue().toInt();
    }
    inline int maxValueInt() const
    {
        return maxValue().toInt();
    }
    inline QVariant minValue() const
    {
        if(isValid())
            return def->fieldMinValue(vid);
        return QVariant();
    }
    inline QVariant maxValue() const
    {
        if(isValid())
            return def->fieldMaxValue(vid);
        return QVariant();
    }
    inline int virtualID() const
    {
        return vid;
    }
    inline const TQAbstractRecordTypeDef *recordDef() const
    {
        return def;
    }

    friend class TQAbstractRecordTypeDef;
};



//Q_DECLARE_INTERFACE(AbstractFieldType, "com.allrecall.traquera.AbstractFieldType")
//Q_DECLARE_INTERFACE(AbstractFieldDef, "com.allrecall.traquera.AbstractFieldDef")
Q_DECLARE_INTERFACE(TQAbstractRecordTypeDef, "com.allrecall.traquera.TQAbstractRecordTypeDef")


#endif // TQPLUG_H
