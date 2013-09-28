#include "trklogin.h"
#include "settings.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSettings>

TrkConnect::TrkConnect(QWidget *parent)
	: QDialog(parent)
{
	setupUi(this);
	db = QSqlDatabase::addDatabase("QODBC","trackerlogin");
	connect(connectButton,SIGNAL(clicked()),this,SLOT(reloadProjects()));
}

TrkDb * TrkConnect::connectTracker()
{
	TrkConnect *d = new TrkConnect();
	QString server = settings->value("TrackerSQLServer","").toString(); // "SHMAKOVTHINK\\SQLEXPRESS"
	QString user = settings->value("TrackerUser","").toString();
	d->serverEdit->setText(server);
	d->userEdit->setText(user);
	if(!d->exec())
		return NULL;
	TrkDb *res;
	res = new TrkDb();
	//res->db = d->db;
	res->openProject("Driver={SQL Server};Server="+d->serverEdit->text()+";Trusted_Connection=yes",d->prjBox->currentText(),d->userEdit->text());
	settings->setValue("TrackerSQLServer",d->serverEdit->text());
	settings->setValue("TrackerUser",d->userEdit->text());
	return res;
}

void TrkConnect::reloadProjects()
{
	db.close();
	QString dsn = "Driver={SQL Server};Server="+serverEdit->text()+";Trusted_Connection=yes";
	db.setDatabaseName(dsn);
	if(!db.open())
	{
        QMessageBox::critical(0, "Cannot open database",
			db.lastError().text(), QMessageBox::Cancel);
		return ;
	}
	QSqlQuery query(db);
	query.exec("select prjDescr from trkmaster.dbo.trkprj");
	prjBox->clear();
	while(query.next())
	{
		prjBox->addItem(query.value(0).toString());
	}
}