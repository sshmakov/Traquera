#include "tqservicedb.h"
#include <QtNetwork>
#include <QtXml>
#include "replyhdl.h"
#include "ttutils.h"


TQServiceDB::TQServiceDB(QObject *parent) :
    TQAbstractDB(parent)
{
    man = new QNetworkAccessManager(this);
    readyReplies = new QList<QObject*>();
    connect(man,SIGNAL(finished(QNetworkReply*)),SLOT(replyFinished(QNetworkReply*)));
}

TQServiceDB::~TQServiceDB()
{
    delete man;
    delete readyReplies;
}

QStringList TQServiceDB::dbmsTypes()
{
    return QStringList("http://localhost:8080");
}

void TQServiceDB::setDbmsParams(const QString &dbmsName, const QString &dbmsUser, const QString &dbmsPass)
{
}

QStringList TQServiceDB::projects(const QString &dbmsType)
{
    TQProjectListReplyHandler handler;
    QString query = "<GetProjects/>";
    /*
    QNetworkRequest req;
    req.setUrl(dbmsType);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    QNetworkReply *reply = man.post(req,query.toUtf8());
    QXmlInputSource source(reply);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    if(reader.parse(source))
    */
    if(sendRequest(dbmsType, "GetProjects", query, &handler))
        return handler.projects;
    return QStringList();
}

TQAbstractProject *TQServiceDB::openProject(const QString &dbmsType, const QString &projectName, const QString &user, const QString &pass)
{
    TQLoginReplyHandler handler;
    QString query = QString("<Login project=\"%1\" user=\"%2\" password=\"%3\"/>").
            arg(projectName,user,pass);
    /*
    QNetworkRequest req;
    req.setUrl(dbmsType);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    QNetworkReply *reply = man.post(req,query.toUtf8());
    QXmlInputSource source(reply);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    if(reader.parse(source))
    */
    if(sendRequest(dbmsType, "Login", query, &handler))
    {
        if(!handler.isFault)
        {
            TQServiceProject *prj = new TQServiceProject(this);
            prj->sessionID = handler.sessionID;
            prj->projectID = handler.projectID;
            prj->dbmsType = dbmsType;
            prj->opened = true;
            prj->defRecType = 1; // need to chenge to answered type
            idProjects[handler.projectID] = prj;
            return prj;
        }
    }
    return 0;
}

bool TQServiceDB::sendRequest(const QString &dbmsType, const QString &action, const QString &query, QXmlContentHandler *handler)
{
    QString env("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                "<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\""
                " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
                " xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n"
                "<soapenv:Body>\n"
                "%1\n"
                "</soapenv:Body>\n"
                "</soapenv:Envelope>");

    env = env.arg(query);
    qDebug(env.toLocal8Bit().constData());
    QNetworkRequest req;
    req.setUrl(QUrl(dbmsType));
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    req.setRawHeader("SOAPAction",action.toLocal8Bit());
    QScopedPointer<QNetworkReply> reply(man->post(req,env.toUtf8()));
    QDateTime endTime = QDateTime::currentDateTime().addSecs(10);
    while(!readyReplies->contains(reply.data()))
    {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(QDateTime::currentDateTime() > endTime)
            return false;
    }
    readyReplies->removeAll(reply.data());
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug(reply->errorString().toLocal8Bit().constData());
        return false;
    }
    /*
    QFile f;
    f.open(stdout,QIODevice::WriteOnly);
    QTextStream out(&f);

    QList<QByteArray> hs = reply->rawHeaderList();
    foreach(QByteArray a, hs)
    {
        QString s(a.constData());
        out << s;
    }

    */
    QByteArray text = reply->readAll();
    QString s(text.constData());
    qDebug(text.constData());
    QBuffer buf(&text);
    buf.open(QIODevice::ReadOnly);


    QXmlInputSource source(&buf);
//    QXmlInputSource source(reply.data());
    QXmlSimpleReader reader;
    reader.setContentHandler(handler);
    bool res = reader.parse(source);
    return res;
}

void TQServiceDB::replyFinished(QNetworkReply *reply)
{
    readyReplies->append(reply);
}

/****************************************************************
 *
 *   TQServiceProject
 *
 ****************************************************************/

TQServiceProject::TQServiceProject(TQServiceDB *db)
    : TQBaseProject(db), serviceDb(db)
{
}

TQServiceProject::~TQServiceProject()
{
}

int TQServiceProject::defaultRecType() const
{
    return defRecType;
}

bool TQServiceProject::sendRequest(const QString &action, const QString &query, QXmlContentHandler *handler)
{
    QString sess("<Session sessionID=\"%1\"/>\n"
                 "<Project projectID=\"%2\"/>\n"
                 "%3");
    return serviceDb->sendRequest(dbmsType, action, sess.arg(sessionID,projectID,query), handler);
}

TrkToolModel *TQServiceProject::openQueryModel(const QString &queryName, int recType, bool emitEvent)
{
    TQRecordsetReply handler;
    if(sendRequest("Query",
                   QString("<Query queryName=\"%1\" recordType=\"%2\"/>")
                   .arg(queryName)
                   .arg(recType),
                   &handler))
    {
        TrkToolModel *model = new TrkToolModel(this, recType, this);
        foreach(QString s, handler.idList)
        {
            bool ok;
            int id = s.toInt(&ok);
            if(ok)
            {
                TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(createRecordById(id, recType));
                if(rec)
                    model->append(rec);
            }
        }
        return model;
    }
    return 0;
}

TrkToolModel *TQServiceProject::openIdsModel(const IntList &ids, int recType, bool emitEvent)
{
    TQRecordsetReply handler;
    if(sendRequest("Query",
                   QString("<GetRecords ids='$1'/>").arg(intListToString(ids)),
                   &handler))
    {
        TrkToolModel *model = new TrkToolModel(this, recType, this);
        foreach(QString s, handler.idList)
        {
            bool ok;
            int id = s.toInt(&ok);
            if(ok)
            {
                TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(createRecordById(id, recType));
                if(rec)
                    model->append(rec);
            }
        }
        return model;
    }
    return 0;
}

QList<int> TQServiceProject::getQueryIds(const QString &name, int type, qint64 afterTransId)
{
    QList<int> res;
    return res;
}

void TQServiceProject::refreshModel(QAbstractItemModel *model)
{
}

void TQServiceProject::initQueryModel(int type)
{
    if(theQueryModel.contains(type))
        theQueryModel[type]->clearRecords();
    TQQueryListReply handler;
    if(sendRequest("QueryList",
                   QString("<QueryList rectype=\"%1\"/>").arg(type),
                   &handler))
    {
        foreach(const TQQueryListReply::QueryItem &item, handler.queries)
        {
            TrkToolQryModel *model;
            if(theQueryModel.contains(type))
                model = theQueryModel[type];
            else
            {
                model = new TrkToolQryModel(this);
                theQueryModel.insert(type, model);
            }
            model->appendQry(item.name, item.pub, item.rectype);
        }
    }
}

QAbstractItemModel *TQServiceProject::queryModel(int type)
{
    if(!theQueryModel.contains(type))
        initQueryModel(type);
    if(theQueryModel.contains(type))
        return theQueryModel[type];
    return 0;
}

TQAbstractRecordTypeDef *TQServiceProject::recordTypeDef(int rectype)
{
    if(recDefs.contains(rectype))
        return recDefs.value(rectype);
    TQRecordDefReplyHandler handler;
    if(sendRequest("RecordDef",QString("<RecordType recordType=\"%1\"/>").arg(rectype),&handler))
    {
        TQServiceRecordDef *def = handler.def;
        def->setParent(this);
        recDefs.insert(rectype, def);
        return def;
    }
    return 0;
}

TQRecord *TQServiceProject::createRecordById(int id, int rectype)
{
    TQRecord *rec = new TQRecord(this, rectype, id);
    return rec;
}

bool TQServiceProject::readRecordWhole(TQRecord *record)
{
    return false;
}

bool TQServiceProject::readRecordFields(TQRecord *record)
{
    return false;
}

bool TQServiceProject::readRecordTexts(TQRecord *record)
{
    return false;
}

bool TQServiceProject::readRecordBase(TQRecord *record)
{
    return false;
}

QVariant TQServiceProject::getFieldValue(const TQRecord *record, const QString &fname, bool *ok)
{
    return QVariant();
}

QVariant TQServiceProject::getFieldValue(const TQRecord *record, int vid, bool *ok)
{
    return QVariant();
}

bool TQServiceProject::setFieldValue(TQRecord *record, const QString &fname, const QVariant &value)
{
    return false;
}

bool TQServiceProject::updateRecordBegin(TQRecord *record)
{
    return false;
}

bool TQServiceProject::commitRecord(TQRecord *record)
{
    return false;
}

bool TQServiceProject::cancelRecord(TQRecord *record)
{
    return false;
}

QList<TQToolFile> TQServiceProject::attachedFiles(TQRecord *record)
{
    return QList<TQToolFile>();
}

QStringList TQServiceProject::historyList(TQRecord *record)
{
    return QStringList();
}

QHash<int, QString> TQServiceProject::baseRecordFields(int rectype)
{
    return QHash<int, QString>();
}


bool TQServiceProject::isSystemModel(QAbstractItemModel *model) const
{
    return false;
}


TQRecord *TQServiceProject::recordOfIndex(const QModelIndex &index)
{
    return 0;
}
