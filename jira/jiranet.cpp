#include "jiranet.h"

#include <QEventLoop>

class JiraNetWorkerPrivate {
public:
    QNetworkAccessManager *man;
};

JiraNetWorker::JiraNetWorker()
{
    d = new JiraNetWorkerPrivate();
    d->man = new QNetworkAccessManager(this);
    connect(d->man, SIGNAL(finished(QNetworkReply*)), SIGNAL(finished(QNetworkReply*)));
//    connect(d->man, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
//    connect(d->man,SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));
}

JiraNetWorker::~JiraNetWorker()
{
    delete d;
}

void JiraNetWorker::send(int id, const QString &method, QNetworkRequest request, const QByteArray &body)
{
    QNetworkReply *reply;
    if(method == "GET") // QNetworkAccessManager::GetOperation;
        reply = d->man->get(request);
    else if(method == "POST") // QNetworkAccessManager::PostOperation;
        reply = d->man->post(request, body);
    else if(method == "HEAD") //QNetworkAccessManager::HeadOperation;
        reply = d->man->head(request);
    else if(method == "DELETE") // QNetworkAccessManager::DeleteOperation;
        reply = d->man->deleteResource(request);
    else if(method == "PUT") // QNetworkAccessManager::PutOperation;
        reply = d->man->put(request, body);
    else
    {
        emit error(&request, 0, tr("Unknown method"));
        return;
    }
    emit requestSent(id, reply);
//    QEventLoop loop;
//    connect(d->man, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
//    loop.exec();
}


JiraNet::JiraNet(QObject *parent) : QObject(parent)
{
    nextId = 0;
    JiraNetWorker *worker = new JiraNetWorker();
    worker->moveToThread(&workerThread);
    connect(&workerThread, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(this, SIGNAL(send(int,QString,QNetworkRequest,QByteArray)), worker, SLOT(send(int,QString,QNetworkRequest,QByteArray)));
    connect(worker, SIGNAL(requestSent(int,QNetworkReply*)),SLOT(onSent(int,QNetworkReply*)));
    connect(worker, SIGNAL(finished(QNetworkReply*)), SLOT(onReceived(QNetworkReply*)));
    workerThread.start();
}

JiraNet::~JiraNet()
{
    workerThread.quit();
    workerThread.wait();
}

QNetworkReply *JiraNet::sendWait(const QString &method, QNetworkRequest &request, const QByteArray &body)
{
    mutex.lock();
    int id = ++nextId;
    mutex.unlock();

    QEventLoop loop;
    connect(this, SIGNAL(received(QNetworkReply*)), &loop, SLOT(quit()));
    emit send(id, method, request, body);
    QNetworkReply *reply;
    while(!replies.contains(id))
        loop.exec();
    reply = replies.take(id);
    return reply;
}

void JiraNet::onSent(int id, QNetworkReply *reply)
{
    replies.insert(id, reply);
}

void JiraNet::onReceived(QNetworkReply *reply)
{
    //replies.append(reply);
    emit received(reply);
}

