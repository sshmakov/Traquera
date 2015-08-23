#ifndef TQCOND_H
#define TQCOND_H

#include <QObject>
#include <QtCore>
#include <tqplugin_global.h>
#include <tqplug.h>

class TQQueryCond;
class TQQueryDef;
class TQAbstractRecordTypeDef;

class TQPLUGIN_SHARED TQCond : public QObject
{
    Q_OBJECT
protected:
    int m_vid;
//    bool m_isAnd;
//    bool m_isOpenBracket;
//    bool m_isCloseBracket;
//    bool m_isNot;

public:
    TQQueryDef *queryDef;

    enum StringPos {
        First, Middle, Last
    };
    enum Flag {
        OrFlag = 0x1,
        NotFlag = 0x2,
        OpenFlag = 0x4,
        CloseFlag = 0x8
    };
    Q_DECLARE_FLAGS(CondFlags, Flag)

protected:
    CondFlags m_flags, m_mask;

public:
    explicit TQCond(TQQueryDef *parent=0);
    TQCond(const TQCond &src);
    virtual TQCond &operator =(const TQCond &src);
    virtual QString fieldName() const;
    virtual QString makeQueryString(int pos = TQCond::Middle) const;
    virtual QString condSubString() const;
    virtual bool editProperties();
    TQCond::CondFlags flags() const;
    TQCond::CondFlags mask() const;
    virtual int vid() const;
    virtual bool isOr() const;
    virtual bool isOpenBracket() const;
    virtual bool isCloseBracket() const;
    virtual bool isNot() const;
    virtual void setVid(int value);
    virtual void setIsOr(bool value);
    virtual void setIsOpenBracket(bool value);
    virtual void setIsNot(bool value);
    virtual void setIsCloseBracket(bool value);
    virtual void setFlags(TQCond::CondFlags value);
    virtual void setMask(TQCond::CondFlags value);
    TQAbstractRecordTypeDef *recordType() const;
    /*
    virtual bool parseSavedString(const QString &s);
    virtual QString makeSavedString() const;
    */
//    bool isAndOp() const;
//    void setAndOp(bool op);
signals:
    
public slots:
    
};


Q_DECLARE_OPERATORS_FOR_FLAGS(TQCond::CondFlags)

/*
class TQFieldCond: public TQCond
{
    Q_OBJECT
protected:
    int m_vid;
public:
    explicit TQFieldCond(TQQueryDef *parent=0);
    virtual TQCond &operator =(const TQCond &src);
    virtual QString fieldName() const;
    virtual QString condSubString() const;
    virtual int vid() const;
    virtual void setVid(int value);
};
*/

class TQPLUGIN_SHARED TQChoiceCond: public TQCond
{
    Q_OBJECT
public:
    enum Operations
    {
        Null,
        Any,
        Selected
    };

    int op; // 1 - null, 2 - any, 3 - selected values
    TQChoiceList values;
    explicit TQChoiceCond(TQQueryDef *parent=0);
    TQChoiceCond(const TQChoiceCond &src);
    TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
};

class QPushButton;
class QAction;

class TQPLUGIN_SHARED TQNumberCond: public TQCond
{
    Q_OBJECT
public:
    enum Operations
    {
        Equals = 1, Between, GreaterThan, LessThan
    };

    int op; //  1 = equals, 2 = between, 3 = greater than, 4 = less than
    QVariant value1, value2;

    explicit TQNumberCond(TQQueryDef *parent=0);
    TQNumberCond(const TQNumberCond &src);
    TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    bool editProperties();
};

class TQPLUGIN_SHARED TQUserCond: public TQCond
{
    Q_OBJECT
public:
//    The optional flag is distinguished by having its high bit (0x80000000) set.
//    If the low bit (0x1) is set, then any following IDs represent groups rather than users.
//    If the next bit (0x2) is set, then active users will be included in the target set.
//    If the following bit (0x4) is set, then deleted users will be included in the target set.
//    If no flag is given, then any IDs are assumed to be user IDs and only active users will be included in the target set.
    bool isGroups;
    bool isActiveIncluded;
    bool isDeletedIncluded;
    bool isNullIncluded;
    bool isCurrentIncluded;
    QList<uint> ids;
    explicit TQUserCond(TQQueryDef *parent = 0);
    TQCond &operator = (const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
};

class TQPLUGIN_SHARED TQDateCond: public TQCond
{
    Q_OBJECT
public:
    enum Operations
    {
        Equals, Between, GreaterThan, LessThan
    };

    Operations op; //  1 = equals, 2 = between, 3 = greater than, 4 = less than.
        // 7 = between (days ago), 6 = greater than (days ago), 5 = less than (days ago)
    bool isDaysValue;
    bool isCurrentDate1, isCurrentDate2;
    QDateTime value1, value2;
    int days1, days2;
    int flags1, flags2; // 0 = use the next number as the date value; 1 = use current date/time as the date value; 2 = <<Unassigned>>.
    explicit TQDateCond(TQQueryDef *parent=0);
    TQCond &operator = (const TQCond &src);
    virtual QString condSubString() const;
};

class TQPLUGIN_SHARED TQStringCond: public TQCond
{
    Q_OBJECT
public:
    bool isCaseSensitive;
    QString value;
    explicit TQStringCond(TQQueryDef *parent=0);
    TQCond &operator = (const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
};

/*
class TQQueryCond: public QObject
{
    Q_OBJECT
public:
    QList<TQCond *> nestedCond;
//    bool nestedAnd;
public:
    explicit TQQueryCond(TQQueryDef *parent);
    TQQueryCond(const TQQueryCond &src);
    virtual bool parseSavedString(QString &s);
    virtual QString makeSavedString() const;
};
*/

class TQAbstractProject;
class TQAbstractRecordTypeDef;

class TQPLUGIN_SHARED TQQueryDef: public QObject
{
    Q_OBJECT
protected:
    TQAbstractProject *m_project;
    QList<TQCond *> nestedCond;
    int recordType;
    QString m_name;
public:
    //TQQueryCond *condition;
    explicit TQQueryDef(TQAbstractProject *prj, int rectype);
    virtual TQAbstractRecordTypeDef *recordDef();
    TQAbstractProject *project() const;
    virtual QStringList queryDefLines();
    virtual QString condLine(int index);
    TQCond *condition(int index);
    int indexOf(TQCond *cond);
    virtual bool editProperties(int index);
    virtual TQCond *newCondition(int fieldVid);
    virtual void insertCondition(int before, TQCond *cond);
    virtual void appendCondition(TQCond *cond);
    virtual void removeCondition(int index);
    virtual TQCond *takeCondition(int index);
    virtual int count();
//    virtual QList<QAction *>actionsAddCond();
    virtual QStringList miscActions();
    virtual QString name();
    virtual void setName(const QString &name);
public slots:
    void miscActionTriggered(const QString &actionText);
signals:
    void changed();
};

#endif // TQCOND_H
