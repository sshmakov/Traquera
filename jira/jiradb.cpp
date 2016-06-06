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
#include <tqcond.h>
#include "jiraqry.h"
#include "jiraquerydialog.h"
#include <ttglobal.h>
#include <tqdebug.h>
#include "jirafinduser.h"

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

    QCoreApplication *app = QCoreApplication::instance();
    QString locale = QLocale::system().name();
    QTranslator *translator = new QTranslator();
    for(int i=1; i<app->argc(); i++)
    {
        if(app->arguments().value(i).trimmed().compare("-lang") == 0)
        {
            if(++i<app->argc())
                locale=app->arguments().value(i);
            break;
        }
    }
    if(!translator->load(QString("jira.") + locale,pluginModule+"/lang"))
        tqDebug() << "Can't load jira translator";
    app->installTranslator(translator);
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

struct SavedSession
{
    QString dbServer;
    QString user;
    QString password;
    QList<QNetworkCookie> cookies;
    QByteArray jsessionId;
};

static QMultiHash<QString, SavedSession> logins; //by dbServer

class JiraDBPrivate
{
public:
    QString token;
    QString token_secret;
    QString callback_confirm;
    QString access_token;
    bool isLogged;
    SavedSession session;
    QList<QNetworkCookie> lastCookies;
    int lastHTTPCode;
    QString lastHTTPError;

    JiraDBPrivate() : isLogged(false), lastHTTPCode(0)
    {
    }
};

// ======================== JiraDB ==================================
JiraDB::JiraDB(QObject *parent)
    : TQAbstractDB(parent), webForm(0), oa(new TQOAuth(this)), parser(new TQJson(this)),
      d(new JiraDBPrivate()),
      timeOutSecs(10)
{
    parser->setCharset("UTF-8");
    man =
            //new QNetworkAccessManager(this);
            ttglobal()->networkManager();
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
    Q_UNUSED(dbmsType)
    if(!d->isLogged)
        if(!loginCookie(user, pass))
            return QStringList();
    /*
    if(!cookies.size())
    {
        if(!showLoginPage(dbmsServer(), user, pass))
            return QStringList();
    }
    */
    JiraPrjInfoList list = getProjectList();
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
#define JIRA_LOGIN_PATH  "/login.jsp"

TQAbstractProject *JiraDB::openProject(const QString &projectName, const QString &user, const QString &pass)
{
    QString baseUrl = this->dbmsServer();
    if(baseUrl.isEmpty())
        return 0;
    if(!d->isLogged)
    {
        if(connectMethod == OAuth)
        {
            if(!oauthLogin())
                return 0;
        }
        else if(connectMethod == CookieAuth)
        {
            if(!loginCookie(user, pass))
                return 0;
        }
    }
    JiraPrjInfoList prjList = getProjectList();
    if(prjList.isEmpty())
        return 0;

    JiraProject *project = new JiraProject(this);
//    project->dbmsServer = baseUrl;
    QString pname = projectName;
    int pid = 10000;
    QString key = "";
    if(pname.isEmpty())
    {
        if(prjList.size())
        {
            JiraProjectInfo &info = prjList.first();
            pname = info.name;
            pid = info.id;
            key = info.key;
        }
    }
    foreach(const JiraProjectInfo &info, prjList)
    {
        if(pname == info.name)
        {
            pid = info.id;
            key = info.key;
        }
    }
    project->name = pname;
    project->projectId = pid; // !!!
    project->projectKey = key;
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
        d->isLogged = true;
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

QVariant JiraDB::sendRequest(const QString &method, const QUrl &url, QVariantMap bodyMap)
{
//    QString link(dbmsServer() + url);
//    QUrl url(link);
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QNetworkReply *r;
    if(connectMethod == OAuth)
    {
        QByteArray body = parser->toByteArray(bodyMap);
        dumpRequest(&req, method, body);
        r = oa->signedGet(&req);
    }
    else if(connectMethod == CookieAuth)
    {
        if(false && d->isLogged)
        {
            QVariantMap headers = bodyMap.value("headers", QVariantMap()).toMap();
            headers.insert("cookie", QString("JSESSIONID=") + d->session.jsessionId);
            bodyMap.insert("headers", headers);
        }
        QByteArray body = parser->toByteArray(bodyMap);
        QList<QNetworkCookie> list = man->cookieJar()->cookiesForUrl(dbmsServer());
        if(list.size())
            req.setHeader(QNetworkRequest::SetCookieHeader, QVariant::fromValue(list));
        dumpRequest(&req, method, body);
        r = sendWait(method, req, body);
    }
    else //if(connectMethod == BaseAuth)
    {
        QByteArray body = parser->toByteArray(bodyMap);
        QByteArray v = QByteArray("Basic ") + QString(dbmsUser() + ":" + dbmsPass()).toLocal8Bit().toBase64();
        req.setRawHeader("Authorization", v);
        r = sendWait(method, req, body);
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
    d->lastHTTPCode = reply->error();
    d->lastHTTPError = reply->errorString();
    QString contType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if(contType.contains(';'))
        contType = contType.split(';').value(0);
    contType = contType.trimmed();
    d->lastCookies = r->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie> >();    
    QByteArray buf = reply->readAll();

    dumpReply(r, buf);
//    QString s(buf.constData());
//    qDebug(text.constData());
//    QBuffer buf(&text);
//    buf.open(QIODevice::ReadOnly);

    bool success;
//    QVariant  obj = QtJson::parse(s, success);
    QVariant obj;
    if(!contType.compare("application/json",Qt::CaseInsensitive))
        obj = parser->toVariant(buf);
    return obj;
}

QUrl JiraDB::queryUrl(const QString &query) const
{
    return QUrl(dbmsServer() + query);
}

QNetworkReply *JiraDB::sendRequestNative(const QUrl &url, const QString &method, const QByteArray &body)
{
    QNetworkRequest req;
    req.setUrl(url);
    dumpRequest(&req, method, body);
    tqDebug() << method << url;
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply *r = 0;
    if(connectMethod == OAuth)
        r = oa->signedGet(&req);
    else if(connectMethod == CookieAuth)
    {
        QList<QNetworkCookie> list = man->cookieJar()->cookiesForUrl(dbmsServer());
        if(list.size())
            req.setHeader(QNetworkRequest::SetCookieHeader, QVariant::fromValue(list));
        r = sendWait(method, req, body);
    }
    else //if(connectMethod == BaseAuth)
    {
        QByteArray v = QByteArray("Basic ") + QString(dbmsUser() + ":" + dbmsPass()).toLocal8Bit().toBase64();
        req.setRawHeader("Authorization", v);
        r = sendWait(method, req, body);
    }
    dumpReply(r, QByteArray());
    return r;
}

int JiraDB::lastHTTPCode() const
{
    return d->lastHTTPCode;
}

QString JiraDB::lastHTTPErrorString() const
{
    return d->lastHTTPError;
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
    return sendWaitOp((QNetworkAccessManager::Operation)op, request, body);
}

QNetworkReply *JiraDB::sendWaitOp(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &body)
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

TQJson *JiraDB::jsonParser()
{
    return parser;
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
        tqDebug() << "Error:" << QString::number(reply->error())
                 << reply->errorString();
        tqDebug() << reply->readAll();
        return QVariant();
    }

    QByteArray buf = reply->readAll();

    tqDebug() << buf;
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
    QStringList hier = path.split("/");
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

JiraPrjInfoList JiraDB::getProjectList()
{
    QString server = dbmsServer();
    if(!d->isLogged)
    {
        if(!loginCookie(dbmsUser(), dbmsPass()))
            return JiraPrjInfoList();
    }
    projectInfo.clear();
    if(!projectInfo.contains(server))
    {
        JiraPrjInfoList list;
        QVariantList reply = sendRequest("GET",queryUrl("rest/api/2/project")).toList();
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

bool JiraDB::showLoginPage(const QString &server, const QString &user, const QString &pass)
{
    bool res = false;
    WebForm * web = new WebForm();
    if(web->request(server + "login.jsp",QRegExp("/secure/Dashboard.jspa")))
    {
        cookies = web->cookies(QUrl(server));
        res = true;
    }

    delete web;
    return res;
}

bool JiraDB::loginCookie(const QString &user, const QString &pass)
{
//    bool res = false;
    QVariantMap reqMap;
    reqMap.insert("username", user);
    reqMap.insert("password", pass);
    QVariantMap repMap = sendRequest("POST", queryUrl("rest/auth/1/session"), reqMap).toMap();
    /*
    "Server" "nginx/1.2.1"
    "Date" "Wed, 11 May 2016 18:52:53 GMT"
    "Content-Type" "application/json;charset=UTF-8"
    "Transfer-Encoding" "chunked"
    "Connection" "keep-alive"
    "X-AREQUESTID" "1372x889x1"
    "X-ASEN" "SEN-4595696"
    "X-AUSERNAME" "anonymous"
    "Set-Cookie" "atlassian.xsrf.token=BZ2I-WBNJ-LGWW-L4GH|9d4e3600bc9f12f3aac1cc7c37cd244b7149748f|lout; Path=/
    JSESSIONID=908C0596F20620CB47D0BB302911721B; Path=/; HttpOnly"
    "X-Seraph-LoginReason" "OK"
    "Content-Encoding" "gzip"
    "Vary" "User-Agent"
    "Cache-Control" "no-cache, no-store, no-transform"
    "X-Content-Type-Options" "nosniff"
    "{"session":{"name":"JSESSIONID","value":"908C0596F20620CB47D0BB302911721B"},"loginInfo":{"failedLoginCount":48,"loginCount":3654,"lastFailedLoginTime":"2016-05-11T15:00:28.257+0400","previousLoginTime":"2016-05-11T22:37:37.025+0400"}}"
     */
    if(!repMap.contains("session"))
        return false;
    QVariantMap session = repMap.value("session").toMap();
    QByteArray sessId = session.value("value").toByteArray();
    SavedSession item;
    item.dbServer = dbmsServer();
    item.user = user;
    item.password = pass;
    item.jsessionId = sessId;
    logins.insert(item.dbServer, item);
    d->isLogged = true;
    d->session = item;
    return true;
}

void JiraDB::dumpRequest(QNetworkRequest *req, const QString &method, const QByteArray &body)
{
    tqDebug() << "Request:";
    tqDebug() << method << req->url();
    foreach(const QByteArray &header, req->rawHeaderList())
    {
        QByteArray value = req->rawHeader(header);
        tqDebug() << header << value;
    }
    tqDebug() << body;
    tqDebug() << "---";
}

void JiraDB::dumpReply(QNetworkReply *reply, const QByteArray &body)
{
    if(!reply)
    {
        tqDebug() << "Response: 0";
        return;
    }
    tqDebug() << "Response:" << QString::number(reply->error());
    if(reply->error() != QNetworkReply::NoError)
    {
        tqDebug() << "Error:" << reply->errorString();
    }
    foreach(const QNetworkReply::RawHeaderPair &pair, reply->rawHeaderPairs())
    {
        tqDebug() << pair.first << pair.second;
    }
    //        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
    tqDebug() << body;
    tqDebug() << "---";
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
    : TQBaseProject(db), filters(new JiraFilterModel(this))
{
    this->db = qobject_cast<JiraDB *>(db);
}

void JiraProject::loadDefinition()
{
    loadRecordTypes();
    loadQueries();
    loadUsers();
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
    int index = favSearch.value(queryName);
    const JiraFilter &fitem = filters->at(index);
    QString jql = fitem.jql;
    QVariantMap map = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/search?jql=%1").arg(jql))).toMap();
    QVariantList issueList = map.value("issues").toList();
    TQRecModel *model = new TQRecModel(this, recType, this);
    int pos = projectKey.length()+1;
    foreach(QVariant i, issueList)
    {
        QVariantMap issue = i.toMap();
        QString key = issue.value("key").toString();
        int id = key.mid(pos).toInt();
        JiraRecord *rec = new JiraRecord(this, recType, id);
        rec->key = key;
        rec->internalId = issue.value("id").toInt();
        model->append(rec);
    }
    return model;
}

TQRecModel *JiraProject::openIdsModel(const IntList &ids, int recType, bool emitEvent)
{
    TQAbstractRecordTypeDef *rdef = recordTypeDef(recType);
    if(!rdef)
        return 0;
    QStringList list;
    foreach(int i, ids)
        list.append(projectKey + "-" + QString::number(i));
    QString jql = QString("Key in (%1)").arg(list.join(","));
    QVariantMap map = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/search?jql=%1&fields=key,id").arg(jql))).toMap();
    QVariantList issueList = map.value("issues").toList();
    TQRecModel *model = new TQRecModel(this, recType, this);
    model->setHeaders(rdef->fieldNames());
    QList<TQRecord*> records;
    int pos = projectKey.length()+1;
    foreach(QVariant i, issueList)
    {
        QVariantMap issue = i.toMap();
        QString key = issue.value("key").toString();
        int id = key.mid(pos).toInt();
        JiraRecord *rec = new JiraRecord(this, recType, id);
        rec->key = key;
        rec->internalId = issue.value("id").toInt();
        readRecordFields(rec);
        records.append(rec);
    }
    model->append(records);
    return model;
}

TQRecord *JiraProject::createRecordById(int id, int rectype)
{
    JiraRecord *rec = new JiraRecord(this, rectype, id);
    rec->key = projectKey + "-" + QString::number(id);
    rec->internalId = 0;
    connect(rec, SIGNAL(changed(int)), this, SIGNAL(recordChanged(int)));
    readRecordFields(rec);
    return rec;
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
    if(rec->mode() == TQRecord::Insert)
        return true;
    const JiraRecTypeDef *rdef = dynamic_cast<const JiraRecTypeDef *>(record->typeDef());
    if(!rdef)
        return false;
    QVariantMap issue = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/issue/%1?expand=attachment").arg(rec->jiraKey()))).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    rec->values.clear();
    rec->displayValues.clear();
    for(i = fields.begin(); i!=fields.end(); i++)
        storeReadedField(rec, rdef, i.key(), i.value());
    rec->isFieldsReaded = true;
    QVariantList comments = fields.value("comment").toMap().value("comments").toList();
    rec->notesCol.clear();
    foreach(QVariant c, comments)
    {
        QVariantMap com = c.toMap();
        TQNote note;
        QVariantMap author = com.value("author").toMap();
        note.author = author.value("name").toString();
        note.authorFullName = author.value("displayName").toString();
        note.text = com.value("body").toString();
        note.title = "Comment";
        note.crdate = QDateTime::fromString(com.value("created").toString(), Qt::ISODate);
        note.mddate = QDateTime::fromString(com.value("updated").toString(), Qt::ISODate);
        rec->notesCol.append(note);
    }
    rec->isTextsReaded = true;

    rec->files.clear();
    if(fields.contains("attachment"))
    {
        QVariantList list = fields.value("attachment").toList();
        foreach(QVariant f, list)
        {
            QVariantMap map = f.toMap();
            TQAttachedFile file;
            file.fileName = map.value("filename").toString();
            file.id = map.value("id").toInt();
            file.size = map.value("size").toInt();
            file.data = map.value("content");
            file.createDateTime = map.value("created").toDateTime();
            rec->files.append(file);
        }
    }
    emit rec->changed(rec->recordId());
    return true;
}

bool JiraProject::readRecordFields(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    if(rec->mode() == TQRecord::Insert)
        return true;
    const JiraRecTypeDef *rdef = dynamic_cast<const JiraRecTypeDef *>(record->typeDef());
    if(!rdef)
        return false;
    QVariantMap issue = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/issue/%1?fields=*all,-description,-comment").arg(rec->jiraKey()))).toMap();
    if(!issue.contains("id"))
        return false;
    rec->internalId = issue.value("id").toInt();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    for(i = fields.begin(); i!=fields.end(); i++)
        storeReadedField(rec, rdef, i.key(), i.value());
    rec->isFieldsReaded = true;
    emit rec->changed(rec->recordId());
    return true;
}

bool JiraProject::readRecordTexts(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    if(rec->mode() == TQRecord::Insert)
        return true;
    QVariantMap issue = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/issue/%1?fields=description,comment").arg(rec->jiraKey()))).toMap();
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
    rec->isTextsReaded = true;
    emit rec->changed(rec->recordId());
    return true;
}

bool JiraProject::readRecordBase(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    if(rec->mode() == TQRecord::Insert)
        return true;
    const JiraRecTypeDef *rdef = dynamic_cast<const JiraRecTypeDef *>(record->typeDef());
    if(!rdef)
        return false;
    QHash<int, QString> fieldList = baseRecordFields(record->recordType());
    QStringList list = fieldList.values();
    QVariantMap issue = db->sendRequest("GET",db->queryUrl(QString("rest/api/2/issue/%1?fields=%2").arg(rec->jiraKey()).arg(list.join(",")))).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    for(i = fields.begin(); i!=fields.end(); i++)
        storeReadedField(rec, rdef, i.key(), i.value());
    emit rec->changed(rec->recordId());
    return true;
}

QVariant JiraProject::getFieldValue(const TQRecord *record, const QString &fname, bool *ok)
{
    const TQAbstractRecordTypeDef *rdef = record->typeDef();
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
    int vid = record->typeDef()->fieldVid(fname);
    if(vid)
    {
        record->setValue(vid, value);
        return true;
    }
    return false; // !!!
}

bool JiraProject::updateRecordBegin(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    rec->setMode(TQRecord::Edit);
    return true;
}

bool JiraProject::commitRecord(TQRecord *record)
{
    if(record->mode() == TQRecord::View)
        return false;
    JiraRecord *jRec = qobject_cast<JiraRecord*>(record);
    if(!jRec)
        return false;
    const JiraRecTypeDef *recDef = dynamic_cast<const JiraRecTypeDef *>(jRec->typeDef());
    QVariantMap issue;
    QVariantMap fields;
    TQFieldValues vidChanges = jRec->vidChanges();
    TQFieldValues::const_iterator i;
    for(i = vidChanges.begin(); i != vidChanges.end(); ++i)
    {
        int vid = i.key();
        QVariant value = i.value();
        if(!recDef->containFieldVid(vid))
            continue;
        const JiraFieldDesc &fdesc = recDef->fields[vid];
        if(fdesc.isChoice())
        {
            QVariantMap v;
            v.insert("id", value.toString());
            value = v;
        }
        fields.insert(fdesc.id, value);
    }
    QVariantMap v2;
    v2.insert("key", projectKey);
    fields.insert("project", v2);
    v2.clear();
    v2.insert("name", jRec->def->recTypeName);
    fields.insert("issuetype", v2);
    v2.clear();
    fields.insert("summary",jRec->title());
    fields.insert("description",jRec->description());
    issue.insert("fields", fields);
    QVariantList comments;
    foreach(const TQNote &note, jRec->notes())
    {
        if(note.isAdded || note.isChanged)
        {
            QVariantMap com;
            com.insert("body", note.text);
        }
    }
    issue.insert("comments", comments);
    QVariantMap res;
    bool wasError;
    if(record->mode() == TQRecord::Edit)
    {
        res = db->sendRequest("PUT",db->queryUrl(QString("rest/api/2/issue/%1").arg(jRec->jiraKey())),issue).toMap();
        wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200;
    }
    else
    {
        res = db->sendRequest("POST",db->queryUrl(QString("rest/api/2/issue")),issue).toMap();
        wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200;// !res.contains("id");
        if(!wasError)
        {
            /*{
                "id": "10000",
                "key": "TST-24",
                "self": "http://www.example.com/jira/rest/api/2/issue/10000"
            }*/
            QString key = res.value("key").toString();
            bool ok = false;
            int id = key.mid(key.indexOf("-")+1).toInt(&ok);
            if(ok)
            {
                jRec->doSetRecordId(id);
                readRecordBase(jRec);
            }
        }
    }
    if(wasError)
    {
        QString error;
        QVariantList elist = res.value("errorMessages").toList();
        foreach(QVariant e, elist)
            error += e.toString() + "\n";
        QVariantMap objErrors = res.value("errors").toMap();
        QVariantMap::iterator i;
        for(i=objErrors.begin(); i!=objErrors.end(); ++i)
        {
            QString key = i.key();
            QString s = i.value().toString();
            error += tr("%1: %2\n").arg(key, s);
        }
        error = tr("Ошибка сохранения запроса\n") + error;
        error = error.trimmed();
        QMessageBox::critical(0, "Jira error", error);
        return false;
    }
    jRec->internalId = res.value("id").toInt();
    jRec->key = res.value("key").toString();
    jRec->setMode(TQRecord::View);
    jRec->refresh();
    return true;
}

bool JiraProject::cancelRecord(TQRecord *record)
{
    if(record->mode() == TQRecord::View)
        return false;
    record->setMode(TQRecord::View);
}

TQRecord *JiraProject::newRecord(int rectype)
{
    JiraRecord *rec = new JiraRecord(this, rectype, 0);
    rec->setMode(TQRecord::Insert);
    return rec;
}

QList<TQAttachedFile> JiraProject::attachedFiles(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return QList<TQAttachedFile>();
    return rec->files;
}

bool JiraProject::saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    if(fileIndex<0 || fileIndex >= rec->files.size())
        return false;
    const TQAttachedFile &f = rec->files.at(fileIndex);
    QNetworkReply *r = db->sendRequestNative(f.data.toString(), "GET");
    if(!r)
        return false;
    QFile destFile(dest);
    destFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
//    QDataStream out(destFile);
    while(!r->atEnd())
    {
        char buf[1024*16];
        qint64 readed = r->read(buf, sizeof(buf));
        if(!readed)
            break;
        destFile.write(buf, readed);
    }
    r->deleteLater();
    return true;
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
    sets->beginGroup(projectName());
    return sets;
}

QAbstractItemModel *JiraProject::queryModel(int type)
{
    if(recordTypes.contains(type))
        return filters;
    return 0;
}

TQQueryDef *JiraProject::queryDefinition(const QString &queryName, int rectype)
{
    int index = favSearch.value(queryName, -1);
    if(index<0)
        return 0;
    JiraQry *def = new JiraQry(this, rectype);
    def->setName(queryName);
    def->setQueryLine(filters->at(index).jql);
    return def;
}

TQAbstractQWController *JiraProject::queryWidgetController(int rectype)
{
    return new JiraQueryDialogController(this);
}

QVariant JiraProject::optionValue(const QString &option) const
{
    QSettings *sets = projectSettings();
    QVariant v = sets->value(option);
    delete sets;
    if(v.isValid())
        return v;

    if(option == TQOPTION_VIEW_TEMPLATE
            || option == TQOPTION_EDIT_TEMPLATE
            || option == TQOPTION_PRINT_TEMPLATE
            || option == TQOPTION_EMAIL_TEMPLATE
            )
        return jira->dataDir.absoluteFilePath("issue.xq");
    if(option == TQOPTION_GROUP_FIELDS)
        return jira->dataDir.absoluteFilePath("jira.xml");
    return ttglobal()->optionDefaultValue(option);
}

QString JiraProject::jiraProjectKey() const
{
    return projectKey;
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
    QVariant obj = db->sendRequest("GET", db->queryUrl("rest/api/2/field"));
    QVariantList fieldList = obj.toList();
    QVariantList types = db->sendRequest("GET",db->queryUrl("rest/api/2/issuetype")).toList();
    foreach(const QVariant &t, types)
    {
        QVariantMap typeMap = t.toMap();
        int recordType = typeMap.value("id",0).toInt();
        if(!recordType)
            continue;

        JiraRecTypeDef *rdef = new JiraRecTypeDef(this);
        rdef->recType = recordType;
        rdef->recTypeName = typeMap.value("name").toString();

        obj = db->sendRequest("GET", db->queryUrl(QString("rest/api/2/issue/createmeta?projectIds=%1&issuetypeIds=%2&expand=projects.issuetypes.fields").arg(projectId).arg(recordType)));
        QVariantMap createFields = db->parseValue(obj, "projects/0/issuetypes/0/fields").toMap();
        int vid = 1;
        int nativeType = 1;
        foreach(QVariant v, fieldList)
        {
            QVariantMap map = v.toMap();
            JiraFieldDesc f;
            f.id = map.value("id",0).toString();
            f.name = map.value("name").toString();
            f.custom = map.value("custom", false).toBool();
            if(f.custom)
                f.name = f.name + tr(" (cf)");
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
            {
                f.choiceTable = "Table_" + f.id;
                if(rdef->systemChoices.contains(f.id))
                    f.choices = loadChoiceTables(rdef, f.id);
            }
            else
                f.choiceTable = QString();
            if(createFields.contains(f.id))
            {
                QVariantMap createMeta = createFields.value(f.id).toMap();
                f.createShow = true;
                f.createRequired = createMeta.value("required","false").toString() == "true";
                f.autoCompleteUrl = createMeta.value("autoCompleteUrl").toString();
                if(createMeta.contains("allowedValues"))
                {
                    QVariantList values = createMeta.value("allowedValues").toList();
                    int pos=0;
                    foreach(QVariant v, values)
                    {
                        QVariantMap vmap = v.toMap();
                        TQChoiceItem item;
                        item.displayText = vmap.value("name").toString();
                        item.fieldValue = vmap.value("id");
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
            {
                int i = 2;
                QString name = f.name;
                while(rdef->vids.contains(f.name))
                    f.name = name + " " +QString::number(i++);
                rdef->vids.insert(f.name, f.vid);
                rdef->fields.insert(f.vid,f);
            }
        }
        recordDefs.insert(recordType, rdef);
        recordTypes.insert(rdef->recType, rdef->recTypeName);
    }
}

TQChoiceList JiraProject::loadChoiceTables(JiraRecTypeDef *rdef, const QString &url)
{
    QVariant obj = db->sendRequest("GET", db->queryUrl("rest/api/2/"+url));
    QVariantList statusList = obj.toList();
    TQChoiceList list;
    int order = 0;
    foreach(QVariant stateV, statusList)
    {
        QVariantMap state = stateV.toMap();
        TQChoiceItem item;
        item.displayText = state.value("name").toString();
        item.fieldValue = state.value("id").toInt();
        item.id = state.value("id").toInt();
        item.weight = item.id;
        item.order = order++;
        list.append(item);
    }
    return list;
}
typedef QPair<QString,QString> QStringPair;

void JiraProject::loadQueries()
{
    QVariantList favs = db->sendRequest("GET", db->queryUrl("rest/api/2/filter/favourite")).toList();
    foreach(QVariant v, favs)
    {
        QVariantMap favMap = v.toMap();
        JiraFilter item;
        item.name = favMap.value("name").toString();
        item.jql = favMap.value("jql").toString();
        item.isServerStored = true;
        item.isSystem = false;
        filters->append(item);
        favSearch.insert(item.name, filters->rowCount()-1);
    }
    QList<QStringPair > preDef;
    preDef
            << qMakePair(tr("Мои открытые запросы"),
                         QString("assignee = currentUser() AND resolution = Unresolved ORDER BY updatedDate DESC"))
            << qMakePair(tr("Созданные мной"),
                         QString("reporter = currentUser() ORDER BY createdDate DESC"))
//            << qMakePair(tr("Последние просмотренные"),
//                         QString("issuekey in issueHistory() ORDER BY lastViewed DESC"))
            << qMakePair(tr("Все запросы"),
                         QString("ORDER BY createdDate DESC"))
            << qMakePair(tr("Открытые запросы"),
                         QString("resolution = Unresolved order by priority DESC,updated DESC"))
            << qMakePair(tr("Добавленные недавно"),
                         QString("created >= -1w order by created DESC"))
            << qMakePair(tr("Решенные недавно"),
                         QString("resolutiondate >= -1w order by updated DESC"))
            << qMakePair(tr("Обновленные недавно"),
                         QString("updated >= -1w order by updated DESC"))
               ;

    foreach(const QStringPair &def, preDef)
    {
        JiraFilter item;
        item.name = def.first;
        item.jql = def.second;
        item.isServerStored = false;
        item.isSystem = true;
        filters->append(item);
        favSearch.insert(item.name, filters->rowCount()-1);
    }
}

void JiraProject::loadUsers()
{
    m_userList.clear();
    QVariantList owners = db->sendRequest("GET", db->queryUrl("rest/api/2/user/assignable/search?project="+projectKey)).toList();
    int i=1;
    foreach(QVariant v, owners)
    {
        QVariantMap userMap = v.toMap();
        TQUser user;
        user.login = userMap.value("key").toString();
        user.displayName = userMap.value("displayName",user.login).toString();
        user.isDeleted = userMap.value("active").toBool() == false;
        user.fullName = user.displayName;
        user.id = i++;
        m_userList.insert(user.login, user);
        appendUserToKnown(userMap);
    }
}

void JiraProject::storeReadedField(JiraRecord *rec, const JiraRecTypeDef *rdef, const QString &fid, const QVariant &value)
{
    int fvid = rdef->fieldVidSystem(fid);
    if(fid == "description")
        rec->desc = value.toString();
    else
    {
        if(rdef->systemChoices.contains(fid))
        {
            int id = value.toMap().value("id").toInt();
            QString display = value.toMap().value("name").toString();
            rec->values.insert(fvid, id);
            rec->displayValues.insert(fvid, display);
        }
        else
        {
            const JiraFieldDesc &fdef = rdef->fields[fvid];
            if(fdef.isUser())
            {
                QVariantMap uMap = value.toMap();
                QString login = uMap.value("name").toString();
                QString displayName = uMap.value("displayName").toString();
                rec->values.insert(fvid, login);
                rec->displayValues.insert(fvid, displayName);
                appendUserToKnown(uMap);
            }
            else if(value.type() == QVariant::Map)
            {
                int id = value.toMap().value("id").toInt();
                QString display = value.toMap().value("name").toString();
                rec->values.insert(fvid, id);
                rec->displayValues.insert(fvid, display);
            }
            else
            {
                rec->values.insert(fvid, value);
                QString displayValue = rdef->valueToDisplay(fvid,value);
                rec->displayValues.insert(fvid, displayValue);
            }
        }
    }
}

void JiraProject::appendUserToKnown(const QVariantMap &userRec)
{
    QString name = userRec.value("name").toString();
    if(knownUsers.contains(name))
        return;
    JiraUser item;
    item.key = userRec.value("key").toString();
    item.name = name;
    item.displayName = userRec.value("displayName").toString();
    item.email = userRec.value("emailAddress").toString();
    item.isActive = userRec.value("active").toBool();
    knownUsers.insert(name, item);
}

void JiraProject::showSelectUser()
{
}


// ======================== JiraRecTypeDef ==================================
JiraRecTypeDef::JiraRecTypeDef(JiraProject *project)
    : TQBaseRecordTypeDef(project), prj(project)
{
    schemaToSimple.insert("array", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("date", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("datetime", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("group", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("issuetype", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("number", TQ::TQ_FIELD_TYPE_NUMBER);
    schemaToSimple.insert("option-with-child", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("option", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("priority", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("progress", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("project", TQ::TQ_FIELD_TYPE_STRING);
    schemaToSimple.insert("resolution", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("securitylevel", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("status", TQ::TQ_FIELD_TYPE_CHOICE);
    schemaToSimple.insert("string", TQ::TQ_FIELD_TYPE_STRING);
    schemaToSimple.insert("timetracking", TQ::TQ_FIELD_TYPE_DATE);
    schemaToSimple.insert("user", TQ::TQ_FIELD_TYPE_USER);
    schemaToSimple.insert("votes", TQ::TQ_FIELD_TYPE_NUMBER);
    schemaToSimple.insert("issuekey", TQ::TQ_FIELD_TYPE_NUMBER);

    roleFields.insert(TQAbstractRecordTypeDef::IdField,"issuekey");
    roleFields.insert(TQAbstractRecordTypeDef::TitleField, "summary");
    roleFields.insert(TQAbstractRecordTypeDef::DescriptionField, "description");
    roleFields.insert(TQAbstractRecordTypeDef::StateField, "status");
    roleFields.insert(TQAbstractRecordTypeDef::SubmitDateTimeField, "created");
    roleFields.insert(TQAbstractRecordTypeDef::SubmitterField, "creator");
    roleFields.insert(TQAbstractRecordTypeDef::OwnerField, "assignee");
    roleFields.insert(TQAbstractRecordTypeDef::IdFriendlyField,"key");

    systemChoices
            << "status"
            << "issuetype"
            << "priority"
            << "resolution"
            << "project"
//            << "issueLinkTypes"
               ;
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
    if(tableName == "Users")
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
        QString fSystemName = tableName.mid(QString(pref).length());
        int vid = systemNames.value(fSystemName, -1);
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

QString JiraRecTypeDef::fieldSystemName(int vid) const
{
    if(!fields.contains(vid))
        return QString();
    const JiraFieldDesc &desc = fields.value(vid);
    return desc.id;
}

QString JiraRecTypeDef::fieldRoleName(int vid) const
{
    return QString();
}

QIODevice *JiraRecTypeDef::defineSource() const
{
    QString fileName = project()->optionValue(TQOPTION_GROUP_FIELDS).toString();
    QFile *file = new QFile(fileName);
    if(!file->exists())
    {
        delete file;
        return 0;
    }
    return file;
    //return new QFile("data/jira.xml");
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
    {
        QDateTime dt = value.toDateTime();
        QString dv = dt.toString(dateTimeFormat());
        return dv;
    }
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
        dt = QDateTime::fromString(text,dateTimeFormat());
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

//QString JiraRecTypeDef::dateTimeFormat() const
//{
//    QLocale locale = QLocale::system();
//    return locale.dateFormat(QLocale::ShortFormat)+" "+locale.timeFormat(QLocale::LongFormat);
//}

QStringList JiraRecTypeDef::noteTitleList() const
{
    return QStringList();
}

TQAbstractProject *JiraRecTypeDef::project() const
{
    return prj;
}

bool JiraRecTypeDef::hasFieldCustomEditor(int vid) const
{
    int type = fieldSimpleType(vid);
    return (type == TQ::TQ_FIELD_TYPE_USER);
}

QWidget *JiraRecTypeDef::createCustomEditor(int vid, QWidget *parent) const
{
    JiraUserComboBox *box = new JiraUserComboBox(prj, parent);
    const JiraFieldDesc &desc = fields[vid];
    if(!desc.autoCompleteUrl.isEmpty())
    {
        QString s = desc.autoCompleteUrl;
        s.replace(QString("issueKey=null&"),QString("project=%1&").arg(prj->jiraProjectKey()));
        box->setCompleteLink(s);
    }
    QStringList items;
    foreach(const JiraUser &user, prj->knownUsers)
    {
        items.append(user.displayName);
    }
    items.sort();
    box->insertItems(0, items);
    return box;
    /*
    QLineEdit *editor = new QLineEdit(parent);
    JiraFindUser *win = new JiraFindUser(prj);
    win->connect(editor,SIGNAL(destroyed()),SLOT(deleteLater()));
    if(win->exec())
    {
        editor->setText(win->text());
    }
    return editor;
    */
}

int JiraRecTypeDef::schemaToSimpleType(const QString &schemaType)
{
    return schemaToSimple.value(schemaType,TQ::TQ_FIELD_TYPE_NONE);
}

// ====================== Jira Record ===================
JiraRecord::JiraRecord()
    : TQRecord(), def(0)
{
    isFieldsReaded = false;
    isTextsReaded = false;
}

JiraRecord::JiraRecord(TQAbstractProject *prj, int rtype, int id)
    : TQRecord(prj, rtype, id)
{
    isFieldsReaded = false;
    isTextsReaded = false;
    if(prj)
        def = dynamic_cast<JiraRecTypeDef*>(prj->recordTypeDef(rtype));
}

JiraRecord::JiraRecord(const TQRecord &src)
    : TQRecord(src)
{
    isFieldsReaded = false;
    isTextsReaded = false;
    if(project())
        def = dynamic_cast<JiraRecTypeDef*>(project()->recordTypeDef(recordType()));
}

QString JiraRecord::jiraKey() const
{
    return key;
}

int JiraRecord::recordInternalId() const
{
    return internalId;
}

QVariant JiraRecord::value(int vid, int role) const
{
    if(def && vid == def->idVid)
    {
        switch(role)
        {
        case Qt::DisplayRole:
            //return QString("%1-%2").arg(def->prj->projectKey).arg(recordId());
            return jiraKey();
        case Qt::EditRole:
            return recordId();
        }
        return QVariant();
    }
#ifdef QT_DEBUG
    QString fname = def->fieldName(vid);
#endif
    if(!isFieldsReaded)
    {
        TQRecord *rec = const_cast<JiraRecord *>(this);
        project()->readRecordFields(rec);
    }
    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
    if(def && vid == def->descVid)
        return desc;
    QVariant value;
    switch(role)
    {
    case Qt::DisplayRole:
        value = displayValues.value(vid, QVariant());
        break;
    case Qt::EditRole:
        value = values.value(vid, QVariant());
    }
#ifdef QT_DEBUG
    QString s = value.toString();
#endif

    return value;
}

bool JiraRecord::setValue(int vid, const QVariant &newValue)
{
    if(!def)
        return false;
    if(vid == def->idVid)
        return false;
    if(vid == def->descVid)
    {
        desc = newValue.toString();
        return true;
    }
    if(!def->fields.contains(vid))
        return false;
    bool res = TQRecord::setValue(vid, newValue);
    if(res)
    {
        values.insert(vid, newValue);
        displayValues.insert(vid, def->valueToDisplay(vid, newValue));
    }
    return true;
}

TQNotesCol JiraRecord::notes() const
{
    if(!isTextsReaded)
    {
        JiraRecord *rec = const_cast<JiraRecord *>(this);
        project()->readRecordTexts(rec);
    }
    return notesCol;
}

const TQAbstractRecordTypeDef *JiraRecord::typeDef() const
{
    return def;
}


void JiraDB::setConnectMethod(JiraDB::JiraConnectMethod method)
{
    connectMethod = method;
}


JiraFilterModel::JiraFilterModel(QObject *parent)
    : BaseRecModel(parent)
{
    headers
            << tr("Имя фильтра")
            << tr("Группа");
}

QVariant JiraFilterModel::displayColData(const JiraFilter &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.name;
    case 1:
        return rec.isSystem ? tr("Системные фильтры") : rec.isServerStored ? tr ("Избранные фильтры") : tr("Локальные фильтры");
    }
    return QVariant();
}
