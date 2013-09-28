#include "ttglobal.h"
#include "settings.h"
#include "mainwindow.h"
#include <QDomDocument>
#include <QtSql>
#include <QtCore>
#include <QFile>
#include <QTextStream>
#include <QXmlInputSource>

static TTGlobal *ttGlobal=0;

TTGlobal::TTGlobal(QObject *parent) :
    QObject(parent),
    userDb()
{
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
    userDbType = settings->value("UserDBType").toString();
    userDbPath = settings->value("UserDBPath").toString();
    userDbUser = settings->value("UserDBUser").toString();
    userDbPassword = settings->value("UserDBPass").toString();
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
