#include "jiradb.h"
#include "qt-json/json.h"
#include <QtNetwork>
#include <tqmodels.h>
#include "jiraoptions.h"
#include <tqplugui.h>
#include <tqoauth.h>
#include "webform.h"
#include <tqjson.h>
#include "jiralogin.h"

Q_EXPORT_PLUGIN2("jira", JiraPlugin)

static JiraPlugin *jira = 0;

static QWidget *getJiraOptionsWidget(const QString &path, GetOptionsWidgetFunc func)
{
    return new JiraOptions();
}

JiraPlugin::JiraPlugin(QObject *parent)
    : QObject(parent)
{
    jira = this;
//    servers << "http://rt.allrecall.com:8081/";
}



void JiraPlugin::initPlugin(QObject *obj, const QString &modulePath)
{
    globalObject = obj;
    mainWindow = 0;
    QMetaObject::invokeMethod(globalObject, "mainWindow", Qt::DirectConnection,
                                  Q_RETURN_ARG(QMainWindow *, mainWindow));
    QFileInfo fi(modulePath);
    pluginModule = fi.absoluteFilePath();
    QDir pDir;
    pDir = pluginModule; //fi.absoluteDir();
    if(pDir.dirName().compare("debug",Qt::CaseInsensitive) == 0)
    {
        pDir = QDir(pDir.filePath(".."));
        pDir.makeAbsolute();
    }
    pluginDir = pDir;
    dataDir = QDir(pDir.filePath("data"));
//    initProjectModel();
    if(obj)
    {
        QSettings *s;
        if(QMetaObject::invokeMethod(globalObject,"settings",
                                     Q_RETURN_ARG(QSettings *, s)))
            settings = s;
    }
    if(!settings)
        settings = new QSettings("AllRecall","JiraPlugin",this);
    loadSettings();
    TQAbstractDB::registerDbClass("Jira",JiraDB::createJiraDB);
    QMetaObject::invokeMethod(globalObject, "registerOptionsWidget", Qt::DirectConnection,
                              Q_ARG(const QString &,"Plugins/Jira"),
                              Q_ARG(void *, getJiraOptionsWidget));
}

bool JiraPlugin::saveSettings()
{
    settings->beginGroup("JiraPlugin");
    settings->beginGroup("Servers");
    settings->remove("");
    for(int i=0; i<servers.size(); i++)
        settings->setValue(QString("Server%1").arg(i), servers.value(i));
    settings->endGroup();
    settings->setValue("KeyFile", keyFile);
    settings->setValue("KeyPass", keyPass);
    settings->endGroup();
    return true;
}

bool JiraPlugin::loadSettings()
{
    settings->beginGroup("JiraPlugin");
    settings->beginGroup("Servers");
    servers.clear();
    foreach(QString key, settings->allKeys())
    {
        QString s = settings->value(key).toString();
        if(!s.isEmpty())
            servers.append(s);
    }
    settings->endGroup();
    keyFile = settings->value("KeyFile").toString();
    keyPass = settings->value("KeyPass").toString();
    settings->endGroup();
    return true;
}

JiraPlugin *JiraPlugin::plugin()
{
    return jira;
}


class JiraDBPrivate
{
public:
    QString token;
    QString token_secret;
    QString callback_confirm;
    QString access_token;
};

// ======================== JiraDB ==================================
JiraDB::JiraDB(QObject *parent)
    : TQAbstractDB(parent), webForm(0), oa(new TQOAuth(this)), parser(new TQJson(this)),
      d(new JiraDBPrivate()),
      timeOutSecs(10)
{
    parser->setCharset("UTF-8");
    man = new QNetworkAccessManager(this);
    connect(man, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));

    /*QObject *obj;

    if(QMetaObject::invokeMethod(jira->globalObject,"oauth",
                                 Q_RETURN_ARG(QObject *, obj)))
        oa = qobject_cast<TQOAuth*>(obj);
        */
    oa->loadPrivateKey(jira->keyFile, jira->keyPass);
}

JiraDB::~JiraDB()
{
    delete d;
}

QStringList JiraDB::dbmsTypes()
{
    return jira->servers;
//    return QStringList("http://rt.allrecall.com:8081/");
}

QStringList JiraDB::projects(const QString &dbmsType, const QString &user, const QString &pass)
{
    JiraPrjInfoList list = getProjectList(dbmsType);
    QStringList res;
    foreach(const JiraProjectInfo &info, list)
    {
        res.append(info.name);
    }
    return res;
}

#define JIRA_REQTOKEN_PATH "/plugins/servlet/oauth/request-token"
#define JIRA_AUTHORIZE_PATH  "/plugins/servlet/oauth/authorize"
#define JIRA_ACCTOKEN_PATH "/plugins/servlet/oauth/access-token"

TQAbstractProject *JiraDB::openProject(const QString &projectName, const QString &user, const QString &pass)
{
    QString baseUrl = this->dbmsServer();
    if(baseUrl.isEmpty())
        return 0;
    if(connectMethod == OAuth)
    {
        if(!oauthLogin())
            return 0;
    }
    JiraPrjInfoList prjList = getProjectList(baseUrl);
    if(prjList.isEmpty())
        return 0;

    JiraProject *project = new JiraProject(this);
    project->dbmsServer = baseUrl;
    QString pname = projectName;
    int pid = 10000;
    if(pname.isEmpty())
    {
        if(prjList.size())
        {
            JiraProjectInfo &info = prjList.first();
            pname = info.name;
            pid = info.id;
        }
    }
    foreach(const JiraProjectInfo &info, prjList)
    {
        if(pname == info.name)
            pid = info.id;
    }
    project->name = pname;
    project->projectId = pid; // !!!
    project->loadDefinition();
    project->opened = true;
    return project;
}

bool JiraDB::oauthLogin()
{
    QString baseUrl = this->dbmsServer();
    if(baseUrl.isEmpty())
        return false;
    if(!oa)
        return false;
    QString callbackUrl = "http://oauth/" ; //+ QUuid::createUuid().toString();
    QMap< QString, QString> result;
    result = oa->getRequestToken("POST",
                                 baseUrl + JIRA_REQTOKEN_PATH, // "http://rt.allrecall.com:8081/plugins/servlet/oauth/request-token",
                                 callbackUrl);
    d->token = result.value("oauth_token");
    d->token_secret = result.value("oauth_token_secret");
    d->callback_confirm = result.value("oauth_callback_confirmed");

    if(d->token.isEmpty())
        return 0;
    QScopedPointer<WebForm> web(new WebForm());
    if(web->request(QString("%1?oauth_token=%2").arg(baseUrl + JIRA_AUTHORIZE_PATH, d->token), QRegExp(callbackUrl)))
    {
        QUrl url = web->foundUrl();
        oa->setFoundUrl(url);
        d->access_token = oa->getAccessToken("POST",
                                                  baseUrl + JIRA_ACCTOKEN_PATH,
                                                  d->token);
        return true;
    }
    return false;
}

TQAbstractProject *JiraDB::openConnection(const QString &connectString)
{
    QVariantMap params = parser->toVariant(connectString).toMap();

    /*QStringList values = connectString.split(";");
    QHash<QString, QString> params;
    foreach(QString v, values)
    {
        int p = v.indexOf("=");

        QString par = v.left(p);
        QString val = v.mid(p+1);
        params.insert(par,val);
    }*/

    /*QString sRecType = params.value("RecordType").toString();
    bool okRecType;
    int recType = sRecType.toInt(&okRecType);*/

    int recType = params.value(PRJPARAM_RECORDTYPE).toInt();

    setConnectString(connectString);
    TQAbstractProject *prj = openProject(params.value(PRJPARAM_NAME).toString(),
                                         params.value(DBPARAM_USER).toString(),
                                         params.value(DBPARAM_PASSWORD).toString());
    return prj;
}

void JiraDB::setConnectString(const QString &connectString)
{
    TQAbstractDB::setConnectString(connectString);
    QVariantMap params = parser->toVariant(connectString).toMap();
    int method = params.value("ConnectMethod").toInt();
    setConnectMethod((JiraConnectMethod)method);
}

QVariant JiraDB::sendRequest(const QString &dbmsServer, const QString &method, const QString &query, const QString &body)
{
    QString link(dbmsServer + "rest/api/2/" + query);
    QUrl url(link);

    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *r;
    if(connectMethod == OAuth)
        r = oa->signedGet(&req);
    else
    {
        QByteArray v = QByteArray("Basic ") + QString(dbmsUser() + ":" + dbmsPass()).toLocal8Bit().toBase64();
        req.setRawHeader("Authorization", v);
        r = sendWait(method, req, body.toLocal8Bit());
    }
    QScopedPointer<QNetworkReply> reply(r);
    /*
    QDateTime endTime = QDateTime::currentDateTime().addSecs(10);
    while(!readyReplies.contains(reply.data()))
    {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(QDateTime::currentDateTime() > endTime)
            return QVariant();
    }
    readyReplies.removeAll(reply.data());
    */
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->error()
                 << reply->errorString();
        qDebug() << reply->readAll();
        return QVariant();
    }

    QByteArray buf = reply->readAll();
    qDebug() << buf;
//    QString s(buf.constData());
//    qDebug(text.constData());
//    QBuffer buf(&text);
//    buf.open(QIODevice::ReadOnly);

    bool success;
//    QVariant  obj = QtJson::parse(s, success);
    QVariant obj = parser->toVariant(buf);
    return obj;
}

static int method2op(const QString &method)
{
    if(method == "GET")
        return QNetworkAccessManager::GetOperation;
    if(method == "POST")
        return QNetworkAccessManager::PostOperation;
    if(method == "HEAD")
        return QNetworkAccessManager::HeadOperation;
    else if(method == "DELETE")
        return QNetworkAccessManager::DeleteOperation;
    else if(method == "PUT")
        return QNetworkAccessManager::PutOperation;
    return -1;
}


QNetworkReply *JiraDB::sendWait(const QString &method, QNetworkRequest &request, const QByteArray &body)
{
    int op = method2op(method);
    return sendWait((QNetworkAccessManager::Operation)op, request, body);
}

QNetworkReply *JiraDB::sendWait(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &body)
{
    QNetworkReply *reply;
    switch(op)
    {
    case -1:
        return 0;
    case QNetworkAccessManager::HeadOperation:
        reply = man->head(request);
        break;
    case QNetworkAccessManager::GetOperation:
        reply = man->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = man->put(request, body);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = man->post(request, body);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = man->deleteResource(request);
        break;
    case QNetworkAccessManager::CustomOperation:
        return 0;
    }
    waitReply(reply);
    return reply;
}

bool JiraDB::waitReply(QNetworkReply *reply)
{
    if(!reply)
        return false;
    QDateTime endTime = QDateTime::currentDateTime().addSecs(timeOutSecs);
    while(!readyReplies.contains(reply))
    {
        if(QDateTime::currentDateTime() > endTime)
            return false;
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    readyReplies.removeAll(reply);
    return true;
}



QVariant JiraDB::sendSimpleRequest(const QString &dbmsType, const QString &method, const QString &query, const QString &body)
{
    QString link(dbmsType + "rest/api/2/" + query);
    QUrl url(link);

    QNetworkAccessManager man;
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *r = oa->sendWait(method, req);
    QScopedPointer<QNetworkReply> reply(r);
    /*
    QDateTime endTime = QDateTime::currentDateTime().addSecs(10);
    while(!readyReplies.contains(reply.data()))
    {
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(QDateTime::currentDateTime() > endTime)
            return QVariant();
    }
    readyReplies.removeAll(reply.data());
    */
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Error:" << reply->error()
                 << reply->errorString();
        qDebug() << reply->readAll();
        return QVariant();
    }

    QByteArray buf = reply->readAll();

    qDebug() << buf;
    QString s(buf.constData());
//    qDebug(text.constData());
//    QBuffer buf(&text);
//    buf.open(QIODevice::ReadOnly);

    bool success;
//    QVariant  obj = QtJson::parse(s, success);
    QVariant obj = parser->toVariant(buf);
    return obj;
}

QDialog *JiraDB::createConnectDialog() const
{
    return new JiraLogin();
}

QVariant JiraDB::parseValue(const QVariant &source, const QString &path)
{
    QStringList hier = path.split(",");
    QVariant cur = source;
    for(int i = 0; i<hier.size(); i++)
    {
        QVariantHash hash;
        QVariantMap map;
        QVariantList list;
        int index;
        bool ok;
        QString name = hier[i];
        switch(cur.type())
        {
        case QVariant::Hash:
            hash = cur.toHash();
            cur = hash.value(name);
            break;
        case QVariant::Map:
            map = cur.toMap();
            cur = map.value(name);
            break;
        case QVariant::List:
            index = name.toInt(&ok);
            if(!ok)
                return QVariant();
            list = cur.toList();
            cur = list.value(index);
            break;
        default:
            return QVariant();
        }
    }
    return cur;
}

TQAbstractDB *JiraDB::createJiraDB(QObject *parent)
{
    return new JiraDB(parent);
}

JiraPrjInfoList JiraDB::getProjectList(const QString& serverUrl)
{
    QString server = serverUrl;
    if(server.isEmpty())
        server = dbmsServer();

    projectInfo.clear();
    if(!projectInfo.contains(server))
    {
        JiraPrjInfoList list;
        QVariantList reply = sendRequest(server, "GET","project").toList();
        foreach(QVariant v, reply)
        {
            JiraProjectInfo info;
            QVariantMap map = v.toMap();
            info.self = map.value("self").toString();
            info.id = map.value("id").toInt();
            info.key = map.value("key").toString();
            info.name = map.value("name").toString();
            list.append(info);
        }
        projectInfo.insert(server, list);
    }
    return projectInfo.value(server);
}

void JiraDB::replyFinished(QNetworkReply *reply)
{
    readyReplies.append(reply);
}

void JiraDB::callbackClicked()
{
    if(!webForm)
        return;
    webForm->close();
    webForm->deleteLater();
    webForm = 0;
}

// ======================== JiraProject ==================================
JiraProject::JiraProject(TQAbstractDB *db)
    : TQBaseProject(db)
{
    this->db = qobject_cast<JiraDB *>(db);
}

void JiraProject::loadDefinition()
{
    loadRecordTypes();
    loadQueries();
}

TQAbstractRecordTypeDef *JiraProject::recordTypeDef(int recordType)
{    
    return recordDefs.value(recordType, 0);
}

int JiraProject::defaultRecType() const
{
    if(recordDefs.isEmpty())
        return -1;
    return recordDefs.keys().first();
}

TQRecModel *JiraProject::openQueryModel(const QString &queryName, int recType, bool emitEvent)
{
    if(!favSearch.contains(queryName))
        return 0;
    QString jql = favSearch.value(queryName);
    QVariantMap map = db->sendRequest(dbmsServer,"GET",QString("search&jql=%1").arg(jql)).toMap();
    QVariantList issueList = map.value("issues").toList();
    TQRecModel *model = new TQRecModel(this, recType, this);
    foreach(QVariant i, issueList)
    {
        QVariantMap issue = i.toMap();
        JiraRecord *rec = new JiraRecord(this, recType, issue.value("id").toInt());
        model->append(rec);
    }
    return model;
}

QAbstractItemModel *JiraProject::openIdsModel(const IntList &ids, int recType, bool emitEvent)
{
    TQAbstractRecordTypeDef *rdef = recordTypeDef(recType);
    if(!rdef)
        return 0;
    QStringList list;
    foreach(int i, ids)
        list.append(QString::number(i));
    QString jql = QString("id in (%1)").arg(list.join(","));
    QVariantMap map = db->sendRequest(dbmsServer,"GET",QString("search?jql=%1").arg(jql)).toMap();
    QVariantList issueList = map.value("issues").toList();
    TQRecModel *model = new TQRecModel(this, recType, this);
    model->setHeaders(rdef->fieldNames());
    QList<TQRecord*> records;
    foreach(QVariant i, issueList)
    {
        QVariantMap issue = i.toMap();
        JiraRecord *rec = new JiraRecord(this, recType, issue.value("id").toInt());
        readRecordFields(rec);
        records.append(rec);
    }
    model->append(records);
    return model;
}

void JiraProject::refreshModel(QAbstractItemModel *model)
{
}

TQRecord *JiraProject::recordOfIndex(const QModelIndex &index)
{
    if(!index.isValid())
        return 0;
    const TQRecModel *model = qobject_cast<const TQRecModel*>(index.model());
    if(!model)
        return 0;
    return model->at(index.row());
}

QList<int> JiraProject::getQueryIds(const QString &name, int type, qint64 afterTransId)
{
    return QList<int>();
}

bool JiraProject::readRecordWhole(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    JiraRecTypeDef *rdef = dynamic_cast<JiraRecTypeDef *>(record->recordDef());
    if(!rdef)
        return false;
    QVariantMap issue = db->sendRequest(dbmsServer,"GET",QString("issue/%1").arg(rec->recordId())).toMap();
    rec->key = issue.value("key").toString();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    rec->values.clear();
    rec->displayValues.clear();
    for(i = fields.begin(); i!=fields.end(); i++)
    {
        QString fid = i.key();
        QVariant value = i.value();
        int vid = rdef->fieldVidSystem(fid);
        if(fid == "description")
            rec->desc = value.toString();
        else
        {
            rec->values.insert(vid, value);
            rec->displayValues.insert(vid, value);
        }
    }
    QVariantList comments = fields.value("comment").toMap().value("comments").toList();
    foreach(QVariant c, comments)
    {
        QVariantMap com = c.toMap();
        TQNote note;
        note.author = com.value("author").toMap().value("name").toString();
        note.text = com.value("body").toString();
        note.title = "Comment";
        note.crdate = QDateTime::fromString(com.value("created").toString(), Qt::ISODate);
        note.mddate = QDateTime::fromString(com.value("updated").toString(), Qt::ISODate);
        rec->notesCol.append(note);
    }

    return true;
}

bool JiraProject::readRecordFields(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    JiraRecTypeDef *rdef = dynamic_cast<JiraRecTypeDef *>(record->recordDef());
    if(!rdef)
        return false;
    QVariantMap issue = db->sendRequest(dbmsServer,"GET",QString("issue/%1?fields=*all,-description,-comment").arg(rec->recordId())).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    for(i = fields.begin(); i!=fields.end(); i++)
    {
        QString fid = i.key();
        QVariant value = i.value();
        int vid = rdef->fieldVidSystem(fid);
        if(vid != TQ::TQ_NO_VID)
        {
            rec->values.insert(vid, value);
            rec->displayValues.insert(vid,value);
        }
    }
}

bool JiraProject::readRecordTexts(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    QVariantMap issue = db->sendRequest(dbmsServer,"GET",QString("issue/%1?fields=description,comment").arg(rec->recordId())).toMap();
    rec->desc = db->parseValue(issue,"fields/description").toString();
    QVariantList comments = db->parseValue(issue,"comment/comments").toList();
    rec->notesCol.clear();
    foreach(QVariant c, comments)
    {
        QVariantMap com = c.toMap();
        TQNote note;
        note.author = com.value("author").toMap().value("name").toString();
        note.text = com.value("body").toString();
        note.title = "Comment";
        note.crdate = QDateTime::fromString(com.value("created").toString(), Qt::ISODate);
        note.mddate = QDateTime::fromString(com.value("updated").toString(), Qt::ISODate);
        rec->notesCol.append(note);
    }

    return true;
}

bool JiraProject::readRecordBase(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    JiraRecTypeDef *rdef = dynamic_cast<JiraRecTypeDef *>(record->recordDef());
    if(!rdef)
        return false;
    QHash<int, QString> fieldList = baseRecordFields(record->recordType());
    QStringList list = fieldList.values();
    QVariantMap issue = db->sendRequest(dbmsServer,"GET",QString("issue/%1?fields=%2").arg(rec->recordId()).arg(list.join(","))).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    for(i = fields.begin(); i!=fields.end(); i++)
    {
        QString fid = i.key();
        QVariant value = i.value();
        int vid = rdef->fieldVidSystem(fid);
        if(fid == "description")
            rec->desc = value.toString();
        else
        {
            rec->values.insert(vid, value);
            rec->displayValues.insert(vid, value);
        }
    }
}

QVariant JiraProject::getFieldValue(const TQRecord *record, const QString &fname, bool *ok)
{
    TQAbstractRecordTypeDef *rdef = record->recordDef();
    int vid = rdef->fieldVid(name);
    return getFieldValue(record,vid,ok);
}

QVariant JiraProject::getFieldValue(const TQRecord *record, int vid, bool *ok)
{
    const JiraRecord *rec = qobject_cast<const JiraRecord *>(record);
    if(!rec)
        return QVariant();
    return rec->value(vid, Qt::EditRole);
}

bool JiraProject::setFieldValue(TQRecord *record, const QString &fname, const QVariant &value)
{
    return false; // !!!
}

bool JiraProject::updateRecordBegin(TQRecord *record)
{
    return false;
}

bool JiraProject::commitRecord(TQRecord *record)
{
    return false;
}

bool JiraProject::cancelRecord(TQRecord *record)
{
    return false;
}

QStringList JiraProject::historyList(TQRecord *record)
{
    return QStringList();
}

QHash<int, QString> JiraProject::baseRecordFields(int rectype)
{
    TQAbstractRecordTypeDef *rdef = recordTypeDef(rectype);
    QHash<int, QString> res;
    QList<int> roles;
    roles << TQAbstractRecordTypeDef::IdField
          << TQAbstractRecordTypeDef::IdFriendlyField
          << TQAbstractRecordTypeDef::TitleField
//                <<  TQAbstractRecordTypeDef::DescriptionField
          << TQAbstractRecordTypeDef::StateField
          << TQAbstractRecordTypeDef::SubmitDateTimeField
          << TQAbstractRecordTypeDef::SubmitterField
          << TQAbstractRecordTypeDef::OwnerField;
    foreach(int role, roles)
    {
        int vid = rdef->roleVid(role);
        if(vid != TQ::TQ_NO_VID)
        {
            QString fName = rdef->fieldName(vid);
            res[vid] = fName;
        }
    }
    return res;
}

bool JiraProject::isSystemModel(QAbstractItemModel *model) const
{
    return false;
}

QSettings *JiraProject::projectSettings() const
{
    QString iniFile = jira->dataDir.absoluteFilePath("jira.ini");
    QSettings *sets = new QSettings(iniFile, QSettings::IniFormat);
    return sets;
}

/*TQAbstractRecordTypeDef *JiraProject::loadRecordTypeDef(int recordType)
{
    JiraRecTypeDef *rdef = new JiraRecTypeDef(this);

    QVariant obj = db->sendRequest("GET", "field");
    QVariantList fieldList = obj.toList();
    obj = db->sendRequest(dbmsType,"GET", QString("issue/createmeta?projectIds=%1&issuetypeIds=%2").arg(projectId).arg(recordType));
    QVariantMap createFields = db->parseVariant(obj, "projects/0/issuetypes/fields").toMap();
    int vid = 0;
    int nativeType = 1;
    foreach(QVariant v, fieldList)
    {
        QVariantMap map = v.toMap();
        JireFieldDesc f;
        f.id = map.value("id",0).toString();
        f.name = map.value("name").toString();
        f.custom = map.value("custom", false).toBool();
        f.orderable = map.value("orderable", false).toBool();
        f.navigable = map.value("navigable", false).toBool();
        f.searchable = map.value("searchable", false).toBool();
        f.clauseNames = map.value("clauseNames",QStringList()).toList();
        QVariantMap s = map.value("schema", QVariantMap()).toMap();
        f.schemaType = s.value("type").toString();
        f.schemaItems = s.value("items").toString();
        f.schemaSystem = s.value("system").toString();
        f.vid = ++vid;
        if(!rdef->schemaTypes.contains(f.schemaType))
        {
            rdef->schemaTypes.append(f.schemaType);
            rdef->nativeTypes.insert(f.schemaType, nativeType++)
        }
        f.nativeType = rdef->schemaTypes.indexOf(f.schemaType);
        f.simpleType = rdef->schemaToSimpleType(f.schemaType);
        if(createFields.contains(f.name))
        {
            f.createShow = true;
            QVariantMap map = createFields.value(f.name).toMap();
            f.createRequired = map.value("required","false").toString() == "true";
        }
        else
        {
            f.createShow = false;
            f.createRequired = false;
        }
        rdef->fields.insert(f.vid,f);
        rdef->vids.insert(f.name, f.vid);
    }
    return rdef;
}
*/

void JiraProject::loadRecordTypes()
{
    QVariant obj = db->sendRequest(dbmsServer,"GET", "field");
    QVariantList fieldList = obj.toList();
    QVariantList types = db->sendRequest(dbmsServer,"GET","issuetype").toList();
    foreach(const QVariant &t, types)
    {
        QVariantMap typeMap = t.toMap();
        int recordType = typeMap.value("id",0).toInt();
        if(!recordType)
            continue;

        JiraRecTypeDef *rdef = new JiraRecTypeDef(this);
        rdef->recType = recordType;
        rdef->recTypeName = typeMap.value("name").toString();

        obj = db->sendRequest(dbmsServer,"GET", QString("issue/createmeta?projectIds=%1&issuetypeIds=%2").arg(projectId).arg(recordType));
        QVariantMap createFields = db->parseValue(obj, "projects/0/issuetypes/fields").toMap();
        int vid = 1;
        int nativeType = 1;
        foreach(QVariant v, fieldList)
        {
            QVariantMap map = v.toMap();
            JiraFieldDesc f;
            f.id = map.value("id",0).toString();
            f.name = map.value("name").toString();
            f.custom = map.value("custom", false).toBool();
            f.orderable = map.value("orderable", false).toBool();
            f.navigable = map.value("navigable", false).toBool();
            f.searchable = map.value("searchable", false).toBool();
            f.clauseNames = map.value("clauseNames",QStringList()).toStringList();
            QVariantMap s = map.value("schema", QVariantMap()).toMap();
            f.schemaType = s.value("type").toString();
            f.schemaItems = s.value("items").toString();
            f.schemaSystem = s.value("system").toString();
            f.vid = ++vid;
            if(f.id == "issuekey")
            {
                f.schemaType = "issuekey";
                rdef->idVid = f.vid;
            }
            else if(f.id == "description")
            {
                rdef->descVid = f.vid;
            }
            if(!rdef->schemaTypes.contains(f.schemaType))
            {
                rdef->schemaTypes.append(f.schemaType);
                rdef->nativeTypes.insert(f.schemaType, nativeType++);
            }
            f.nativeType = rdef->schemaTypes.indexOf(f.schemaType); // not work for issuekey
            f.simpleType = rdef->schemaToSimpleType(f.schemaType);
            if(f.simpleType == TQ::TQ_FIELD_TYPE_USER)
                f.choiceTable = "Users";
            else if(f.simpleType == TQ::TQ_FIELD_TYPE_CHOICE)
                f.choiceTable = "Table_" + f.id;
            else
                f.choiceTable = QString();
            if(createFields.contains(f.id))
            {
                QVariantMap createMeta = createFields.value(f.id).toMap();
                f.createShow = true;
                f.createRequired = createMeta.value("required","false").toString() == "true";
                if(createMeta.contains("allowedValues"))
                {
                    QVariantList values = createMeta.value("allowedValues").toList();
                    int pos=0;
                    foreach(QVariant v, values)
                    {
                        QVariantMap vmap = v.toMap();
                        TQChoiceItem item;
                        item.displayText = vmap.value("name").toString();
                        item.fieldValue = vmap.value("id").toInt();
                        item.id = vmap.value("id").toInt();
                        item.weight = 0;
                        item.order = pos++;
                        f.choices.append(item);
                    }
                }
            }
            else
            {
                f.createShow = false;
                f.createRequired = false;
            }
            rdef->systemNames.insert(f.id, f.vid);
            rdef->fields.insert(f.vid,f);
            rdef->vids.insert(f.name, f.vid);
        }

        recordDefs.insert(recordType, rdef);
    }
}

void JiraProject::loadQueries()
{
    QVariantList favs = db->sendRequest(dbmsServer,"GET", "filter/favourite").toList();
    foreach(QVariant v, favs)
    {
        QVariantMap favMap = v.toMap();
        favSearch.insert(favMap.value("name").toString(), favMap.value("jql").toString());
    }
}


// ======================== JiraRecTypeDef ==================================
JiraRecTypeDef::JiraRecTypeDef(JiraProject *project)
    : TQAbstractRecordTypeDef(), prj(project)
{
    schemaToSimple.insert("array", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("date", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("datetime", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("issuetype", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("number", TQ::TQ_FIELD_TYPE_NUMBER);
    schemaToSimple.insert("priority", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("progress", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("project", TQ::TQ_FIELD_TYPE_STRING);
    schemaToSimple.insert("resolution", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("securitylevel", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("status", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("string", TQ::TQ_FIELD_TYPE_STRING);
    schemaToSimple.insert("timetracking", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("user", TQ::TQ_FIELD_TYPE_USER);
    schemaToSimple.insert("issuekey", TQ::TQ_FIELD_TYPE_NUMBER);

    roleFields.insert(TQAbstractRecordTypeDef::IdField,"issuekey");
    roleFields.insert(TQAbstractRecordTypeDef::TitleField, "summary");
    roleFields.insert(TQAbstractRecordTypeDef::DescriptionField, "description");
    roleFields.insert(TQAbstractRecordTypeDef::StateField, "status");
    roleFields.insert(TQAbstractRecordTypeDef::SubmitDateTimeField, "created");
    roleFields.insert(TQAbstractRecordTypeDef::SubmitterField, "creator");
    roleFields.insert(TQAbstractRecordTypeDef::OwnerField, "assignee");
    roleFields.insert(TQAbstractRecordTypeDef::IdFriendlyField,"key");
}

QStringList JiraRecTypeDef::fieldNames() const
{
    QStringList list;
    foreach(const JiraFieldDesc &desc, fields)
        list.append(desc.name);
    return list;
}

TQAbstractFieldType JiraRecTypeDef::getFieldType(int vid, bool *ok) const
{
    if(fields.contains(vid))
    {
        TQAbstractFieldType ftype(this, vid);
        if(ok)
            *ok = true;
        return ftype;
    }
    if(ok)
        *ok = false;
    return TQAbstractFieldType();
}

TQAbstractFieldType JiraRecTypeDef::getFieldType(const QString &name, bool *ok) const
{
    int vid = fieldVid(name);
    if(vid)
        return getFieldType(vid, ok);
    if(ok)
        *ok = false;
    return TQAbstractFieldType();
}

QString JiraRecTypeDef::fieldSchemaType(int vid) const
{
    if(!fields.contains(vid))
        return QString();
    return fields[vid].schemaType;
}

int JiraRecTypeDef::fieldNativeType(int vid) const
{
    if(!fields.contains(vid))
        return 0;
    return fields[vid].nativeType;
}

int JiraRecTypeDef::fieldSimpleType(int vid) const
{
    if(!fields.contains(vid))
        return TQ::TQ_FIELD_TYPE_NONE;
    return fields[vid].simpleType;
}

bool JiraRecTypeDef::canFieldSubmit(int vid) const
{
    if(!fields.contains(vid))
        return false;
    return fields[vid].createShow;
}

bool JiraRecTypeDef::canFieldUpdate(int vid) const
{
    return true;
}

bool JiraRecTypeDef::isNullable(int vid) const
{
    if(!fields.contains(vid))
        return false;
    const JiraFieldDesc &desc = fields[vid];
    return !desc.createRequired;
}

bool JiraRecTypeDef::hasChoiceList(int vid) const
{
    if(!fields.contains(vid))
        return false;
    const JiraFieldDesc &desc = fields[vid];
    if(desc.simpleType == TQ::TQ_FIELD_TYPE_USER
            || desc.simpleType == TQ::TQ_FIELD_TYPE_CHOICE)
        return true;
    return false;
}

TQChoiceList JiraRecTypeDef::choiceTable(const QString &tableName) const
{
    static const char *pref = "Table_";
    if(tableName == "users")
    {
        QMap<QString, TQUser> users = prj->userList();
        TQChoiceList res;
        int pos = 0;
        foreach(QString login, users.keys())
        {
            TQChoiceItem item;
            const TQUser &user = users[login];
            item.fieldValue = login;
            item.displayText = user.displayName;
            item.id = user.id;
            item.order = pos++;
            item.weight = 0;
            res.append(item);
        }
        return res;
    }
    else if(tableName.indexOf(pref)==0)
    {
        QString fName = tableName.mid(QString(pref).length());
        int vid = vids.value(fName, -1);
        if(vid<0)
            return TQChoiceList();
        const JiraFieldDesc &desc = fields.value(vid);
        return desc.choices;
    }
    return TQChoiceList();
}

bool JiraRecTypeDef::containFieldVid(int vid) const
{
    return fields.contains(vid);
}

int JiraRecTypeDef::fieldVid(const QString &name) const
{
    return vids.value(name, TQ::TQ_NO_VID);
}

int JiraRecTypeDef::fieldVidSystem(const QString &systemName) const
{
    return systemNames.value(systemName, TQ::TQ_NO_VID);
}

QList<int> JiraRecTypeDef::fieldVids() const
{
    QList<int> list;
    foreach(const JiraFieldDesc &desc, fields)
        list.append(desc.vid);
    return list;
//    return vids.values();
}

QString JiraRecTypeDef::fieldName(int vid) const
{
    if(!fields.contains(vid))
        return QString();
    const JiraFieldDesc &desc = fields.value(vid);
    return desc.name;
}

QIODevice *JiraRecTypeDef::defineSource() const
{
    return new QFile("data/jira.xml");
}

int JiraRecTypeDef::recordType() const
{
    return recType;
}

QString JiraRecTypeDef::valueToDisplay(int vid, const QVariant &value) const
{
    const JiraFieldDesc &desc = fields.value(vid);
    switch(desc.simpleType)
    {
    case TQ::TQ_FIELD_TYPE_NONE:
        return QString();
    case TQ::TQ_FIELD_TYPE_CHOICE:
    {
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return "";
        TQChoiceList list = choiceTable(table);
        foreach(const TQChoiceItem &c, list)
        {
            if(c.fieldValue == value)
                return c.displayText;
        }
        return "";
    }
    case TQ::TQ_FIELD_TYPE_STRING:
    case TQ::TQ_FIELD_TYPE_NUMBER:
        return value.toString();
    case TQ::TQ_FIELD_TYPE_DATE:
        return value.toDateTime().toString(dateTimeFormat());
    case TQ::TQ_FIELD_TYPE_USER:
        return project()->userFullName(value.toString());

    }
    return QString();
}

QVariant JiraRecTypeDef::displayToValue(int vid, const QString &text) const
{
    QDateTime dt;
    int simple = fieldSimpleType(vid);
    switch(simple)
    {
    case TQ::TQ_FIELD_TYPE_DATE:
        dt = QDateTime::fromString(dateTimeFormat());
        return QVariant(dt);
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
    {
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return "";
        TQChoiceList list = choiceTable(table);
        foreach(const TQChoiceItem &c, list)
        {
            if(c.displayText.compare(text,Qt::CaseInsensitive) == 0)
                return c.fieldValue;
        }
        return QVariant(QVariant::String);
    }
    case TQ::TQ_FIELD_TYPE_STRING:
        return text;
    case TQ::TQ_FIELD_TYPE_NUMBER:
        bool ok;
        int v = text.toInt(&ok);
        if(!ok)
            return QVariant(QVariant::Int);
        return v;
    }
    return text;
}

QVariant JiraRecTypeDef::fieldDefaultValue(int vid) const
{
    const JiraFieldDesc &desc = fields.value(vid);
    return desc.defaultValue;
}

QVariant JiraRecTypeDef::fieldMinValue(int vid) const
{
    const JiraFieldDesc &desc = fields.value(vid);
    return desc.minValue;
}

QVariant JiraRecTypeDef::fieldMaxValue(int vid) const
{
    const JiraFieldDesc &desc = fields.value(vid);
    return desc.maxValue;
}

QString JiraRecTypeDef::fieldChoiceTable(int vid) const
{
    const JiraFieldDesc &desc = fields.value(vid);
    if(!desc.isValid() || !desc.isChoice())
        return QString();
    if(desc.isUser())
        return "Users";
    return "Table_" + desc.id;
}

int JiraRecTypeDef::roleVid(int role) const
{
    QString fSystem = roleFields.value(role);
    if(fSystem.isEmpty())
        return TQ::TQ_NO_VID;
    return systemNames.value(fSystem, TQ::TQ_NO_VID);
}

QString JiraRecTypeDef::dateTimeFormat() const
{
    QLocale locale = QLocale::system();
    return locale.dateFormat(QLocale::ShortFormat)+" "+locale.timeFormat(QLocale::LongFormat);
}

QStringList JiraRecTypeDef::noteTitleList() const
{
    return QStringList();
}

TQAbstractProject *JiraRecTypeDef::project() const
{
    return prj;
}

int JiraRecTypeDef::schemaToSimpleType(const QString &schemaType)
{
    return schemaToSimple.value(schemaType,TQ::TQ_FIELD_TYPE_NONE);
}

// ====================== Jira Record ===================
JiraRecord::JiraRecord()
    : TQRecord(), def(0)
{
}

JiraRecord::JiraRecord(TQAbstractProject *prj, int rtype, int id)
    : TQRecord(prj, rtype, id)
{
    if(prj)
        def = dynamic_cast<JiraRecTypeDef*>(prj->recordTypeDef(rtype));
}

JiraRecord::JiraRecord(const TQRecord &src)
    : TQRecord(src)
{
    if(project())
        def = dynamic_cast<JiraRecTypeDef*>(project()->recordTypeDef(recordType()));
}

QString JiraRecord::jiraKey()
{
    return key;
}

QVariant JiraRecord::value(int vid, int role) const
{
    if(def && vid == def->idVid)
        return recordId();
    if(def && vid == def->descVid)
        return desc;
    if(!values.contains(vid))
    {
//        project()->readRecordFields(this);
    }
    switch(role)
    {
    case Qt::DisplayRole:
        return displayValues.value(vid, QVariant());
    case Qt::EditRole:
        return values.value(vid, QVariant());
    }
    return QVariant();
}

TQNotesCol JiraRecord::notes() const
{
    return notesCol;
}


void JiraDB::setConnectMethod(JiraDB::JiraConnectMethod method)
{
    connectMethod = method;
}