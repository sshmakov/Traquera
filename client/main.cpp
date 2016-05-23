#include <QtGui>

#include "database.h"
#include "mainwindow.h"
#include "settings.h"
#include "ttglobal.h"

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(masterdetail);

//    QCoreApplication::addLibraryPath("C:/gits/build/traquera-main-Desktop-Debug/plugins/tracker");
    QApplication app(argc, argv);
//	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
//	QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
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
			break;
		}
	}
    QTranslator translator;
    translator.load(QString("traquera.") + locale,app.applicationDirPath() + "/lang");
    app.installTranslator(&translator);

	 //QApplication::setStyle(new QCleanlooksStyle);

    MainWindow window;
    window.show();
    return app.exec();
}
