#include "tqconnectwidget.h"
#include "ui_tqconnectwidget.h"
#include "ttglobal.h"
#include "trklogin.h"
#include <QtSql>
#include <QMenu>

/*
 sqlite>
    CREATE TABLE "connections"(
        id integer primary key autoincrement,
        project varchar(255),
        dbClass varchar(255),
        dbType varchar(255),
        dbServer varchar(255),
        user varchar(255),
        password varchar(255),
        dbOsUser boolean,
        dbmsUser varchar(255),
        dbmsPass varchar (255),
        autoLogin boolean);
 */


TQConnectWidget::TQConnectWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TQConnectWidget)
{
    ui->setupUi(this);
    sqlModel = new QSqlTableModel(this, ttglobal()->userDatabase());
    sqlModel->setTable("Connections");
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();
    sqlModel->setHeaderData(FieldId,        Qt::Horizontal, tr("Id"));                             // id integer primary key autoincrement,
    sqlModel->setHeaderData(FieldProject,   Qt::Horizontal, tr("Проект"));                         // project varchar(255),
    sqlModel->setHeaderData(FieldDbClass,   Qt::Horizontal, tr("Система"));                        // dbClass varchar(255),
    sqlModel->setHeaderData(FieldDbType,    Qt::Horizontal, tr("Тип БД"));                         // dbType varchar(255),
    sqlModel->setHeaderData(FieldDbServer,  Qt::Horizontal, tr("Сервер"));                         // dbServer varchar(255),
    sqlModel->setHeaderData(FieldUser,      Qt::Horizontal, tr("Логин"));                          // user varchar(255),
    sqlModel->setHeaderData(FieldPassword,  Qt::Horizontal, tr("Пароль"));                         // password varchar(255),
    sqlModel->setHeaderData(FieldDbOsUser,  Qt::Horizontal, tr("Интегрированная авторизация"));    // dbOsUser boolean,
    sqlModel->setHeaderData(FieldDbmsUser,  Qt::Horizontal, tr("Логин к БД"));                     // dbmsUser varchar(255),
    sqlModel->setHeaderData(FieldDbmsPass,  Qt::Horizontal, tr("Пароль к БД"));                    // dbmsPass varchar (255),
    sqlModel->setHeaderData(FieldAutoLogin, Qt::Horizontal, tr("Автологин"));                     // autoLogin boolean);
    ui->tableView->setModel(sqlModel);
    ui->tableView->hideColumn(0);
    connect(ui->tableView->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(slotCurrentRowChanged(QModelIndex,QModelIndex)));
    ui->dbClassComboBox->clear();
    ui->dbClassComboBox->addItems(TQAbstractDB::registeredDbClasses());
    loadSettings();
}

TQConnectWidget::~TQConnectWidget()
{
    delete ui;
}

ConnectParams TQConnectWidget::currentParams() const
{
    ConnectParams params;
    params.id        = connectionId;
    params.dbClass   = ui->dbClassComboBox->currentText();
    params.dbType    = ui->dbmsEdit->text().trimmed();
    params.dbServer  = ui->serverEdit->text().trimmed();
    params.dbOsUser  = ui->trustedUserBox->isChecked();
    if(!params.dbOsUser)
    {
        params.dbmsUser  = ui->sqlUserEdit->text().trimmed();
        params.dbmsPass  = ui->sqlPassEdit->text().trimmed();
    }
    params.project   = ui->projectEdit->text().trimmed();
    params.user      = ui->userEdit->text().trimmed();
    params.password  = ui->passwordEdit->text().trimmed();
    params.autoLogin = ui->autoLoginCheck->isChecked();
    return params;
}

void TQConnectWidget::setParams(const ConnectParams &params)
{
    ui->dbClassComboBox->setEditText( params.dbClass   );
    ui->dbmsEdit->setText(            params.dbType    );
    ui->serverEdit->setText(          params.dbServer  );
    ui->trustedUserBox->setChecked(   params.dbOsUser  );
    ui->sqlUserEdit->setText(         params.dbmsUser  );
    ui->sqlPassEdit->setText(         params.dbmsPass  );
    ui->projectEdit->setText(         params.project   );
    ui->userEdit->setText(            params.user      );
    ui->passwordEdit->setText(        params.password  );
    ui->autoLoginCheck->setChecked(   params.autoLogin );
}

void TQConnectWidget::loadSettings()
{
    QSettings *settings = ttglobal()->settings();
    ui->dbClassComboBox->setEditText(settings->value("TrackerDBMSClass",ui->dbClassComboBox->currentText()).toString());
    ui->dbmsEdit->setText(settings->value("TrackerDBMSType",ui->dbmsEdit->text()).toString());
    ui->serverEdit->setText(settings->value("TrackerSQLServer",ui->serverEdit->text()).toString());
    ui->sqlUserEdit->setText(settings->value("TrackerSQLUser",ui->sqlUserEdit->text()).toString());
    ui->sqlPassEdit->setText(settings->value("TrackerSQLPass",ui->sqlPassEdit->text()).toString());
    ui->trustedUserBox->setChecked(settings->value("TrackerSQL",ui->trustedUserBox->isChecked()).toBool());
    //trustChanged(trustedUserBox->checkState());
    ui->projectEdit->setText(settings->value("TrackerProject",ui->projectEdit->text()).toString());
    ui->userEdit->setText(settings->value("TrackerUser",ui->userEdit->text()).toString());
}


void TQConnectWidget::slotCurrentRowChanged(QModelIndex current, QModelIndex previous)
{
    tableRow = current.row();
    QSqlRecord rec = sqlModel->record(tableRow);
    ConnectParams params;
    recordToParams(rec, params);
    setParams(params);
    connectionId = rec.value(FieldId).toInt();
}

void TQConnectWidget::on_btnAdd_clicked()
{
    QSqlRecord rec = sqlModel->record();
    paramsToRecord(currentParams(),rec);
    sqlModel->database().transaction();
    sqlModel->insertRecord(-1,rec);
    if(sqlModel->submitAll())
        sqlModel->database().commit();
    else
        sqlModel->database().rollback();
}

void TQConnectWidget::on_btnDel_clicked()
{
    QModelIndex index = ui->tableView->currentIndex();
    if(index.isValid())
    {
        sqlModel->database().transaction();
        sqlModel->removeRow(index.row());
        if(sqlModel->submitAll())
            sqlModel->database().commit();
        else
            sqlModel->database().rollback();
    }
}

void TQConnectWidget::on_btnOpen_clicked()
{
    ConnectParams params = currentParams();
    emit connectClicked(params);
}

void TQConnectWidget::on_btnClose_clicked()
{

}

void TQConnectWidget::recordToParams(const QSqlRecord &rec, ConnectParams &params)
{
    params.id        = rec.value(FieldId).toInt();
    params.dbClass   = rec.value(FieldDbClass).toString();
    params.dbType    = rec.value(FieldDbType).toString();
    params.dbServer  = rec.value(FieldDbServer).toString();
    params.dbOsUser  = rec.value(FieldDbOsUser).toBool();
    params.dbmsUser  = rec.value(FieldDbmsUser).toString();
    params.dbmsPass  = rec.value(FieldDbmsPass).toString();
    params.project   = rec.value(FieldProject).toString();
    params.user      = rec.value(FieldUser).toString();
    params.password  = rec.value(FieldPassword).toString();
    params.autoLogin = rec.value(FieldAutoLogin).toBool();
}

void TQConnectWidget::paramsToRecord(const ConnectParams &params, QSqlRecord &rec)
{
    rec.setValue(FieldDbClass,  params.dbClass   );
    rec.setValue(FieldDbType,   params.dbType    );
    rec.setValue(FieldDbServer, params.dbServer  );
    rec.setValue(FieldDbOsUser, params.dbOsUser  );
    rec.setValue(FieldDbmsUser, params.dbmsUser  );
    rec.setValue(FieldDbmsPass, params.dbmsPass  );
    rec.setValue(FieldProject,  params.project   );
    rec.setValue(FieldUser,     params.user      );
    rec.setValue(FieldPassword, params.password  );
    rec.setValue(FieldAutoLogin,params.autoLogin );
}

//// TQConnectModel
TQConnectModel::TQConnectModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    sqlModel = new QSqlTableModel(this, ttglobal()->userDatabase());
    sqlModel->setTable("Connections");
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->select();
    sqlModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    sqlModel->setHeaderData(1, Qt::Horizontal, tr("Salary"));

}

TQConnectModel::~TQConnectModel()
{
}

void TQConnectModel::open()
{
}

int TQConnectModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int TQConnectModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QModelIndex TQConnectModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex TQConnectModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

QVariant TQConnectModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant TQConnectModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

bool TQConnectModel::removeRows(int row, int count, const QModelIndex &parent)
{
    return false;
}

bool TQConnectModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags TQConnectModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f;
    return f;
}


void TQConnectWidget::on_btnDBMS_clicked()
{
    QScopedPointer<QSignalMapper> typesMapper(new QSignalMapper);
    connect(typesMapper.data(),SIGNAL(mapped(QString)),SLOT(setDBType(QString)));
    QString dbClass = ui->dbClassComboBox->currentText();
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    QStringList types = db->dbmsTypes();
    QScopedPointer<QMenu> menu(new QMenu());
    for(int i = 0; i< types.count(); i++)
    {
        QAction *a = menu->addAction(types[i],typesMapper.data(), SLOT(map()));
        typesMapper->setMapping(a, types[i]);
    }
//    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
    menu->exec(QCursor::pos());
}

void TQConnectWidget::setDBType(const QString &type)
{
    ui->dbmsEdit->setText(type);
    QString dbClass = ui->dbClassComboBox->currentText();
    if(dbClass.isEmpty())
        return;
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    db->setDbmsType(type);
    QString dsn = db->dbmsServer();
    ui->serverEdit->setText(dsn);
}

void TQConnectWidget::setProject(const QString &project)
{
    ui->projectEdit->setText(project);
}

void TQConnectWidget::on_btnProjects_clicked()
{
    QScopedPointer<QSignalMapper> projectMapper(new QSignalMapper);
    connect(projectMapper.data(),SIGNAL(mapped(QString)),SLOT(setProject(QString)));
    QString dbClass = ui->dbClassComboBox->currentText();
    QString dbType = ui->dbmsEdit->text().trimmed();
    QString dbServer = ui->serverEdit->text().trimmed();
    if(dbClass.isEmpty())
        return;
    QScopedPointer<TQAbstractDB> db(TQAbstractDB::createDbClass(dbClass));
    db->setDbmsServer(dbServer);
    db->setDbmsUser(ui->sqlUserEdit->text().trimmed(), ui->sqlPassEdit->text().trimmed());
    QStringList projects = db->projects(dbType, ui->userEdit->text(), ui->passwordEdit->text());
    QScopedPointer<QMenu> menu(new QMenu());
    for(int i = 0; i< projects.count(); i++)
    {
        QAction *a = menu->addAction(projects[i],projectMapper.data(), SLOT(map()));
        projectMapper->setMapping(a, projects[i]);
    }
//    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
    menu->exec(QCursor::pos());
}
