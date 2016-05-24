#include "database.h"
//#include "tracker.h"
#include <QtGui>
#include <QAxBase>
#include <QAxObject>
//#include <QTestCodec>

QSqlDatabase persdb;

//TrkDb trkdb;

bool openPersonal()
{
	persdb = QSqlDatabase::addDatabase("QODBC","personal");
	persdb.setDatabaseName("persproj");
	return persdb.open();
}

/*
bool createConnection()
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
	
	//if(!openProject("test.mpp"))
	//	return false;
	
	if(!trkdb.open("Driver={SQL Server};Server=SHMAKOVTHINK\\SQLEXPRESS;Trusted_Connection=yes", "RS-Bank V.6"))
	//if(!trkdb.open("TRK64", "RS-Bank V.6"))
		return false;

	if(!openPersonal())
		return false;
	
	return true;
}

bool createConnection2()
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
            qApp->tr("Unable to establish a database connection.\n"
                     "This example needs SQLite support. Please read "
                     "the Qt SQL driver documentation for information how "
                     "to build it.\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }
    QSqlQuery query(db);
	query.exec("create table authors(id int primary key, name varchar(200))");

	QSqlDatabase auth = QSqlDatabase::addDatabase("QODBC","auths");
	QString afile="C:\\Develop\\Projects\\tracks\\data\\authors.xls";
	auth.setDatabaseName("DRIVER={Microsoft Excel Driver (*.xls)};FIL={MS Excel};DBQ="+afile);
	if(!auth.open())
	{
		//QString mes = QString("Cannot open authors\n%1\nClick Cancel to exit.").arg(qa.lastError().text());
		QString mes = auth.lastError().text();
        QMessageBox::critical(0, qApp->tr("Cannot open authors"), mes, 
					 QMessageBox::Cancel);
        return false;
    }
		
	//	setDatabaseName("DSN=auth");
	QSqlQuery qa(auth);
	
	//if(!qa.exec(QString("select * from %1").arg(auth.tables()[0])))
	if(!qa.exec("select * from [Лист1$]"))
	{
		//QString mes = QString("Cannot open authors\n%1\nClick Cancel to exit.").arg(qa.lastError().text());
		QString mes = qa.lastError().text();
        QMessageBox::critical(0, qApp->tr("Cannot open authors"), mes, 
					 QMessageBox::Cancel);
        return false;
    }

	query.prepare("insert into authors(id, name) values(:id, :name)");
	while (qa.next())
	{
		int i = qa.value(0).toInt();
		QString s = qa.value(1).toString();
		if(i)
		{
			query.bindValue(":id", i);
			query.bindValue(":name", s);
		}
		query.exec();
	}

	QSqlDatabase reqs = QSqlDatabase::addDatabase("QODBC","reqs");
	//reqs.setDatabaseName("reqs");
	QString rfile="C:\\Develop\\Projects\\tracks\\data\\requests.xls";
	reqs.setDatabaseName("DRIVER={Microsoft Excel Driver (*.xls)};FIL={MS Excel};DBQ="+rfile);
	if(!reqs.open())
	{
        QMessageBox::critical(0, qApp->tr("Cannot open reqs"),
            qApp->tr("Cannot open reqs\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

	QSqlQuery qr(reqs);
	if(!qr.exec("select * from [Лист1$]"))
	{
        QMessageBox::critical(0, qApp->tr("Cannot open reqs"),
            qApp->tr("Cannot open reqs\n\n"
                     "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }

	query.exec("create table requests(id int primary key, title varchar(200), authorid int)");
	query.prepare("insert into requests(id, title, authorid) values(:id, :title, :aid)");
	while (qr.next())
	{
		query.bindValue(":id", qr.value(0).toInt());
		query.bindValue(":title", qr.value(1).toString());
		query.bindValue(":aid", qr.value(2).toString());
		if(!query.exec())
			QMessageBox::critical(0,"Exec error",query.lastError().text(), QMessageBox::Cancel);
	}


    return true;
}
*/
