#include <tqplug.h>
//#include "tracker.h"
#include "trkview.h"
#ifdef CLIENT_APP
#include "settings.h"
#include "ttglobal.h"
#include <tqqrywid.h>
#ifdef TRKVIEE_NEED
#include "trkdecorator.h"
#include <QHeaderView>
#include "messager.h"
#endif
#endif
#include "ttutils.h"
#include "trkcond.h"
#include "trkplugin.h"

#include <QSettings>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <tqviewcontroller.h>
//#include <QXmlQuery>
#include <QAuthenticator>
#include <tqjson.h>
//#include <QtWebKit>
//#include <QtSql>
//#include <Windows.h>
//#include <odbcinst.h>
//#include <sqlext.h>


int isTrkOK(int result, bool show = true)
{
    if(result ==  TRK_SUCCESS)
        return true;
    QString error;
    switch(result)
    {
    case TRK_SUCCESS:
        return true;
    case TRK_E_END_OF_LIST			       :
        return false;
    case TRK_E_VERSION_MISMATCH		       :    error = QObject::tr("TRK_E_VERSION_MISMATCH"); break;
    case TRK_E_OUT_OF_MEMORY		 	   :    error = QObject::tr("TRK_E_OUT_OF_MEMORY"); break;
    case TRK_E_BAD_HANDLE			       :    error = QObject::tr("TRK_E_BAD_HANDLE"); break;
    case TRK_E_BAD_INPUT_POINTER		   :    error = QObject::tr("TRK_E_BAD_INPUT_POINTER"); break;
    case TRK_E_BAD_INPUT_VALUE		       :    error = QObject::tr("TRK_E_BAD_INPUT_VALUE"); break;
    case TRK_E_DATA_TRUNCATED		       :    error = QObject::tr("TRK_E_DATA_TRUNCATED"); break;
    case TRK_E_NO_MORE_DATA			       :    error = QObject::tr("TRK_E_NO_MORE_DATA"); break;
    case TRK_E_LIST_NOT_INITIALIZED	       :    error = QObject::tr("TRK_E_LIST_NOT_INITIALIZED"); break;
//    case TRK_E_END_OF_LIST			       :    error = QObject::tr("TRK_E_END_OF_LIST"); break;
    case TRK_E_NOT_LOGGED_IN			   :    error = QObject::tr("TRK_E_NOT_LOGGED_IN"); break;
    case TRK_E_SERVER_NOT_PREPARED	       :    error = QObject::tr("TRK_E_SERVER_NOT_PREPARED"); break;
    case TRK_E_BAD_DATABASE_VERSION	       :    error = QObject::tr("TRK_E_BAD_DATABASE_VERSION"); break;
    case TRK_E_UNABLE_TO_CONNECT		   :    error = QObject::tr("TRK_E_UNABLE_TO_CONNECT"); break;
    case TRK_E_UNABLE_TO_DISCONNECT	       :    error = QObject::tr("TRK_E_UNABLE_TO_DISCONNECT"); break;
    case TRK_E_UNABLE_TO_START_TIMER	   :    error = QObject::tr("TRK_E_UNABLE_TO_START_TIMER"); break;
    case TRK_E_NO_DATA_SOURCES		       :    error = QObject::tr("TRK_E_NO_DATA_SOURCES"); break;
    case TRK_E_NO_PROJECTS			       :    error = QObject::tr("TRK_E_NO_PROJECTS"); break;
    case TRK_E_WRITE_FAILED			       :    error = QObject::tr("TRK_E_WRITE_FAILED"); break;
    case TRK_E_PERMISSION_DENIED		   :    error = QObject::tr("TRK_E_PERMISSION_DENIED"); break;
    case TRK_E_SET_FIELD_DENIED		       :    error = QObject::tr("TRK_E_SET_FIELD_DENIED"); break;
    case TRK_E_ITEM_NOT_FOUND		       :    error = QObject::tr("TRK_E_ITEM_NOT_FOUND"); break;
    case TRK_E_CANNOT_ACCESS_DATABASE      :    error = QObject::tr("TRK_E_CANNOT_ACCESS_DATABASE"); break;
    case TRK_E_CANNOT_ACCESS_QUERY	       :    error = QObject::tr("TRK_E_CANNOT_ACCESS_QUERY"); break;
    case TRK_E_CANNOT_ACCESS_INTRAY	       :    error = QObject::tr("TRK_E_CANNOT_ACCESS_INTRAY"); break;
    case TRK_E_CANNOT_OPEN_FILE		       :    error = QObject::tr("TRK_E_CANNOT_OPEN_FILE"); break;
    case TRK_E_INVALID_DBMS_TYPE		   :    error = QObject::tr("TRK_E_INVALID_DBMS_TYPE"); break;
    case TRK_E_INVALID_RECORD_TYPE	       :    error = QObject::tr("TRK_E_INVALID_RECORD_TYPE"); break;
    case TRK_E_INVALID_FIELD			   :    error = QObject::tr("TRK_E_INVALID_FIELD"); break;
    case TRK_E_INVALID_CHOICE		       :    error = QObject::tr("TRK_E_INVALID_CHOICE"); break;
    case TRK_E_INVALID_USER			       :    error = QObject::tr("TRK_E_INVALID_USER"); break;
    case TRK_E_INVALID_SUBMITTER		   :    error = QObject::tr("TRK_E_INVALID_SUBMITTER"); break;
    case TRK_E_INVALID_OWNER			   :    error = QObject::tr("TRK_E_INVALID_OWNER"); break;
    case TRK_E_INVALID_DATE			       :    error = QObject::tr("TRK_E_INVALID_DATE"); break;
    case TRK_E_INVALID_STORED_QUERY	       :    error = QObject::tr("TRK_E_INVALID_STORED_QUERY"); break;
    case TRK_E_INVALID_MODE			       :    error = QObject::tr("TRK_E_INVALID_MODE"); break;
    case TRK_E_INVALID_MESSAGE		       :    error = QObject::tr("TRK_E_INVALID_MESSAGE"); break;
    case TRK_E_VALUE_OUT_OF_RANGE	       :    error = QObject::tr("TRK_E_VALUE_OUT_OF_RANGE"); break;
    case TRK_E_WRONG_FIELD_TYPE		       :    error = QObject::tr("TRK_E_WRONG_FIELD_TYPE"); break;
    case TRK_E_NO_CURRENT_RECORD		   :    error = QObject::tr("TRK_E_NO_CURRENT_RECORD"); break;
    case TRK_E_NO_CURRENT_NOTE		       :    error = QObject::tr("TRK_E_NO_CURRENT_NOTE"); break;
    case TRK_E_NO_CURRENT_ATTACHED_FILE    :    error = QObject::tr("TRK_E_NO_CURRENT_ATTACHED_FILE"); break;
    case TRK_E_NO_CURRENT_ASSOCIATION      :    error = QObject::tr("TRK_E_NO_CURRENT_ASSOCIATION"); break;
    case TRK_E_NO_RECORD_BEGIN		       :    error = QObject::tr("TRK_E_NO_RECORD_BEGIN"); break;
    case TRK_E_NO_MODULE				   :    error = QObject::tr("TRK_E_NO_MODULE"); break;
    case TRK_E_USER_CANCELLED		       :    error = QObject::tr("TRK_E_USER_CANCELLED"); break;
    case TRK_E_SEMAPHORE_TIMEOUT		   :    error = QObject::tr("TRK_E_SEMAPHORE_TIMEOUT"); break;
    case TRK_E_SEMAPHORE_ERROR		       :    error = QObject::tr("TRK_E_SEMAPHORE_ERROR"); break;
    case TRK_E_INVALID_SERVER_NAME	       :    error = QObject::tr("TRK_E_INVALID_SERVER_NAME"); break;
    case TRK_E_NOT_LICENSED			       :    error = QObject::tr("TRK_E_NOT_LICENSED"); break;
    case TRK_E_RECORD_LOCKED			   :
        error = QObject::tr("TRK_E_RECORD_LOCKED");
        break;
    case TRK_E_RECORD_NOT_LOCKED		   :    error = QObject::tr("TRK_E_RECORD_NOT_LOCKED"); break;
    case TRK_E_UNMATCHED_PARENS		       :    error = QObject::tr("TRK_E_UNMATCHED_PARENS"); break;
    case TRK_E_NO_CURRENT_TRANSITION	   :    error = QObject::tr("TRK_E_NO_CURRENT_TRANSITION"); break;
    case TRK_E_NO_CURRENT_RULE		       :    error = QObject::tr("TRK_E_NO_CURRENT_RULE"); break;
    case TRK_E_UNKNOWN_RULE			       :    error = QObject::tr("TRK_E_UNKNOWN_RULE"); break;
    case TRK_E_RULE_ASSERTION_FAILED	   :    error = QObject::tr("TRK_E_RULE_ASSERTION_FAILED"); break;
    case TRK_E_ITEM_UNCHANGED		       :    error = QObject::tr("TRK_E_ITEM_UNCHANGED"); break;
    case TRK_E_TRANSITION_NOT_ALLOWED      :    error = QObject::tr("TRK_E_TRANSITION_NOT_ALLOWED"); break;
    case TRK_E_NO_CURRENT_STYLESHEET	   :    error = QObject::tr("TRK_E_NO_CURRENT_STYLESHEET"); break;
    case TRK_E_NO_CURRENT_FORM		       :    error = QObject::tr("TRK_E_NO_CURRENT_FORM"); break;
    case TRK_E_NO_CURRENT_VALUE		       :    error = QObject::tr("TRK_E_NO_CURRENT_VALUE"); break;
    case TRK_E_FORM_FIELD_ACCESS		   :    error = QObject::tr("TRK_E_FORM_FIELD_ACCESS"); break;
    case TRK_E_INVALID_QBID_STRING	       :    error = QObject::tr("TRK_E_INVALID_QBID_STRING"); break;
    case TRK_E_FORM_INVALID_FIELD	       :    error = QObject::tr("TRK_E_FORM_INVALID_FIELD"); break;
    case TRK_E_PARTIAL_SUCCESS		       :    error = QObject::tr("TRK_E_PARTIAL_SUCCESS"); break;
    }
    if(error.isEmpty())
        error = QString(QObject::tr("Tracker Error %1")).arg(result);
#ifdef CLIENT_APP

    if(show)
        ttShowError(error);
//        TTGlobal::global()->showError(error);
#endif
    return false;
}


static TRK_UINT Do_TrkSetNoteData(TRK_NOTE_HANDLE noteHandle, const QByteArray &data)
{
    return TrkSetNoteData(noteHandle, data.size(), data.constData(), 0);
}

static TRK_UINT Do_TrkSetDescription(TRK_RECORD_HANDLE recHandle, const QByteArray &data)
{
    return TrkSetDescriptionData(recHandle, data.size(), data.constData(), 0);
}

#ifdef CLIENT_APP
#ifdef TRK_VIEW_NEED
// =================== TrkView ===============
TrkView::TrkView(QWidget *parent) : 
	QTableView(parent),
	trkmodel(parent)
{
	isInteractive = true;
	isDefLoaded = false;
}

void TrkView::headerChanged()
{
	if(isInteractive)
        TTGlobal::global()->settings()->setValue("TrackerGrid", horizontalHeader()->saveState());
}

void TrkView::headerToggled(bool checked)
{
	QAction *a = qobject_cast<QAction*>(sender());
	if(a)
	{
		int i = headerActions.indexOf(a);
		if(i>=0)
			horizontalHeader()->setSectionHidden(i,!checked);
	}
}

void TrkView::initPopupMenu()
{
	QHeaderView *hv=horizontalHeader();
	for(int i=0; i<hv->count(); i++)
	{
		QString label = model()->headerData(i,Qt::Horizontal).toString().trimmed(); 
		QAction *action = new QAction(label,this);
		action->setCheckable(true);
		action->setChecked(!hv->isSectionHidden(i));
		connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
		//hv->addAction(action);
		headerActions.append(action);
	}
	hv->addActions(headerActions);
	hv->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void TrkView::setQuery(int id, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQuery(id,trkdb);
	setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void TrkView::setQueryById(const QString& numbers, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQueryById(numbers,trkdb);
	setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void TrkView::loadDefinition()
{
    decorator->loadViewDef(this);
	isDefLoaded = true;
	initPopupMenu();
}

int TrkView::findColumn(const QString &label) const
{
	QHeaderView *hv = horizontalHeader();
	QAbstractItemModel *model = hv->model();
	for(int i=0; i<model->columnCount(); i++)
	{
		if(!QString::compare(
			label.trimmed(),
			model->headerData(i,Qt::Horizontal).toString().trimmed()))
			return i;
	}
	return -1;
}
#endif
#endif
//============================================
static QString findRegDSN(const QString &desc)
{
    // HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\ODBC\ODBC.INI\ODBC Data Sources
    QSettings sets("ODBC","ODBC.INI");
    sets.beginGroup("ODBC Data Sources");
    QStringList sources = sets.allKeys();
    sets.endGroup();
    foreach(QString dsn, sources)
    {
        sets.beginGroup(dsn);
        if(sets.value("Description").toString() == desc)
            return dsn;
        sets.endGroup();
    }
    return QString();
}

static QString findRegServer(const QString &desc)
{
    // HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\ODBC\ODBC.INI\ODBC Data Sources
    QSettings sets("ODBC","ODBC.INI");
    sets.beginGroup("ODBC Data Sources");
    QStringList sources = sets.allKeys();
    sets.endGroup();
    foreach(QString dsn, sources)
    {
        sets.beginGroup(dsn);
        if(sets.value("Description").toString() == desc)
            return sets.value("Server").toString();
        sets.endGroup();
    }
    return QString();
}

// =============== TrkToolDB =================
TrkToolDB::TrkToolDB(TrkPlugin *pluginObject, QObject *parent)
    :TQAbstractDB(parent), dbmsTypeList()
{
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
    plugin = pluginObject;
}

TrkToolDB::~TrkToolDB()
{
    TrkHandleFree(&handle);
}

void TrkToolDB::setDbmsType(const QString &dbType)
{
    TQAbstractDB::setDbmsType(dbType);

    // HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\ODBC\ODBC.INI\ODBC Data Sources
    QSettings sets("ODBC","ODBC.INI");
    sets.beginGroup("ODBC Data Sources");
    QStringList sources = sets.allKeys();
    sets.endGroup();
    foreach(QString dsn, sources)
    {
        sets.beginGroup(dsn);
        if(sets.value("Description").toString() == dbType)
        {
            odbcDSN = dsn;
            odbcServer = sets.value("Server").toString();
            break;
        }
        sets.endGroup();
    }
}

QStringList TrkToolDB::dbmsTypes()
{
	if(dbmsTypeList.isEmpty())
        if(isTrkOK(TrkInitDBMSTypeList(handle)))
		{
			char buf[1024];
            while(TRK_SUCCESS == TrkGetNextDBMSType(handle, 1024, buf))
				dbmsTypeList.append(QString::fromLocal8Bit(buf));
		}
    return dbmsTypeList;
}

QString TrkToolDB::dbmsServer() const
{
    QString customServer = TQAbstractDB::dbmsServer();
    if(!customServer.isEmpty())
        return customServer;
    return odbcServer;
//    QString server = findRegServer(dbmsType());
//    return server;
}

QStringList TrkToolDB::projects(const QString &dbmsType, const QString &user, const QString &pass)
{
    Q_UNUSED(user)
    Q_UNUSED(pass)
	if(!projectList.contains(dbmsType) || projectList[dbmsType].isEmpty())
	{
		TRK_UINT res;
        if(dbmsUser().isEmpty())
			res = TrkInitProjectList(handle, 
            dbmsType.toLocal8Bit().constData(),
            dbmsServer().toLocal8Bit().constData(),
			"", 
			"", 
			TRK_USE_DEFAULT_DBMS_LOGIN);
		else
			res = TrkInitProjectList(handle, 
			dbmsType.toLocal8Bit().constData(), 
            dbmsServer().toLocal8Bit().constData(),
            dbmsUser().toLocal8Bit().constData(),
            dbmsPass().toLocal8Bit().constData(),
			TRK_USE_SPECIFIED_DBMS_LOGIN);
        if(isTrkOK(res))
		{
			char buf[1024];
			QStringList list;
            while(TRK_SUCCESS == TrkGetNextProject(handle, 1024, buf))
				list.append(QString::fromLocal8Bit(buf));
			projectList[dbmsType] = list;
		}
	}
    return projectList[dbmsType];
}

TQAbstractProject *TrkToolDB::openProject(
	const QString &projectName,
	const QString &user, 
	const QString &pass)
{
	TrkToolProject *prj = new TrkToolProject(this);
    prj->login(user, pass, projectName);
    openedProjects[projectName]=prj;
    registerProject(prj);
    return prj;
}

TQAbstractProject *TrkToolDB::openConnection(const QString &connectString)
{
    TQJson parser;
    QVariantMap map = parser.toVariant(connectString).toMap();
//    QStringList values = connectString.split(";");
    QHash<QString, QString> params;
    foreach(QString key, map.keys())
    {
//        int p = v.indexOf("=");
//        QString par = v.left(p);
//        QString val = v.mid(p+1);
        params.insert(key, map.value(key).toString());
    }
    QString sRecType = params.value("RecordType");
    bool okRecType;
    int recType = sRecType.toInt(&okRecType);

    setDbmsType(params.value(DBPARAM_TYPE));
    setDbmsServer(params.value(DBPARAM_SERVER));
    if(!map.value(DBPARAM_OSUSER).toBool())
        setDbmsUser(params.value(DBPARAM_USER),params.value(DBPARAM_PASSWORD));
    else
        setDbmsUser("","");
    TQAbstractProject *prj = openProject(params.value(PRJPARAM_NAME),
                                         params.value(PRJPARAM_USER),
                                         params.value(PRJPARAM_PASSWORD));
    return prj;
}

QHash<QString, TrkToolProject *> TrkToolDB::openedProjects;

TQAbstractProject *TrkToolDB::getProject(const QString &projectName)
{
    return openedProjects[projectName];
}

QDir TrkToolDB::dataDir() const
{
    if(plugin)
        return plugin->dataDir;
    return "data";
}

QSqlDatabase TrkToolDB::openSqlDatabase()
{
    QSqlDatabase sqlDb = QSqlDatabase::addDatabase("QODBC","trackerquery");
    sqlDb.setDatabaseName(odbcDSN);
    sqlDb.setUserName(dbmsUser());
    sqlDb.setPassword(dbmsPass());
    sqlDb.open();
    return sqlDb;
}

// ============= TrkToolProject ==============
TrkToolProject::TrkToolProject(TrkToolDB *parent)
    : TQAbstractProject(parent),
      qList()
    , handle(0)
    , theQueryModel()
    , opened(false)
    , selected()
    , handlers()
{
    db=parent;
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
    setProperty("groupFile", QString("data/tracker.xml"));
}

TrkToolProject::~TrkToolProject()
{
    close();
}

bool TrkToolProject::isOpened() const
{
    return opened;
}

QString TrkToolProject::currentUser() const
{
    return user;
}

QString TrkToolProject::projectName() const
{
    return name;
}

QList<QAction *> TrkToolProject::actions(const TQRecordList &records)
{
    QList<QAction *> res;
    /*
#ifdef CLIENT_APP
    static QAction *a = 0;
    if(!a)
        a = new QAction("Unlock",0);
    res << a;
#endif
*/
    return res;
}

void TrkToolProject::onActionTriggered(TQViewController *controller, QAction *action)
{
    /*
    QObjectList list = controller->selectedRecords();
    foreach (QObject *obj, list) {
        TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(obj);
        if(!obj)
            continue;
        unlockRecord(rec);
    }
    */
}

bool TrkToolProject::unlockRecord(TrkToolRecord *record)
{
    TrkScopeRecHandle h(this, record);
    TRK_RECORD_HANDLE &handle = h.nativeHandle();
    int type = record->recordType();
    unsigned long res = TrkUpdateRecordBeginEx(handle, true);
    if(!isTrkOK(res))
        return false;
    res = TrkRecordCancelTransaction(handle);
    return isTrkOK(res);
}


bool TrkToolProject::login(const QString &userName,
        const QString &password,
        const QString &project)
{
	TRK_UINT res;
	TRK_UINT mode;
    QString user = userName;
    QString pass = password;
    QString dbmsType = db->dbmsType();
    QString dbmsServer = db->dbmsServer();
    QString dbmsUser = db->dbmsUser();
    QString dbmsPass = db->dbmsPass();

    if(user.isEmpty())
    {
        QAuthenticator auth;
        auth.setOption("project", project);
        emit db->projectAuthenticationRequired(db, project, &auth);
        if(auth.isNull())
        {
            return false;
        }
        user = auth.user();
        pass = auth.password();
    }

    if(!dbmsType.isEmpty())
    {
        if(dbmsUser.isEmpty())
            mode = TRK_USE_DEFAULT_DBMS_LOGIN;
        else
            mode = TRK_USE_SPECIFIED_DBMS_LOGIN;
        res = TrkProjectLogin(handle,
                              user.toLocal8Bit().constData(),
                              pass.toLocal8Bit().constData(),
                              project.toLocal8Bit().constData(),
                              dbmsType.toLocal8Bit().constData(),
                              dbmsServer.toLocal8Bit().constData(),
                              dbmsUser.toLocal8Bit().constData(),
                              dbmsPass.toLocal8Bit().constData(),
                              mode);
    }
    else if(!dbmsServer.isEmpty())
    {
        res = TrkProjectLoginEx(handle,
                                user.toLocal8Bit().constData(),
                                pass.toLocal8Bit().constData(),
                                project.toLocal8Bit().constData(),
                                dbmsServer.toLocal8Bit().constData());
    }
    if(isTrkOK(res))
	{
		opened = true;
		name = project;
        this->user = user;
        readProjectDatabaseName();
        readNoteTitles();
        readUserList();
        readQueryList();
		readDefs();
	}
    else
    {
        QString text = tr("Ошибка открытия проекта %1 (Error code %2)").arg(project).arg(res);
#ifdef CONSOLE_APP
        qDebug() << text;
#else
        QMessageBox::critical(0,tr("Ошибка подключения"), text);
#endif
    }
	return opened;
}

void TrkToolProject::close()
{
	TrkHandleFree(&handle);
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
	recordTypes.clear();
    foreach(TrkRecordTypeDef *p, recordDef)
		delete p;
    recordDef.clear();
	opened = false;
    db->openedProjects.remove(name);
//    foreach(QStringList *list, qList)
//        delete list;
}

QStringList TrkToolProject::queryList(TRK_RECORD_TYPE type) const
{
    const QStringList list = qList[type];
    return list;
}

bool TrkToolProject::readQueryList()
{
    bool rc = isTrkOK(TrkInitQueryNameList(handle));
	if(!rc)
		return false;
	char buf[256];
	TRK_RECORD_TYPE recType;
    QTextCodec *codec = QTextCodec::codecForLocale();
    while(TRK_SUCCESS == TrkGetNextQueryName(handle, 256, buf, &recType))
	{
        QString name =  codec->toUnicode(buf);
        if(!qList.contains(recType))
            qList.insert(recType, QStringList());
        qList[recType].append(name);
	}
    initQueryModel();
    return rc;
}

bool TrkToolProject::refreshQueryList()
{
    QHash<TRK_RECORD_TYPE, QStringList> newQList; //QueryList
    TRK_UINT res = TRK_SCR_TYPE;
    unsigned long r = TrkProjectRefresh(handle, res);
    bool rc = isTrkOK(TrkInitQueryNameList(handle));
    if(!rc)
        return false;
    char buf[256];
    TRK_RECORD_TYPE recType=0;
    while(TRK_SUCCESS == TrkGetNextQueryName(handle, 256, buf, &recType))
    {
        QString name = QString::fromLocal8Bit(buf);
        TrkGetQueryRecordType(handle, buf, &recType);
        if(!newQList.contains(recType))
            newQList[recType] = QStringList();
        newQList[recType].append(name);
        recType=0;
    }
    foreach(recType, newQList.keys())
    {
        QStringList &nlist = newQList[recType];
        QStringList &olist = qList[recType];
        QSet<QString> nSet = nlist.toSet();
        QSet<QString> oSet = olist.toSet();
        QSet<QString> added = nSet - oSet;
        QSet<QString> removed = oSet - nSet;

        TrkToolQryModel *model  = theQueryModel.value(recType);
        if(!model)
        {
            model = new TrkToolQryModel(this);
            theQueryModel.insert(recType, model);
        }
        foreach(QString qryName, removed)
            model->removeQry(qryName, recType);
        foreach(QString qryName, added)
        {
            TRK_UINT res=0;

            TrkGetQueryIsPublic(handle, qryName.toLocal8Bit().constData(), &res);
            model->appendQry(qryName, (res!=0), recType);
        }
    }
    qList = newQList;
    return rc;
}

bool TrkToolProject::readDefs()
{
	recordTypes.clear();
	//bool rc;
    if(!isTrkOK(TrkInitRecordTypeList(handle)))
		return false;
	TRK_RECORD_TYPE recType;
    while(TRK_SUCCESS == TrkGetNextRecordType(handle, &recType))
	{
		char buf[1024];
        isTrkOK(TrkGetRecordTypeName(handle, recType, 1024, buf));
		recordTypes[recType] = QString::fromLocal8Bit(buf);
	}
	QHash<TRK_RECORD_TYPE, QString>::const_iterator ri;
	for(ri = recordTypes.constBegin(); ri != recordTypes.constEnd(); ++ri)
	{
        recType = ri.key();
        if(!recordDef.contains(ri.key()))
        {
            TrkRecordTypeDef *p = new TrkRecordTypeDef(this);
            recordDef[ri.key()] = p;
            p->recType = ri.key();
            p->recTypeName = ri.value();
        }
        TrkVidDefs def;
        if(isTrkOK(TrkInitFieldList(handle, ri.key())))
		{
            char bufname[1024];
			TRK_FIELD_TYPE fType;
            while(TRK_SUCCESS == TrkGetNextField(handle, 1024, bufname, &fType))
			{
				TRK_VID vid;
                TrkGetFieldVid(handle, bufname, ri.key(), &vid);
                QString fieldName = QString::fromLocal8Bit(bufname);
                TRK_BOOL nullValid=0;
                TrkGetFieldIsNullValid(handle,bufname,ri.key(), &nullValid);
                recordDef[ri.key()]->nameVids[fieldName] = vid;
                TrkRecordTypeDef *rDef = recordDef[ri.key()];
                TrkFieldDef *id = new TrkFieldDef(rDef,vid);
                id->name = fieldName;
                id->nativeType = fType;
                id->nullable = nullValid;

                if(fType == TRK_FIELD_TYPE_NUMBER)
                {
                    TRK_UINT minV=0, maxV=INT_MAX;
                    if(TRK_SUCCESS == TrkGetFieldRange(handle,bufname,ri.key(),&minV,&maxV))
                    {
                        id->minValue = minV;
                        id->maxValue = maxV;
                    }
                    TRK_UINT v;
                    if(TRK_SUCCESS == TrkGetFieldDefaultNumericValue(handle,bufname,ri.key(),&v))
                        id->defaultValue = QVariant::fromValue<int>(v);
                    else
                        id->defaultValue = 0;
                }
                if(fType == TRK_FIELD_TYPE_STRING
                        || fType ==  TRK_FIELD_TYPE_SUBMITTER
                        || fType ==  TRK_FIELD_TYPE_OWNER
                        || fType ==  TRK_FIELD_TYPE_USER
                        || fType ==  TRK_FIELD_TYPE_STATE
                        || fType ==  TRK_FIELD_TYPE_CHOICE
                        )
                {
                    char buf[1024];
                    if(TRK_SUCCESS == TrkGetFieldDefaultStringValue(handle,bufname,ri.key(),sizeof(buf),buf))
                        id->defaultValue = QString::fromLocal8Bit(buf);
                    else
                        id->defaultValue = "";
                }

                //id.p_choiceList->clear();
                /*
                if((
                            (fType == TRK_FIELD_TYPE_CHOICE) || (fType == TRK_FIELD_TYPE_STATE)
                            )
                    && TRK_SUCCESS == TrkInitChoiceList(handle, bufname, ri.key()))
				{
					TrkToolChoice ch;
					char chname[1024];
					int order = 0;
					while(TRK_SUCCESS == TrkGetNextChoice(handle, sizeof(chname), chname))
					{
                        ch.displayText = QString::fromLocal8Bit(chname);
                        ch.fieldValue = ch.displayText;
                        //ch.order = order++;
                        //ch.weight = 0;
                        id.p_choiceList->append(ch);
					}
				}
                */
                def[vid] =  id;
//				if(!nameVids.contains(ri.key()))
//					nameVids[ri.key()] = NameVid();
//				nameVids[ri.key()][id.name] = vid;
            }
            QSqlDatabase sqlDb = db->openSqlDatabase();
            QSqlQuery query(sqlDb);
            query.prepare("select fldVid, fldId from "+dbName+"..trkfld where fldTypeId = ?");
            int iRecType = recType;
            query.bindValue(0, iRecType);
            if(query.exec())
                while(query.next())
                {
                    int vid = query.value(0).toInt();
                    int id = query.value(1).toInt();
                    if(def.contains(vid))
                    {
                        TrkFieldDef *fdef = def[vid];
                        fdef->internalId = id;
                    }
                }
            else
                qDebug() << query.lastError().text();
        }
        recordDef[ri.key()]->fieldDefs = def;
        if(def.contains(VID_Id))
            recordDef[ri.key()]->baseFields[VID_Id] = def[VID_Id]->name;
        if(def.contains(VID_Title))
            recordDef[ri.key()]->baseFields[VID_Title] = def[VID_Title]->name;
        if(def.contains(VID_Owner))
            recordDef[ri.key()]->baseFields[VID_Owner] = def[VID_Owner]->name;
        if(def.contains(VID_Submitter))
            recordDef[ri.key()]->baseFields[VID_Submitter] = def[VID_Submitter]->name;
        //recordDef[ri.key()].rectype = ri.key();
	}
    return true;
}

void TrkToolProject::readUserList()
{
    QMap<QString, QString> loginByName;
    m_userList.clear();
    int index;
    if(isTrkOK(TrkInitUserList(handle)))
    {
        char userName[1024];
        index=0;
        while(TRK_SUCCESS == TrkGetNextUser(handle, sizeof(userName), userName)) //logins
        {
            TQUser item;
            item.login = QString::fromLocal8Bit(userName);
            char fullName[1024];
            if(TRK_SUCCESS == TrkGetUserFullName(handle, userName, sizeof(fullName), fullName))
                item.fullName = QString::fromLocal8Bit(fullName);
            else
                item.fullName = item.login;
            item.displayName = item.fullName;
            item.id = 0;
            item.isDeleted = false;
            m_userList.insert(item.login,item);
            loginByName.insert(item.fullName, item.login);
        }
    }
    if(isTrkOK(TrkInitUserList(handle)))
    {
        char buf[1024];
        TRK_UINT id = 0;
        TRK_UINT deleted = 0;
        index = 0;
        while(TRK_SUCCESS == TrkGetNextUserInternal(handle, sizeof(buf), buf, &id, &deleted)) //fullname + id +
        {
            QString userFullName = QString::fromLocal8Bit(buf);
            if(loginByName.contains(userFullName))
            {
                QString login = loginByName.value(userFullName);
                TQUser item = m_userList.value(login);
                item.id = id;
                item.isDeleted = deleted;
                m_userList.insert(item.login,item);
            }
            index++;
        }
    }
}

void TrkToolProject::readNoteTitles()
{
    noteTitles.clear();
/*
    QFile file("data/tracker.xml");
    QXmlInputSource source(&file);
    QDomDocument dom;
    if(!dom.setContent(&source,false))
        return;
    QDomElement doc = dom.documentElement();
    QDomElement notes = doc.firstChildElement("notes");
    if(!notes.isNull())
    {
        for(QDomElement title = notes.firstChildElement("title");
            !title.isNull();
            title = title.nextSiblingElement("title"))
        {
            noteTitles.append(title.text());
        }
    }

*/

    TrkScopeRecHandle recHandle(this);
//    if(!isTrkOK(TrkNewRecordBegin(*recHandle,defaultRecType())))
//        return;
    TrkScopeNoteHandle noteHandle(*recHandle);
//    TRK_RECORD_HANDLE rr = (TRK_RECORD_HANDLE)defaultRecType();

    if(isTrkOK(TrkInitNoteTitleList(*noteHandle)))
    {
        char buf[1024];
        TRK_UINT type; // 1 - user title, 2 - system title
        while(TRK_SUCCESS == TrkGetNextNoteTitle(*noteHandle, sizeof(buf), buf, &type))
        {
            noteTitles.append(QString::fromLocal8Bit(buf));
        }
    }
//    TrkRecordCancelTransaction(*recHandle);
}

void TrkToolProject::initQueryModel(int type)
{
    foreach(TrkToolQryModel *model, theQueryModel)
        model->clearRecords();
    //theQueryModel.headers << tr("Query Name");
	if(!qList.isEmpty() && qList.contains(type))
     {
        foreach(const QString &qryName, qList[type])
        {
            TRK_UINT res=0;

            TrkGetQueryIsPublic(handle, qryName.toLocal8Bit().constData(), &res);
            TrkToolQryModel *model;
            if(theQueryModel.contains(type))
                model = theQueryModel[type];
            else
            {
                model = new TrkToolQryModel(this);
                theQueryModel.insert(type, model);
            }
            model->appendQry(qryName, (res!=0), type);
        }
    }
}

bool displayLessThan(const TQChoiceItem &c1, const TQChoiceItem &c2)
{
    return QString::compare(c1.displayText,c2.displayText,Qt::CaseInsensitive)<0;
}

TQChoiceList *TrkToolProject::fieldChoiceList(const QString &name, int recType)
{
    TQChoiceList *list = new TQChoiceList();
    TRK_FIELD_TYPE fType = fieldNativeType(name, recType);
    if((fType == TRK_FIELD_TYPE_CHOICE) || (fType == TRK_FIELD_TYPE_STATE))
    {
        if(isTrkOK(TrkInitChoiceList( handle, name.toLocal8Bit().constData(), recType)))
        {
            TQChoiceItem ch;
            char chname[1024];
            TRK_UINT order = 0;
            TRK_UINT id, weight;
//            while(TRK_SUCCESS == TrkGetNextChoice(handle, sizeof(chname), chname))
            while(TRK_SUCCESS == TrkGetNextChoiceInternal(handle, sizeof(chname), chname, &id, &order, &weight))
            {
                ch.displayText = QString::fromLocal8Bit(chname);
                ch.fieldValue = ch.displayText;
                ch.id = id;
                ch.order = order;
                ch.weight = weight;
                list->append(ch);
            }
        }
    }
    else if ((fType == TRK_FIELD_TYPE_SUBMITTER)
             || (fType == TRK_FIELD_TYPE_OWNER)
             || (fType == TRK_FIELD_TYPE_USER))
    {
        int order = 0;
        foreach(const QString &login, m_userList.keys())
        {
            TQChoiceItem ch;
            ch.id = 0;
            ch.fieldValue = login;
            ch.displayText = m_userList[login].displayName;
            ch.order = ++order;
            ch.weight = 1;
            list->append(ch);
        }
        qSort(list->begin(),list->end(),displayLessThan);
            /*
        if(isTrkOK(TrkInitUserList(handle)))
        {
            TrkToolChoice ch;
            char userName[1024];
            //int order = 0;
            while(TRK_SUCCESS == TrkGetNextUser(handle, sizeof(userName), userName))
            {
                ch.fieldValue = QString::fromLocal8Bit(userName);
                char fullName[1024];
                if(isTrkOK(TrkGetUserFullName(handle, userName, sizeof(fullName), fullName)))
                    ch.displayText = QString::fromLocal8Bit(fullName);
                else
                    ch.displayText = QString::fromLocal8Bit(userName);
                //ch.order = order++;
                //ch.weight = 0;
                list->append(ch);
            }
        }
        */
    }
    return list;
}

int TrkToolProject::fieldNativeType(const QString &name, int recType)
{
    const TrkRecordTypeDef *rdef = recordDef[recType];
    TRK_FIELD_TYPE fType = rdef->fieldNativeType(name);
    return fType;
}

TQRecModel *TrkToolProject::selectedModel(int recType)
{
    if(!selectedModels.contains(recType))
    {
        QList<int> ids = selected[recType].toList();
        TQRecModel *model = (TQRecModel *)openIdsModel(ids,recType,false);
        selectedModels.insert(recType,model);
        return model;
    }
    return selectedModels[recType];
}

bool TrkToolProject::canFieldSubmit(int vid, int recType)
{
    //TRK_VID vid = recordDef[recType]->nameVids[name];
    if(vid == VID_Id)
        return false;

    QString fname = fieldVID2Name(recType, vid);
    TRK_UINT rights;
    if(isTrkOK(TrkGetFieldSubmitRights(handle,fname.toLocal8Bit().constData(),recType,&rights)))
        return rights;
    return false;
}

bool TrkToolProject::canFieldUpdate(int vid, int recType)
{
    //TRK_VID vid = recordDef[recType]->nameVids[name];
    if(vid == VID_Id)
        return false;

    QString fname = fieldVID2Name(recType, vid);
    TRK_UINT rights;
    if(isTrkOK(TrkGetFieldUpdateRights(handle,fname.toLocal8Bit().constData(),recType,&rights)))
        return rights;
    return false;
}

QString TrkToolProject::userFullName(const QString &login)
{
    QString res;
    if(login.isEmpty())
        return res;
    else if(m_userList.contains(login))
        res = m_userList[login].fullName;
    else
    {
        char buf[256];
        buf[0]=0;
        if(TRK_SUCCESS == TrkGetUserFullName(handle,login.toLocal8Bit().constData(),sizeof(buf),buf))
            res = QString::fromLocal8Bit(buf);
        if(res.isEmpty())
            return login;
    }
    return res;
}

TQRecord *TrkToolProject::newRecord(int rectype)
{
    TrkToolRecord *rec = new TrkToolRecord(this,rectype);
    /*
    if(!isTrkOK(TrkRecordHandleAlloc(handle,&rec->lockHandle)))
    {
        delete rec;
        return 0;
    }
    if(!isTrkOK(TrkNewRecordBegin(rec->lockHandle ,rec->recordType())))
    {
        delete rec;
        return 0;
    }
    */
    /*
    if(!isTrkOK(TrkSetStringFieldValue(rec->lockHandle,"Title","Test title")))
    {
        delete rec;
        return 0;
    }
    const char *desc="Test desc";
    if(!isTrkOK(TrkSetDescriptionData(rec->lockHandle,qstrlen(desc),desc,0)))
    {
        delete rec;
        return 0;
    }
    */
    TrkRecordTypeDef* recDef = recordDef[rectype];
    QList<int> vids = recDef->fieldVids();
    foreach(int vid, vids)
    {
        QVariant value = recDef->getFieldType(vid).defaultValue();
        rec->values[vid] = value;
    }


    rec->handleAllocated = isTrkOK(TrkRecordHandleAlloc(handle, &rec->trkHandle));
    if(!rec->handleAllocated || !isTrkOK(TrkNewRecordBegin(rec->trkHandle, rectype)))
    {
        delete rec;
        return 0;
    }
    //rec->commit();
    rec->doSetMode(TQRecord::Insert);
    return rec;
}

TQAbstractRecordTypeDef *TrkToolProject::recordTypeDef(int rectype)
{
    return recordDef.value(rectype,0);
}

QDomDocument TrkToolProject::recordTypeDefDoc(int rectype)
{
    const TQAbstractRecordTypeDef *def = recordTypeDef(rectype);
    if(!def)
        return QDomDocument();
    QDomDocument doc("RecordTypeDef");
    QDomElement root = doc.createElement("RecordTypeDef");

    QDomElement fields = doc.createElement("Fields");

    QDomElement choices = doc.createElement("Choices");

    foreach(const QString &fname, def->fieldNames())
    {
        QDomElement f = doc.createElement("Field");
        TQAbstractFieldType fdef = def->getFieldType(fname);
        f.setAttribute("name",fname);
        f.setAttribute("nativeType",fdef.nativeType());
        f.setAttribute("simpleType",fdef.simpleType());
        f.setAttribute("vid",fdef.virtualID());
        f.setAttribute("canSubmit",fdef.canSubmit() ? "true" : "false");
        f.setAttribute("canUpdate",fdef.canUpdate() ? "true" : "false");
        f.setAttribute("nullable",fdef.isNullable() ? "yes" : "no");
        f.setAttribute("minValue",fdef.minValue().toString());
        f.setAttribute("maxValue",fdef.maxValue().toString());
        if(fdef.isChoice())
        {
            QString tableName = fdef.choiceTableName(); //"Table_" + fname.trimmed();
            f.setAttribute("choices", tableName);
            QDomElement ch = doc.createElement("ChoiceTable");
            ch.setAttribute("name", tableName);
            TQChoiceList chList = def->choiceTable(tableName);
            foreach(const TQChoiceItem& item, chList)
            {
                QDomElement ci = doc.createElement("ChoiceItem");
                ci.setAttribute("displayText", item.displayText);
                ci.setAttribute("value",item.fieldValue.toString());
                int order = item.order;
                ci.setAttribute("order",order);
                int weight = item.weight;
                ci.setAttribute("weight", weight);
                ch.appendChild(ci);
            }
            choices.appendChild(ch);
        }
        fields.appendChild(f);

    }
    root.appendChild(fields);
    root.appendChild(choices);
    doc.appendChild(root);
    return doc;
}

TQRecord *TrkToolProject::recordOfIndex(const QModelIndex &index)
{
    const TQRecModel *model = qobject_cast<const TQRecModel *>(index.model());
    if(!model)
        return 0;
    return model->recordInRow(index.row());
}

bool TrkToolProject::isSystemModel(QAbstractItemModel *model) const
{
    foreach(const TQRecModel *m, selectedModels)
    {
        if(m == model)
            return true;
    }
    return false;
}

static QString nextCItem(QString &str)
{
    int i = str.indexOf(',');
    QString res;
    if(i<0)
    {
        res = str;
        str = QString();
    }
    else
    {
        res =str.left(i);
        str = str.mid(i+1);
    }
    return res;
}

TQQueryDef *TrkToolProject::queryDefinition(const QString &queryName, int rectype)
{
    TrkRecordTypeDef *recDef = recordDef.value(rectype);
    if(!recDef)
        return 0;
    QString str;
    QSqlDatabase sqlDb = db->openSqlDatabase();
    if(sqlDb.isOpen())
    {
        QSqlQuery query2(sqlDb);
        QVariant vQueryName(queryName);
        query2.prepare("select qryId,qryCont,qryName,qryTitle,qryComment from "+dbName+"..trkqry where qryName = ? and qryTypeId in (?, ?)");
        query2.bindValue(0, vQueryName);
        query2.bindValue(1, rectype);
        query2.bindValue(2, rectype+2);
        if(query2.exec())
        {
            if(query2.next())
            {
                str = query2.value(1).toString();
                TrkQueryDef *resultDef = new TrkQueryDef(this, recDef);
                resultDef->setName(query2.value(2).toString());
                resultDef->setTitle(query2.value(3).toString());
                resultDef->setComment(query2.value(4).toString());
                resultDef->parseSavedString(str);
                return resultDef;
            }
        }
        else
        {
            qDebug() << query2.lastError().text();
            return 0;
        }
    }
    return 0;
    /*
    TrkCond res;
    while(!str.isEmpty())
    {
        bool moduleSearch = nextCItem(str).toInt() == 1;
        bool changeSearch = nextCItem(str).toInt() == 1;
        int fcount = nextCItem(str).toInt();
        while(!str.isEmpty() && fcount--)
        {
            TQConditionLine line;
            line.vid = nextCItem(str).toInt();
            int ftype = recDef->fieldNativeType(line.vid);
            switch(ftype)
            {
            case TRK_FIELD_TYPE_CHOICE:

                break;
            }
        }
    }
    return QString();
    */
}

TQQueryDef *TrkToolProject::createQueryDefinition(int rectype)
{
    TrkRecordTypeDef *rDef = (TrkRecordTypeDef *)recordTypeDef(rectype);
    if(!rDef)
        return 0;
    TrkQueryDef *qDef = new TrkQueryDef(this, rDef);
    return qDef;
}

TQAbstractQWController *TrkToolProject::queryWidgetController(int rectype)
{
    Q_UNUSED(rectype)
#ifdef CLIENT_APP
    return new TQQueryWidgetController(this);
#else
    return 0;
#endif
}

bool TrkToolProject::saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype)
{
    TrkQueryDef *def = qobject_cast<TrkQueryDef *>(queryDefinition);
    if(!def)
        return false;
    QString saveString = def->makeSaveString();
//    return !saveString.isEmpty();

    return isTrkOK(TrkSaveQuery(handle,
                                queryName.toLocal8Bit().constData(),
                                def->comment().toLocal8Bit().constData(),
                                saveString.toLocal8Bit().constData(),
                                def->title().toLocal8Bit().constData(),
                                rectype
                                )
                   );

}

QStringList TrkToolProject::userNames()
{
    QStringList names;
    foreach(const TQUser &user, m_userList)
        names.append(user.fullName);
    return names;
}

QMap<QString, TQUser> TrkToolProject::userList()
{
    return m_userList;
}

QString TrkToolProject::userFullName(int userId)
{
    QString login = userLogin(userId);
    return userFullName(login);
}

QString TrkToolProject::userLogin(int userId)
{
    foreach(const TQUser &user, userList())
        if(user.id == userId)
            return user.login;
    return QString();
}

int TrkToolProject::userId(const QString &login)
{
    QMap<QString, TQUser> list = userList();
    if(list.contains(login))
        return list[login].id;
    return -1;
}

TQGroupList TrkToolProject::userGroups()
{
    TQGroupList groups;
    if(isTrkOK(TrkInitGroupList(handle)))
    {
        char buf[1024];
        TRK_UINT id, order;
        while(isTrkOK(TrkGetNextGroup(handle,sizeof(buf),buf, &id, &order)))
        {
            TQGroup gr;
            gr.name = QString::fromLocal8Bit(buf);
            gr.id = id;
            groups.append(gr);
        }
    }
//    groups.sort();
    return groups;
}

bool TrkToolProject::renameQuery(const QString &oldName, const QString &newName, int recordType)
{
    foreach(const QStringList list, qList)
    {
        if(list.contains(newName, Qt::CaseInsensitive))
            return false;
    }
    TQQueryDef *def = queryDefinition(oldName, recordType);
    if(def && saveQueryDefinition(def,newName,recordType))
    {
        deleteQuery(oldName, recordType);
        return true;
    }
    return false;
}

bool TrkToolProject::deleteQuery(const QString &queryName, int recordType)
{
    Q_UNUSED(recordType)
    return isTrkOK(TrkDeleteQuery(handle, queryName.toLocal8Bit().constData()));
}



void TrkToolProject::readProjectDatabaseName()
{
    dbName.clear();
    QSqlDatabase sqlDb = db->openSqlDatabase();
    if(sqlDb.isOpen())
    {
        QSqlQuery query(sqlDb);
        query.prepare("select prjDbName from trkmaster..trkprj where prjDescr = ?");
        query.bindValue(0, projectName());
        if(query.exec() && query.next())
            dbName = query.value(0).toString();

    }
}

bool TrkToolProject::fillModel(TQRecModel *model, const QString &queryName,
                               int type, qint64 afterTransId)
{
    QList<int> list = getQueryIds(queryName, type, afterTransId);
    foreach(int id, list)
    {
        TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(createRecordById(id, type));
        if(rec)
            model->append(rec);
    }
    model->setQueryName(queryName);
    return true;

    /*
    bool rc;
    TrkScopeRecHandle recHandle(this);
    //TRK_RECORD_HANDLE recHandle;
    //rc = isTrkOK(TrkRecordHandleAlloc(prj->handle, &recHandle));
    if(recHandle.isValid())
    {
        qint64 lastTransId = 0;
        qint64 prevTransId = afterTransId;
        rc = isTrkOK(TrkQueryInitRecordList(*recHandle, queryName.toLocal8Bit().constData(), prevTransId, &lastTransId));
        if(!rc)
            return false;
        while(rc = (TRK_SUCCESS == (TrkGetNextRecord(*recHandle))))
        {
            TrkToolRecord *rec = createRecordByHandle(*recHandle, type);
            model->append(rec);
            //model->appendRecordId();
            //appendRecordByHandle(*recHandle);
        }
        //TrkRecordHandleFree(&recHandle);
        return true;
    }
    return false;
    */
}

TQRecord *TrkToolProject::createRecordById(int id, int rectype)
{
    TrkToolRecord *rec=0;
    TrkScopeRecHandle recHandle(this, 0, id, rectype);
    if(!recHandle.isValid())
        return 0;
    rec = new TrkToolRecord(this, rectype);
//    rec->values[VID_Id] = QVariant::fromValue<int>(id);
    rec->setRecordId(id);
    doReadBaseFields(rec, *recHandle);
    connect(rec,SIGNAL(changed(int)),this,SIGNAL(recordChanged(int)));
    return rec;


    /*
    TRK_RECORD_HANDLE recHandle;
    if(!isTrkOK(TrkRecordHandleAlloc(handle, &recHandle)))
        return 0;
    if(isTrkOK(TrkGetSingleRecord(recHandle, id, rectype)))
    {
        rec = new TrkToolRecord(this, rectype);
        rec->values[VID_Id] = QVariant::fromValue<int>(id);
        doReadBaseFields(rec, recHandle);
        connect(rec,SIGNAL(changed(int)),this,SIGNAL(recordChanged(int)));
    }
    TrkRecordHandleFree(&recHandle);
    return rec;
    */
}

QAbstractItemModel *TrkToolProject::queryModel(int type)
{
    return theQueryModel[type];
}

TQQueryGroups TrkToolProject::queryGroups(int type)
{
    TQQueryGroups list;
    TQQueryGroup item;
    item.name = tr("Общие выборки");
    item.isCreateEnabled = false;
    item.isModifyEnabled = false;
    item.isDeleteEnabled = false;
    item.filterString = "true";
    list.append(item);

    item.name = tr("Личные выборки");
    item.isCreateEnabled = true;
    item.isModifyEnabled = true;
    item.isDeleteEnabled = true;
    item.filterString = "false";
    list.append(item);
    return list;
}

#ifdef CLIENT_APP
/*
QAbstractItemModel *TrkToolProject::createProxyQueryModel(int filter, QObject *parent, TRK_RECORD_TYPE type )
{
    TrkQryFilter *proxy = new TrkQryFilter(parent);
    proxy->filter = filter;
    proxy->setSourceModel(&theQueryModel);
    return proxy;
}
    */
#endif

TQRecModel *TrkToolProject::openQueryModel(const QString &name, int type)
{
    TQRecModel *model = new TQRecModel(this, type, this);
    fillModel(model, name, type);
//    if(emitEvent)
//        emit openedModel(model);
    return model;
}

QList<int> TrkToolProject::getQueryIds(const QString &name, int type, qint64 afterTransId)
{
    QString fieldName = fieldVID2Name(type,VID_Id);
    bool rc;
    TrkScopeRecHandle recHandle(this,0,0,type);
    //TRK_RECORD_HANDLE recHandle;
    //rc = isTrkOK(TrkRecordHandleAlloc(handle, &recHandle));
    QList<int> list;
    if(recHandle.isValid())
    {
        TRK_TRANSACTION_ID prevTransId = afterTransId, lastTransId=0;
        rc = isTrkOK(TrkQueryInitRecordList(*recHandle, name.toLocal8Bit().constData(), prevTransId, &lastTransId));
        while(rc = (TRK_SUCCESS == (TrkGetNextRecord(*recHandle))))
        {
            TRK_UINT value;
            TrkGetNumericFieldValue(*recHandle, fieldName.toLocal8Bit().constData(), &value);
            list.append(value);
        }
        //TrkRecordHandleFree(&recHandle);
    }
    return list;
}

/*
TrkToolModel *TrkToolProject::openRecentModel(int afterTransId, const QString &name, TRK_RECORD_TYPE type)
{
    TrkToolModel *model = new TrkToolModel(this, type, this);
    model->openQuery(name,afterTransId);
    return model;
}
*/

TQRecModel *TrkToolProject::openIdsModel(const QList<int> &ids, int type, bool emitEvent)
{
    TQRecModel *model = new TQRecModel(this, type, this);
    //beginResetModel();
    QList <int> unique = uniqueIntList(ids);
//    model->prevTransId=0;
    QList<TQRecord*> records;
    foreach(int id, unique)
    {
        TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(createRecordById(id, type));
        if(rec)
            records.append(rec);
    }
    model->append(records);
    model->setQueryName(intListToString(unique));
    model->setQueryType(false);
    //endResetModel();

//	model->openIds(ids);
    if(emitEvent)
        emit openedModel(model);
    return model;
}

TQRecModel *TrkToolProject::openRecords(const QString &queryText, int recType, bool emitEvent)
{
    QList<int> ids = stringToIntList(queryText);
    return openIdsModel(ids, recType, emitEvent);
}

void TrkToolProject::refreshModel(QAbstractItemModel *model)
{
    TQRecModel *recModel = qobject_cast<TQRecModel *>(model);
    if(recModel)
    {
        return;
    }
    TrkToolQryModel *qryModel = qobject_cast<TrkToolQryModel *>(model);
    if(qryModel)
    {
        refreshQueryList();
    }
}

QVariant TrkToolProject::optionValue(const QString &option) const
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
        return db->dataDir().absoluteFilePath("pvcs-scr.xq");
    if(option == TQOPTION_EDIT_TEMPLATE)
        return db->dataDir().absoluteFilePath("pvcs-edit.xq");
    if(option == TQOPTION_GROUP_FILE)
        return db->dataDir().absoluteFilePath("tracker.xml");
#ifdef CLIENT_APP
    return ttglobal()->optionDefaultValue(option);
#else
    return QVariant();
#endif
}

QStringList TrkToolProject::noteTitleList()
{
    return noteTitles;
}

/*
NotesCol TrkToolProject::getNotes(int recId, TRK_RECORD_TYPE type) const
{
    NotesCol col;
    TrkScopeRecHandle recHandle(handle);
    if(isTrkOK(TrkGetSingleRecord(*recHandle, recId, type)))
        col = doGetNotes(*recHandle);
    return col;
}
*/

bool TrkToolProject::readNoteRec(TRK_NOTE_HANDLE noteHandle, TQNote *note) const
{
    char buf[1024];
    if(isTrkOK(TrkGetNoteTitle(noteHandle, sizeof(buf), buf)))
        note->title = QString::fromLocal8Bit(buf);
    if(isTrkOK(TrkGetNoteAuthor(noteHandle, sizeof(buf), buf)))
        note->author = QString::fromLocal8Bit(buf);
    TRK_UINT len;
    if(isTrkOK(TrkGetNoteDataLength(noteHandle, &len) && len))
    {
        TRK_UINT remain = len;
        note->text = "";
        TRK_UINT res;
        while(remain)
        {
            //int blen = remain < sizeof(buf) ? remain : sizeof(buf);
            res = TrkGetNoteData(noteHandle, sizeof(buf), buf, &remain);
            note->text += QString::fromLocal8Bit(buf);
            if(res!=TRK_SUCCESS && res !=TRK_E_DATA_TRUNCATED)
                break;
        }
        note->text = filterUtf16(note->text);
    }
    TRK_TIME time;
    if(isTrkOK(TrkGetNoteCreateTime(noteHandle, &time)))
        note->crdate = QDateTime::fromTime_t(time);
    if(isTrkOK(TrkGetNoteModifyTime(noteHandle, &time)))
        note->mddate = QDateTime::fromTime_t(time);
    TRK_UINT perm;
    if(isTrkOK(TrkGetNotePermissions(noteHandle,&perm)))
        note->perm = perm;
    return true;
}

QString TrkToolProject::doGetDesc(TRK_RECORD_HANDLE recHandle) const
{
    QString result;
    TRK_UINT remain;
    if(isTrkOK(TrkGetDescriptionDataLength(recHandle, &remain)))
    {
        char buf[1024];
        TRK_UINT res;
        bool tryAgain = true;
        while(remain)
        {
            TRK_UINT newRemain = remain;
            res = TrkGetDescriptionData(recHandle,sizeof(buf),buf,&newRemain);
//            if(res == TRK_E_NO_MORE_DATA && remain && !newRemain && tryAgain)
//            {
//                tryAgain = false;
//                continue;
//            }
            remain = newRemain;
            tryAgain = false;
            if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
                break;
            result += QString::fromLocal8Bit(buf);
        }
        return filterUtf16(result);
    }
    return QString();
}

TQNotesCol TrkToolProject::doGetNotes(TRK_RECORD_HANDLE recHandle) const
{
    TQNotesCol col;
    TrkScopeNoteHandle noteHandle(recHandle);
    if(noteHandle.isValid())
    {
        if(isTrkOK(TrkInitNoteList(*noteHandle)))
        {
            while(TRK_SUCCESS == TrkGetNextNote(*noteHandle))
            {
                TQNote note;
                readNoteRec(*noteHandle, &note);
                note.isAdded = false;
                note.isChanged = false;
                note.isDeleted = false;
                col.append(note);
            }
        }
    }
    return col;
}

QVariant TrkToolProject::doGetValue(TRK_RECORD_HANDLE recHandle, const QString &fname, TRK_FIELD_TYPE fType, bool *ok)
{
    bool dummy;
    bool &p = ok ? *ok : dummy;
    char buf[1024];
    switch(fType)
    {
    case TRK_FIELD_TYPE_DATE:
        if(isTrkOK(TrkGetStringFieldValue(recHandle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
        {
            QDateTime dt = QDateTime::fromString(QString::fromLocal8Bit(buf),TT_DATETIME_FORMAT);
            p = true;
            return QVariant::fromValue<QDateTime>(dt);
        }
        p = false;
        return QVariant();
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STATE:
    case TRK_FIELD_TYPE_STRING:
        if(isTrkOK(TrkGetStringFieldValue(recHandle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
        {
            p = true;
            return QVariant::fromValue<QString>(QString::fromLocal8Bit(buf));
        }
        p = false;
        return QVariant::fromValue<QString>(QString(""));
    case TRK_FIELD_TYPE_NUMBER:
        TRK_UINT value;
        if(isTrkOK(TrkGetNumericFieldValue(recHandle, fname.toLocal8Bit().constData(), &value)))
        {
            p = true;
            return QVariant::fromValue<int>(value);
        }
        p = false;
        return QVariant::fromValue<int>(0);
    case TRK_FIELD_TYPE_NONE:
        p = false;
        return QVariant();
    }
    p = false;
    return QVariant();
}

bool TrkToolProject::doReadBaseFields(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle)
{
    QHash<int, QString> baseFields = baseRecordFields(record->recordType());
    foreach(int vid, baseFields.keys())
    {
        QString fname = baseFields[vid];
        TRK_FIELD_TYPE fType = recordDef[record->recordType()]->fieldNativeType(vid);
        record->values[vid] = doGetValue(recHandle, fname, fType);
    }
    emit recordValuesLoaded(record->recordId());
    return true;
}

bool TrkToolProject::readRecordWhole(TQRecord *record)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    resetRecHandler(recHandle.recHandler());
    if(record->mode() == TrkToolRecord::Insert)
        return true;
    bool res = readRecordFields(record) && readRecordTexts(record);
    emit recordValuesLoaded(trec->recordId());
    //record->somethingChanged();
    return res;
    /*
    TrkScopeRecHandle recHandle(handle, record->lockHandle, record->recordId(), record->recordType());
    if(!recHandle.isValid())
        return false;
    QList<int> vids = recordDef[record->rectype]->fieldIds();
    foreach(int vid, vids)
    {
        QString fname = recordDef[record->rectype]->fieldName(vid);
        TRK_FIELD_TYPE fType = recordDef[record->rectype]->fieldType(vid);
        record->values[vid] = doGetValue(*recHandle, fname, fType);
        // getFieldValue(record, fname);
    }
    record->desc = doGetDesc(*recHandle);
    record->descChanged = false;
    record->descLoaded = true;
    record->notesList = doGetNotes(*recHandle);
    record->notesReaded = true;
    return true;
    */
}


bool TrkToolProject::readRecordFields(TQRecord *record)
{
    if(record->mode() == TQRecord::Insert)
        return true;
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return false;
    QList<int> vids = recordDef[record->recordType()]->fieldVids();
    foreach(int vid, vids)
    {
        QString fname = recordDef[record->recordType()]->fieldName(vid);
        TRK_FIELD_TYPE fType = recordDef[record->recordType()]->fieldNativeType(vid);
        trec->values[vid] = doGetValue(*recHandle, fname, fType);
        // getFieldValue(record, fname);
    }
    emit recordValuesLoaded(record->recordId());
    return true;
}

bool TrkToolProject::readRecordTexts(TQRecord *record)
{
    if(record->mode() == TQRecord::Insert)
        return true;
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return false;
    resetRecHandler(recHandle.recHandler());
    trec->desc = doGetDesc(*recHandle);
    trec->descChanged = false;
    trec->notesList = doGetNotes(*recHandle);
    trec->textsReaded = true;
    emit recordValuesLoaded(record->recordId());
    return true;
}

bool TrkToolProject::readRecordBase(TQRecord *record)
{
    if(record->mode() == TrkToolRecord::Insert)
        return true;
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return false;
    return doReadBaseFields(trec, *recHandle);
}

//TrkToolRecord *TrkToolProject::createRecordByHandle(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype)
//{
//    TrkToolRecord *record = new TrkToolRecord(this, rectype);
//    doReadBaseFields(record, recHandle);
//    return record;
//}


QVariant TrkToolProject::getFieldValue(const TQRecord *record, const QString &fname, bool *ok)
{
    if(record->mode() == TrkToolRecord::Insert)
        return true;
    const TrkToolRecord *trec = qobject_cast<const TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return false;
    TRK_FIELD_TYPE fType = recordDef[record->recordType()]->fieldNativeType(fname);
    bool dummy;
    bool *p =ok ? ok: &dummy;
    return doGetValue(*recHandle, fname, fType, p);
}

QVariant TrkToolProject::getFieldValue(const TQRecord *record, int vid, bool *ok)
{
    QString fname = fieldVID2Name(record->recordType(), vid);
    return getFieldValue(record, fname, ok);
}

/*
bool TrkToolProject::insertRecordBegin(TrkToolRecord *record)
{
    TrkRecHandler h;
    if(!record->lockHandle)
        if(!isTrkOK(TrkRecordHandleAlloc(handle,&record->lockHandle)))
            return false;
    if(!isTrkOK(TrkNewRecordBegin(record->lockHandle,record->rectype)))
        return false;
    record->recMode = TrkToolRecord::Insert;
    return true;
}
*/

bool TrkToolProject::updateRecordBegin(TQRecord *record)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    TrkRecHandler *ph = recHandle.recHandler(); //allocRecHandler(record->recordId(), record->recordType());
    if(!ph)
        return false;
    if(!ph->isModify)
    {
        //resetRecHandler(ph);
#ifdef CLIENT_APP
        if(!isTrkOK(TrkUpdateRecordBeginEx(recHandle.nativeHandle(), true)))
            return false;
#else
        if(!isTrkOK(TrkUpdateRecordBegin(recHandle.nativeHandle())))
            return false;
#endif
        ph->isModify = true;
    }
    record->setMode(TrkToolRecord::Edit);
    emit recordStateChanged(record->recordId());
    return true;
}

bool TrkToolProject::commitRecord(TQRecord *record)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    bool res = false;
    if(trec->mode() == TrkToolRecord::Insert)
        res = doCommitInsert(trec);
    else if(trec->mode() == TrkToolRecord::Edit)
        res = doCommitUpdate(trec);
    if(res)
        emit recordStateChanged(trec->recordId());
    return res;
}

bool TrkToolProject::cancelRecord(TQRecord *record)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    bool res = false;
    if(trec->mode() == TrkToolRecord::Insert)
        res = doCancelInsert(trec);
    else //if(record->mode() == TrkToolRecord::Edit)
        res = doCancelUpdate(trec);
    if(res)
        emit recordStateChanged(trec->recordId());
    return res;

//    if(!locks.contains(record))
//    {
//        record->recMode = TrkToolRecord::View;
//        return true;
//    }
//    TrkRecHandler &h = locks[record];
//    isTrkOK(TrkRecordCancelTransaction(h.handle));
//    isTrkOK(TrkRecordHandleFree(&h.handle));
//    record->recMode = TrkToolRecord::View;
//    locks.remove(record);
//    return true;
}

QList<TQAttachedFile> TrkToolProject::attachedFiles(TQRecord *record)
{
    QList<TQAttachedFile> res;
//    return res;
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return QList<TQAttachedFile>();
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return res;
    TRK_ATTFILE_HANDLE attHandle;
    if(!isTrkOK(TrkAttachedFileHandleAlloc(*recHandle,&attHandle)))
        return QList<TQAttachedFile>();
    if(isTrkOK(TrkInitAttachedFileList(attHandle)))
    {
        int id = 0;
        while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
        {
            TQAttachedFile file;
            char buf[1024];
            if(TRK_SUCCESS == TrkGetAttachedFileName(attHandle,sizeof(buf),buf))
                file.fileName = QString::fromLocal8Bit(buf);
            TRK_TIME time;
            if(TRK_SUCCESS == TrkGetAttachedFileTime(attHandle, &time))
                file.createDateTime = QDateTime::fromTime_t(time);
            file.isAdded = false;
            file.isChanged = false;
            file.isDeleted = false;
            file.index = id++;
            /*
                TRK_FILE_STORAGE_MODE mode;
                TRK_UINT sz;
                char buf1[1024],buf2[1024],buf3[1024],buf4[1024],buf5[1024];
                isTrkOK(TrkGetAttachedFileInfo(attHandle,buf1,buf2,buf3,buf4,buf5));
                */
            res.append(file);
        }
    }
    TrkAttachedFileHandleFree(&attHandle);
    return res;
}

QVariantList TrkToolProject::historyList(TQRecord *record)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return QVariantList();
    TrkScopeRecHandle recHandle(this, trec);
    if(!recHandle.isValid())
        return QVariantList();
    QStringList changes = doGetHistoryList(*recHandle);
    QVariantList history;
    int index = 0;
    foreach(const QString &item, changes)
    {
        QVariantMap map;
        QStringList sections = item.split(QChar(' '));
        QString changeDate = sections[0];
        QString changeTime = sections[1];//item.left(10+1+5);
        QDateTime changeDateTime = QDateTime::fromString(changeDate + " " + changeTime, TT_DATETIME_FORMAT);
        QString changeAuthor = sections[2].mid(1,sections[2].length()-3);
        QString changeDesc = QStringList(sections.mid(5)).join(" ");

        map.insert("author", changeAuthor);
        map.insert("datetime", changeDateTime);
        map.insert("createdate", changeDate + " " + changeTime);
        map.insert("action", changeDesc);
        map.insert("index",index++);
        history.append(map);
    }
    return history;
}

QHash<int, QString> TrkToolProject::baseRecordFields(int rectype)
{
    TrkRecordTypeDef *def = recordDef[rectype];
    return def->baseFields;
}

bool TrkToolProject::saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this,trec);
    bool res=false;
//    bool ch=!lockHandle;
//    TRK_RECORD_HANDLE handle;
//    if(ch)
//    {
//        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
//            return true;
//    }
//    else
//        handle=lockHandle;
    TRK_ATTFILE_HANDLE attHandle;
    if(/*isTrkOK(TrkGetSingleRecord(*recHandle, recordId(), rectype))
            &&*/ isTrkOK(TrkAttachedFileHandleAlloc(*recHandle,&attHandle))
            && isTrkOK(TrkInitAttachedFileList(attHandle)))
    {
        while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
        {
            if(!fileIndex--)
            {
                res = isTrkOK(TrkExtractAttachedFile(attHandle,dest.toLocal8Bit().constData()));
                break;
            }
        }
    }
    TrkAttachedFileHandleFree(&attHandle);
//    if(ch)
//        TrkRecordHandleFree(&handle);
    return res;
}

int TrkToolProject::attachFileToRecord(TQRecord *record, const QString &filePath)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return -1;
    TrkScopeRecHandle recHandle(this, trec);
    TRK_ATTFILE_HANDLE attHandle;
    if(!isTrkOK(TrkAttachedFileHandleAlloc(*recHandle, &attHandle)))
        return -1;
    int res = -1;
    int nextIndex = 0;
    if(trec->mode() != TQRecord::Insert)
        if(isTrkOK(TrkInitAttachedFileList(attHandle)))
        {
            while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
                nextIndex++;
        }
    if(isTrkOK(TrkAddNewAttachedFile(attHandle, filePath.toLocal8Bit(), TRK_FILE_BINARY)))
    {
        res = nextIndex;
        TQAttachedFile file;
        file.fileName = filePath;
        file.isAdded = true;
        file.isChanged = false;
        file.isDeleted = false;
        file.index = trec->files.count();
        trec->files.append(file);
    }
    TrkAttachedFileHandleFree(&attHandle);
    trec->filesReaded = false;
    if(res != -1)
        record->setModified(true);
    return res;
}

bool TrkToolProject::removeFileFromRecord(TQRecord *record, int fileIndex)
{
    TrkToolRecord *trec = qobject_cast<TrkToolRecord *>(record);
    if(!trec)
        return false;
    TrkScopeRecHandle recHandle(this, trec);
    TRK_ATTFILE_HANDLE attHandle;
    if(!isTrkOK(TrkAttachedFileHandleAlloc(*recHandle, &attHandle)))
        return false;
    int res = false;
    int nextIndex = 0;
    if(isTrkOK(TrkInitAttachedFileList(attHandle)))
        while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
            if(nextIndex++ == fileIndex)
            {
                res = isTrkOK(TrkDeleteAttachedFile(attHandle));
                if(res)
                    trec->files.removeAt(fileIndex);
                break;
            }
    TrkAttachedFileHandleFree(&attHandle);
    trec->filesReaded = false;
    if(res)
        record->setModified(true);
    return res;
}

bool TrkToolProject::doCommitInsert(TrkToolRecord *record)
{
    TrkScopeRecHandle recHandle(this, record);
//    if(!isTrkOK(TrkNewRecordBegin(*recHandle,record->recordType())))
//        return false;
    if(!doSaveFields(record, *recHandle))
        return false;
    if(!doSaveNotes(record, *recHandle))
        return false;
    TRK_TRANSACTION_ID last = record->lastTransaction;
    if(isTrkOK(TrkNewRecordCommit(*recHandle, &last)))
    {
        record->lastTransaction = last;
        record->setRecordId(doGetRecordId(*recHandle, record->recordType()));
        if(record->handleAllocated)
        {
            TrkRecordHandleFree(&record->trkHandle);
            record->handleAllocated = false;
        }
        record->doSetMode(TQRecord::View);
        record->setModified(false);
        record->readFullRecord();
        record->somethingChanged();
        emit recordChanged(record->recordId());
        return true;
    }
    return false;
}

bool TrkToolProject::doCommitUpdate(TrkToolRecord *record)
{
    TrkScopeRecHandle recHandle(this,record);
    if(!recHandle.isValid())
        return false;
//    if(!ph->isModify)
//        return false;
    if(!doSaveFields(record, *recHandle))
        return false;
    TRK_TRANSACTION_ID last = record->lastTransaction;
    if(doSaveNotes(record, *recHandle) && isTrkOK(TrkUpdateRecordCommit(*recHandle, &last)))
    {
        recHandle.recHandler()->isModify = false;
        record->setMode(TrkToolRecord::View);
        record->lastTransaction = last;
        record->setModified(false);
        record->somethingChanged();
        emit recordChanged(record->recordId());
        return true;
    }
    return false;
}

bool TrkToolProject::doCancelInsert(TrkToolRecord *record)
{
    record->doSetMode(TQRecord::View);
    return true;
}

bool TrkToolProject::doCancelUpdate(TrkToolRecord *record)
{
    TrkScopeRecHandle recHandle(this,record);
    if(!recHandle.isValid())
        return false;
    isTrkOK(TrkRecordCancelTransaction(*recHandle));
    recHandle.recHandler()->isModify = false;
    record->doSetMode(TQRecord::View);
    record->somethingChanged();
    emit recordChanged(record->recordId());
    return true;
}

bool TrkToolProject::doSaveFields(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle)
{
    if(!record->isEditing())
        return false;
    bool res = true;
    if(record->descChanged)
        res = isTrkOK(Do_TrkSetDescription(recHandle, stringToLocal8Bit(record->desc)));
    foreach(int vid, record->values.keys())
    {
        if(record->changedValue.contains(vid) && record->changedValue[vid])
            res = res && doSetValue(record, recHandle, vid, record->values[vid]);
        if(res)
            record->changedValue[vid] = false;
    }
    return res;
}

bool TrkToolProject::doSetValue(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle, TRK_VID vid, const QVariant& value)
{
    if(!record->isEditing())
        return false;
    QString fieldName = fieldVID2Name(record->recordType(), vid);
    //TRK_VID vid = fieldName2VID(record->rectype, fieldName);
    TQAbstractFieldType def = recordDef[record->recordType()]->getFieldType(vid);
    QString s;
    QDateTime dt;
    switch(def.nativeType())
    {
    case TRK_FIELD_TYPE_DATE:
        dt = value.toDateTime();
        s = dt.toString(TT_DATETIME_FORMAT);
        return isTrkOK(TrkSetStringFieldValue(recHandle, fieldName.toLocal8Bit().constData(), //buf
                                          s.toLocal8Bit().constData()));
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STATE:
    case TRK_FIELD_TYPE_STRING:
        s = value.toString();
        return isTrkOK(TrkSetStringFieldValue(recHandle, fieldName.toLocal8Bit().constData(), //buf
                                          s.toLocal8Bit().constData()));
    case TRK_FIELD_TYPE_NUMBER:
        TRK_UINT uint = value.toUInt();
        return isTrkOK(TrkSetNumericFieldValue(recHandle, fieldName.toLocal8Bit().constData(), uint));
    }
    return false;
}

bool TrkToolProject::doSaveNotes(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle)
{
    if(!record->isEditing())
        return false;
    TrkScopeNoteHandle noteHandle(recHandle);
    if(!noteHandle.isValid())
        return false;
    if(record->mode() == TrkToolRecord::Edit && !isTrkOK(TrkInitNoteList(*noteHandle)))
        return false;
    TQNotesCol notes = record->notesList;
    for(int i=0; i<notes.count();)
    {
        TQNote &note = record->notesList[i];
        if(note.isAdded)
        {
            if(!isTrkOK(TrkAddNewNote(*noteHandle)))
                return false;
            if(!isTrkOK(TrkSetNoteTitle(*noteHandle, note.title.toLocal8Bit().constData())))
                return false;
            if(!isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(note.text))))
                return false;
            note.isAdded = false;
        }
        else if(record->mode() == TrkToolRecord::Edit)
        {
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return false;
            if(note.isChanged)
            {
                if(!isTrkOK(TrkSetNoteTitle(*noteHandle, note.title.toLocal8Bit().constData())))
                    return false;
                if(!isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(note.text))))
                    return false;
                note.isChanged = false;
            }
            else if(note.isDeleted)
            {
                if(!isTrkOK(TrkDeleteNote(*noteHandle)))
                    return false;
                notes.removeAt(i);
                continue;
            }
        }
        i++;
    }
    record->notesList = notes;
    return true;
}

bool TrkToolProject::doAppendNote(TRK_RECORD_HANDLE recHandle, const QString &noteTitle, const QString &noteText)
{
    TrkScopeNoteHandle noteHandle(recHandle);
    if(!noteHandle.isValid())
        return false;
    return isTrkOK(TrkAddNewNote(*noteHandle))
            && isTrkOK(TrkSetNoteTitle(*noteHandle, noteTitle.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(noteText))); // note.toLocal8Bit()));
}

bool TrkToolProject::doDeleteNote(TRK_RECORD_HANDLE recHandle, int noteId)
{
    TrkScopeNoteHandle noteHandle(recHandle);
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=noteId; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return false;
        return isTrkOK(TrkDeleteNote(*noteHandle));
    }
    return false;
}

bool TrkToolProject::doChangeNote(TRK_RECORD_HANDLE recHandle, int noteId, const QString &noteTitle, const QString &noteText)
{
    TrkScopeNoteHandle noteHandle(recHandle);
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=noteId; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return false;
        return isTrkOK(TrkSetNoteTitle(*noteHandle, noteTitle.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(noteText))); // text.toLocal8Bit()));
    }
    return false;
}

int TrkToolProject::doGetRecordId(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype)
{
    QString fieldName = recordTypeDef(rectype)->fieldName(VID_Id);
    TRK_UINT val=0;
    if(isTrkOK(TrkGetNumericFieldValue(recHandle, fieldName.toLocal8Bit().constData(), &val)))
        return val;
    return 0;
}

QStringList TrkToolProject::doGetHistoryList(TRK_RECORD_HANDLE recHandle)
{
    QStringList result;
    char buf[1024];
    if(isTrkOK(TrkInitChangeList(recHandle)))
    {
        while(1)
        {
            TRK_UINT res = TrkGetNextChange(recHandle, sizeof(buf), buf);
            if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
                break;
            result.append(QString::fromLocal8Bit(buf));
        }
    }
    return result;
}

/*
QString TrkToolProject::doFieldVID2Name(TRK_RECORD_TYPE rectype, TRK_VID vid)
{
	char buf[1024];
	buf[0]=0;
    TrkGetFieldNameFromVid(handle,rectype, vid, sizeof(buf), buf);
	if(buf[0])
		return QString::fromLocal8Bit(buf);
	switch(vid)
	{
	case VID_InternalId:
		return "InternalId";
	case VID_Id:
		return "Id";
	case VID_Title:
		return "Title";
	case VID_Description:
		return "Description";
	case VID_Submitter:
		return "Submitter";
	case VID_SubmitDate:
		return "SubmitDate";
	case VID_Owner:
		return "Owner";
	}
	return "";
}
*/

QString TrkToolProject::fieldVID2Name(int rectype, int vid)
{
    TrkRecordTypeDef* def = recordDef.value(rectype,0);
    if(def && def->fieldDefs.contains(vid))
        return def->fieldDefs[vid]->name;
    return QString();
}

int TrkToolProject::fieldName2VID(int rectype, const QString &fname)
{
    TrkRecordTypeDef* def = recordDef.value(rectype,0);
    if(!def)
        return 0;
    NameVid & nv = recordDef[rectype]->nameVids;
    return nv[fname];
}

int TrkToolProject::defaultRecType() const
{
    return TRK_SCR_TYPE;
}

bool TrkToolProject::isSelectedId(int id, int recType) const
{
    return selected.contains(recType) && selected[recType].contains(id);
}

void TrkToolProject::setSelectedId(int id, bool value, int recType)
{
	if(value)
    {
        if(!selected[recType].contains(id))
        {
            selected[recType].insert(id);
            if(selectedModels.contains(recType))
                selectedModels[recType]->appendRecordId(id);
        }
    }
    else if(selected.contains(recType))
    {
        if(selectedModels.contains(recType))
            selectedModels[recType]->removeRecordId(id);
        selected[recType].remove(id);
    }
}

void TrkToolProject::clearSelected(int recType)
{
    selected[recType].clear();
    if(selectedModels.contains(recType))
        selectedModels[recType]->clearRecords();
}



//====================== TrkToolRecordSet =====================

TrkToolRecordSet::TrkToolRecordSet(QObject * parent)
    : QObject(parent)
{
}

void TrkToolRecordSet::clear()
{
	records.clear();
}

int TrkToolRecordSet::recCount() const
{
	return records.count();
}

//====================== TrkToolRecord =====================


TrkToolRecord::TrkToolRecord(TrkToolProject *parent, TRK_RECORD_TYPE rtype)
    : TQRecord(parent,rtype,0), prj(parent),
      //rectype(rtype),
      //lockHandle(0),
      fieldList(), //links(0),
      textsReaded(false),
      filesReaded(false),
      handleAllocated(false),
      files()
{
    fieldList = prj->recordTypeDef(recordType())->fieldNames();
    init();
}

TrkToolRecord::TrkToolRecord(const TrkToolRecord &src)
    : TQRecord(src), prj(src.prj),
      //rectype(src.rectype), recMode(View),
      //lockHandle(0),
      fieldList(), //links(0),
      textsReaded(false),
      filesReaded(false),
      handleAllocated(false),
      files()
{
    fieldList = prj->recordTypeDef(recordType())->fieldNames();
    init();
}

/*
TrkToolRecord &TrkToolRecord::operator =(const TrkToolRecord &src)
{
    init();
    prj = src.prj;
    fieldList.clear();
    links = 0;
    TQRecord::operator =(src);
//    recType = src.recType;
//    setMode(View);
    //lockHandle = 0;
    //nextNoteId = -1;

    fieldList = prj->recordTypeDef(recType)->fieldNames();
    return *this;
}
*/


TrkToolRecord::~TrkToolRecord()
{
//    disconnect();
//    links=0;
    if(mode() != TQRecord::View)
        cancel();

    if(handleAllocated)
    {
        TrkRecordHandleFree(&trkHandle);
    }
}

QVariant TrkToolRecord::value(const QString& fieldName, int role) const
{
    TRK_VID vid = prj->fieldName2VID(recordType(), fieldName);
    if(vid)
        return value(vid, role);
    else
        return QVariant();
}

QVariant TrkToolRecord::value(int vid, int role) const
{
    if(!vid)
        return QVariant();
    if(vid == 10001 && mode() == Insert)
        return 0;
    if(!values.contains(vid))
    {
        bool ok;
        QVariant v = prj->getFieldValue(this, vid, &ok);
        if(!ok)
            return QVariant();
        values[vid] = v;
        //readFullRecord();
    }
    QVariant v = values[vid];
    if(role == Qt::DisplayRole)
    {
        TQAbstractFieldType fdef = fieldDef(vid);
        return fdef.valueToDisplay(v);
    }
//    if(role == Qt::DisplayRole && v.type() == QMetaType::QDateTime)
//        return QVariant(v.toDateTime().toString(TT_DATETIME_FORMAT));
    return v;
}

int TrkToolRecord::recordId() const
{
    return values[VID_Id].toInt();
}

void TrkToolRecord::setRecordId(TRK_UINT id)
{
    values[VID_Id] = QVariant::fromValue(id);
    doSetRecordId(id);
}

QString TrkToolRecord::title() const
{
    return value(VID_Title).toString();
}

bool TrkToolRecord::setTitle(const QString &newTitle)
{
    return setValue(VID_Title, newTitle);
}

/* !!!!!!!!!!!!!!!
bool TrkToolRecord::insertBegin()
{
	if(recMode == Insert)
		return true;
	if(recMode == Edit)
		return false;
	if(!lockHandle)
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle,&lockHandle)))
			return false;
    if(!isTrkOK(TrkNewRecordBegin(lockHandle,rectype)))
		return false;

    QList<int> vids = prj->recordDef[rectype]->fieldIds();
    foreach(int vid, vids)
    {
        values[vid] = fieldDef(vid).defaultValue();
    }

	recMode = Insert;
    emit changedState(recMode);
	return true;
}
*/

//bool TrkToolRecord::updateBegin()
//{
//    bool res = prj->updateRecordBegin(this);
//    if(res)
//        emit changedState(recMode);
//    return res;
    /* !!!!!!!!!!!!!!!!!!
	if(recMode == Edit)
		return true;
	if(recMode == Insert)
		return false;
	if(!lockHandle)
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle,&lockHandle)))
			return false;
    if(!isTrkOK(TrkGetSingleRecord(lockHandle, recordId(), rectype)))
		return false;
    readHandle(lockHandle,true);
    if(!isTrkOK(TrkUpdateRecordBegin(lockHandle)))
		return false;
	recMode = Edit;
    emit changedState(recMode);
	return true;
    */
//}

//bool TrkToolRecord::commit()
//{
//    bool res = prj->commitRecord(this);
//    if(res)
//        emit changedState(recMode);
//    return res;
    /*
	TRK_UINT res;
	switch(recMode)
	{
		case Edit:
			res = TrkUpdateRecordCommit(lockHandle, &lastTransaction);
            if(isTrkOK(res))
			{
                readHandle(lockHandle,true);
				TrkRecordHandleFree(&lockHandle);
				lockHandle=0;
				recMode = View;
                //addedNotes.clear();
                //deletedNotes.clear();
                emit changedState(recMode);
                emit changed(recordId());
				return true;
			}
			return false;
		case Insert:
			res = TrkNewRecordCommit(lockHandle, &lastTransaction);
            if(isTrkOK(res))
			{
                readHandle(lockHandle,true);
                TrkRecordHandleFree(&lockHandle);
                lockHandle=0;
				recMode = View;
                //addedNotes.clear();
                //deletedNotes.clear();
                emit changedState(recMode);
                emit changed(recordId());
                return true;
			}
			return false;
		default:
			return false;
	}
    */
//}

//bool TrkToolRecord::cancel()
//{
//    bool res = prj->cancelRecord(this);
//    if(res)
//        emit changedState(recMode);
//    return res;
    /* !!!!!!!!!
	if(!lockHandle)
	{
		recMode = View;
        emit changedState(recMode);
		return false;
	}
    if(recMode != View && isTrkOK(TrkRecordCancelTransaction(lockHandle)))
	{
		if(recMode == Edit)
            readHandle(lockHandle,true);
		TrkRecordHandleFree(&lockHandle);
		lockHandle=0;
        //addedNotes.clear();
        //deletedNotes.clear();
		recMode = View;
        emit changedState(recMode);
        emit changed(recordId());
        return true;
	}
	return false;
    */
//}

bool TrkToolRecord::isInsertMode() const
{
    return mode() == TrkToolRecord::Insert;
}

bool TrkToolRecord::isEditMode() const
{
    return mode() == TrkToolRecord::Edit;
}

/* !!!!!!!!!!
void TrkToolRecord::readHandle(TRK_RECORD_HANDLE handle, bool force)
{
    readed = false;
    //char buf[1024];
    //TRK_VID v = prj->recordDef[rectype]->fieldVid(VID_Id);
    readFieldValue(handle, VID_Id);
    if(!force)
        return;
    QList<int> vids = prj->recordDef[rectype]->fieldIds();
    foreach(int vid, vids)
        readFieldValue(handle, vid);
}
*/

/* !!!!!!!!!
void TrkToolRecord::readFieldValue(TRK_RECORD_HANDLE handle, TRK_VID vid)
{
    char buf[1024];
    TRK_FIELD_TYPE fType = prj->recordDef[rectype]->fieldType(vid);
    QString fname = prj->recordDef[rectype]->fieldName(vid);
    switch(fType)
    {
    case TRK_FIELD_TYPE_DATE:
        if(isTrkOK(TrkGetStringFieldValue(handle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
        {
            QDateTime dt = QDateTime::fromString(QString::fromLocal8Bit(buf),TT_DATETIME_FORMAT);
            values[vid] = QVariant::fromValue<QDateTime>(dt);
        }
        break;
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STATE:
    case TRK_FIELD_TYPE_STRING:
        if(isTrkOK(TrkGetStringFieldValue(handle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
            values[vid] = QVariant::fromValue<QString>(QString::fromLocal8Bit(buf));
        break;
    case TRK_FIELD_TYPE_NUMBER:
        TRK_UINT value;
        if(isTrkOK(TrkGetNumericFieldValue(handle, fname.toLocal8Bit().constData(), &value)))
            values[vid] = QVariant::fromValue<int>(value);
        break;
    }
}
*/

void TrkToolRecord::readFullRecord()
{
    prj->readRecordWhole(this);
    somethingChanged();
}

void TrkToolRecord::init()
{
    doSetMode(TQRecord::View);
    //lockHandle = 0;
//    links = 0;
    //nextNoteId = -1;
    descChanged = false;
    textsReaded = false;
    historyReaded = false;
    filesReaded = false;
}

QList<TQAttachedFile> TrkToolRecord::fileList()
{
//    if(recMode == TQRecord::Insert)
//        return filesAdded;
    if(!filesReaded)
    {
        files = prj->attachedFiles(this);
        filesReaded = true;
    }
    return files;
}

bool TrkToolRecord::saveFile(int fileIndex, const QString &dest)
{
    return prj->saveFileFromRecord(this, fileIndex, dest);
}

int TrkToolRecord::appendFile(const QString &filePath)
{
    if(isEditing())
        return prj->attachFileToRecord(this, filePath);
    return -1;
}

bool TrkToolRecord::removeFile(int fileIndex)
{
    if(!isEditing())
        return false;
    return prj->removeFileFromRecord(this, fileIndex);
}

void TrkToolRecord::refresh()
{
    if(mode() == Insert)
        return;
    filesReaded = false;
    prj->readRecordWhole(this);
    valuesReloaded();
}

void TrkToolRecord::releaseBuffer()
{
    foreach(int key, values.keys())
    {
        if(key != VID_Id)
            values.remove(key);
    }
}

void TrkToolRecord::somethingChanged() const
{
    emit changed(recordId());
}

void TrkToolRecord::valuesReloaded()
{
    //emit loaded(recordId());
}

bool TrkToolRecord::setValue(const QString& fieldName, const QVariant& value)
{
    if(!isEditing())
        return false;
    TRK_VID vid = prj->fieldName2VID(recordType(), fieldName);
    return setValue(vid, value);
    /*
	if(recMode != Edit && recMode !=Insert)
		return;
	TRK_VID vid = prj->fieldName2VID(rectype, fieldName);
    TrkFieldType def = prj->recordDef[rectype]->getFieldDef(vid);
    QString s;
    QDateTime dt;
    switch(def.fType())
	{
    case TRK_FIELD_TYPE_DATE:
        dt = value.toDateTime();
        s = dt.toString(TT_DATETIME_FORMAT);
        if(isTrkOK(TrkSetStringFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), //buf
                                          s.toLocal8Bit().constData())))
        {
            if(recMode != Insert)
                readFieldValue(lockHandle, vid);
            //values[vid] = value;
            emit changed(recordId());
        }
        break;
        //case TRK_FIELD_TYPE_NONE:
	case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
	case TRK_FIELD_TYPE_OWNER:
	case TRK_FIELD_TYPE_USER:
	case TRK_FIELD_TYPE_ELAPSED_TIME:
	case TRK_FIELD_TYPE_STATE:
	case TRK_FIELD_TYPE_STRING:
        //char buf[1024];
        //strncpy(buf,value.toString().toLocal8Bit().constData(),sizeof(buf));
        s = value.toString();
        if(isTrkOK(TrkSetStringFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), //buf
                                                 s.toLocal8Bit().constData())))
        {
            if(recMode != Insert)
                readFieldValue(lockHandle, vid);
            //values[vid] = value;
            emit changed(recordId());
        }
		break;
	case TRK_FIELD_TYPE_NUMBER:
		TRK_UINT uint = value.toUInt();
        if(isTrkOK(TrkSetNumericFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), uint)))
        {
            if(recMode != Insert)
                readFieldValue(lockHandle, vid);
            //values[vid] = QVariant::fromValue<int>(uint);
            emit changed(recordId());
        }
		break;
	}
    */
}

bool TrkToolRecord::setValue(int vid, const QVariant &value)
{
    if(isEditing())
    {
        values[vid] = value;
        changedValue[vid] = true;
        return true;
    }
    return false;
}

QDomDocument TrkToolRecord::toXML()
{
    if(!textsReaded)
        readFullRecord();
	QDomDocument xml("scr");
	QDomElement root=xml.createElement("scr");
	xml.appendChild(root);
	QDomElement flds =xml.createElement("fields");
//    QHash<TRK_VID, TrkFieldDef>::const_iterator fi;
    QList<int> vids = prj->recordTypeDef(recordType())->fieldVids();
    foreach(int vid, vids) //fi = prj->recordDef[rectype]->fieldDefs.constBegin(); fi!= prj->recordDef[rectype]->fieldDefs.constEnd(); ++fi)
	{
        //QString fname = fi.value().name;
        //TRK_VID vid = fi.key();
        QString fname = prj->recordTypeDef(recordType())->fieldName(vid);
        QVariant ftext = value(vid,Qt::DisplayRole);
        QVariant fvalue = value(vid,Qt::EditRole);

		QDomElement f = xml.createElement("field");
		//QString fname = headers[c];
		f.setAttribute("name", fname);
        f.setAttribute("value",fvalue.toString());
		//QString fvalue = index(row, c).data().toString();
        QDomText v = xml.createTextNode(ftext.toString());
		f.appendChild(v);
		flds.appendChild(f);
	}
	root.appendChild(flds);
	QDomElement desc = xml.createElement("Description");
    desc.setAttribute("name", prj->recordTypeDef(recordType())->fieldName(VID_Description));
	QDomText v = xml.createTextNode(description());
	desc.appendChild(v);
	root.appendChild(desc);

    // fill <notes>
	QDomElement notes = xml.createElement("notes");

    TQNotesCol notesCol = this->notes();
//    if(lockHandle)
//        notesCol = prj->getNotes(lockHandle);
//    else
//        notesCol = prj->getNotes(recordId(),rectype);

    int index=0;
    foreach(const TQNote &tn, notesCol)
    {
        QDomElement note = xml.createElement("note");
        //const TrkNote &tn = notesCol.at(i);
        note.setAttribute("title", tn.title);
        QString fullName = tn.authorFullName;
        if(tn.authorFullName.isEmpty())
            fullName = project()->userFullName(tn.author);
        note.setAttribute("author", fullName);
        note.setAttribute("cdatetime", tn.crdate.toString(Qt::ISODate));
        note.setAttribute("createdate", tn.crdate.toString(TT_DATETIME_FORMAT));
        note.setAttribute("mdatetime", tn.mddate.toString(Qt::ISODate));
        note.setAttribute("modifydate", tn.mddate.toString(TT_DATETIME_FORMAT));
        note.setAttribute("editable", QString(tn.perm?"true":"false"));
        if(tn.isAdded)
            note.setAttribute("isAdded","true");
        if(tn.isChanged)
            note.setAttribute("isChanged","true");
        if(tn.isDeleted)
            note.setAttribute("isDeleted","true");
        note.setAttribute("index",index++);
        QDomText v = xml.createTextNode(tn.text);
        note.appendChild(v);
        notes.appendChild(note);
    }
    //delete col;
	root.appendChild(notes);

    // fill <history>
    QDomElement history = xml.createElement("history");
    index=0;
    foreach(const QVariant &item, historyList())
    {
        QVariantMap map = item.toMap();
        QDomElement change = xml.createElement("change");
//        QStringList sections = item.split(QChar(' '));
//        QString changeDate = sections[0];
//        QString changeTime = sections[1];//item.left(10+1+5);
//        QString changeDateTimeString = changeDate + " " + changeTime;
//        QDateTime changeDateTime = QDateTime::fromString(changeDateTimeString, TT_DATETIME_FORMAT);
//        QString changeAuthor = sections[2].mid(1,sections[2].length()-3);
//        QString changeDesc = QStringList(sections.mid(5)).join(" ");

        QDateTime changeDateTime = map.value("datetime").toDateTime();
        QString changeDateTimeString = changeDateTime.toString(typeDef()->dateTimeFormat());
        QString changeAuthor = map.value("author").toString();
        QString changeDesc = map.value("action").toString();
        change.setAttribute("author", changeAuthor);
        change.setAttribute("datetime", changeDateTime.toString(Qt::ISODate));
        change.setAttribute("createdate", changeDateTimeString);
        change.setAttribute("action", changeDesc);
        change.setAttribute("index",index++);
        //QDomText v = xml.createTextNode(item);
        //change.appendChild(v);
        history.appendChild(change);
    }
    root.appendChild(history);
    return xml;
}

QVariantList TrkToolRecord::historyList() const
{
    if(mode() == Insert)
        return QVariantList();
    if(historyReaded)
        return historyListMem;
    TrkToolRecord *rec = const_cast<TrkToolRecord *>(this);
    return prj->historyList(rec);
    //return QStringList();
    /*
    TrkScopeRecHandle recHandle(prj->handle, lockHandle);
    if(!recHandle.isValid())
        return QStringList();
    QStringList result;
    if(isTrkOK(TrkGetSingleRecord(*recHandle, recordId(), rectype)))
    {
        char buf[1024];
        if(isTrkOK(TrkInitChangeList(*recHandle)))
        {
            while(1)
            {
                TRK_UINT res = TrkGetNextChange(*recHandle, sizeof(buf), buf);
                if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
                    break;
                result.append(QString::fromLocal8Bit(buf));
            }
        }
    }
    return result;
    */
}


QString TrkToolRecord::description() const
{
    if(!textsReaded && !descChanged)
    {
        TrkToolRecord *rec = const_cast<TrkToolRecord *>(this);
        prj->readRecordTexts(rec);
        somethingChanged();
    }
    return desc;
    /*
	TRK_RECORD_HANDLE handle;
    if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
		return QString();
	QString result;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype)))
	{
		TRK_UINT remain;
        if(isTrkOK(TrkGetDescriptionDataLength(handle, &remain)))
		{
			char buf[1024];
			TRK_UINT res;
			while(remain)
			{
				res = TrkGetDescriptionData(handle,sizeof(buf),buf,&remain);
				if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
					break;
				result += buf;
			}
		}
	}
	TrkRecordHandleFree(&handle);
	return result;
    */
}

bool TrkToolRecord::setDescription(const QString &newDesc)
{
    if(!isEditing())
        return false;
    desc = newDesc;
    descChanged = true;
    return true;
    /*
	if(recMode != Edit && recMode != Insert)
        return false;
	QByteArray loc = desc.toLocal8Bit();
    bool res = isTrkOK(TrkSetDescriptionData(lockHandle, loc.count(), loc.constData(), 0));
        emit changed(recordId());
    return res;
    */
}

const QStringList & TrkToolRecord::fields() const
{
	return fieldList;
}

bool TrkToolRecord::isFieldReadOnly(const QString &field) const
{
    if(mode() == View)
        return true;

    bool ro=true;
    //bool ch=!lockHandle;
    TrkScopeRecHandle recHandle(prj, this);
    if(!recHandle.isValid())
        return true;
    TRK_ACCESS_MODE accMode;
    if(isTrkOK(TrkGetFieldAccessRights(*recHandle,field.toLocal8Bit().constData(),&accMode)))
        ro = !accMode;
    return ro;
}

bool TrkToolRecord::setNote(int index, const QString &title, const QString &text)
{
    if(!isEditing())
        return false;
    if(index < 0 || index >= notesList.count())
        return false;
    TQNote note = notesList[index];
    note.title = title;
    note.text = text;
    note.isChanged = true;
    notesList[index] = note;
    setModified(true);
    return true;
    /*
    bool res=false;
    bool wasView = (mode()==View);
    if(wasView)
        if(!updateBegin())
            return false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            TrkGetNextNote(*noteHandle);
        res = isTrkOK(TrkSetNoteTitle(*noteHandle, title.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(text))); // text.toLocal8Bit()));
    }
    if(wasView)
        if(res)
            commit();
        else
            cancel();
    emit changed(recordId());
    return res;
    */
}

bool TrkToolRecord::setNoteText(int index, const QString &text)
{
    if(!isEditing())
        return false;
    if(index < 0 || index >= notesList.count())
        return false;
    TQNote note = notesList[index];
    note.text = text;
    note.isChanged = true;
    notesList[index] = note;
    setModified(true);
    return true;
    /*
    bool res=false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            TrkGetNextNote(*noteHandle);
        res = isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(text))); // text.toLocal8Bit()));
    }
    emit changed(recordId());
    return res;
    */
}

bool TrkToolRecord::removeNote(int index)
{
    if(!isEditing())
        return false;
    if(index < 0 || index >= notesList.count())
        return false;
    TQNote note = notesList[index];
    note.isDeleted = true;
    notesList[index] = note;
    setModified(true);
    return true;
    /*
    bool res=false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        int i=0;
        for(int i=0; i<=index; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return false;
        res = isTrkOK(TrkDeleteNote(*noteHandle));
        //if(res)
        //    deletedNotes.append(i);
    }
    emit changed(recordId());
    return res;*/
}

int TrkToolRecord::addNote(const QString &noteTitle, const QString &noteText)
{
    if(!isEditing())
        return -1;
    TQNote note;
    note.title = noteTitle;
    note.text = noteText;
    note.isAdded = true;
    note.crdate = QDateTime::currentDateTime();
    note.mddate = QDateTime::currentDateTime();
    notesList.append(note);
    //addedNotes[id] = note;
    setModified(true);
    return notesList.count()-1;
}

/*
bool TrkToolRecord::appendNote(const QString &noteTitle, const QString &note)
{

    if(!isEditing())
        return false;
    bool res=false;
    bool wasView = (mode()==View);
    if(wasView)
        if(!updateBegin())
            return false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkAddNewNote(*noteHandle)))
    {
        res = isTrkOK(TrkSetNoteTitle(*noteHandle, noteTitle.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, stringToLocal8Bit(note))); // note.toLocal8Bit()));
//        if(res)
//        {
//            TrkNote tmpNote;
//            project()->readNoteRec(*noteHandle,&tmpNote);
//            addedNotes.append(tmpNote);
//        }
    }
    if(wasView)
        if(res)
            commit();
        else
            cancel();
    emit changed(recordId());
    return res;

}
*/

QString TrkToolRecord::noteTitle(int index)
{
    return notesList.length() > index ? notesList[index].title : QString();
    /*
    TRK_RECORD_HANDLE handle=0;
    if(isEditing())
        handle = lockHandle;
    TrkScopeRecHandle rec(project()->handle, handle);
    if(!isEditing())
        rec.getRecord(recordId(),recordType());
    TrkScopeNoteHandle noteHandle(*rec);
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return QString();
        char buf[1024];
        TrkGetNoteTitle(*noteHandle,sizeof(buf),buf);
        return QString::fromLocal8Bit(buf);
    }
    return QString();
    */
}

QString TrkToolRecord::noteText(int index)
{
    return notesList.length() > index ? notesList[index].text : QString();
    /*
    TRK_RECORD_HANDLE handle=0;
    if(isEditing())
        handle = lockHandle;
    TrkScopeRecHandle rec(project()->handle, handle);
    if(!isEditing())
        rec.getRecord(recordId(),recordType());
    TrkScopeNoteHandle noteHandle(*rec);
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return QString();
        QString text;
        TRK_UINT len;
        if(isTrkOK(TrkGetNoteDataLength(*noteHandle, &len) && len))
        {
            char buf[1024];
            TRK_UINT remain = len;
            text = "";
            TRK_UINT res;
            while(remain)
            {
                //int blen = remain < sizeof(buf) ? remain : sizeof(buf);
                res = TrkGetNoteData(*noteHandle, sizeof(buf), buf, &remain);
                text += QString::fromLocal8Bit(buf);
                if(res!=TRK_SUCCESS && res !=TRK_E_DATA_TRUNCATED)
                    break;
            }
        }
        return text;
    }
    return QString();
    */
}

/*
void TrkToolRecord::addLink()
{
    links++;
}

void TrkToolRecord::removeLink(const QObject *receiver)
{
    if(!this)
        return;
    if(receiver)
        disconnect(receiver);
    if(--links <= 0)
        deleteLater();
}
*/

TQNotesCol TrkToolRecord::notes() const
{
    if(!textsReaded)
    {
        prj->readRecordTexts(const_cast<TrkToolRecord*>(this));
        somethingChanged();
    }
    return notesList;
    /*
    NotesCol res;
    if(lockHandle)
        res = prj->doGetNotes(lockHandle);
    else
        res = prj->getNotes(recordId(),rectype);
    //res.append(addedNotes);
    return res;
    */
}


//================== TrkHistory =======================

TrkHistory::TrkHistory(QObject *parent)
    : BaseRecModel(parent),
      prj(0)
{
    headers << tr("Выборка");
    headers << tr("Тип");
    headers << tr("Записи");
    headers << tr("Тип записей");
    headers << tr("Время");
}

TrkHistory::~TrkHistory()
{
}

void TrkHistory::setProject(TQAbstractProject *project)
{
    if(prj!=project)
    {
        clearRecords();
//        if(prj)
//            prj->disconnect(this);
        prj = project;
        if(prj)
            connect(prj,SIGNAL(openedModel(const TQRecModel*)),this,SLOT(openedModel(const TQRecModel*)));
    }
}

void TrkHistory::setUnique(bool value)
{
    unique = value;
}

void TrkHistory::removeLast()
{
    records.removeLast();
}

QVariant TrkHistory::displayColData(const TrkHistoryItem &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.queryName;
    case 1:
        return rec.isQuery;
    case 2:
        return rec.foundIds;
    case 3:
        return QVariant((uint)rec.rectype);
    case 4:
        return rec.createDateTime;
    }
    return QVariant();
}

void TrkHistory::openedModel(const TQRecModel *model)
{
    TrkHistoryItem item;
    bool isNew=true;
    QString qName = model->queryName();
    int i;
    if(unique)
        for(i = 0; i < records.count(); i++)
        {
            const TrkHistoryItem &fi = records[i];
            if(fi.projectName == model->prj->projectName()
                    && fi.queryName == qName)
            {
                isNew = false;
                break;
            }
        }
    if(!isNew)
        item = records[i];
    else
    {
        item.projectName = model->prj->projectName();
        item.queryName = model->queryName();
    }
    item.foundIds = intListToString(model->getIdList());
    item.isQuery = model->isQueryType();
    item.rectype = model->recordType();
    item.createDateTime = QDateTime::currentDateTime();
    if(isNew)
        append(item);
    else
        records[i]=item;
}


// ======================= TrkToolQryModel ====================================

TrkToolQryModel::TrkToolQryModel(QObject *parent)
    : BaseRecModel(parent)
{
    headers
            << tr("Название выборки")
            << tr("Публичная")
//            << tr("Группа")
            << tr("Тип записи")
               ;

}

void TrkToolQryModel::appendQry(const QString &queryName, bool isPublic, TRK_UINT rectype)
{
    TrkQuery query;
    query.qryName = queryName;
    query.isPublic = isPublic;
    query.qryTypeId = rectype;
    append(query);
}

void TrkToolQryModel::appendQry(const QStringList &queryList, bool isPublic, TRK_UINT rectype)
{
    QList<TrkQuery> list;
    foreach(QString qryName, queryList)
    {
        TrkQuery query;
        query.qryName = qryName;
        query.isPublic = isPublic;
        query.qryTypeId = rectype;
        list.append(query);
    }
    append(list);
}

void TrkToolQryModel::removeQry(const QString &queryName, TRK_UINT rectype)
{
    int r=0;
    foreach(const TrkQuery &rec, records)
    {
        if(rec.qryName == queryName && rec.qryTypeId == rectype)
        {
            beginRemoveRows(QModelIndex(),r,r);
            records.removeAt(r);
            endRemoveRows();
        }
        r++;
    }
}

QVariant TrkToolQryModel::displayColData(const TrkQuery &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.qryName;
    case 1:
        return rec.isPublic ? "true" : "false";
    case 2:
        return rec.qryTypeId;
    case 3:
        return rec.isPublic;
    }
    return QVariant();
}

//ChoiceList TrkFieldDef::emptyChoices;

TrkFieldDef::TrkFieldDef(TrkRecordTypeDef *recordDef, int fieldVID)
    : //AbstractFieldDef(recordDef, fieldVID)
    recDef(recordDef), name(), nativeType(TRK_FIELD_TYPE_NONE),  cashedChoiceList(new TQChoiceList()),
    vid(fieldVID),
      nullable(false), minValue(0), maxValue(INT_MAX)
{

}

TrkFieldDef::TrkFieldDef(const TrkFieldDef &src)
    : //AbstractFieldDef(src)
    recDef(src.recDef), name(src.name), nativeType(src.nativeType), cashedChoiceList(new TQChoiceList(*src.cashedChoiceList)),
      nullable(src.nullable), minValue(src.minValue), maxValue(src.maxValue), defaultValue(src.defaultValue),
    vid(src.vid)
{

}

TQChoiceList TrkFieldDef::choiceList() const
{
    if(!isChoice())
        return TQChoiceList();

    if(recDef && !cashedChoiceList->count())
    {
        if(isNullable())
        {
            TQChoiceItem ch;
            ch.displayText="";
            ch.fieldValue=QVariant();
            ch.order = 0;
            ch.weight = 0;
            cashedChoiceList->append(ch);
        }
        QString table = recDef->fieldChoiceTable(vid);
        if(!table.isEmpty())
            cashedChoiceList->append(recDef->choiceTable(table));
    }
    return *cashedChoiceList;
}

QStringList TrkFieldDef::choiceStringList(bool isDisplayText) const
{
    QStringList list;
    //const ChoiceList &chList = choiceList();
    foreach(const TQChoiceItem &item, choiceList())
    {
        if(isDisplayText)
            list.append(item.displayText);
        else
            list.append(item.fieldValue.toString());
    }
    return list;
}

/*
bool TrkFieldDef::canSubmit()
{
    return recDef->canFieldSubmit(vid);
}

bool TrkFieldDef::canUpdate()
{
    return recDef->canFieldUpdate(vid);
}
*/

QString TrkFieldDef::valueToDisplay(const QVariant &value) const
{
    return recDef->valueToDisplay(vid, value);
}

QVariant TrkFieldDef::displayToValue(const QString &text) const
{
    return recDef->displayToValue(vid, text);
}

TrkRecordTypeDef *TrkFieldDef::recordDef() const
{
    return (TrkRecordTypeDef *)recDef;
}

// ================ RecordTypeDef =================
TrkRecordTypeDef::TrkRecordTypeDef(TrkToolProject *project)
    : QObject(project), TQBaseRecordTypeDef(project), prj(project), fieldDefs(), nameVids(), recType(-1), isReadOnly(true), choices()
{}
//RecordTypeDef(const RecordTypeDef & src): prj(src.prj), fieldDefs(src.fieldDefs), nameVids(src.nameVids) {}

TrkRecordTypeDef::~TrkRecordTypeDef()
{
    clearFieldDefs();
    nameVids.clear();
    QHash<QString, TQChoiceList *>::iterator i = choices.begin();
    while (i != choices.end()) {
        TQChoiceList *list = i.value();
        i++;
        delete list;
    }
    choices.clear();
}

int TrkRecordTypeDef::roleVid(int role) const
{
    switch(role)
    {
    case TQAbstractRecordTypeDef::IdField:
        return VID_Id;
    case TQAbstractRecordTypeDef::TitleField:
        return VID_Title;
    case TQAbstractRecordTypeDef::DescriptionField:
        return VID_Description;
    case TQAbstractRecordTypeDef::StateField:
        return VID_State;
    case TQAbstractRecordTypeDef::SubmitDateTimeField:
        return VID_SubmitDate;
    case TQAbstractRecordTypeDef::SubmitterField:
        return VID_Submitter;
    case TQAbstractRecordTypeDef::OwnerField:
        return VID_Owner;
    }
    return 0;
}

void TrkRecordTypeDef::clearFieldDefs()
{
    TrkVidDefs::iterator i;
    while((i = fieldDefs.begin()) != fieldDefs.end())
    {
        TrkFieldDef *def = i.value();
        fieldDefs.erase(i);
        delete def;
    }
    fieldDefs.clear();
}


QStringList TrkRecordTypeDef::fieldNames() const
{
    return nameVids.keys();
    /*
    QStringList res;
    NameVid::const_iterator nv = nameVids.begin();
    while(nv != nameVids.end())
    {
        res << nv.key();
        nv++;
    }
    return res;
    */
}
TQAbstractFieldType TrkRecordTypeDef::getFieldType(int vid, bool *ok) const
{
    bool res = fieldDefs.contains(vid);
    if(ok)
        *ok = res;
    if(!res)
        return TQAbstractFieldType();
    return TQAbstractFieldType(this,vid);
}
TQAbstractFieldType TrkRecordTypeDef::getFieldType(const QString &name, bool *ok) const
{
    TRK_VID vid = nameVids[name];
    return getFieldType(vid,ok);
}

int TrkRecordTypeDef::fieldNativeType(const QString &name) const
{
    TRK_VID vid = fieldVid(name);
    return fieldNativeType(vid);
}

/*
const TrkFieldDef * findFieldDef(const QString &name) const
{
    if(!nameVids.contains(name))
        return 0;
    TRK_VID vid = nameVids[name];
    return fieldDefs.  [vid];
}
*/
/*
int RecordTypeDef::fieldNativeType(const QString &name) const // not work????
{
    return getFieldDef(name).fType();
}
*/

int TrkRecordTypeDef::fieldNativeType(int vid) const
{
    const TrkFieldDef *fdef = fieldDefs.value(vid,0);
    if(fdef)
        return fdef->nativeType;
    return TRK_FIELD_TYPE_NONE;
}

int TrkRecordTypeDef::fieldSimpleType(int vid) const
{
    int native = fieldNativeType(vid);
    switch(native)
    {
    case TRK_FIELD_TYPE_NONE:
        return TQ::TQ_FIELD_TYPE_NONE;
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_STATE:
        return TQ::TQ_FIELD_TYPE_CHOICE;
    case TRK_FIELD_TYPE_STRING:
        return TQ::TQ_FIELD_TYPE_STRING;
    case TRK_FIELD_TYPE_NUMBER:
        return TQ::TQ_FIELD_TYPE_NUMBER;
    case TRK_FIELD_TYPE_DATE:
        return TQ::TQ_FIELD_TYPE_DATE;
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
        return TQ::TQ_FIELD_TYPE_USER;
    case TRK_FIELD_TYPE_ELAPSED_TIME:
        return TQ::TQ_FIELD_TYPE_STRING;

    }
    return TQ::TQ_FIELD_TYPE_NONE;
}


bool TrkRecordTypeDef::canFieldSubmit(int vid) const
{
    return prj->canFieldSubmit(vid, recType);
}

bool TrkRecordTypeDef::canFieldUpdate(int vid) const
{
    return prj->canFieldUpdate(vid, recType);
}

bool TrkRecordTypeDef::isNullable(int vid) const
{
    if(fieldDefs.contains(vid))
        return fieldDefs.value(vid)->nullable;
    return false;
}

bool TrkRecordTypeDef::hasChoiceList(int vid) const
{
    switch(fieldNativeType(vid))
    {
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_STATE:
        return true;
    }
    return false;
}

/*
ChoiceList TQRecordTypeDef::choiceList(int vid) const
{
    if(!choices.contains(vid))
    {
        QString fName = fieldName(vid);
        if(fName.isEmpty())
            return ChoiceList();
        choices.insert(vid, prj->fieldChoiceList(fName, recType));
    }
    return *choices[vid];
}


ChoiceList TQRecordTypeDef::choiceList(const QString &fieldName) const
{
    TRK_VID vid = nameVids.contains(fieldName) ? nameVids[fieldName] : -1;
    if(vid == -1)
        return QList<TrkToolChoice>(); //&emptyChoices;
    return choiceList(vid);
}
*/

TQChoiceList TrkRecordTypeDef::choiceTable(const QString &tableName) const
{
    const char *ttag = "Table_";
    if(choices.contains(tableName))
    {
        return *choices[tableName];
    }
    else if(tableName == "Users")
    {
//        if(!userChoices.isEmpty())
//            return userChoices;
        TQChoiceList res;
        QMultiMap<QString, QString> nameToLogin;
        foreach(const TQUser &item, prj->m_userList)
            nameToLogin.insert(item.displayName, item.login);
        int order = 0;
        foreach(QString login, nameToLogin)
        {
//        QMap<QString, TQUser>::const_iterator i;
//        for (i = prj->m_userList.constBegin(); i != prj->m_userList.constEnd(); ++i)
//        {
            const TQUser &u = prj->m_userList[login];
            TQChoiceItem item;
            item.id = u.id;
//            QString login = i.key();
            item.fieldValue = login;
            item.displayText = u.displayName;
            item.order = ++order;
            item.weight = 1;
            res.append(item);
        }
        return res;
    }
    else if(!tableName.indexOf(QString(ttag)))
    {
        QString fName = tableName.mid(QString(ttag).length());
//        return choiceList(fName);
        if(fName.isEmpty())
            return TQChoiceList();
        if(!choices.contains(tableName))
        {
            TQChoiceList *list = prj->fieldChoiceList(fName, recType);
            choices.insert(tableName, list);
            return *list;
        }
        return *choices[tableName];
    }
    return emptyChoices;
}

bool TrkRecordTypeDef::containFieldVid(int vid) const
{
    return fieldDefs.contains(vid);
}

/* unify interface
QList<TRK_VID> RecordTypeDef::fieldVids() const
{
    return nameVids.values();
}
*/

int TrkRecordTypeDef::fieldVid(const QString &name) const
{
    return nameVids.value(name);
}

QList<int> TrkRecordTypeDef::fieldVids() const
{
    QList<int> vids;
    foreach(int vid, nameVids.values())
        vids << vid;
    return vids;
}

int TrkRecordTypeDef::recordType() const
{
    return recType;
}

QString TrkRecordTypeDef::fieldName(int vid) const
{
    if(fieldDefs.contains(vid))
        return fieldDefs.value(vid)->name;
    return QString();
}

QString TrkRecordTypeDef::fieldSystemName(int vid) const
{
    return fieldName(vid);
}

QString TrkRecordTypeDef::fieldRoleName(int vid) const
{
    return QString();
}

QIODevice *TrkRecordTypeDef::defineSource() const
{
    QString fileName = project()->optionValue(TQOPTION_GROUP_FILE).toString();
    QFile *file = new QFile(fileName);
    if(!file->exists())
    {
        delete file;
        return 0;
    }
    return file;
//    return new QFile("data/tracker.xml");
}

QString TrkRecordTypeDef::valueToDisplay(int vid, const QVariant &value) const
{
    QDateTime dt;
    switch(fieldNativeType(vid))
    {
    case TRK_FIELD_TYPE_DATE:
            dt = value.toDateTime();
            return dt.toString(TT_DATETIME_FORMAT);
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_STATE:
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
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STRING:
    case TRK_FIELD_TYPE_NUMBER:
        return value.toString();
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
        return project()->userFullName(value.toString());
    }
    return value.toString();
}

QVariant TrkRecordTypeDef::displayToValue(int vid, const QString &text) const
{
    QDateTime dt;
    switch(fieldNativeType(vid))
    {
    case TRK_FIELD_TYPE_DATE:
        dt = QDateTime::fromString(text, dateTimeFormat()); //  TT_DATETIME_FORMAT
        return QVariant(dt);
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_STATE:
    {
        QString table = fieldChoiceTable(vid);
        if(!table.isEmpty())
        {
            TQChoiceList list = choiceTable(table);
            foreach(const TQChoiceItem &c, list)
            {
                if(c.displayText.compare(text,Qt::CaseInsensitive) == 0)
                    return c.fieldValue;
            }
        }
        return QVariant(QVariant::String);
    }
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STRING:
        return text;
    case TRK_FIELD_TYPE_NUMBER:
        bool ok;
        int v = text.toInt(&ok);
        if(!ok)
            return QVariant(QVariant::Int);
        return v;

    }
    return text;
}

QVariant TrkRecordTypeDef::fieldDefaultValue(int vid) const
{
    const TrkFieldDef *fDef = fieldDefs.value(vid,0);
    if(!fDef)
        return QVariant();
    return fDef->defaultValue;
}

QVariant TrkRecordTypeDef::fieldMinValue(int vid) const
{
    const TrkFieldDef *fDef = fieldDefs.value(vid,0);
    if(!fDef)
        return QVariant();
    if(fDef->nativeType == TRK_FIELD_TYPE_DATE)
        return QVariant(QDateTime::fromTime_t(0));
    else if(fDef->nativeType == TRK_FIELD_TYPE_NUMBER)
        return fDef->minValue;
    return QVariant();
}

QVariant TrkRecordTypeDef::fieldMaxValue(int vid) const
{
    const TrkFieldDef *fDef = fieldDefs.value(vid,0);
    if(!fDef)
        return QVariant();
    return fDef->maxValue;
}

QString TrkRecordTypeDef::fieldChoiceTable(int vid) const
{
    const TrkFieldDef *fDef = fieldDefs.value(vid,0);
    if(!fDef || !fDef->isChoice())
        return QString();
    if(fDef->isUser())
        return "Users";
    return "Table_" + fDef->name;
}

int TrkRecordTypeDef::fieldRole(int vid) const
{
    if(vid == VID_Id)
        return TQAbstractRecordTypeDef::IdField;
    if(vid == VID_Description)
        return TQAbstractRecordTypeDef::DescriptionField;
    if(vid == VID_Title)
        return TQAbstractRecordTypeDef::TitleField;
    if(vid == VID_Owner)
        return TQAbstractRecordTypeDef::OwnerField;
    if(vid == VID_SubmitDate)
        return TQAbstractRecordTypeDef::SubmitDateTimeField;
    if(vid == VID_Submitter)
        return TQAbstractRecordTypeDef::SubmitterField;
    return NoRole;
}

QString TrkRecordTypeDef::dateTimeFormat() const
{
    return TT_DATETIME_FORMAT;
}

QStringList TrkRecordTypeDef::noteTitleList() const
{
    return ((TrkToolProject *)project())->noteTitleList();
}

TQAbstractProject *TrkRecordTypeDef::project() const
{
    return prj;
}

int TrkRecordTypeDef::fieldVidByInternalId(int id) const
{
    foreach(const TrkFieldDef *fDef, fieldDefs)
        if(fDef->internalId == id)
            return fDef->vid;
    return 0;
}

int TrkRecordTypeDef::fieldInternalIdByVid(int vid) const
{
    const TrkFieldDef *fDef = fieldDefs.value(vid,0);
    if(!fDef)
        return 0;
    return fDef->internalId;
}

QString TrkRecordTypeDef::typeName() const
{
    return recTypeName;
}

// ============ TrkScopeRecHandle =================
/*
TrkScopeRecHandle::TrkScopeRecHandle(TRK_HANDLE prjHandle, TRK_RECORD_HANDLE recHandle, TRK_UINT recId, TRK_RECORD_TYPE rectype)
    :handle(0), isOk(true), fromPrj(false), pRecHandler(0)
{
    if(recHandle)
    {
        handle = recHandle;
        isTemp = false;
    }
    else
    {
        isOk = isTrkOK(TrkRecordHandleAlloc(prjHandle, &handle));
        isTemp = true;
    }
    if(isTemp && recId > 0)
        isOk = isOk && isTrkOK(TrkGetSingleRecord(handle, recId, rectype));
}
*/

TrkScopeRecHandle::TrkScopeRecHandle(TrkToolProject *prj, TRK_RECORD_HANDLE recHandle, TRK_UINT recId, TRK_RECORD_TYPE rectype)
    :handle(0), isOk(true), fromPrj(true), isTemp(true), project(prj), fromRec(false)
{
    if(recHandle)
    {
        handle = recHandle;
        isTemp = false;
    }
    else
    {
        pRecHandler = prj->allocRecHandler(recId, rectype);
        handle = pRecHandler ? pRecHandler->handle : 0;
        isOk = handle != 0;
    }
}

TrkScopeRecHandle::TrkScopeRecHandle(TrkToolProject *prj, const TrkToolRecord *record)
    :handle(0), isOk(true), fromPrj(true), isTemp(true), project(prj)
{
    if(record->handleAllocated)
    {
        pRecHandler = new TrkRecHandler();
        pRecHandler->handle = record->trkHandle;
        pRecHandler->isInsert = record->mode() == TQRecord::Insert;
        pRecHandler->isModify = record->mode() == TQRecord::Edit;
        pRecHandler->recType = record->recordType();
        pRecHandler->id = record->recordId();
        fromPrj = false;
        fromRec = true;
    }
    else
    {
        pRecHandler = prj->allocRecHandler(record->recordId(), record->recordType());
        fromRec = false;
    }
    handle = pRecHandler ? pRecHandler->handle : 0;
    isOk = handle != 0;
}

TrkScopeRecHandle::~TrkScopeRecHandle()
{
    if(handle && isTemp)
        if(fromPrj)
        {
            if(pRecHandler)
                project->freeRecHandler(pRecHandler);
            handle = 0;
        }
        else
            if(!fromRec)
                TrkRecordHandleFree(&handle);
}

TRK_RECORD_HANDLE &TrkScopeRecHandle::nativeHandle()
{
    if(fromPrj)
        return pRecHandler ? pRecHandler->handle : handle;
    else
        return handle;
}

//int TrkScopeRecHandle::getRecord(TRK_UINT id, TRK_RECORD_TYPE rectype)
//{
//    return TrkGetSingleRecord(handle, id, rectype);
//}


TrkScopeNoteHandle::TrkScopeNoteHandle(TRK_RECORD_HANDLE recHandle)
{
    handle = 0;
    if(recHandle)
        TrkNoteHandleAlloc(recHandle, &handle);
}

TrkScopeNoteHandle::~TrkScopeNoteHandle()
{
    if(handle)
        TrkNoteHandleFree(&handle);
}

static const int MAX_HANDLERS_COUNT = 100;

TrkRecHandler *TrkToolProject::allocRecHandler(int recID, TRK_RECORD_TYPE recType)
{
    QMutexLocker locker(&handlerMutex);
    /*
    TrkRecHandler *ph = pointTrkRecHandler(recID, recType);
    if(ph)
    {
        ph->links++;
        return ph->handle;
    }
    */

    for(int i=0; i<handlers.count(); i++)
    {
        TrkRecHandler &ph = handlers[i];
        if(ph.recType == recType && ph.id == recID)
        {
            ph.links++;
            return &ph;
        }
    }

    for(int i=0; i<handlers.count(); i++)
    {
        TrkRecHandler &ph = handlers[i];
        if(ph.recType == recType && ph.links <= 0 && !ph.isModify && !ph.isInsert)
        {
            if(recID && !isTrkOK(TrkGetSingleRecord(ph.handle, recID, recType)))
                return 0;
            ph.links = 1;
            ph.id = recID;
            return &ph;
        }
    }

    if(handlers.count() >= MAX_HANDLERS_COUNT)
    {
        for(int i=0; i<handlers.count(); i++)
        {
            TrkRecHandler &h = handlers[i];
            if(h.links <= 0 && !h.isModify && !h.isInsert)
            {
                TrkRecordHandleFree(&h.handle);
                handlers.removeAt(i);
                break;
            }
        }
    }
    TrkRecHandler h;
    if(!isTrkOK(TrkRecordHandleAlloc(handle,&h.handle)))
        return 0;
    if(recID > 0 && !isTrkOK(TrkGetSingleRecord(h.handle, recID, recType)))
    {
        TrkRecordHandleFree(&h.handle);
        return 0;
    }
    h.id = recID;
    h.isInsert = false;
    h.isModify = false;
    h.links = 1;
    h.recType = recType;
    handlers.append(h);
    return &handlers.last();
}

void TrkToolProject::freeRecHandler(TrkRecHandler *recHandler)
{
    QMutexLocker locker(&handlerMutex);
    if(!recHandler)
        return;
    for(int i=0; i<handlers.count(); i++)
    {
        TrkRecHandler &h = handlers[i];
        if(&h == recHandler)
        {
            --h.links;
//            if(h.links == 0 && !h.isInsert && !h.isModify)
//            {
//                h.id = -1;
//                TrkRecordHandleFree(&h.handle);
//                handlers.removeAt(i);
//            }
            return;
        }
    }
}

void TrkToolProject::clearAllHandlers()
{
    QMutexLocker locker(&handlerMutex);
    for(int i=0; i<handlers.count(); i++)
    {
        TrkRecHandler &h = handlers[i];
        if(h.handle)
            TrkRecordHandleFree(&h.handle);
    }
    handlers.clear();
}

bool TrkToolProject::resetRecHandler(TrkRecHandler *rec)
{
    QMutexLocker locker(&handlerMutex);
    if(rec->id > 0)
        return isTrkOK(TrkGetSingleRecord(rec->handle,rec->id,rec->recType));
    return true;
}

void TrkToolProject::invalidateRecHandler(int recID)
{
    QMutexLocker locker(&handlerMutex);
    for(int i=0; i<handlers.count(); i++)
    {
        TrkRecHandler &h = handlers[i];
        if(h.id == recID && h.links <= 0)
            h.id = -1;
    }
}

//TrkRecHandler *TrkToolProject::pointTrkRecHandler(int recID, TRK_RECORD_TYPE recType)
//{
//    for(int i=0; i<handlers.count(); i++)
//    {
//        TrkRecHandler &h = handlers[i];
//        if(h.recType == recType && h.id == recID)
//            return &h;
//    }
//}
