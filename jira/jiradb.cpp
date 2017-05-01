#include "jiradb.h"
#include "qt-json/json.h"
#include <QtNetwork>
#include <tqmodels.h>
#include "jiraoptions.h"
#include <tqplugui.h>
#include <tqoauth.h>
#include <tqviewcontroller.h>
#include "webform.h"
#include <tqjson.h>
#include "jiralogin.h"
#include <tqcond.h>
#include "jiraqry.h"
#include "jiraquerydialog.h"
#include "jirarecmodel.h"
#include "uniondevice.h"
#include <ttglobal.h>
#include <tqdebug.h>
#include <QtCore>
#include "jirafinduser.h"
#include <QtSql>
//#include <ttutils.h>

Q_EXPORT_PLUGIN2("jira", JiraPlugin)

static JiraPlugin *jira = 0;

static bool isIntListOnly(const QString &text)
{
    static QRegExp reg("^\\s*\\d+\\s*(,\\s*\\d+)*\\s*$");
    return reg.exactMatch(text);
}

static QList<int> stringToIntList(const QString &s)
{
    QList<int> ilist;
    QStringList slist = s.split(',');
    foreach(const QString &si, slist)
    {
        bool ok;
        int i = si.toInt(&ok);
        if(ok)
            ilist << i;
    }
    return ilist;
}



static QWidget *getJiraOptionsWidget(const QString &path, GetOptionsWidgetFunc func)
{
    Q_UNUSED(path)
    Q_UNUSED(func)
    return new JiraOptions();
}

JiraPlugin::JiraPlugin(QObject *parent)
    : QObject(parent)
{
    jira = this;
    qRegisterMetaType<JiraProject*>("JiraProject*");

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

    QCoreApplication *app = QCoreApplication::instance();
    QLocale locale = QLocale::system();
    QTranslator *translator = new QTranslator(app);
    for(int i=1; i<app->argc(); i++)
    {
        if(app->arguments().value(i).trimmed().compare("-locale") == 0)
        {
            if(++i<app->argc())
                locale = QLocale(app->arguments().value(i));
            break;
        }
    }
    if(translator->load(locale, "jira", ".", pluginModule+"/lang"))
        app->installTranslator(translator);
    else
    {
        tqDebug() << "Can't load jira translator";
        delete translator;
    }

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

void JiraPlugin::showError(int code, const QString &errorText)
{
    emit error("Plugin for JIRA", tr("(%1) %2").arg(code).arg(errorText));
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

static QStringList skippedFields = QStringList()
        << "attachment"
        << "description"
        << "comment"
        << "worklog"
           ;

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
    QString tempFolder;
    QStringList knownProjects;

    JiraDBPrivate() : isLogged(false), lastHTTPCode(0)
    {
    }
};

// ======================== JiraRecTypeDef ==================================
struct JiraRecTypeDefPrivate {
    JiraProject *prj;
    int recType;
    QString recTypeName;
    QMap<int, JiraFieldDesc*> fields;
    QMap<QString, int> systemNames;
    QMap<QString, int> vids;
    QMap<int, QString> roleFields;
//    QStringList systemChoices;
//    QHash<QString, TQChoiceList> systemChoicesList;
    //    QHash<int,int> fIndexByVid;
    QStringList schemaTypes;
    QMap<QString, int> nativeTypes; // schema type to native type
    QMap<QString, int> schemaToSimple;
    int idVid, descVid, summaryVid, assigneeVid, creatorVid, createdVid;
    //    QMap<QString, JiraUser> knownUsers; // by name

};

JiraRecTypeDef::JiraRecTypeDef(JiraProject *project)
    : TQBaseRecordTypeDef(project), d(new JiraRecTypeDefPrivate())
{
    d->prj = project;
    d->schemaToSimple.insert("array", TQ::TQ_FIELD_TYPE_ARRAY);
    d->schemaToSimple.insert("date", TQ::TQ_FIELD_TYPE_DATE);
    d->schemaToSimple.insert("datetime", TQ::TQ_FIELD_TYPE_DATE);
    d->schemaToSimple.insert("group", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("issuetype", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("number", TQ::TQ_FIELD_TYPE_NUMBER);
    d->schemaToSimple.insert("option-with-child", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("option", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("priority", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("progress", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("project", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("resolution", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("securitylevel", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("status", TQ::TQ_FIELD_TYPE_CHOICE);
    d->schemaToSimple.insert("string", TQ::TQ_FIELD_TYPE_STRING);
    d->schemaToSimple.insert("timetracking", TQ::TQ_FIELD_TYPE_STRING);
    d->schemaToSimple.insert("user", TQ::TQ_FIELD_TYPE_USER);
    d->schemaToSimple.insert("votes", TQ::TQ_FIELD_TYPE_NUMBER);
    d->schemaToSimple.insert("issuekey", TQ::TQ_FIELD_TYPE_NUMBER);

    d->roleFields.insert(TQAbstractRecordTypeDef::IdField,"issuekey");
    d->roleFields.insert(TQAbstractRecordTypeDef::TitleField, "summary");
    d->roleFields.insert(TQAbstractRecordTypeDef::DescriptionField, "description");
    d->roleFields.insert(TQAbstractRecordTypeDef::StateField, "status");
    d->roleFields.insert(TQAbstractRecordTypeDef::SubmitDateTimeField, "created");
    d->roleFields.insert(TQAbstractRecordTypeDef::SubmitterField, "creator");
    d->roleFields.insert(TQAbstractRecordTypeDef::OwnerField, "assignee");
    d->roleFields.insert(TQAbstractRecordTypeDef::IdFriendlyField,"key");
    d->roleFields.insert(TQAbstractRecordTypeDef::RecordTypeField,"issuetype");
    d->roleFields.insert(TQAbstractRecordTypeDef::PriorityField,"priority");
    d->roleFields.insert(ProjectField,"project");

    /*
    d->systemChoices
            << "status"
            << "issuetype"
            << "priority"
            << "resolution"
            << "project"
//            << "issueLinkTypes"
               ;
               */
}

JiraRecTypeDef::JiraRecTypeDef(JiraRecTypeDef *src)
    : TQBaseRecordTypeDef(src->project()), d(new JiraRecTypeDefPrivate())
{
    *d = *(src->d);
    d->fields.clear();
    foreach(int vid, src->d->fields.keys())
    {
        const JiraFieldDesc *srcDesc = src->d->fields[vid];
        d->fields.insert(vid, new JiraFieldDesc(*srcDesc));
    }
}

JiraRecTypeDef::~JiraRecTypeDef()
{
    foreach(int vid, d->fields.keys())
        delete d->fields.take(vid);
    delete d;
}

QStringList JiraRecTypeDef::fieldNames() const
{
    QStringList list;
    foreach(const JiraFieldDesc *desc, d->fields)
    {
        if(!skippedFields.contains(desc->id))
        list.append(desc->name);
    }
    return list;
}

TQAbstractFieldType JiraRecTypeDef::getFieldType(int vid, bool *ok) const
{
    if(d->fields.contains(vid))
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
    if(!d->fields.contains(vid))
        return QString();
    return d->fields[vid]->schemaType;
}

int JiraRecTypeDef::fieldNativeType(int vid) const
{
    if(!d->fields.contains(vid))
        return 0;
    return d->fields[vid]->nativeType;
}

int JiraRecTypeDef::fieldSimpleType(int vid) const
{
    if(!d->fields.contains(vid))
        return TQ::TQ_FIELD_TYPE_NONE;
    return d->fields[vid]->simpleType;
}

bool JiraRecTypeDef::canFieldSubmit(int vid) const
{
    if(!d->fields.contains(vid))
        return false;
    return d->fields[vid]->createShow;
}

bool JiraRecTypeDef::canFieldUpdate(int vid) const
{
    if(!d->fields.contains(vid))
        return false;
    return d->fields[vid]->editable;
}

bool JiraRecTypeDef::isNullable(int vid) const
{
    if(!d->fields.contains(vid))
        return false;
    const JiraFieldDesc *desc = d->fields[vid];
    return !desc->createRequired;
}

bool JiraRecTypeDef::hasChoiceList(int vid) const
{
    if(!d->fields.contains(vid))
        return false;
    const JiraFieldDesc *desc = d->fields[vid];
    if(desc->simpleType == TQ::TQ_FIELD_TYPE_USER
            || desc->simpleType == TQ::TQ_FIELD_TYPE_CHOICE
            || desc->simpleType == TQ::TQ_FIELD_TYPE_ARRAY
            )
        return true;
    return false;
}

TQChoiceList JiraRecTypeDef::choiceTable(const QString &tableName) const
{
    static const char *pref = "Table_";
    if(tableName == "Users")
    {
        QMap<QString, TQUser> users = d->prj->userList();
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
        int vid = d->systemNames.value(fSystemName, -1);
        if(vid<0)
            return TQChoiceList();
        const JiraFieldDesc *desc = d->fields.value(vid);
        return desc->choices;
    }
    return TQChoiceList();
}

bool JiraRecTypeDef::containFieldVid(int vid) const
{
    return d->fields.contains(vid);
}

int JiraRecTypeDef::fieldVid(const QString &name) const
{
    return d->vids.value(name, TQ::TQ_NO_VID);
}

int JiraRecTypeDef::fieldVidSystem(const QString &systemName) const
{
    return d->systemNames.value(systemName, TQ::TQ_NO_VID);
}

QList<int> JiraRecTypeDef::fieldVids() const
{
    QList<int> list;
    foreach(const JiraFieldDesc *desc, d->fields)
        list.append(desc->vid);
    return list;
//    return vids.values();
}

QString JiraRecTypeDef::fieldName(int vid) const
{
    if(!d->fields.contains(vid))
        return QString();
    const JiraFieldDesc *desc = d->fields.value(vid);
    return desc->name;
}

QString JiraRecTypeDef::fieldSystemName(int vid) const
{
    if(!d->fields.contains(vid))
        return QString();
    const JiraFieldDesc *desc = d->fields.value(vid);
    return desc->id;
}

int JiraRecTypeDef::fieldRole(int vid) const
{
    if(vid == d->idVid)
        return TQAbstractRecordTypeDef::IdField;
    if(vid == d->descVid)
        return TQAbstractRecordTypeDef::DescriptionField;
    if(vid == d->summaryVid)
        return TQAbstractRecordTypeDef::TitleField;
    if(vid == d->assigneeVid)
        return TQAbstractRecordTypeDef::OwnerField;
    if(vid == d->createdVid)
        return TQAbstractRecordTypeDef::SubmitDateTimeField;
    if(vid == d->createdVid)
        return TQAbstractRecordTypeDef::SubmitterField;
    return NoRole;
}

QIODevice *JiraRecTypeDef::defineSource() const
{
    QString fileName = project()->optionValue(TQOPTION_GROUP_FILE).toString();
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
    return d->recType;
}

QString JiraRecTypeDef::valueToDisplay(int vid, const QVariant &value) const
{
    const JiraFieldDesc *fdesc = fieldDesc(vid);
    if(!fdesc)
        return QString();
    if(!fdesc->schemaSystem.compare("timetracking"))
    {
        QVariantMap map = value.toMap();
        QString orig = map.value("originalEstimate").toString();
        QString remain = map.value("remainingEstimate").toString();
        if(!orig.isEmpty())
            return orig + " / " + remain;
        return QString();
    }
    switch(fdesc->simpleType)
    {
    case TQ::TQ_FIELD_TYPE_NONE:
        return QString();
    case TQ::TQ_FIELD_TYPE_ARRAY:
    {
        QVariantList valueList = value.toList();
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return QString();
        TQChoiceList list = choiceTable(table);
        QStringList resultList;
        foreach(const TQChoiceItem &c, list)
        {
            if(valueList.contains(c.fieldValue))
                resultList.append(c.displayText);
        }
        return resultList.join("; ");
    }
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
    const JiraFieldDesc *fdesc = fieldDesc(vid);
    if(!fdesc)
        return QVariant();
    if(fdesc->schemaSystem == "timetracking")
    {
        QStringList list = text.split("/");
        QString origin = list.value(0,QString()).trimmed();
        QString remain = list.value(1,QString()).trimmed();
        QVariantMap map;
        if(!origin.isEmpty() || !remain.isEmpty())
        {
            map.insert("originalEstimate", origin);
            map.insert("remainingEstimate", remain);
        }
        return map;
    }
    int simple = fdesc->simpleType;
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
    case TQ::TQ_FIELD_TYPE_ARRAY:
    {
        QStringList displayList = text.split(QRegExp("\\s*;\\s*"));
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return QVariant();
        TQChoiceList list = choiceTable(table);
        QVariantList result;
        foreach(const TQChoiceItem &c, list)
        {
            if(displayList.contains(c.displayText,Qt::CaseInsensitive))
                result.append(c.fieldValue);
        }
        return result;
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
    const JiraFieldDesc *desc = d->fields.value(vid);
    return desc->defaultValue;
}

QVariant JiraRecTypeDef::fieldMinValue(int vid) const
{
    const JiraFieldDesc *desc = d->fields.value(vid);
    return desc->minValue;
}

QVariant JiraRecTypeDef::fieldMaxValue(int vid) const
{
    const JiraFieldDesc *desc = d->fields.value(vid);
    return desc->maxValue;
}

QString JiraRecTypeDef::fieldChoiceTable(int vid) const
{
    const JiraFieldDesc *desc = d->fields.value(vid);
    if(!desc->isValid())
        return QString();
    if(desc->isUser())
        return "Users";
    if(desc->isChoice() || desc->isArray())
        return "Table_" + desc->id;
    return QString();
}

int JiraRecTypeDef::roleVid(int role) const
{
    QString fSystem = d->roleFields.value(role);
    if(fSystem.isEmpty())
        return TQ::TQ_NO_VID;
    return d->systemNames.value(fSystem, TQ::TQ_NO_VID);
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
    return d->prj;
}

bool JiraRecTypeDef::hasFieldCustomEditor(int vid) const
{
    int type = fieldSimpleType(vid);
    return (type == TQ::TQ_FIELD_TYPE_USER);
}

QWidget *JiraRecTypeDef::createCustomEditor(int vid, QWidget *parent) const
{
    JiraUserEdit *box = new JiraUserEdit(d->prj, this, parent);
    const JiraFieldDesc *desc = d->fields[vid];
    if(!desc->autoCompleteUrl.isEmpty())
    {
        QString s = desc->autoCompleteUrl;
        s.replace(QString("issueKey=null&"),QString("project=%1&").arg(d->prj->jiraProjectKey()));
        box->setCompleteLink(s);
    }
    QStringList items;
    foreach(const JiraUser &user, d->prj->knownUsers)
    {
        items.append(user.displayName);
    }
    items.sort();
//    box->insertItems(0, items);
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

const JiraFieldDesc *JiraRecTypeDef::fieldDesc(int vid) const
{
    if(!d->fields.contains(vid))
        return 0;
    const JiraFieldDesc *fdesc = d->fields.value(vid);
    return fdesc;
}

QString JiraRecTypeDef::typeName() const
{
    return d->recTypeName;
}

int JiraRecTypeDef::idVid() const
{
    return d->idVid;
}

int JiraRecTypeDef::descVid() const
{
    return d->descVid;
}

int JiraRecTypeDef::schemaToSimpleType(const QString &schemaType)
{
    return d->schemaToSimple.value(schemaType,TQ::TQ_FIELD_TYPE_NONE);
}

// ======================== JiraDB ==================================
JiraDB::JiraDB(QObject *parent)
    : TQAbstractDB(parent), webForm(0), oa(new TQOAuth(this)), parser(new TQJson(this)),
      d(new JiraDBPrivate()),
      timeOutSecs(10),
      connectMethod(CookieAuth)
{
    parser->setCharset("UTF-8");
    man =
            new QNetworkAccessManager(this);
//            ttglobal()->networkManager();
    connect(man, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
    connect(man,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));

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
    d->knownProjects = res;
    return res;
}

QStringList JiraDB::knownProjects()
{
    return d->knownProjects;
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
    project->user =  user;
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

//    int recType = params.value(PRJPARAM_RECORDTYPE).toInt();

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
    int method = params.value(JIRAPARAM_METHOD).toInt();
    setConnectMethod((JiraConnectMethod)method);
    QString proxy = params.value(JIRAPARAM_PROXY,"Default").toString();
    if(proxy == "NoProxy")
    {
        QNetworkProxy p;
        p.setType(QNetworkProxy::NoProxy);
        man->setProxy(p);
    }
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
            if(!d->session.jsessionId.isEmpty())
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
    QVariantMap errorMap;
    errorMap.insert("code", d->lastHTTPCode);
    errorMap.insert("message", d->lastHTTPError);
//    if(d->lastHTTPCode != QNetworkReply::NoError)
//        emit
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

//    bool success;
//    QVariant  obj = QtJson::parse(s, success);
    QVariant obj;
    if(!contType.compare("application/json",Qt::CaseInsensitive))
        obj = parser->toVariant(buf);
    r->deleteLater();
    return obj; //QVariantList() << obj << errorMap;
}

/*
QVariant JiraDB::get(const QString &query, QVariantMap bodyMap)
{
    return sendRequest("GET", queryUrl(query), bodyMap);
}

QVariant JiraDB::post(const QString &query, QVariantMap bodyMap)
{
    return sendRequest("POST", queryUrl(query), bodyMap);
}

QVariant JiraDB::put(const QString &query, QVariantMap bodyMap)
{
    return sendRequest("PUT", queryUrl(query), bodyMap);
}

QVariant JiraDB::del(const QString &query, QVariantMap bodyMap)
{
    return sendRequest("DELETE", queryUrl(query), bodyMap);
}
*/

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

QVariant JiraDB::sendFile(const QUrl &url, const QString &fileName, QIODevice *file)
{
    /*
    QHttpMultiPart multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QNetworkRequest request(url);
    QString bound(multiPart.boundary());
    bound = "A1234";
    request.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=")+bound);
    request.setRawHeader("X-Atlassian-Token", "nocheck");
//    request.setHeader(QNetworkRequest::ContentLengthHeader, 166);
    QByteArray begin, end;
    begin.append(QString("\r\n--%1\r\n").arg(bound));
    begin.append(QString("Content-Disposition: form-data; name=\"file\"; filename=\"%1\"\r\n").arg(fileName));
    begin.append(QString("Content-Type: application/octet-stream\r\n"));
    begin.append("\r\n");
    end.append(QString("\r\n--%1--\r\n").arg(bound));

    QBuffer bufBegin(&begin), bufEnd(&end);
    bufBegin.open(QIODevice::ReadOnly);
    bufEnd.open(QIODevice::ReadOnly);
    UnionDevice *dev = new UnionDevice(this);
    dev->appendDevice(&bufBegin);
    dev->appendDevice(file);
    dev->appendDevice(&bufEnd);
    dev->open(QIODevice::ReadOnly);

    QByteArray body = dev->readAll();

    dumpRequest(&request, "POST", body);

    QNetworkReply *reply = man->post(request, body);
    dev->setParent(reply);
*/

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"file\"; filename=\"%1\"").arg(fileName));
    filePart.setBodyDevice(file);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

    multiPart->append(filePart);

    QNetworkRequest request(url);
    request.setRawHeader("X-Atlassian-Token", "nocheck");
//    QList<QNetworkCookie> list = man->cookieJar()->cookiesForUrl(dbmsServer());
//    if(list.size())
//        request.setHeader(QNetworkRequest::SetCookieHeader, QVariant::fromValue(list));

    dumpRequest(&request, "post", QByteArray());

    QNetworkReply *reply = man->post(request, multiPart);
    multiPart->setParent(reply); // delete the multiPart with the reply

    waitReply(reply);

    d->lastHTTPCode = reply->error();
    d->lastHTTPError = reply->errorString();
    QString contType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    if(contType.contains(';'))
        contType = contType.split(';').value(0);
    contType = contType.trimmed();
    QByteArray buf = reply->readAll();

    dumpReply(reply, buf);
    QVariant obj;
    if(!contType.compare("application/json",Qt::CaseInsensitive))
        obj = parser->toVariant(buf);
    reply->deleteLater();
    return obj;
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

QString JiraDB::tempFolder()
{
    if(d->tempFolder.isEmpty())
    {
        QDir temp = QDir::temp();
        d->tempFolder = temp.absoluteFilePath("tqjira");
        temp.mkpath(d->tempFolder);
    }
    return d->tempFolder;
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


/*
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
    if(reply->error() != QNetworkReply::NoError)
    {
        tqDebug() << "Error:" << QString::number(reply->error())
                 << reply->errorString();
        tqDebug() << reply->readAll();
        return QVariant();
    }

    QByteArray buf = reply->readAll();

    tqDebug() << buf;
//    QString s(buf.constData());
//    bool success;
//    QVariant  obj = QtJson::parse(s, success);
    QVariant obj = parser->toVariant(buf);
    return obj;
}
*/

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

/*
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
*/

bool JiraDB::loginCookie(const QString &user, const QString &pass)
{
    if(user.isEmpty())
    {
        d->isLogged = true;
        return true;
    }
//    bool res = false;
    QVariantMap reqMap, repMap;
    reqMap.insert("username", user);
    reqMap.insert("password", pass);
    while(true)
    {
//        QVariantMap repMap = sendRequest("POST", queryUrl("rest/auth/1/session"), reqMap).toMap();
        QByteArray body = parser->toByteArray(reqMap);
        QNetworkReply *reply = sendRequestNative(queryUrl("rest/auth/1/session"), "POST", body);
        d->lastHTTPCode = reply->error();
        d->lastHTTPError = reply->errorString();
        QByteArray buf = reply->readAll();
//        QList<QNetworkReply::RawHeaderPair> pairs = reply->rawHeaderPairs();
        dumpReply(reply, buf);
        if(d->lastHTTPCode && d->lastHTTPCode!=200)
        {
            // X-Seraph-LoginReason AUTHENTICATION_DENIED
            // X-Authentication-Denied-Reason CAPTCHA_CHALLENGE; login-url=https://bugreports.qt.io/login.jsp

            JiraPlugin::plugin()->showError(d->lastHTTPCode, d->lastHTTPError);
            QByteArray seraph = reply->rawHeader("X-Seraph-LoginReason");
            if(seraph != "AUTHENTICATION_DENIED")
                return false;
            QString reason = reply->rawHeader("X-Authentication-Denied-Reason");
            QStringList list = reason.split(QRegExp(";\\s*"));
            if(!list.contains("CAPTCHA_CHALLENGE"))
                return false;
            QString s = list.value(1);
            if(s.isEmpty())
                return false;
            QUrl url = s.mid(s.indexOf("=")+1);
            QScopedPointer<WebForm> web(new WebForm());
            if(man)
                web->setNetworkAccessManager(man);
            if(web->request(url/*, QRegExp("Dashboard\\.jspa")*/))
                continue;
            return false;

        }
        QString contType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
        if(contType.contains(';'))
            contType = contType.split(';').value(0);
        contType = contType.trimmed();
        d->lastCookies = reply->header(QNetworkRequest::SetCookieHeader).value<QList<QNetworkCookie> >();
        repMap = parser->toVariant(buf).toMap();
        reply->deleteLater();
        if(repMap.contains("session"))
            break;
        return false;
    }
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
        QByteArray h = header;
        h+=":";
        tqDebug() << h  << value;
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

void JiraDB::onSslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach(const QSslError &err, errors)
        tqInfo() << err.errorString();
    reply->ignoreSslErrors(errors);
}

// ======================== JiraProject ==================================
class JiraProjectPrivate
{
public:
    QHash<QString, QAction *> actions; // actions by id
    bool hasLocalDb;
};


JiraProject::JiraProject(TQAbstractDB *db)
    : TQBaseProject(db), filters(new JiraFilterModel(this)), d(new JiraProjectPrivate())
{
    this->db = qobject_cast<JiraDB *>(db);
    systemChoices
            << "status"
            << "issuetype"
            << "priority"
            << "resolution"
            << "project"
//            << "issueLinkTypes"
               ;
    d->hasLocalDb = false;
}

void JiraProject::loadDefinition()
{
    loadRecordTypes();
    loadQueries();
    loadUsers();
}

JiraDB *JiraProject::jiraDb() const
{
    return db;
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

TQRecModel *JiraProject::openQuery(const QString &queryName, int recType)
{
    const JiraFilter *fitem = filters->filter(queryName);
    if(!fitem)
        return 0;
    QString jql = fitem->jql;
    JiraRecModel *model = new JiraRecModel(this, recType, this);
    model->setJQuery(jql);
    model->open();
    return model;
}

TQRecModel *JiraProject::queryIds(const IntList &ids, int recType, bool emitEvent)
{
    TQAbstractRecordTypeDef *rdef = recordTypeDef(recType);
    if(!rdef)
        return 0;
    QStringList list;
    foreach(int i, ids)
        list.append(projectKey + "-" + QString::number(i));
    return queryKeys(list, recType, emitEvent);
}

TQRecModel *JiraProject::queryKeys(const QStringList &keys, int recType, bool emitEvent)
{
    TQAbstractRecordTypeDef *rdef = recordTypeDef(recType);
    if(!rdef)
        return 0;
    QString jql = QString("Key in (%1)").arg(keys.join(","));
    return queryJQL(jql, recType);
}

TQRecModel *JiraProject::query(const QString &queryText, int recType)
{
    if(isIntListOnly(queryText))
        return queryIds(stringToIntList(queryText), recType);
    QString proj = db->knownProjects().join("|");
    // try ids
    QString s = QString("\\s*(\\w+-\\d+)(?:\\s*,\\s*(\\w+-\\d+))*\\s*");
    QRegExp re(s);
    if(re.exactMatch(queryText))
    {
        QStringList keys, groups = re.capturedTexts().mid(1);
        foreach(QString s, groups)
            if(!s.trimmed().isEmpty())
                keys << s.trimmed();
        if(keys.isEmpty())
            return 0;
        return queryKeys(keys, recType);
    }
    return queryJQL(queryText, recType);
}

TQRecModel *JiraProject::queryJQL(const QString &jql, int recType)
{
    JiraRecModel *model = new JiraRecModel(this, recType, this);
    model->setJQuery(jql);
    model->open();
    return model;
}

TQRecModel *JiraProject::openRecords(const QString &queryText, int recType, bool emitEvent)
{
    if(isIntListOnly(queryText))
        return queryIds(stringToIntList(queryText), recType, emitEvent);
    return queryJQL(queryText, recType);
}

TQRecord *JiraProject::createRecordById(int id, int rectype)
{
    JiraRecord *rec = new JiraRecord(this, rectype, id);
    rec->key = projectKey + "-" + QString::number(id);
    rec->internalId = 0;
//    connect(rec, SIGNAL(changed(int)), this, SIGNAL(recordChanged(int)));
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
    return model->recordInRow(index.row());
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
    if(rec->mode() == TQRecord::Insert || rec->mode() == TQRecord::Edit)
        return true;
    const JiraRecTypeDef *rdef = dynamic_cast<const JiraRecTypeDef *>(record->typeDef());
    if(!rdef)
        return false;
    QVariantMap issue = serverGet(QString("rest/api/2/issue/%1?fields=*all&expand=attachment,changelog").arg(rec->jiraKey())).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    rec->clearReadedFields();
    for(i = fields.begin(); i!=fields.end(); i++)
        rec->storeReadedField(i.key(), i.value());
    rec->isFieldsReaded = true;
    doParseComments(rec, issue);
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
            file.index = rec->files.size();
            file.size = map.value("size").toInt();
            file.data = map;
            file.createDateTime = map.value("created").toDateTime();
            file.isAdded = false;
            file.isChanged = false;
            file.isDeleted = false;
            rec->files.append(file);
        }
    }
    QVariantMap changelog = issue.value("changelog").toMap();
    doParseChangelog(rec, changelog);
    QVariantList issueLinks = fields.value("issuelinks").toList();
    doParseLinks(rec, issueLinks);
//    emit recordChanged(rec->recordId());
//    emit rec->changed(rec->recordId());
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
    QVariantMap issue = serverGet(QString("rest/api/2/issue/%1?fields=*all,-description,-comment").arg(rec->jiraKey())).toMap();
    if(!issue.contains("id"))
        return false;
    rec->internalId = issue.value("id").toInt();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    rec->clearReadedFields();
    for(i = fields.begin(); i!=fields.end(); i++)
        rec->storeReadedField(i.key(), i.value());
    rec->isFieldsReaded = true;
//    emit recordChanged(rec->recordId());
//    emit rec->changed(rec->recordId());
    return true;
}

bool JiraProject::readRecordTexts(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    if(rec->mode() == TQRecord::Insert)
        return true;
    QVariantMap issue = serverGet(QString("rest/api/2/issue/%1?fields=description,comment").arg(rec->jiraKey())).toMap();
    rec->desc = db->parseValue(issue,"fields/description").toString();
    doParseComments(rec, issue);
    /*
    QVariantList comments = db->parseValue(issue,"fields/comment/comments").toList();
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
        note.internalId = com.value("id").toInt();
        rec->notesCol.append(note);
    }
    */
    rec->isTextsReaded = true;
//    emit recordChanged(rec->recordId());
//    emit rec->changed(rec->recordId());
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
    QVariantMap issue = serverGet(QString("rest/api/2/issue/%1?fields=%2").arg(rec->jiraKey()).arg(list.join(","))).toMap();
    QVariantMap fields = issue.value("fields").toMap();
    QVariantMap::iterator i;
    for(i = fields.begin(); i!=fields.end(); i++)
        rec->storeReadedField(i.key(), i.value());
    return true;
}

bool JiraProject::updateRecordBegin(TQRecord *record)
{
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    if(!rec)
        return false;
    rec->setMode(TQRecord::Edit);
    return true;
}

static QVariant arrayValue(const QVariant &value)
{
    QVariantList list;
    foreach(QVariant item, value.toList())
    {
        QVariantMap v;
        v.insert("id", item.toString());
        list.append(v);
    }
    return list;
}

bool JiraProject::commitRecord(TQRecord *record)
{
    if(record->mode() == TQRecord::View)
        return false;
    if(record->mode() == TQRecord::Edit)
        return doCommitUpdateRecord(record);
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
        const JiraFieldDesc *fdesc = recDef->fieldDesc(vid);
        if(!fdesc)
            continue;
        if(fdesc->isUser())
        {
            QVariantMap v;
            v.insert("name", value.toString());
            value = v;
        }
        else if(fdesc->isChoice())
        {
            QVariantMap v;
            v.insert("id", value.toString());
            value = v;
        }
        else if(fdesc->isArray())
            value = arrayValue(value);
        fields.insert(fdesc->id, value);
    }
    QVariantMap v2;
    v2.insert("key", projectKey);
    fields.insert("project", v2);
    v2.clear();
    fields.insert("summary",jRec->title());
    fields.insert("description",jRec->description());
    issue.insert("fields", fields);
    QVariantList comments;
    foreach(const TQNote &note, jRec->notes())
    {
        QVariantMap com;
        com.insert("body", note.text);
        comments.append(com);
    }
    QVariantMap commentMap;
    commentMap.insert("comments",comments);
    issue.insert("comment", commentMap);
    QVariantMap res;
    bool wasError;
    if(record->mode() == TQRecord::Edit)
    {
        res = serverPut(QString("rest/api/2/issue/%1").arg(jRec->jiraKey()),issue).toMap();
        wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200;
    }
    else
    {
        res = serverPost(QString("rest/api/2/issue"),issue).toMap();
        wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200;// !res.contains("id");
        if(!wasError)
        {
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
    emit recordChanged(jRec->recordId());
    return true;
}

bool JiraProject::doCommitUpdateRecord(TQRecord *record)
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
        const JiraFieldDesc *fdesc = recDef->fieldDesc(vid);
        if(!fdesc)
            continue;
        if(fdesc->isUser())
        {
            QVariantMap v;
            v.insert("name", value.toString());
            value = v;
        }
        else if(fdesc->isChoice())
        {
            QVariantMap v;
            v.insert("id", value.toString());
            value = v;
        }
        else if(fdesc->isArray())
            value = arrayValue(value);
        fields.insert(fdesc->id, value);
    }
    QVariantMap res;
    bool wasError = false;
    if(!fields.isEmpty())
    {
        issue.insert("fields", fields);
        res = serverPut(QString("rest/api/2/issue/%1").arg(jRec->jiraKey()),issue).toMap();
        wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200;

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
    }
    foreach(const TQNote &note, jRec->notes())
    {
        if(note.isAdded)
        {
            QVariantMap com;
            com.insert("body", note.text);
            res = serverPost(QString("rest/api/2/issue/%1/comment").arg(jRec->jiraKey()),com).toMap();
            int errorCode = db->lastHTTPCode();
            wasError |=  errorCode && errorCode != 200  && errorCode != 204;
        }
        else if(note.isChanged)
        {
            QVariantMap com;
            com.insert("body", note.text);
            res = serverPut(QString("rest/api/2/issue/%1/comment/%2").arg(jRec->jiraKey()).arg(note.internalId),com).toMap();
            wasError |= db->lastHTTPCode() && db->lastHTTPCode() != 200  && db->lastHTTPCode() != 204;
        }
        else if(note.isDeleted)
        {
            res = serverDelete(QString("rest/api/2/issue/%1/comment/%2").arg(jRec->jiraKey())
                               .arg(note.internalId)).toMap();
            wasError |= db->lastHTTPCode() && db->lastHTTPCode() != 200  && db->lastHTTPCode() != 204;
        }
    }
    if(!wasError)
    {
        jRec->setMode(TQRecord::View);
        jRec->refresh();
        emit recordChanged(jRec->recordId());
    }
    return !wasError;
}

void JiraProject::doParseComments(JiraRecord *rec, const QVariantMap &issue)
{
    QVariantList comments = db->parseValue(issue,"fields/comment/comments").toList();
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
        note.internalId = com.value("id").toInt();
        rec->notesCol.append(note);
    }
}

void JiraProject::doParseChangelog(JiraRecord *rec, const QVariantMap &changelog)
{
    bool noFiles = rec->files.size() == 0;
    QVariantList res;
    QVariantList hist = changelog.value("histories").toList();
    QList<TQAttachedFile> files;
    foreach(QVariant v, hist)
    {
        QVariantMap m = v.toMap();
        QVariantList items = m.value("items").toList();
        foreach(QVariant vi, items)
        {
            QVariantMap item = vi.toMap();
            QVariantMap hi;
            hi["author"] = m.value("author").toMap().value("displayName").toString();
            QDateTime created = QDateTime::fromString(m.value("created").toString(), Qt::ISODate);
            hi["datetime"] = created;
            QString field = item.value("field").toString();
            hi["field"] = field;
            hi["fromValue"] = item.value("from");
            QString fromS = item.value("fromString").toString();
            hi["fromString"] = fromS;
            hi["toValue"] = item.value("to");
            QString toS = item.value("toString").toString();
            hi["toString"] = toS;
            hi["action"] = field + ": " + fromS + " ==> " + toS;
            res.append(hi);
            if(noFiles && field == "Attachment" && item.value("fieldtype").toString() == "jira")
            {
                if(fromS.isEmpty()) // added
                {
                    TQAttachedFile file;
                    file.fileName = toS;
                    file.index = rec->files.size() + files.size();
//                    file.size = map.value("size").toInt();
                    file.createDateTime = created;
                    file.isAdded = false;
                    file.isChanged = false;
                    file.isDeleted = false;
                    QVariantMap map;
                    map["id"] = item.value("to");
                    map["filename"] = file.fileName;
                    file.data = map;
                    files.append(file);
                }
                else if(toS.isEmpty()) //deleted
                {
                    for(int i=0; i<files.size(); i++)
                    {
                        const TQAttachedFile &file = files.at(i);
                        if(file.fileName == fromS)
                        {
                            files.removeAt(i);
                            break;
                        }
                    }
                }
            }
        }
    }
    rec->historyArray = res;
    if(noFiles && files.size())
    {
        rec->files.append(files);
        for(int i = 0; i<rec->files.size();i++)
        {
            TQAttachedFile &file = rec->files[i];
            file.index = i;
        }
    }
}

void JiraProject::doParseLinks(JiraRecord *rec, const QVariantList &issuelinks)
{
    QVariantList res;
    foreach(QVariant v, issuelinks)
    {
        QVariantMap m = v.toMap();
        QVariantMap li;
        QVariantMap t = m.value("type").toMap();
        QVariantMap issue;
        if(m.contains("outwardIssue"))
        {
            issue = m.value("outwardIssue").toMap();
            li["type"] = t.value("outward").toString();
        }
        else
        {
            issue = m.value("inwardIssue").toMap();
            li["type"] = t.value("inward").toString();
        }
        li["id"] = issue.value("key").toString();
        li["url"] = issue.value("self").toString();
        li["title"] = issue.value("fields").toMap().value("summary").toString();
        li["status"] = issue.value("fields").toMap().value("status").toMap().value("name").toString();
        res.append(li);
    }
    rec->issueLinks = res;
}

bool JiraProject::cancelRecord(TQRecord *record)
{
    if(record->mode() == TQRecord::View)
        return false;
    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
    record->setMode(TQRecord::View);
    emit rec->changed(record->recordId());
    return true;
}

TQRecord *JiraProject::newRecord(int rectype)
{
    JiraRecord *rec = new JiraRecord(this, rectype, 0);
    rec->setMode(TQRecord::Insert);
    int vid;
    QList<int> vids = rec->typeDef()->fieldVids();
    foreach(vid, vids)
    {
        QVariant v = rec->typeDef()->fieldDefaultValue(vid);
        if(v.isValid())
            rec->setValue(vid,v);
    }
    vid = rec->typeDef()->roleVid(JiraRecTypeDef::ProjectField);
    QVariant v = rec->typeDef()->displayToValue(vid,projectName());
    rec->setValue(vid,v);
    rec->setModified(false);
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
    TQAttachedFile f = rec->files.at(fileIndex);
    QVariantMap map = f.data.toMap();
    QUrl url = map.value("content").toString();
    if(url.isEmpty() && map.contains("id"))
    {
        QVariant att = db->sendRequest("GET", db->queryUrl("rest/api/2/attachment/" + map["id"].toString()));
        QString content = att.toMap().value("content").toString();
        if(!content.isEmpty())
        {
            url = content;
            map["content"] = content;
            f.data = map;
            rec->files[fileIndex] = f;
        }
    }
    if(url.isEmpty())
        return false;
    QNetworkReply *r = db->sendRequestNative(url, "GET");
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

//QVariantList JiraProject::historyList(TQRecord *record)
//{
//    QVariantList hist;
//    JiraRecord *rec = qobject_cast<JiraRecord *>(record);
//    if(rec)
//    {
//        return rec->historyList();
//    }
//    return hist;
//}

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
    Q_UNUSED(model);
    return false;
}

QSettings *JiraProject::projectSettings() const
{
    return TQAbstractProject::projectSettings();
    /*
    QString iniFile = jira->dataDir.absoluteFilePath("jira.ini");
    QSettings *sets = new QSettings(iniFile, QSettings::IniFormat);
    sets->beginGroup(projectName());
    return sets;
    */
}

QAbstractItemModel *JiraProject::queryModel(int type)
{
    if(recordTypes.contains(type))
        return filters;
    return 0;
}

TQQueryGroups JiraProject::queryGroups(int type)
{
    TQQueryGroups list;
    TQQueryGroup item;
    item.name = tr("Фильтры");
    item.filterString = "all";
    item.isCreateEnabled = true;
    item.isModifyEnabled = true;
    item.isDeleteEnabled = true;
    list.append(item);

    /*
    item.name = tr("Серверные фильтры");
    item.filterString = "server";
    item.isCreateEnabled = true;
    item.isModifyEnabled = true;
    item.isDeleteEnabled = true;
    list.append(item);

    item.name = tr("Личные фильтры");
    item.filterString = "local";
    item.isCreateEnabled = true;
    item.isModifyEnabled = true;
    item.isDeleteEnabled = true;
    list.append(item);
*/
    return list;
}

TQQueryDef *JiraProject::queryDefinition(const QString &queryName, int rectype)
{
    for(int row = 0; row<filters->rowCount(); row++)
    {
        const JiraFilter &f = filters->at(row);
        if(f.name == queryName)
        {
            JiraQry *def = new JiraQry(this, rectype);
            def->setName(f.name);
            def->setQueryLine(f.jql);
            return def;
        }
    }
    return 0;
}

TQQueryDef *JiraProject::createQueryDefinition(int rectype)
{
    return new JiraQry(this, rectype);
}

static QVariant strValue(const QString &str)
{
    if(str.isNull())
        return QString("");
    return QVariant(str);
}

bool JiraProject::saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype)
{
    JiraQry *def = qobject_cast<JiraQry*>(queryDefinition);
    if(!def)
        return false;
    QString oldName;
    int serverId = 0;
    bool isModify = false;


    QSqlDatabase udb = ttglobal()->userDatabase();
    if(!udb.open())
        return false;
    QSqlQuery query(udb);
    udb.transaction();
    query.prepare("select name, serverId from jiraqueries where project = ? and login = ?");
    query.bindValue(0, strValue(this->projectName()));
    query.bindValue(1, strValue(this->currentUser()));
    query.exec();
    while(query.next())
    {
        oldName = query.value(0).toString();
        int oldServerId = query.value(1).toInt();
        if(oldName.compare(queryName,Qt::CaseInsensitive) == 0)
        {
            QSqlQuery q(udb);
            q.prepare("delete from jiraqueries where project = ? and login = ? and name = ?");
            q.bindValue(0, strValue(this->projectName()));
            q.bindValue(1, strValue(this->currentUser()));
            q.bindValue(2, strValue(oldName));
            q.exec();
            isModify = true;
            serverId = oldServerId;
            break;
        }
    }
    query.prepare("insert into jiraqueries(project, login, name, jql, serverId) values(?,?,?,?,?)");
    query.bindValue(0, strValue(this->projectName()));
    query.bindValue(1, strValue(this->currentUser()));
    query.bindValue(2, strValue(queryName));
    query.bindValue(3, strValue(def->queryLine()));
    query.bindValue(4, serverId);
    if(!query.exec())
    {
        udb.rollback();
        return false;
    }
    udb.commit();
    if(isModify)
        for(int r = 0; r<filters->rowCount(); r++)
        {
            const JiraFilter &f = filters->at(r);
            if(f.name.compare(oldName, Qt::CaseInsensitive) == 0)
            {
                if(f.serverId)
                    serverId = f.serverId;
                filters->removeRow(r);
                break;
            }
        }
    JiraFilter f;
    f.serverId = serverId;
    f.isSystem = false;
    f.jql = def->queryLine();
    f.name = queryName;
    if(f.serverId)
    {
        QVariantMap body;
        body["name"] = f.name;
        body["jql"] = f.jql;
        body["favourite"] = true;
        serverPut("rest/api/2/filter/" + QString::number(f.serverId), body);
    }
    else
    {
        QVariantMap body;
        body["name"] = f.name;
        body["jql"] = f.jql;
        body["favourite"] = true;
        QVariantMap map = serverPost("rest/api/2/filter", body).toMap();
        if(map.contains("id"))
        {
            f.serverId = map.value("id").toInt();
            query.prepare("update jiraqueries set serverId = ? where project = ? and login = ? and name = ?");
            query.bindValue(0, f.serverId);
            query.bindValue(1, strValue(this->projectName()));
            query.bindValue(2, strValue(this->currentUser()));
            query.bindValue(3, strValue(queryName));
        }
    }
    filters->append(f);
    return true;
}

bool JiraProject::renameQuery(const QString &oldName, const QString &newName, int recordType)
{
    QSqlDatabase udb = ttglobal()->userDatabase();
    if(!udb.open())
        return false;
    QSqlQuery query(udb);
    query.prepare("update jiraqueries set name = ? where project = ? and login = ? and name = ?");
    query.bindValue(0, strValue(newName));
    query.bindValue(1, strValue(this->projectName()));
    query.bindValue(2, strValue(this->currentUser()));
    query.bindValue(3, strValue(oldName));
    if(query.exec())
        for(int r = 0; r<filters->rowCount(); r++)
        {
            JiraFilter &f = filters->operator [](r);
            if(f.name.compare(oldName, Qt::CaseInsensitive) == 0)
            {
                filters->setData(filters->index(r,0), newName);
                f.name = newName;
//                filters->dataChanged(, filters->index(r,0));
                if(f.serverId)
                {
                    QVariantMap body;
                    body["name"] = f.name;
                    body["jql"] = f.jql;
                    body["favourite"] = true;
                    serverPut("rest/api/2/filter/" + QString::number(f.serverId), body);
                }
                return true;
            }
        }
    return false;
}

bool JiraProject::deleteQuery(const QString &queryName, int recordType)
{
    QSqlDatabase udb = ttglobal()->userDatabase();
    if(!udb.open())
        return false;
    QSqlQuery query(udb);
    query.prepare("delete from jiraqueries where project = ? and login = ? and name = ?");
    query.bindValue(0, strValue(this->projectName()));
    query.bindValue(1, strValue(this->currentUser()));
    query.bindValue(2, strValue(queryName));
    if(query.exec())
        for(int r = 0; r<filters->rowCount(); r++)
        {
            JiraFilter &f = filters->operator [](r);
            if(f.name.compare(queryName, Qt::CaseInsensitive) == 0)
            {
                if(f.serverId)
                    serverDelete("rest/api/2/filter/" + QString::number(f.serverId));
                filters->removeRow(r);
                return true;
            }
        }
    return false;
}

TQAbstractQWController *JiraProject::queryWidgetController(int rectype)
{
    JiraQueryDialogController *jq = new JiraQueryDialogController(this, rectype);
    return jq;
}

QVariant JiraProject::optionValue(const QString &option) const
{
    QSettings *sets = projectSettings();
    QVariant v = sets->value(option);
    delete sets;
    if(v.isValid())
        return v;

    if(option == TQOPTION_VIEW_TEMPLATE
            || option == TQOPTION_PRINT_TEMPLATE
            || option == TQOPTION_EMAIL_TEMPLATE
            )
        return jira->dataDir.absoluteFilePath("jira-issue.xq");
    if(option == TQOPTION_EDIT_TEMPLATE)
        return jira->dataDir.absoluteFilePath("jira-edit.xq");
    if(option == TQOPTION_GROUP_FILE)
        return jira->dataDir.absoluteFilePath("jira.xml");
    return ttglobal()->optionDefaultValue(option);
}

QString JiraProject::jiraProjectKey() const
{
    return projectKey;
}

QString JiraProject::userFullName(const QString &login)
{
    if(knownUsers.contains(login))
    {
        JiraUser user = knownUsers[login];
        return user.displayName;
    }
    QVariantMap map = serverGet(QString("rest/api/2/user?username=%1").arg(login)).toMap();
    if(!map.contains("displayName"))
        return login;
    appendUserToKnown(map);
    return map.value("displayName").toString();
}

bool JiraProject::isAnonymousUser() const
{
    return currentUser().isEmpty();
}

QVariant JiraProject::serverGet(const QString &urlPath)
{
    QUrl url(urlPath);
    if(url.isRelative())
        url = db->queryUrl(urlPath);
    QVariant res = db->sendRequest("GET", url);
    checkRequestResult(res);
    return res;
}

QVariant JiraProject::serverPut(const QString &urlPath, const QVariantMap &bodyMap)
{
    QVariant res = db->sendRequest("PUT", db->queryUrl(urlPath), bodyMap);
    checkRequestResult(res);
    return res;
}

QVariant JiraProject::serverPost(const QString &urlPath, const QVariantMap &bodyMap)
{
    QVariant res = db->sendRequest("POST", db->queryUrl(urlPath), bodyMap);
    checkRequestResult(res);
    return res;
}

QVariant JiraProject::serverDelete(const QString &urlPath)
{
    QVariant res = db->sendRequest("DELETE", db->queryUrl(urlPath));
    checkRequestResult(res);
    return res;
}

void JiraProject::checkRequestResult(const QVariant &result)
{
    if(db->lastHTTPCode())
    {
        QVariantMap map = result.toMap();
        QString mes;
        QVariant v = map.value("errorMessages");
        if(v.isValid())
            mes = "\n" + v.toStringList().join("\n");
        emit error(db->lastHTTPCode(), db->lastHTTPErrorString() + mes);
    }
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

void JiraProject::readRecordDef2(JiraRecTypeDef *rdef, const QVariantMap &fieldsMap)
{
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
        f.editable = false;
        f.clauseNames = map.value("clauseNames",QStringList()).toStringList();

        QVariantMap s = map.value("schema", QVariantMap()).toMap();
        f.schemaType = s.value("type").toString();
        f.schemaItems = s.value("items").toString();
        f.schemaSystem = s.value("system").toString();
        f.vid = ++vid;
        if(f.id == "issuekey")
        {
            f.schemaType = "issuekey";
            rdef->d->idVid = f.vid;
        }
        else if(f.id == "description")
            rdef->d->descVid = f.vid;
        else if(f.id == "summary")
            rdef->d->summaryVid = f.vid;
        else if(f.id == "assignee")
            rdef->d->assigneeVid = f.vid;
        else if(f.id == "creator")
            rdef->d->creatorVid = f.vid;
        else if(f.id == "created")
            rdef->d->createdVid = f.vid;
        if(!rdef->d->schemaTypes.contains(f.schemaType))
        {
            rdef->d->schemaTypes.append(f.schemaType);
            rdef->d->nativeTypes.insert(f.schemaType, nativeType++);
        }
        f.nativeType = rdef->d->schemaTypes.indexOf(f.schemaType); // not work for issuekey

        f.simpleType = rdef->schemaToSimpleType(f.schemaType);
        if(f.simpleType == TQ::TQ_FIELD_TYPE_USER)
            f.choiceTable = "Users";
        else if(f.simpleType == TQ::TQ_FIELD_TYPE_CHOICE || f.simpleType == TQ::TQ_FIELD_TYPE_ARRAY)
        {
            f.choiceTable = "Table_" + f.id;
            if(systemChoices.contains(f.id))
            {
                if(systemChoicesList.contains(f.id))
                    f.choices = systemChoicesList.value(f.id);
                else
                {
                    f.choices = loadChoiceTables(rdef, f.id);
                    systemChoicesList.insert(f.id, f.choices);
                }
            }
        }
        /*else if(map.contains("allowedValues"))
        {
            f.simpleType = TQ::TQ_FIELD_TYPE_CHOICE;
            f.choiceTable = "Table_" + f.id;
            f.choices = parseAllowedValues(map.value("allowedValue").toList());
        }*/
        else
            f.choiceTable = QString();
        if(fieldsMap.contains(f.id))
        {
            QVariantMap createMeta = fieldsMap.value(f.id).toMap();
            f.createShow = true;
            f.createRequired = createMeta.value("required","false").toString() == "true";
            f.autoCompleteUrl = createMeta.value("autoCompleteUrl").toString();
            if(!systemChoices.contains(f.id) && createMeta.contains("allowedValues"))
            {
                QVariantList values = createMeta.value("allowedValues").toList();
                f.choices = parseAllowedValues(values);
                if(f.simpleType != TQ::TQ_FIELD_TYPE_CHOICE && f.simpleType != TQ::TQ_FIELD_TYPE_ARRAY)
                {
                    f.simpleType = TQ::TQ_FIELD_TYPE_CHOICE;
                    f.choiceTable = "Table_" + f.id;
                }
                /*
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
                */
            }
        }
        else
        {
            f.createShow = false;
            f.createRequired = false;
        }
        rdef->d->systemNames.insert(f.id, f.vid);
        {
            int i = 2;
            QString name = f.name;
            while(rdef->d->vids.contains(f.name))
                f.name = name + " " +QString::number(i++);
            rdef->d->vids.insert(f.name, f.vid);
            JiraFieldDesc *item = new JiraFieldDesc(f);
            rdef->d->fields.insert(f.vid, item);
        }
    }
}

void JiraProject::readRecordDef(JiraRecTypeDef *rdef, int recordType, const QVariantMap &typeMap)
{
    rdef->d->recType = recordType;
    rdef->d->recTypeName = typeMap.value("name").toString();

    QVariant obj = serverGet(QString("rest/api/2/issue/createmeta?projectIds=%1&issuetypeIds=%2&expand=projects.issuetypes.fields").arg(projectId).arg(recordType));
    QVariantMap createFields = db->parseValue(obj, "projects/0/issuetypes/0/fields").toMap();
    readRecordDef2(rdef, createFields);
    /*
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
            rdef->descVid = f.vid;
        else if(f.id == "summary")
            rdef->summaryVid = f.vid;
        else if(f.id == "assignee")
            rdef->assigneeVid = f.vid;
        else if(f.id == "creator")
            rdef->creatorVid = f.vid;
        else if(f.id == "created")
            rdef->createdVid = f.vid;
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
    */
}

void JiraProject::loadRecordTypes()
{
    QVariant obj = serverGet("rest/api/2/field");
    fieldList = obj.toList();
//    for(QMutableListIterator<QVariant> i(fieldList); i.hasNext(); )
//    {
//        QVariantMap map = i.next().toMap();
//        if(skippedFields.contains(map.value("id").toString()))
//            i.remove();
//    }
    typesList = serverGet("rest/api/2/issuetype").toList();
    foreach(const QVariant &t, typesList)
    {
        QVariantMap typeMap = t.toMap();
        int recordType = typeMap.value("id",0).toInt();
        if(!recordType)
            continue;

        JiraRecTypeDef *rdef = new JiraRecTypeDef(this);
        rdef->d->recType = recordType;
        rdef->d->recTypeName = typeMap.value("name").toString();

        QVariant obj = serverGet(QString("rest/api/2/issue/createmeta?projectIds=%1&issuetypeIds=%2&expand=projects.issuetypes.fields")
                                 .arg(projectId)
                                 .arg(recordType));
        QVariantMap createFields = db->parseValue(obj, "projects/0/issuetypes/0/fields").toMap();
        readRecordDef2(rdef, createFields);
//        readRecordDef(rdef, recordType, typeMap);
        recordDefs.insert(recordType, rdef);
        recordTypes.insert(rdef->d->recType, rdef->d->recTypeName);
    }
}

JiraRecTypeDef *JiraProject::loadEditRecordDef(const JiraRecord *record)
{
    JiraRecTypeDef *srcDef = (JiraRecTypeDef *)record->typeDef();
    JiraRecTypeDef *rdef = new JiraRecTypeDef(srcDef);
    /*
    rdef->d->recType = record->recordType();
    QVariantMap typeMap;
    foreach(const QVariant &t, typesList)
    {
        if(t.toMap().value("id",0).toInt() == rdef->d->recType)
        {
            typeMap = t.toMap();
            break;
        }
    }

    rdef->d->recTypeName = typeMap.value("name").toString();
    */
    QVariant obj = serverGet(QString("rest/api/2/issue/%1/editmeta").arg(record->jiraKey()));
    QVariantMap editFields = db->parseValue(obj, "fields").toMap();
    foreach(QString systemName, editFields.keys())
    {
        QVariantMap map = editFields.value(systemName).toMap();
        int vid = rdef->d->systemNames.value(systemName, TQ::TQ_NO_VID);
        if(vid != TQ::TQ_NO_VID)
        {
            JiraFieldDesc *fdesc = rdef->d->fields[vid];
            fdesc->editable = true;
            fdesc->autoCompleteUrl = map.value("autoCompleteUrl").toString();
            //f.createRequired = createMeta.value("required","false").toString() == "true";
            if(!systemChoices.contains(fdesc->id) && map.contains("allowedValues"))
            {
                QVariantList values = map.value("allowedValues").toList();
                fdesc->choices = parseAllowedValues(values);
                if(fdesc->simpleType != TQ::TQ_FIELD_TYPE_CHOICE && fdesc->simpleType != TQ::TQ_FIELD_TYPE_ARRAY)
                {
                    fdesc->simpleType = TQ::TQ_FIELD_TYPE_CHOICE;
                    fdesc->choiceTable = "Table_" + fdesc->id;
                }
            }
        }
    }

    return rdef;
}

TQChoiceList JiraProject::loadChoiceTables(JiraRecTypeDef *rdef, const QString &url)
{
    QVariant obj = serverGet("rest/api/2/"+url);
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

TQChoiceList JiraProject::parseAllowedValues(const QVariantList &values) const
{
    TQChoiceList list;
    int order = 0;
    foreach(QVariant value, values)
    {
        QVariantMap map = value.toMap();
        TQChoiceItem item;
        if(map.contains("value"))
            item.displayText = map.value("value").toString();
        else if(map.contains("name"))
            item.displayText = map.value("name").toString();
        else
            item.displayText = map.value("id").toString();
        item.fieldValue = map.value("id").toInt();
        item.id = map.value("id").toInt();
        item.weight = item.id;
        item.order = order++;
        list.append(item);
    }
    return list;
}

typedef QPair<QString,QString> QStringPair;
struct FilterDef {
    QString filterName;
    QString jql;
    int serverId;
    inline FilterDef(const QString &a_name, const QString &a_jql, int id = 0)
    {
        filterName = a_name;
        jql = a_jql;
        serverId = id;
    }
};

void JiraProject::loadQueries()
{
    QVariantList favs = serverGet("rest/api/2/filter/favourite").toList();
    foreach(QVariant v, favs)
    {
        QVariantMap favMap = v.toMap();
        JiraFilter item;
        item.name = favMap.value("name").toString();
        item.jql = favMap.value("jql").toString();
        item.serverId = favMap.value("id").toInt();
        item.isSystem = false;
        filters->append(item);
        favSearch.insert(item.name, filters->rowCount()-1);
    }
    QSqlDatabase udb = ttglobal()->userDatabase();
    udb.open();
    d->hasLocalDb = udb.isValid() && udb.isOpen();
    bool an = isAnonymousUser();
    bool needFillDb = false;
    if(d->hasLocalDb)
    {
        if(!udb.tables().contains("jiraqueries"))
        {
            udb.exec("create table jiraqueries(project varchar(255), login varchar(255), name varchar(255), jql varchar(255), serverId int)");
            needFillDb = udb.lastError().type() == QSqlError::NoError;
            udb.exec("create index jiraqueries_1 on jiraqueries(project, login)");
        }
        else
        {
            QSqlQuery q(udb);
            q.prepare("select count(*) from jiraqueries where project = ? and login =?");
            q.bindValue(0, strValue(this->projectName()));
            q.bindValue(1, strValue(this->currentUser()));
            q.exec();
            needFillDb = !q.next() || !q.value(0).toInt();
        }
        QSqlRecord rec = udb.record("jiraqueries");
        if(!rec.contains("serverId"))
        {
            udb.exec("alter table jiraqueries add column serverId int null");
        }

    }
    QList<FilterDef> preDef;
    if(needFillDb)
    {
        if(!an)
        {
            preDef
                    << FilterDef(tr("Мои открытые запросы"),
                                 QString("project = '%1' AND assignee = currentUser() AND resolution = Unresolved ORDER BY updatedDate DESC")
                                 .arg(name))
                    << FilterDef(tr("Созданные мной"),
                                 QString("project = '%1' AND reporter = currentUser() ORDER BY createdDate DESC")
                                 .arg(name));
        }
        preDef
                << FilterDef(tr("Все запросы"),
                             QString("project = '%1' ORDER BY createdDate DESC")
                             .arg(name))
                << FilterDef(tr("Открытые запросы"),
                             QString("project = '%1' AND resolution = Unresolved order by priority DESC,updated DESC")
                             .arg(name))
                << FilterDef(tr("Добавленные недавно"),
                             QString("project = '%1' AND created >= -1w order by created DESC")
                             .arg(name))
                << FilterDef(tr("Решенные недавно"),
                             QString("project = '%1' AND resolutiondate >= -1w order by updated DESC")
                             .arg(name))
                << FilterDef(tr("Обновленные недавно"),
                             QString("project = '%1' AND updated >= -1w order by updated DESC")
                             .arg(name))
                   ;

        QSqlQuery query(udb);
        query.prepare("insert into jiraqueries(project, login, name, jql) values(?,?,?,?)");
        foreach(const FilterDef &def, preDef)
        {
            query.bindValue(0, strValue(this->projectName()));
            query.bindValue(1, strValue(this->currentUser()));
            query.bindValue(2, strValue(def.filterName));
            query.bindValue(3, strValue(def.jql));
            query.exec();
        }
    }
    else
    {
        QSqlQuery query(udb);
        query.prepare("select name, jql, serverId from jiraqueries where project = ? and login = ?");
        query.bindValue(0, strValue(this->projectName()));
        query.bindValue(1, strValue(this->currentUser()));
        query.exec();
        while(query.next())
            preDef << FilterDef(query.value(0).toString(), query.value(1).toString(), query.value(2).toInt());
    }
    foreach(const FilterDef &def, preDef)
    {
        if(favSearch.contains(def.filterName))
            continue;
        JiraFilter item;
        item.name = def.filterName;
        item.jql = def.jql;
        item.serverId = def.serverId;
        item.isSystem = true;
        filters->append(item);
        favSearch.insert(item.name, filters->rowCount()-1);
    }
}

void JiraProject::loadUsers()
{
    m_userList.clear();
    QVariantList owners = serverGet("rest/api/2/user/assignable/search?project="+projectKey).toList();
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

QVariant JiraProject::postFile(JiraRecord *record, const QString &fileName, QFile *src)
{
    QVariant res = db->sendFile(db->queryUrl(QString("rest/api/2/issue/%1/attachments").arg(record->jiraKey())), fileName, src);
    if(db->lastHTTPCode() && db->lastHTTPCode() != 200)
        emit error(db->lastHTTPCode(), db->lastHTTPErrorString());
//    bool wasError = db->lastHTTPCode() && db->lastHTTPCode() != 200 ;
//    return wasError;
    return res;
}

bool JiraProject::removeFile(int fileId)
{
    QVariant res = serverDelete(QString("rest/api/2/attachment/%1").arg(fileId));
    bool wasError = db->lastHTTPCode() && db->lastHTTPCode() != 204 ;
    return wasError;
}

QList<QAction *> JiraProject::actions(const TQRecordList &records)
{
    QList<QAction *> list;
    foreach(TQRecord *rec, records)
    {
        JiraRecord *jrec = qobject_cast<JiraRecord *>(rec);
        if(!jrec)
            continue;
        QList<QAction *> reclist = recordActionList(jrec);
        foreach (QAction *a, reclist) {
            if(!list.contains(a))
                list.append(a);
        }
    }
    return list;
}

QList<QAction *> JiraProject::recordActionList(const JiraRecord *record)
{
    if(record->mode() == TQRecord::Insert)
        return QList<QAction *>();
    QList<QAction *> list;
    QVariant res = serverGet(QString("rest/api/2/issue/%1/transitions?expand=transitions.fields")
                             .arg(record->jiraKey()));
    bool wasError = db->lastHTTPCode();
    if(wasError)
        return list;
    QVariantList tList = res.toMap().value("transitions").toList();
    foreach(QVariant item, tList)
    {
        QVariantMap map = item.toMap();
        QString id = map.value("id").toString();
        if(id.isEmpty())
            continue;
        if(d->actions.contains(id))
            list.append(d->actions.value(id));
        else
        {
            QAction *action = new QAction(this);
            action->setText(map.value("name").toString());
            action->setData(id);
            d->actions.insert(id, action);
            list.append(action);
//            connect(action, SIGNAL(triggered(bool)), SLOT(recordActionTrggered()));
        }
    }
    return list;
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

void JiraProject::onActionTriggered(TQViewController *controller, QAction *action)
{
    if(!d->actions.values().contains(action))
        return;
    QString id = action->data().toString();
    if(id.isEmpty() || !d->actions.contains(id))
        return;
    QObjectList list = controller->selectedRecords();
    foreach (QObject *obj, list) {
        JiraRecord *rec = qobject_cast<JiraRecord *>(obj);
        recordActionTrggered(rec, action);
    }
}

void JiraProject::showSelectUser()
{
}

void JiraProject::recordActionTrggered(JiraRecord *record, QAction *action)
{
//    QAction *action = qobject_cast<QAction*>(sender());
    if(!action)
        return;
    QString id = action->data().toString();
    if(id.isEmpty())
        return;
    QVariantMap body, t;
    t["id"] = id;
    body["transition"] = t;
    QVariant res = serverPost(QString("rest/api/2/issue/%1/transitions")
                              .arg(record->jiraKey()),
                              body);
    bool wasError = db->lastHTTPCode();
    if(wasError)
        return;
    record->refresh();
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

const JiraFilter *JiraFilterModel::filter(const QString &filterName) const
{
    for(int row = 0; row<rowCount(); row++)
    {
        const JiraFilter &f = at(row);
        if(f.name.compare(filterName, Qt::CaseInsensitive) == 0)
            return &f;
    }
    return 0;
}

QVariant JiraFilterModel::displayColData(const JiraFilter &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.name;
    case 1:
        return "all"; //rec.serverId ? "server" : "local";
    }
    return QVariant();
}
