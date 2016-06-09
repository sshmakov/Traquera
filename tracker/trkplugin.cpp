#include <QtCore>
#include "trkplugin.h"
#include "trkview.h"

Q_EXPORT_PLUGIN2("pvcs", TrkPlugin)

static TrkPlugin *plugin;

TrkPlugin::TrkPlugin()
{
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP1251"));
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
//    QMetaObject::invokeMethod(globalObject, "registerOptionsWidget", Qt::DirectConnection,
//                              Q_ARG(const QString &,"Plugins/Jira"),
//                              Q_ARG(void *, getJiraOptionsWidget));


    QFileInfo fi(modulePath);
    pluginModule = fi.absoluteFilePath();

    QCoreApplication *app = QCoreApplication::instance();
    QLocale locale = QLocale::system();
    QTranslator *translator = new QTranslator();
    for(int i=1; i<app->argc(); i++)
    {
        if(app->arguments().value(i).trimmed().compare("-locale") == 0)
        {
            if(++i<app->argc())
                locale = QLocale(app->arguments().value(i));
            break;
        }
    }
    if(!translator->load(locale, "tracker", ".", pluginModule+"/lang"))
        qDebug() << "Can't load tracker translator";
    app->installTranslator(translator);

    TQAbstractDB::registerDbClass("PVCS Tracker", newTrkToolDB);
    /*
    QDir pDir;
    pDir = pluginModule; //fi.absoluteDir();
    if(pDir.dirName().compare("debug",Qt::CaseInsensitive) == 0)
    {
        pDir = QDir(pDir.filePath(".."));
        pDir.makeAbsolute();
    }
    pluginDir = pDir;
    dataDir = QDir(pDir.filePath("data"));
    */
}

bool TrkPlugin::saveSettings()
{
    return true;
}

bool TrkPlugin::loadSettings()
{
    return true;
}
