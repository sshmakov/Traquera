#include "trklogin.h"
#include "settings.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSettings>
#include <ttglobal.h>
#include <tqbase.h>
#include <QtGui>

TrkConnect::TrkConnect(QWidget *parent)
    : QDialog(parent)
{
	setupUi(this);

	db = QSqlDatabase::addDatabase("QODBC","trackerlogin");
//	connect(connectButton,SIGNAL(clicked()),this,SLOT(reloadProjects()));
    dbClassComboBox->clear();
    dbClassComboBox->addItems(TQAbstractDB::registeredDbClasses());
}

TrkDb * TrkConnect::connectTracker()
{
	TrkConnect *d = new TrkConnect();
    QSettings *settings = ttglobal()->settings();
    QString server = settings->value("TrackerSQLServer","").toString(); // "SHMAKOVTHINK\\SQLEXPRESS"
    QString user = settings->value("TrackerUser","").toString();
	d->serverEdit->setText(server);
	d->userEdit->setText(user);
	if(!d->exec())
		return NULL;
	TrkDb *res;
	res = new TrkDb();
	//res->db = d->db;
    res->openProject("Driver={SQL Server};Server="+d->serverEdit->text()+";Trusted_Connection=yes",
                     d->projectEdit->text(),
                     d->userEdit->text());
	settings->setValue("TrackerSQLServer",d->serverEdit->text());
	settings->setValue("TrackerUser",d->userEdit->text());
    return res;
}

QString TrkConnect::dbClass()
{
    return dbClassComboBox->currentText().trimmed();
}

QString TrkConnect::dbType()
{
    return dbmsEdit->text().trimmed();
}

QString TrkConnect::dbServer()
{
    return serverEdit->text().trimmed();
}

QString TrkConnect::user()
{
    return userEdit->text().trimmed();
}

QString TrkConnect::project()
{
    return projectEdit->text().trimmed();
}

QString TrkConnect::password()
{
    return passwordEdit->text().trimmed();
}

bool TrkConnect::dbOsUser()
{
    return trustedUserBox->isChecked();
}

QString TrkConnect::dbmsUser()
{
    return sqlUserEdit->text().trimmed();
}

QString TrkConnect::dbmsPass()
{
    return sqlPassEdit->text();
}

bool TrkConnect::autoLogin()
{
    return autoLoginCheck->isChecked();
}

ConnectParams TrkConnect::params()
{
    ConnectParams p;
    p.dbClass   = dbClass();
    p.dbmsPass  = dbmsPass();
    p.dbmsUser  = dbmsUser();
    p.dbOsUser  = dbOsUser();
    p.dbServer  = dbServer();
    p.dbType    = dbType();
    p.password  = password();
    p.project   = project();
    p.user      = user();
    p.autoLogin = autoLogin();
    return p;
}

void TrkConnect::setParams(const ConnectParams &p)
{
    dbClassComboBox->setEditText(p.dbClass);
    dbmsEdit->setText(p.dbType);
    serverEdit->setText(p.dbServer);
    userEdit->setText(p.user);
    projectEdit->setText(p.project);
    passwordEdit->setText(p.password);
    trustedUserBox->setChecked(p.dbOsUser);
    sqlUserEdit->setText(p.dbmsUser);
    sqlPassEdit->setText(p.dbmsPass);
    sqlUserEdit->setEnabled(!p.dbOsUser);
    sqlPassEdit->setEnabled(!p.dbOsUser);
    autoLoginCheck->setChecked(p.autoLogin);
}

QDialogButtonBox::StandardButton TrkConnect::lastButton()
{
    return m_btn;
}

void TrkConnect::showDBTypesMenu()
{
    QScopedPointer<QSignalMapper> typesMapper(new QSignalMapper);
    connect(typesMapper.data(),SIGNAL(mapped(QString)),SLOT(setDBType(QString)));
    QString dbClass = dbClassComboBox->currentText();
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    QStringList types = db->dbmsTypes();
    QMenu *menu=new QMenu();
    for(int i = 0; i< types.count(); i++)
    {
        QAction *a = menu->addAction(types[i],typesMapper.data(), SLOT(map()));
        typesMapper->setMapping(a, types[i]);
    }
    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
    menu->exec(QCursor::pos());
}

void TrkConnect::showProjectsMenu()
{
    QScopedPointer<QSignalMapper> projectMapper(new QSignalMapper);
    connect(projectMapper.data(),SIGNAL(mapped(QString)),SLOT(setProject(QString)));
    QString dbClass = dbClassComboBox->currentText();
    QString dbType = dbmsEdit->text().trimmed();
    QString dbServer = serverEdit->text().trimmed();
    if(dbClass.isEmpty())
        return;
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    db->setDbmsServer(dbServer);
    db->setDbmsUser(dbmsUser(),dbmsPass());
    QStringList projects = db->projects(dbType);
    QMenu *menu=new QMenu();
    for(int i = 0; i< projects.count(); i++)
    {
        QAction *a = menu->addAction(projects[i],projectMapper.data(), SLOT(map()));
        projectMapper->setMapping(a, projects[i]);
    }
    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
    menu->exec(QCursor::pos());
    return;
           /* !!!!
    QScopedPointer<TQAbstractDB> trkdb(newDb(dbClass,dbType,dbServer));
//    QScopedPointer<TQAbstractDB> trkdb(newDb TrkToolDB(this));
    trkdb->setDbmsUser(sqlUserEdit->text(),
                         sqlPassEdit->text());
    QStringList projects = trkdb->projects(dbmsEdit->text());
    QMenu *menu=new QMenu();
    for(int i = 0; i< projects.count(); i++)
    {
        QAction *a = menu->addAction(projects[i],projectMapper, SLOT(map()));
        projectMapper->setMapping(a, projects[i]);
    }
    menu->popup(QCursor::pos());
    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
    */
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
    projectEdit->clear();
    /*
	while(query.next())
	{
		prjBox->addItem(query.value(0).toString());
    }
    */
}

void TrkConnect::setDBType(const QString &type)
{
    dbmsEdit->setText(type);
    QString dbClass = dbClassComboBox->currentText();
    if(dbClass.isEmpty())
        return;
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    db->setDbmsType(type);
    QString dsn = db->dbmsServer();
    serverEdit->setText(dsn);
}

void TrkConnect::setProject(const QString &project)
{
    projectEdit->setText(project);
}

void TrkConnect::on_trustedUserBox_clicked()
{
    bool editable = !trustedUserBox->isChecked();
    sqlUserEdit->setEnabled(editable);
    sqlPassEdit->setEnabled(editable);
}

void TrkConnect::on_btnProjects_clicked()
{
    showProjectsMenu();
}

void TrkConnect::on_dbClassComboBox_currentIndexChanged(const QString &arg1)
{
}

void TrkConnect::on_btnDBMS_clicked()
{
    showDBTypesMenu();
}

void TrkConnect::on_buttonBox_clicked(QAbstractButton *button)
{
    m_btn = buttonBox->standardButton(button);
}
