#ifndef TRKCOND_H
#define TRKCOND_H

#include <tqbase.h>
//#include <QtGui>

class TrkQueryDef;

class TrkCond : public TQCond
{
    Q_OBJECT
protected:
    TrkQueryDef *q;
public:
    explicit TrkCond(TrkQueryDef *parent = 0);
    TrkCond(TrkCond &src);
    virtual TQCond &operator =(const TQCond &src);
    /*
    bool parseSavedString(QString s);
    virtual QString makeSavedString() const;
    */
signals:
    
public slots:
    
};

class TrkKeywordCond: public TrkCond
{
    Q_OBJECT
public:
    QStringList keys;
    bool isKeyAnd;
    bool isKeyCase;
    bool isKeyInNoteTitles;
    bool isKeyInDesc;
    bool isKeyInRecTitles;
    bool isKeyInNoteText;
    bool isKeyInNoteOnly;
    QString noteTitleSearch;

    explicit TrkKeywordCond(TrkQueryDef *parent = 0);
    virtual TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();

};



class TrkToolProject;
class TrkRecordTypeDef;

/*
class TrkQueryCond : public TQQueryCond
{
protected:
    TrkToolProject *prj;
    TQRecordTypeDef *recDef;
public:
    explicit TrkQueryCond(TrkQueryDef *parent);
//    bool parseSavedString(QString str);
//    QString nextCItem(QString &str);
};
*/

class QAction;

class TrkQueryDef: public TQQueryDef
{
    Q_OBJECT
protected:
    TrkToolProject *prj;
    TrkRecordTypeDef *recDef;
    QStringList listActions;
public:
    explicit TrkQueryDef(TrkToolProject *m_project, TrkRecordTypeDef *def);
    virtual TQCond *newCondition(int fieldVid);
    bool parseSavedString(QString str);
    static QString nextCItem(QString &str);
//    virtual QList<QAction *>actionsAddCond();
    virtual QStringList miscActions();
public slots:
    void miscActionTriggered(const QString &actionText);
protected slots:

    void addKeywordSearch();
};


class TrkModuleCond: public TQCond
{
    Q_OBJECT
public:
    enum ModulesOp {
        Any, Or, And
    };

    int moduleOp;
    QStringList moduleNames;

    explicit TrkModuleCond(TrkQueryDef *parent = 0);
    TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
    /*
    virtual void setIsAnd(bool value);
    virtual void setIsOpenBracket(bool value);
    virtual void setIsNot(bool value);
    virtual void setIsCloseBracket(bool value);
    */
};

class TrkChangeCond: public TQCond
{
    Q_OBJECT
public:
    enum ChangeObjectEnum {
        FieldChange=1, RecordChange, FileChange, NoteChange, ModuleChange
    };
    ChangeObjectEnum changeObject;

    enum ChangeDateEnum {
        AnyDate, BetweenDates, NotBetweenDates, AfterDate, BeforeDate
    };
    ChangeDateEnum changeDate;

    enum DateModeEnum {
        Days, Date, DateTime
    };
    DateModeEnum dateMode;

    enum ChangeTypeEnum {
        AnyChange, AddChange, ModifyChange, DeleteChange, CheckedIn, CheckedOut, AssignLabelChange
        // 0-any, 1-added, 2-modified,  3-removed, 4-checked in, 5-checked out, 6-assigned version label
    };
    ChangeTypeEnum changeType;
//    int fieldVid;
    QString fileName;
    QStringList noteTitles;
    int authorId;
    QList<int> choiceIds1, choiceIds2;
    QDateTime date1, date2;
    int days1, days2;
    QStringList oldValues, newValues;

    TrkQueryDef *qDef;

    explicit TrkChangeCond(TrkQueryDef *parent = 0);
    TQCond &operator =(const TQCond &src);
    virtual QString condSubString() const;
    virtual bool editProperties();
    bool parseString(QString s);
};

#endif // TRKCOND_H
