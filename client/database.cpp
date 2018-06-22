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

