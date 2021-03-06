#include "jiralogin.h"
#include "ui_jiralogin.h"
#include <tqjson.h>
#include <jiradb.h>

class JiraLoginPrivate
{
public:
    JiraPlugin *plugin;
    QString fineUrl(const QString &url)
    {
        QUrl serverUrl = QUrl::fromUserInput(url);
        if(serverUrl.scheme().isEmpty())
            serverUrl.setScheme("http");
        QString server = serverUrl.toString();
        if(server.right(1) != "/")
            server += "/";
        return server;
    }
};

JiraLogin::JiraLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JiraLogin),
    d(new JiraLoginPrivate)
{
    ui->setupUi(this);
    d->plugin = JiraPlugin::plugin();
    ui->cbMethod->setCurrentIndex(1);
}

/*
    QWidget *w = db->createConnectWidget();
    if(!w)
    {
        w = new TQConnectWidget();
        w->setProperty("connectString", QString("{DBClass: \""+dbClass+"\"}"));
    }
    lay->addWidget(w);
    if(dlg.exec())
    {
        QString connString = w->property("connectString").toString();
        QString saveString = w->property("connectSaveString").toString();
        int row = addProjectItem(saveString);
        TQOneProjectTree *tree = qobject_cast<TQOneProjectTree *>(treeModel->sourceModel(row));
        treeView->setCurrentIndex(treeModel->index(row,0));
        readSelectedTreeItem();
        if(tree->open(connString))
        {
            treeView->expand(selectedTreeItem.curIndex);
            setCurrentProject(tree->project());
        }
//        actionOpen_Project->trigger();
        saveProjectTree();
//        TQAbstractProject *project = db->openConnection(connString);
    }
*/

JiraLogin::~JiraLogin()
{
    delete d;
    delete ui;
}

QString JiraLogin::connectString() const
{
    QVariantMap params;
    params.insert(DBPARAM_CLASS, "Jira");
//    params.insert(DBPARAM_TYPE, "");
    QString server = d->fineUrl(ui->cbServerLink->currentText());
    params.insert(DBPARAM_SERVER,  server);
    params.insert(JIRAPARAM_METHOD, ui->cbMethod->currentIndex());
    if(ui->cNoProxy->isChecked())
        params.insert(JIRAPARAM_PROXY, "NoProxy");
    if(!ui->chAnonimus->isChecked())
    {
        params.insert(DBPARAM_USER, ui->leUser->text());
        params.insert(DBPARAM_PASSWORD, ui->lePassword->text());
    }
    params.insert(PRJPARAM_NAME,ui->cbProject->currentText());
    params.insert(PRJPARAM_AUTOLOGIN, ui->cAutoLogin->isChecked());
    QString connectString = TQJson().toString(params);
    return connectString;
}

QString JiraLogin::connectSaveString() const
{
    QVariantMap params;
    params.insert(DBPARAM_CLASS, "Jira");
//    params.insert("DBType", "");
    QString server = d->fineUrl(ui->cbServerLink->currentText());
    params.insert(DBPARAM_SERVER,  server);
    params.insert(JIRAPARAM_METHOD, ui->cbMethod->currentIndex());
    if(ui->cNoProxy->isChecked())
        params.insert(JIRAPARAM_PROXY, "NoProxy");
    if(!ui->chAnonimus->isChecked())
    {
        params.insert(DBPARAM_USER, ui->leUser->text());
        params.insert(DBPARAM_PASSWORD, ui->lePassword->text());
        params.insert(DBPARAM_USER, ui->leUser->text());
        if(ui->cSavePass->isChecked())
        {
            params.insert(DBPARAM_PASSWORD, ui->lePassword->text());
        }
    }
    params.insert(PRJPARAM_NAME,ui->cbProject->currentText());
    params.insert(PRJPARAM_AUTOLOGIN, ui->cAutoLogin->isChecked());
    QString connectString = TQJson().toString(params);
    return connectString;
}

void JiraLogin::setConnectString(const QString &string)
{
    TQJson parser;
    QVariantMap params = parser.toVariant(string).toMap();
    ui->cbServerLink->setEditText(params.value(DBPARAM_SERVER).toString());
    ui->cbMethod->setCurrentIndex(params.value(JIRAPARAM_METHOD,(int)JiraDB::CookieAuth).toInt());
    ui->cNoProxy->setChecked(params.value(JIRAPARAM_PROXY) == "NoProxy");
    QString user = params.value(DBPARAM_USER).toString().trimmed();
    ui->leUser->setText(user);
    ui->lePassword->setText(params.value(DBPARAM_PASSWORD).toString());
    ui->chAnonimus->setChecked(user.isEmpty());
    ui->leUser->setEnabled(!user.isEmpty());
    ui->lePassword->setEnabled(!user.isEmpty());
    ui->cSavePass->setChecked(params.contains(DBPARAM_PASSWORD));
    ui->cbProject->setEditText(params.value(PRJPARAM_NAME).toString());
    ui->cAutoLogin->setChecked(params.contains(PRJPARAM_AUTOLOGIN) && params.value(PRJPARAM_AUTOLOGIN).toBool());
}

QStringList JiraLogin::readProjects()
{
    QScopedPointer<JiraDB> db(new JiraDB(this));
    db->setConnectString(connectString());
    return db->projects(db->dbmsType(), ui->leUser->text(), ui->lePassword->text());
}

void JiraLogin::on_cbMethod_currentIndexChanged(int index)
{
    bool isBase = index == 0;
    bool isCookie = index == 1;
    ui->leUser->setEnabled(isBase || isCookie);
    ui->lePassword->setEnabled(isBase || isCookie);
}

void JiraLogin::on_tbRefreshProjects_clicked()
{
    QString text = ui->cbProject->currentText();
    QStringList projects = readProjects();
    ui->cbProject->clear();
    ui->cbProject->addItems(projects);
    int index = projects.indexOf(text);
    ui->cbProject->setCurrentIndex(index);
}

void JiraLogin::on_chAnonimus_clicked()
{
    ui->leUser->setEnabled(!ui->chAnonimus->isChecked());
    ui->lePassword->setEnabled(!ui->chAnonimus->isChecked());
}
