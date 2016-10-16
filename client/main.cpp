#include <QtGui>

#include "database.h"
#include "mainwindow.h"
#include "settings.h"
#include "logform.h"
#include "ttglobal.h"
#include "tqapplication.h"
//#include <tqdebug.h>
//#include <vld.h>

int main(int argc, char *argv[])
{
    LogHandler::installHandler();
    //Q_INIT_RESOURCE(masterdetail);

//    QCoreApplication::addLibraryPath("C:/gits/build/traquera-main-Desktop-Debug/plugins/tracker");
    TQApplication app(argc, argv);
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    codec = QTextCodec::codecForName("windows-1251");
    QTextCodec::setCodecForLocale(codec);
    app.setApplicationName(PRODUCT_NAME);
    app.setOrganizationName(COMPANY_NAME);

    QLocale locale = QLocale::system();
	QStringList args = app.arguments();
	for(int i=1; i<args.count(); i++)
	{
        if(args[i].trimmed().compare("-locale") == 0)
		{
			if(++i<args.count())
                locale = QLocale(args[i]);
		}
        else if(args[i].trimmed().compare("-debug") == 0)
        {
            if(++i<args.count())
            {
                bool ok;
                int level = args[i].toInt(&ok);
                if(ok)
                    TQDebug::setVerboseLevel(level);
            }
        }
	}
    QTranslator *translator;
    translator = new QTranslator(&app);
    QString langPath;
#ifdef QT_DEBUG
    langPath = "./lang";
#else
    langPath = app.applicationDirPath() + "/lang";
#endif
//    if(translator->load(QString("qt_") + locale,langPath,"_"))
    if(translator->load(locale, "qt", "_", langPath))
        app.installTranslator(translator);
    else
        delete translator;
    translator = new QTranslator(&app);
    if(translator->load(locale,"traquera",".",langPath))
        app.installTranslator(translator);
    else
        delete translator;

	 //QApplication::setStyle(new QCleanlooksStyle);

    MainWindow window;
    window.show();
    return app.exec();
}
