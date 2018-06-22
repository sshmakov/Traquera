#ifndef JIRANET_H
#define JIRANET_H

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QThread>
#include <QMutex>

class JiraNetWorkerPrivate;

class JiraNetWorker : public QObject
{
    Q_OBJECT
    JiraNetWorkerPrivate *d;
    friend class JiraNet;
protected:
    JiraNetWorker();
    ~JiraNetWorker();
public slots:
protected slots:
    void send(int id, const QString &method, QNetworkRequest request, const QByteArray &body);
signals:
    void requestSent(int id, QNetworkReply *reply);
    void finished(QNetworkReply *reply);
    void error(QNetworkRequest *req, int errorCode, QString errorString);
};

class JiraNet : public QObject
{
    Q_OBJECT
    QThread workerThread;
    QHash<int, QNetworkReply *> replies;
    int nextId;
    QMutex mutex;
public:
    explicit JiraNet(QObject *parent = 0);
    ~JiraNet();
signals:
    void send(int id, const QString &method, QNetworkRequest request, const QByteArray &body);
    void received(QNetworkReply *reply);
public:
    QNetworkReply *sendWait(const QString &method, QNetworkRequest &request, const QByteArray &body);
protected slots:
    void onSent(int id, QNetworkReply *reply);
    void onReceived(QNetworkReply *reply);
};

#endif // JIRANET_H
