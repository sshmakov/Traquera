#include "ttglobal.h"
#include "settings.h"
#ifdef CLIENT_APP
#include "mainwindow.h"
#include "querypage.h"
#include "ttrecwindow.h"
#endif
#include <QDomDocument>
#include <QtSql>
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QXmlInputSource>
#include <Windows.h>
#include <QMessageBox>

static TTGlobal *ttGlobal=0;

TTGlobal::TTGlobal(QObject *parent) :
    QObject(parent),
    userDb(),
    handlers(),
    plugins()
{
    settingsObj = new QSettings(COMPANY_NAME, PRODUCT_NAME);
    //ttGlobal = this;
    readInitSettings();
}


TTGlobal *TTGlobal::global()
{
    if(!ttGlobal)
        ttGlobal=new TTGlobal();
    return ttGlobal;
}

QSqlDatabase TTGlobal::userDatabase()
{
    if(!userDb.isOpen())
    {
        userDb = QSqlDatabase::addDatabase(userDbType,"userDatabase");
        userDb.setDatabaseName(userDbPath);
        bool opened;
        if(!userDbUser.isEmpty())
            opened = userDb.open(userDbUser, userDbPassword);
        else
            opened = userDb.open();
        if(opened)
            upgradeUserDB();
        else
            SQLError(userDb.lastError());
    }
    return userDb;
}

QString TTGlobal::toOemString(const QString &s)
{
    return s;
}

QSettings *TTGlobal::settings()
{
    return settingsObj;
}

QMainWindow *TTGlobal::mainWindow()
{
    return mainWin;
}

void TTGlobal::showError(const QString &text)
{
    if(mainWin)
        mainWin->statusBar()->showMessage(text,10000);
}

void TTGlobal::readInitSettings()
{
    QFile file("data/init.xml");
    QXmlInputSource source(&file);
    QDomDocument dom;
    if(!dom.setContent(&source,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return;
    QDomElement dbNode = doc.firstChildElement("userdb");
    if(!dbNode.isNull())
    {
        userDbType = dbNode.attribute("type",userDbType);
        userDbPath = dbNode.attribute("path",userDbPath);
        userDbUser = dbNode.attribute("user",userDbUser);
        userDbPassword = dbNode.attribute("password",userDbPassword);
    }
    userDbType = settingsObj->value("UserDBType",userDbType).toString();
    userDbPath = settingsObj->value("UserDBPath",userDbPath).toString();
    userDbUser = settingsObj->value("UserDBUser",userDbUser).toString();
    userDbPassword = settingsObj->value("UserDBPass",userDbPassword).toString();
    QDomElement plugNode = doc.firstChildElement("plugins");
    if(!plugNode.isNull())
    {
        pluginDirectory = plugNode.attribute("directory","plugins");
        QDomElement p = plugNode.firstChildElement("plugin");
        while(!p.isNull())
        {
            QString path = p.attribute("path");
            if(!path.isEmpty())
                anotherPlugins.append(path);
            p = p.nextSiblingElement("plugin");
        }
    }
}

void TTGlobal::upgradeUserDB()
{
    if(userDb.tables().contains("recordInFolder",Qt::CaseInsensitive))
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
    while (!in.atEnd()) {
        QString line = in.readLine();
        QSqlQuery q(userDb);
        if(!q.exec(line))
        {
            SQLError(q.lastError());
            return false;
        }
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

bool TTGlobal::registerEventHandler(const QString &event, QObject *obj, const QString &method)
{
    handlers.insert(event, qMakePair(obj, method));
    return true;
}

bool TTGlobal::unregisterEventHandler(const QString &event, QObject *obj, const QString &method)
{
    handlers.remove(event, qMakePair(obj, method));
    return true;
}

void TTGlobal::handleEvent(const QString &event, QGenericReturnArgument ret, QGenericArgument val0, QGenericArgument val1,
                           QGenericArgument val2, QGenericArgument val3, QGenericArgument val4, QGenericArgument val5,
                           QGenericArgument val6, QGenericArgument val7, QGenericArgument val8, QGenericArgument val9)
{
    QPair<QObject *,QString> pair;
    foreach(pair, handlers.values(event))
    {
        QMetaObject::invokeMethod(pair.first,pair.second.toLocal8Bit().constData(),Qt::DirectConnection,
                                  ret, val0, val1, val2, val3, val4, val5, val6, val7, val8, val9);
    }
}

void TTGlobal::loadPlugins()
{
    QDir dir(pluginDirectory);
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
    foreach(QString path, anotherPlugins)
    {
        loadSinglePlugin(path);
    }
}

bool TTGlobal::loadSinglePlugin(const QString &path)
{
    QObject *p=0;
    if(!QLibrary::isLibrary(path))
        return false;
    QPluginLoader loader(path);
    if(!loader.load())
        return false;
    p = loader.instance();
    if(!p)
        return false;
    plugins.append(p);
    connect(p,SIGNAL(error(QString,QString)),SLOT(pluginError(QString,QString)));
    QMetaObject::invokeMethod(p, "initPlugin", Q_ARG(QObject *,this), Q_ARG(QString, path));
#ifdef CLIENT_APP
    QWidget *prop=0;
    if(QMetaObject::invokeMethod(p, "getPropWidget",
                                  Q_RETURN_ARG(QWidget *,prop),
                                  Q_ARG(QWidget *, mainWin))
            && prop)
        mainWin->addPropWidget(prop);
#endif
    return true;
}

void TTGlobal::appendContextMenu(QMenu *menu)
{
    foreach(QObject *plugin, plugins)
        QMetaObject::invokeMethod(plugin, "appendContextMenu", Q_ARG(QMenu *,menu));
}

void TTGlobal::queryViewOpened(QWidget *widget, QTableView *view, const QString &recType)
{
    foreach(QObject *plugin, plugins)
        QMetaObject::invokeMethod(plugin, "queryViewOpened",
                                  Q_ARG(QWidget *, widget),
                                  Q_ARG(QTableView *, view),
                                  Q_ARG(const QString &, recType));
}

void TTGlobal::recordOpened(QWidget *widget, const QString &recType)
{
    foreach(QObject *plugin, plugins)
        QMetaObject::invokeMethod(plugin, "recordOpened",
                                  Q_ARG(QWidget *, widget),
                                  Q_ARG(const QString &, recType));
}

bool TTGlobal::insertViewTab(QWidget *view, QWidget *tab, const QString &title)
{
    QueryPage *page = qobject_cast<QueryPage *>(view);
    if(page)
    {
        page->addDetailTab(tab, title, QIcon());
        return true;
    }
    TTRecordWindow *editor = qobject_cast<TTRecordWindow *>(view);
    if(editor)
    {
        editor->addDetailTab(tab, title, QIcon());
        return true;
    }
    return false;
}

void TTGlobal::pluginError(const QString &pluginName, const QString &msg)
{
    Error(tr("(%1) %2").arg(pluginName,msg));
}

