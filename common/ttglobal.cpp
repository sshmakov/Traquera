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

void TTGlobal::showError(const QString &text)
{
    if(mainWindow)
        mainWindow->statusBar()->showMessage(text,10000);
}

void TTGlobal::readInitSettings()
{
    /*
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
        userDbType = dbNode.attribute("type");
        userDbPath = dbNode.attribute("path");
        userDbUser = dbNode.attribute("user");
        userDbPassword = dbNode.attribute("password");
    }
    */
    userDbType = settingsObj->value("UserDBType").toString();
    userDbPath = settingsObj->value("UserDBPath").toString();
    userDbUser = settingsObj->value("UserDBUser").toString();
    userDbPassword = settingsObj->value("UserDBPass").toString();
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
    QObject *p;
    for(int i=0; i<1; i++)
    {
        p = new PlansPlugin(this);
        plugins.append(p);
        if(!QMetaObject::invokeMethod(p, "setGlobalObject", Q_ARG(QObject *,this)))
            continue;
        if(!QMetaObject::invokeMethod(p, "initPlugin"))
            continue;
#ifdef CLIENT_APP
        QWidget *prop=0;
        if(!QMetaObject::invokeMethod(p, "getPropWidget",
                                      Q_RETURN_ARG(QWidget *,prop),
                                      Q_ARG(QWidget *, mainWindow)))
            continue;
        if(prop)
            mainWindow->addPropWidget(prop);
#endif
    }
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
    }
}

