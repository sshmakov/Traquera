#include <QtGui>

#include "database.h"
#include "mainwindow.h"
#include "settings.h"
#include "logform.h"
#include "ttglobal.h"
#include <tqdebug.h>

int main(int argc, char *argv[])
{
    LogHandler::installHandler();
    //Q_INIT_RESOURCE(masterdetail);

//    QCoreApplication::addLibraryPath("C:/gits/build/traquera-main-Desktop-Debug/plugins/tracker");
    QApplication app(argc, argv);
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    codec = QTextCodec::codecForName("windows-1251");
    QTextCodec::setCodecForLocale(codec);
    app.setApplicationName(PRODUCT_NAME);
    app.setOrganizationName(COMPANY_NAME);

	QString locale = QLocale::system().name();
	QStringList args = app.arguments();
	for(int i=1; i<args.count(); i++)
	{
		if(args[i].trimmed().compare("-lang") == 0)
		{
			if(++i<args.count())
				locale=args[i];
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
    QTranslator *translator = new QTranslator();
    if(translator->load(QString("traquera.") + locale,app.applicationDirPath() + "/lang"))
        app.installTranslator(translator);

	 //QApplication::setStyle(new QCleanlooksStyle);

    MainWindow window;
    window.show();
    return app.exec();
}
