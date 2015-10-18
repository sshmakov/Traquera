#include <QtCore>
#include "trkplugin.h"
#include "trkview.h"

Q_EXPORT_PLUGIN2("pvcs", TrkPlugin)

static TrkPlugin *plugin;

TrkPlugin::TrkPlugin()
{
}

static TQAbstractDB *newTrkToolDB(QObject *parent)
{

    return new TrkToolDB(parent);
}

void TrkPlugin::initPlugin(QObject *obj, const QString &modulePath)
{
//    globalObject = obj;
//    mainWindow = 0;
//    QMetaObject::invokeMethod(globalObject, "mainWindow", Qt::DirectConnection,
//                                  Q_RETURN_ARG(QMainWindow *, mainWindow));
//    QFileInfo fi(modulePath);
//    pluginModule = fi.absoluteFilePath();
//    QDir pDir;
//    pDir = fi.absoluteDir();
//    if(pDir.dirName().compare("debug",Qt::CaseInsensitive) == 0)
//    {
//        pDir = QDir(pDir.filePath(".."));
//        pDir.makeAbsolute();
//    }
//    pluginDir = pDir;
//    dataDir = QDir(pDir.filePath("data"));
////    initProjectModel();
//    if(obj)
//    {
//        QSettings *s;
//        if(QMetaObject::invokeMethod(globalObject,"settings",
//                                     Q_RETURN_ARG(QSettings *, s)))
//            settings = s;
//    }
//    if(!settings)
//        settings = new QSettings("AllRecall","JiraPlugin",this);
//    loadSettings();
    TQAbstractDB::registerDbClass("PVCS Tracker", newTrkToolDB);
//    QMetaObject::invokeMethod(globalObject, "registerOptionsWidget", Qt::DirectConnection,
//                              Q_ARG(const QString &,"Plugins/Jira"),
//                              Q_ARG(void *, getJiraOptionsWidget));
}

bool TrkPlugin::saveSettings()
{
    return true;
}

bool TrkPlugin::loadSettings()
{
    return true;
}
