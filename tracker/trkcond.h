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
class TQRecordTypeDef;

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
    TQRecordTypeDef *recDef;
    QStringList listActions;
public:
    explicit TrkQueryDef(TrkToolProject *project, TQRecordTypeDef *def);
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

#endif // TRKCOND_H
