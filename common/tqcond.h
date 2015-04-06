#ifndef TQCOND_H
#define TQCOND_H

#include <QObject>
#include <QtCore>

class TQQueryCond;
class TQQueryDef;

class TQCond : public QObject
{
    Q_OBJECT
public:
    int vid;
    bool isAnd;
    bool isOpenBracket;
    bool isCloseBracket;
    bool isNot;
    TQQueryDef *queryDef;

    enum StringPos {
        First, Middle, Last
    };

public:
    explicit TQCond(TQQueryDef *parent=0);
    TQCond(const TQCond &src);
    virtual TQCond &operator =(const TQCond &src);
    virtual QString fieldName() const;
    virtual QString makeQueryString(int pos = TQCond::Middle) const;
    virtual QString condSubString() const;
    virtual bool editProperties();
    /*
    virtual bool parseSavedString(const QString &s);
    virtual QString makeSavedString() const;
    */
//    bool isAndOp() const;
//    void setAndOp(bool op);
signals:
    
public slots:
    
};

class TQChoiceCond: public TQCond
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
    QVariantList values;
    explicit TQChoiceCond(TQQueryDef *parent=0);
    TQChoiceCond(const TQChoiceCond &src);
    TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
};

class QPushButton;
class QAction;

class TQNumberCond: public TQCond
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

class TQUserCond: public TQCond
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

class TQDateCond: public TQCond
{
    Q_OBJECT
public:
    enum Operations
    {
        Equals, Between, GreaterThan, LessThan
    };

    int op; //  1 = equals, 2 = between, 3 = greater than, 4 = less than.
        // 7 = between (days ago), 6 = greater than (days ago), 5 = less than (days ago)
    bool isDaysValue;
    QDateTime value1, value2;
    int days1, days2;
    int flags1, flags2; // 0 = use the next number as the date value; 1 = use current date/time as the date value; 2 = <<Unassigned>>.
    explicit TQDateCond(TQQueryDef *parent=0);
    virtual QString condSubString() const;
};

class TQStringCond: public TQCond
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

class TQQueryDef: public QObject
{
    Q_OBJECT
protected:
    TQAbstractProject *project;
    QList<TQCond *> nestedCond;
    int recordType;
public:
    //TQQueryCond *condition;
    explicit TQQueryDef(TQAbstractProject *prj, int rectype);
    virtual TQAbstractRecordTypeDef *recordDef();
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
public slots:
    void miscActionTriggered(const QString &actionText);
signals:
    void changed();
};

#endif // TQCOND_H
