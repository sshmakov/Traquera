#ifndef TQSERVICEDB_H
#define TQSERVICEDB_H
#include "trkview.h"
#include <QtCore>
#include <QXmlDefaultHandler>
#include <QtNetwork>

class TQServiceProject;

class TQServiceDB : public TQAbstractDB
{
    Q_OBJECT
protected:
    QHash<QString, TQServiceProject*> idProjects;
public:
    QNetworkAccessManager *man;
    QList<QObject *> *readyReplies;
    explicit TQServiceDB(QObject *parent = 0);
    virtual ~TQServiceDB();

    virtual QStringList dbmsTypes();
    virtual QStringList projects(const QString &dbmsType,
                                 const QString &user = QString(),
                                 const QString &pass = QString());
    virtual TQAbstractProject *openProject(
        const QString &projectName,
        const QString &user,
        const QString &pass);

    bool sendRequest(const QString &dbmsType, const QString &action, const QString &query, QXmlContentHandler *handler);


signals:
    
public slots:
//    friend class TQServiceProject;
protected slots:
    void	replyFinished(QNetworkReply * reply);
};

class TQServiceProject: public TQBaseProject
{
    Q_OBJECT
protected:
    QString sessionID, projectID;
    QString dbmsType;
    TQServiceDB *serviceDb;
    QHash<int, TrkToolQryModel *> theQueryModel;
    int defRecType;
    QHash<int, TQAbstractRecordTypeDef *> recDefs;

public:
    TQServiceProject(TQServiceDB *db = 0);
    ~TQServiceProject();
    virtual int defaultRecType() const;
    bool sendRequest(const QString &action, const QString &query, QXmlContentHandler *handler);

    TQRecModel *openQueryModel(const QString &queryName, int recType, bool emitEvent = true);
    TQRecModel *openIdsModel(const IntList &ids, int recType, bool emitEvent = true);
    QList<int> getQueryIds(const QString &name, int type, qint64 afterTransId = 0);

    void refreshModel(QAbstractItemModel *model);
    virtual void initQueryModel(int type);
    QAbstractItemModel *queryModel(int type);
    TQAbstractRecordTypeDef *recordTypeDef(int rectype);
    virtual TQRecord *createRecordById(int id, int rectype);
    /*
    virtual bool isOpened() const = 0;
    virtual QString currentUser() const = 0;
    virtual QString projectName() const = 0;
public:
    // Standard manipulations
    virtual QString fieldVID2Name(int rectype, int vid) = 0;
    virtual int fieldName2VID(int rectype, const QString &fname) = 0;
    virtual bool isSelectedId(int id, int recType) const = 0;
    virtual void setSelectedId(int id, bool value, int recType) = 0;
    virtual void clearSelected(int recType) = 0;
    virtual int fieldNativeType(const QString &name, int recType) = 0;
    virtual TrkToolModel *selectedModel(int recType) = 0;
    virtual bool canFieldSubmit(int vid, int recType) = 0;
    virtual bool canFieldUpdate(int vid, int recType) = 0;
    virtual QString userFullName(const QString &login) = 0;
    virtual TrkToolRecord *createRecordById(int id, int rectype) = 0;
    virtual TrkToolRecord *newRecord(int rectype) = 0;
    virtual QDomDocument recordTypeDefDoc(int rectype) = 0;
    */
protected:
    /* Record manipulations */
    virtual bool readRecordWhole(TQRecord *record);
    virtual bool readRecordFields(TQRecord *record);
    virtual bool readRecordTexts(TQRecord *record);
    virtual bool readRecordBase(TQRecord *record);
//    virtual TrkToolRecord *createRecordByHandle(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    virtual QVariant getFieldValue(const TQRecord *record, const QString &fname, bool *ok = 0);
    virtual QVariant getFieldValue(const TQRecord *record, int vid, bool *ok = 0);
    virtual bool setFieldValue(TQRecord *record, const QString &fname, const QVariant &value);
    //virtual bool insertRecordBegin(TrkToolRecord *record);
    virtual bool updateRecordBegin(TQRecord *record);
    virtual bool commitRecord(TQRecord *record);
    virtual bool cancelRecord(TQRecord *record);
    virtual QList<TQToolFile> attachedFiles(TQRecord *record);
    virtual QStringList historyList(TQRecord *record);
    virtual QHash<int,QString> baseRecordFields(int rectype);
    TQRecord *recordOfIndex(const QModelIndex &index);
    virtual bool isSystemModel(QAbstractItemModel *model) const;

    friend class TQServiceDB;
};


#endif // TQSERVICEDB_H
