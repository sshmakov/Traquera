#include "tqservicedb.h"
#include "QtNetwork"
#include <QtXml>

static QNetworkAccessManager man;

TQServiceDB::TQServiceDB(QObject *parent) :
    TrkToolDB(parent)
{
}

QStringList TQServiceDB::dbmsTypes()
{
    return QStringList("http://localhost:8080");
}

QStringList TQServiceDB::projects(const QString &dbmsType)
{
    QNetworkRequest req;
    req.setUrl(dbmsType);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    QString query = "<GetProjects/>";
    QNetworkReply *reply = man.post(req,query.toUtf8());
//    QDomDocument
    QXmlInputSource source(reply);
    QXmlSimpleReader reader;
    TQProjectListHandler handler;
    reader.setContentHandler(&handler);
    if(reader.parse(source))
        return handler.projects;
    return QStringList();
}

TrkToolProject *TQServiceDB::openProject(const QString &dbmsType, const QString &projectName, const QString &user, const QString &pass)
{
    QNetworkRequest req;
    req.setUrl(dbmsType);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    QString query = QString("<Login project='$1' user='$2' password='$3'/>").arg(projectName,user,pass);
    QNetworkReply *reply = man.post(req,query.toUtf8());
//    QDomDocument
    QXmlInputSource source(reply);
    QXmlSimpleReader reader;
    TQLoginReplyHandler handler;
    reader.setContentHandler(&handler);
    return 0;
//    if(reader.parse(source))
//        return handler.projects;
//    return QStringList();
}


TQProjectListHandler::TQProjectListHandler()
{
}

bool TQProjectListHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName == "project")
    {
        QString name = atts.value("name");
        if(!name.isEmpty())
            projects.append(name);
        return true;
    }
    return false;
}


TQLoginReplyHandler::TQLoginReplyHandler()
{
}

bool TQLoginReplyHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName == "LoginResult")
    {
        sessionID = atts.value("sessionID");
        projectID = atts.value("projectID");
        errorCode = atts.value("errorCode");
        errorDesc = atts.value("errorDesc");
        return true;
    }
    return false;
}
