#include "ttglobal.h"
#include "settings.h"
#ifdef CLIENT_APP
//#include "mainwindow.h"
//#include "querypage.h"
//#include "ttrecwindow.h"
//#include "proxyoptions.h"
#endif
#include <QDomDocument>
#include <QtSql>
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QXmlInputSource>
#include <Windows.h>
#include <QtGui>
#include <tqoauth.h>
#include <tqbase.h>
#include <QAxObject>
#include <QAxScriptManager>
#include <QtScript>
#include <QtScriptTools>
#include <activefactory.h>


#ifdef Q_WS_WIN
//#include <shlobj.h>
#endif


/*
static TTGlobal *ttGlobal=0;

TTGlobal *TTGlobal::global()
{
    if(!ttGlobal)
        ttGlobal=new TTGlobal();
    return ttGlobal;
}
*/

class TTPluginInfo {
public:
    QPluginLoader *loader;
    QString name;
    QString dir;
    QString ini;
    QString library;
    QObject *instance;
    QObject *p;
    TTPluginInfo(QObject *parent) : p(parent), loader(new QPluginLoader(parent)), instance(0) {}
    bool load()
    {
        if(loader->isLoaded())
            return true;
        loader->setFileName(library);
        loader->setLoadHints(QLibrary::ExportExternalSymbolsHint);
        bool res = loader->load();
        if(res)
        {
            qDebug() << "Loaded" << library;
            instance = loader->instance();
        }
        else
        {
            instance = 0;
            qDebug() << "Not loaded" << library;
        }
        return res;
    }
};

class TTGlobalPrivate {
public:
    QString initFileName;
    QSqlDatabase userDb;
    QString userDbType;
    QString userDbPath;
    QString userDbUser;
    QString userDbPassword;
    QSettings *settingsObj;
    QNetworkAccessManager netman;


    QMultiHash <QString, QPair<QObject*,QString> > handlers;

    QList<TTPluginInfo> plugins;
    QString pluginDirectory;
    QStringList anotherPlugins;
    bool isScriptDebuggerEnabled;

    QMap<QString, GetOptionsWidgetFunc> optionsRegs;

    TTGlobalPrivate()
        : userDb(), handlers(), plugins(), isScriptDebuggerEnabled(true)
    {
    }
};

TTGlobal::TTGlobal(QObject *parent) :
    QObject(parent),
    d(new TTGlobalPrivate()),
    m_oauth(0),
    proc(0)
//    userDb(),
//    handlers(),
//    plugins()
{
    d->settingsObj = new QSettings(COMPANY_NAME, PRODUCT_NAME);
    //ttGlobal = this;
    d->initFileName = "data/init.xml";
    setObjectName("Global");
    readInitSettings();
}

QSqlDatabase TTGlobal::userDatabase()
{
    if(!d->userDb.isOpen())
    {
        d->userDb = QSqlDatabase::addDatabase(d->userDbType,"userDatabase");
        d->userDb.setDatabaseName(d->userDbPath);
        bool opened;
        if(!d->userDbUser.isEmpty())
            opened = d->userDb.open(d->userDbUser, d->userDbPassword);
        else
            opened = d->userDb.open();
        if(opened)
            upgradeUserDB();
        else
            SQLError(d->userDb.lastError());
    }
    return d->userDb;
}

QString TTGlobal::toOemString(const QString &s)
{
    return s;
}

QSettings *TTGlobal::settings()
{
    return d->settingsObj;
}

QMainWindow *TTGlobal::mainWindow()
{
    return proc->mainWindow();
}

QObject *TTGlobal::mainWindowObj()
{
    return proc->mainWindow();
}

bool TTGlobal::addPropWidget(QWidget *prop)
{
    return proc->addPropWidget(prop);
}

QString TTGlobal::getClipboardText() const
{
    return  QApplication::clipboard()->text();
}

void TTGlobal::setClipboardText(const QString &text) const
{
    QApplication::clipboard()->setText(text);
}

QString TTGlobal::getClipboardHTML() const
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    return mimeData->html();
}

void TTGlobal::setClipboardHTML(const QString &text) const
{
    QMimeData *mimeData = new QMimeData();
    mimeData->setHtml(text);
    QApplication::clipboard()->setMimeData(mimeData);
}

QString TTGlobal::curProjectName() const
{
    if(!proc)
        return QString();
    TQAbstractProject *prj = proc->currentProject();
    if(!prj)
        return QString();
    return prj->projectName();
}

QObject *TTGlobal::getRecord(int id)
{
    if(!proc)
        return 0;
    TQAbstractProject *prj = proc->currentProject();
    if(!prj)
        return 0;
    return prj->createRecordById(id,prj->defaultRecType());
}

bool TTGlobal::registerOptionsWidget(const QString &path, void *funcPtr)
{
    GetOptionsWidgetFunc func= (GetOptionsWidgetFunc)funcPtr;
    d->optionsRegs.insert(path, func);
    return true;
}

QNetworkAccessManager *TTGlobal::networkManager() const
{
    return &d->netman;
}

QMap<QString, GetOptionsWidgetFunc> TTGlobal::optionsWidgets() const
{
    return d->optionsRegs;
}

QVariant TTGlobal::CreateObject(const QString &objectName)
{
    QAxObject *obj = new ActiveXObject(objectName, this);
    return obj->asVariant();
}

QString TTGlobal::saveObjectDocumentation(QObject *object, const QString &fileName) const
{
    QAxBase *ax = qobject_cast<QAxBase *>(object);
    QString doc = ax->generateDocumentation();
    if(!fileName.isEmpty())
    {
        QFile file(fileName);
        if(file.open(QFile::WriteOnly | QIODevice::Truncate))
        {
            file.write(doc.toUtf8());
        }
    }
    return doc;
}

void TTGlobal::showError(const QString &text)
{
    if(proc)
        QMessageBox::critical(mainWindow(),tr("Error"),text);
//        mainWindow()->statusBar()->showMessage(text,10000);
}


#ifndef CSIDL_COMMON_APPDATA
#define CSIDL_COMMON_APPDATA	0x0023  // All Users\Application Data
#endif

#ifndef CSIDL_APPDATA
#define CSIDL_APPDATA		0x001a	// <username>\Application Data
#endif

typedef enum {
    SHGFP_TYPE_CURRENT  = 0,   // current value for user, verify it exists
    SHGFP_TYPE_DEFAULT  = 1   // default value, may not exist
} SHGFP_TYPE;

#ifdef Q_OS_WIN
static QString windowsConfigPath(int type)
{
    QString result;
#ifndef Q_OS_WINCE
    QLibrary library(QLatin1String("shell32"));
#else
    QLibrary library(QLatin1String("coredll"));
#endif // Q_OS_WINCE
    typedef HRESULT (WINAPI*GetFolderPath)(
                HWND   hwndOwner,
                int    nFolder,
                HANDLE hToken,
                DWORD  dwFlags,
                LPTSTR pszPath);

    GetFolderPath SHGetFolderPath = (GetFolderPath)library.resolve("SHGetFolderPathW");
    if (SHGetFolderPath) {
        wchar_t path[MAX_PATH];
        path[0] = 0;
        HRESULT res = SHGetFolderPath(0, type, 0, SHGFP_TYPE_DEFAULT, path);
        if(!res)
            result = QString::fromWCharArray(path);
        else
            result = QString();
    }

    if (result.isEmpty()) {
        switch (type) {
#ifndef Q_OS_WINCE
        case CSIDL_COMMON_APPDATA:
            result = QLatin1String("C:\\temp\\qt-common");
            break;
        case CSIDL_APPDATA:
            result = QLatin1String("C:\\temp\\qt-user");
            break;
#else
        case CSIDL_COMMON_APPDATA:
            result = QLatin1String("\\Temp\\qt-common");
            break;
        case CSIDL_APPDATA:
            result = QLatin1String("\\Temp\\qt-user");
            break;
#endif
        default:
            ;
        }
    }

    return result;
}
#endif // Q_OS_WIN



static QString appDataPath()
{
    QString dir;
    QString orgName = QCoreApplication::organizationName();
    QString appName = QCoreApplication::applicationName();
#ifdef Q_WS_WIN
    dir = windowsConfigPath(CSIDL_APPDATA)+"/"+appName;
#else
    // obtain (platform specific) application's data/settings directory

    QSettings ini(QSettings::IniFormat, QSettings::SystemScope, //UserScope,
                  orgName,
                  appName);
    QString fileName = ini.fileName();
    dir = QFileInfo(fileName).absolutePath();
#endif
    return dir;
}

void TTGlobal::readInitSettings()
{
    QFile file(d->initFileName);
    QXmlInputSource source(&file);
    QDomDocument dom;
    if(!dom.setContent(&source,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return;

    QDir appDir(appDataPath());
    if(!appDir.exists())
        appDir.mkpath(".");
    d->userDbType = "QSQLITE";
    d->userDbPath = QFileInfo(appDir,"user3.db").absoluteFilePath();
    d->userDbUser = "";
    d->userDbPassword = "";

    QDomElement dbNode = doc.firstChildElement("userdb");
    if(!dbNode.isNull())
    {
        d->userDbType = dbNode.attribute("type",d->userDbType);
        d->userDbPath = dbNode.attribute("path",d->userDbPath);
        d->userDbUser = dbNode.attribute("user",d->userDbUser);
        d->userDbPassword = dbNode.attribute("password",d->userDbPassword);
    }
    d->userDbType = d->settingsObj->value("UserDBType",d->userDbType).toString();
    d->userDbPath = d->settingsObj->value("UserDBPath",d->userDbPath).toString();
    d->userDbUser = d->settingsObj->value("UserDBUser",d->userDbUser).toString();
    d->userDbPassword = d->settingsObj->value("UserDBPass",d->userDbPassword).toString();
    QDomElement plugNode = doc.firstChildElement("plugins");
    if(!plugNode.isNull())
    {
        d->pluginDirectory = plugNode.attribute("directory","plugins");
        QDomElement p = plugNode.firstChildElement("plugin");
        while(!p.isNull())
        {
            QString path = p.attribute("path");
            if(!path.isEmpty())
                d->anotherPlugins.append(path);
            p = p.nextSiblingElement("plugin");
        }
    }
}

void TTGlobal::upgradeUserDB()
{
    QStringList tables = d->userDb.tables();
    if(tables.isEmpty())
        executeBatchFile(":/sql/create.sql");
    tables = d->userDb.tables();
    if(tables.contains("recordInFolder",Qt::CaseInsensitive))
        executeBatchFile(":/sql/upgrade1.sql");
}

bool TTGlobal::executeBatchFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Error(file.errorString());
        return false;
    }
    QTextStream in(&file);
    QString line;
    while (!in.atEnd())
    {
        do
        {
            line += in.readLine();
        } while(line.trimmed().right(1) != ";" && !in.atEnd());
        QSqlQuery q(d->userDb);
        if(!q.exec(line))
        {
            SQLError(q.lastError());
            return false;
        }
        line.clear();
    }
    return true;
}

void TTGlobal::statusBarMessage(const QString &text)
{
    if(!text.isEmpty())
        showError(text);
}

void TTGlobal::shell(const QString &command)
{
    QProcess::startDetached(command);
}

int TTGlobal::shellLocale(const QString &command, const QString &locale)
{
    QString loc =locale;
    if(loc.isEmpty())
        loc ="System";
    QTextCodec *codec = QTextCodec::codecForName(loc.toLocal8Bit().constData());
    QByteArray enc = codec->fromUnicode(command);
    STARTUPINFOA info;
    PROCESS_INFORMATION pi;
    qMemSet(&info,0,sizeof(info));
    info.cb = sizeof(info);
    //info.wShowWindow = SHOW_OPENWINDOW;
    char *cmd = enc.data();
//    const WCHAR *wcmd = command.utf16();
//    BOOL res = CreateProcessW(0, (LPWSTR)wcmd, 0, 0, false, 0, 0, 0, &info, &pi);
    BOOL res = CreateProcessA(0, cmd, 0, 0, false, 0, 0, 0, &info, &pi);
    if(res)
        return 0;
    return GetLastError();
}

QString TTGlobal::initFileValue(const QString &elementPath, const QString &attr)
{
    QStringList tree = elementPath.split('/');
    QFile file(d->initFileName);
    QXmlInputSource source(&file);
    QDomDocument dom;
    if(!dom.setContent(&source,false))
        return QString();
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return QString();
    QDomElement node = doc;
    foreach(QString el, tree)
    {
        if(!node.isElement())
            return QString();
        node = node.firstChildElement(el);
    }
    if(!node.isElement())
        return QString();
    if(attr.isEmpty())
        return node.text();
    if(attr == ".")
        return node.toDocument().toString();
    return node.attribute(attr);
}

bool TTGlobal::registerEventHandler(const QString &event, QObject *obj, const QString &method)
{
    d->handlers.insert(event, qMakePair(obj, method));
    return true;
}

bool TTGlobal::unregisterEventHandler(const QString &event, QObject *obj, const QString &method)
{
    d->handlers.remove(event, qMakePair(obj, method));
    return true;
}

QObject *TTGlobal::oauth()
{
    if(!m_oauth)
        m_oauth = new TQOAuth(this);
    return m_oauth;
}

void TTGlobal::handleEvent(const QString &event, QGenericReturnArgument ret, QGenericArgument val0, QGenericArgument val1,
                           QGenericArgument val2, QGenericArgument val3, QGenericArgument val4, QGenericArgument val5,
                           QGenericArgument val6, QGenericArgument val7, QGenericArgument val8, QGenericArgument val9)
{
    QPair<QObject *,QString> pair;
    foreach(pair, d->handlers.values(event))
    {
        QMetaObject::invokeMethod(pair.first,pair.second.toLocal8Bit().constData(),Qt::DirectConnection,
                                  ret, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
    }
}

void TTGlobal::loadPlugins()
{
    QString pluginIni("plugin.ini");
    QDir dir(d->pluginDirectory);
    foreach(QString sub, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subDir(dir.filePath(sub));
        loadSinglePlugin(subDir);
        /*
        if(subDir.exists(pluginIni))
        {
            QString ini = subDir.absoluteFilePath(pluginIni);
            QSettings set(ini,QSettings::IniFormat);
            QString dll = set.value("plugin").toString();
            dll = subDir.absoluteFilePath(dll);
            if(!QLibrary::isLibrary(dll))
                continue;
            loadSinglePlugin(dll, ini);
            continue;
        }
        foreach(QString dll, subDir.entryList(QDir::Files))
        {
            dll = subDir.absoluteFilePath(dll);
            if(!QLibrary::isLibrary(dll))
                continue;
            loadSinglePlugin(dll);
        }
        */
    }
    foreach(QString path, d->anotherPlugins)
    {
        loadSinglePlugin(path);
    }
}

/*void TTGlobal::loadPlugins()
{
    QDir dir(d->pluginDirectory);
    foreach(QString sub, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        QDir subDir(dir.filePath(sub));
        foreach(QString dll, subDir.entryList(QDir::Files))
        {
            dll = subDir.absoluteFilePath(dll);
            if(!QLibrary::isLibrary(dll))
                continue;
            loadSinglePlugin(dll);
        }
    }
    foreach(QString path, d->anotherPlugins)
    {
        loadSinglePlugin(path);
    }
}
*/

bool TTGlobal::loadSinglePlugin(const QDir &dir)
{
    QString dll, ini;
    QString pluginIni("plugin.ini");
    if(dir.exists(pluginIni))
    {
        ini = dir.absoluteFilePath(pluginIni);
        QSettings set(ini,QSettings::IniFormat);
        QString file = set.value("plugin").toString();
        file = dir.absoluteFilePath(file);
        if(!QLibrary::isLibrary(file))
            return false;
        dll = file;
    }
    else
        foreach(QString file, dir.entryList(QDir::Files))
        {
            file = dir.absoluteFilePath(file);
            if(QLibrary::isLibrary(file))
            {
                dll = file;
                break;
            }
        }
    if(dll.isEmpty())
        return false;
    TTPluginInfo info(this);
    info.ini = ini;
    info.library = dll;
    QString curLib;
    QCoreApplication::addLibraryPath(dir.absolutePath());
    if(!ini.isEmpty())
    {
        QSettings set(ini,QSettings::IniFormat);
        if(set.contains("path"))
        {
            QString path = set.value("path").toString().trimmed();
            QStringList list;
            if(!path.isEmpty())
                list=path.split(";");
            foreach(path, list)
            {
                path = dir.absoluteFilePath(path);
                QCoreApplication::addLibraryPath(path);
            }
        }
        set.beginGroup("LIBS");
        foreach(QString key, set.allKeys())
        {
            QString libFile=set.value(key).toString();
            if(QLibrary::isLibrary(libFile))
            {
                QLibrary lib;
                lib.setFileName(libFile);
                lib.setLoadHints(QLibrary::ExportExternalSymbolsHint);
                if(lib.load())
                    qDebug() << "Loaded" << libFile;
                else
                    qDebug() << "Not loaded" << libFile;
            }
        }
    }
    curLib = QCoreApplication::libraryPaths().join(";");
    if(!info.load())
        return false;
    d->plugins.append(info);
    connect(info.instance,SIGNAL(error(QString,QString)),SLOT(pluginError(QString,QString)));
    QMetaObject::invokeMethod(info.instance, "initPlugin",
                              Q_ARG(QObject *,this),
                              Q_ARG(QString, dir.absolutePath()));
    /*
//#ifdef CLIENT_APP
    QWidget *prop=0;
    if(QMetaObject::invokeMethod(p, "getPropWidget",
                                  Q_RETURN_ARG(QWidget *,prop),
                                  Q_ARG(QWidget *, proc->mainWindow()))
            && prop)
        proc->addPropWidget(prop);
//#endif
*/
    return true;
}

void TTGlobal::appendContextMenu(QMenu *menu)
{
    foreach(const TTPluginInfo &info, d->plugins)
    {
        if(info.loader->isLoaded())
            QMetaObject::invokeMethod(info.instance, "appendContextMenu", Q_ARG(QMenu *,menu));
    }
}

void TTGlobal::queryViewOpened(QWidget *widget, QTableView *view, const QString &recType)
{
    foreach(const TTPluginInfo &info, d->plugins)
        if(info.loader->isLoaded())
            QMetaObject::invokeMethod(info.instance, "queryViewOpened",
                                      Q_ARG(QWidget *, widget),
                                      Q_ARG(QTableView *, view),
                                      Q_ARG(const QString &, recType));
}

void TTGlobal::recordOpened(QWidget *widget, const QString &recType)
{
    foreach(const TTPluginInfo &info, d->plugins)
        if(info.loader->isLoaded())
            QMetaObject::invokeMethod(info.instance, "recordOpened",
                                      Q_ARG(QWidget *, widget),
                                      Q_ARG(const QString &, recType));
}

bool TTGlobal::insertViewTab(QWidget *view, QWidget *tab, const QString &title)
{
    return proc->insertViewTab(view, tab, title);

}

void TTGlobal::pluginError(const QString &pluginName, const QString &msg)
{
    Error(tr("(%1) %2").arg(pluginName,msg));
}

void TTGlobal::axScriptError(QAxScript *script, int code, const QString &description, int sourcePosition, const QString &sourceText)
{
//    QObject *s = sender();
    QString s = description;
    if(code)
        s += tr(" ���:%1").arg(code);
    if(!sourceText.isEmpty())
        s += tr(" �� ������ '%1'(%2)").arg(sourceText).arg(sourcePosition);
    Error(s);
}


static QScriptValue ActiveXObjectConstructor(QScriptContext *context,
                                         QScriptEngine *engine)
{
    QAxObject *object = 0;
    QScriptValue c = context->argument(0);
    if(c.isUndefined())
        object = new QAxObject(engine);
    else if(c.isString())
    {
        object = new QAxObject(c.toString(), engine);
        //QVariant v = object->property("Application");
        //IDispatch *disp = v.value<IDispatch*>();
    }
    else //if(c.isVariant())
    {
        QVariant v = c.toVariant();
        IDispatch *disp = v.value<IDispatch*>();
        object = new QAxObject(disp, engine);
    }
    return engine->newQObject(object, QScriptEngine::ScriptOwnership);
}


QScriptEngine *TTGlobal::newScriptEngine()
{
    QScriptEngine *engine = new QScriptEngine(this);
    if(d->isScriptDebuggerEnabled)
    {
        QScriptEngineDebugger *debugger = new QScriptEngineDebugger(this);
        debugger->attachTo(engine);
    }
    QScriptValue globalObj = engine->newQObject(this);
    engine->globalObject().setProperty("Global", globalObj);

    QScriptValue ctor = engine->newFunction(ActiveXObjectConstructor);
//    QScriptValue metaObject = engine.newQMetaObject(&QAxObject::staticMetaObject, ctor);
    engine->globalObject().setProperty("ActiveXObject", ctor);
    return engine;
}

QAxScriptManager *TTGlobal::newAxScriptManager()
{
    QAxScriptManager *man = new QAxScriptManager(this);
    connect(man, SIGNAL(error(QAxScript*,int,QString,int,QString)),SLOT(axScriptError(QAxScript*,int,QString,int,QString)));
    man->addObject(this);
    return man;
}

