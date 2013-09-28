#include <QtGui>

#include "database.h"
#include "mainwindow.h"
#include "settings.h"

QSettings *settings;


int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(masterdetail);
	settings = new QSettings(COMPANY_NAME, PRODUCT_NAME);
    QApplication app(argc, argv);
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
	QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));

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
    translator.load(QString("lang/tracks_") + locale);
    app.installTranslator(&translator);

	 //QApplication::setStyle(new QCleanlooksStyle);

    MainWindow window;
    window.show();
    return app.exec();
}
