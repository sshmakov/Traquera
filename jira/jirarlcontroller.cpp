#include "jiradb.h"
#include "jirarecmodel.h"
#include "jirarlcontroller.h"

JiraRecLoader::JiraRecLoader(JiraRecModel *model): QObject()
{
    this->model = model;
    this->project = model->jiraProject();
}

void JiraRecLoader::doWork(QStringList issues) {
    foreach(QString key, issues)
    {
        QVariantMap issue = project->serverGet("rest/api/2/issue/" + key + "?fields=-comment").toMap();
        if(!issue.isEmpty())
            emit issueReaded(key, issue.value("fields").toMap());
    }
    emit resultReady(QString());
}

JiraRLController::JiraRLController(JiraRecModel *parent) : QObject(parent)
{
    JiraRecLoader *worker = new JiraRecLoader(parent);
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(operate(QStringList)), worker, SLOT(doWork(QStringList)));
    connect(worker, SIGNAL(issueReaded(QString,QVariantMap)), parent, SLOT(updateIssue(QString,QVariantMap)));
    connect(worker, SIGNAL(issueReaded(QString,QVariantMap)), SLOT(issueReaded(QString)));
    connect(worker, SIGNAL(resultReady(QString)), this, SLOT(handleResults(QString)));
    workerThread.start();
}


JiraRLController::~JiraRLController() {
    workerThread.quit();
    workerThread.wait();
}

void JiraRLController::start(const QStringList &keys)
{
    QSet<QString> skeys = keys.toSet().subtract(keysInProcess);
    keysInProcess.unite(skeys);
    emit operate(skeys.toList());
}

void JiraRLController::handleResults(const QString &result)
{

}

void JiraRLController::issueReaded(const QString &key)
{
    keysInProcess.remove(key);
}


