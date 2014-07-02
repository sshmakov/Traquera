#ifndef TQSERVICEDB_H
#define TQSERVICEDB_H
#include "trkview.h"
#include <QXmlDefaultHandler>

class TQServiceDB : public TrkToolDB
{
    Q_OBJECT
public:
    explicit TQServiceDB(QObject *parent = 0);

    virtual QStringList dbmsTypes();
    virtual QStringList projects(const QString &dbmsType);
    virtual TrkToolProject *openProject(
        const QString &dbmsType,
        const QString &projectName,
        const QString &user,
        const QString &pass);

signals:
    
public slots:
    
};

class TQServiceProject: public TrkToolProject
{
    Q_OBJECT
public:
    TQServiceProject(TQServiceDB *db = 0);
    ~TQServiceProject();
protected:
    /* Record manipulations */
    virtual bool readRecordWhole(TrkToolRecord *record);
    virtual bool readRecordFields(TrkToolRecord *record);
    virtual bool readRecordTexts(TrkToolRecord *record);
    virtual bool readRecordBase(TrkToolRecord *record);
    virtual TrkToolRecord *createRecordByHandle(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    virtual QVariant getFieldValue(const TrkToolRecord *record, const QString &fname, bool *ok = 0);
    virtual QVariant getFieldValue(const TrkToolRecord *record, TRK_VID vid, bool *ok = 0);
    virtual bool setFieldValue(TrkToolRecord *record, const QString &fname, const QVariant &value);
    //virtual bool insertRecordBegin(TrkToolRecord *record);
    virtual bool updateRecordBegin(TrkToolRecord *record);
    virtual bool commitRecord(TrkToolRecord *record);
    virtual bool cancelRecord(TrkToolRecord *record);
    virtual QList<TrkToolFile> attachedFiles(TrkToolRecord *record);
    virtual QStringList historyList(TrkToolRecord *record);
    virtual QHash<int,QString> baseRecordFields(TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);

};

class TQProjectListHandler: public QXmlDefaultHandler
{
public:
    QStringList projects;
    TQProjectListHandler();
    virtual bool	startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

class TQLoginReplyHandler: public QXmlDefaultHandler
{
public:
    QString sessionID, projectID, errorCode, errorDesc;
    TQLoginReplyHandler();
    virtual bool	startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

#endif // TQSERVICEDB_H
