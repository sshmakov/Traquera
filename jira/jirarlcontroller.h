#ifndef JIRARLCONTROLLER_H
#define JIRARLCONTROLLER_H

#include <QObject>
#include <QSet>
#include <QStringList>
#include <QThread>
#include <QVariantMap>

class JiraRecModel;
class JiraProject;

class JiraRecLoader : public QObject
{
    Q_OBJECT
    JiraRecModel *model;
    JiraProject *project;
public:
    explicit JiraRecLoader(JiraRecModel *model);

public slots:
    void doWork(QStringList issues);

signals:
    void issueReaded(const QString &key, QVariantMap fields);
    void resultReady(const QString &result);
};

// one controller by jira project
class JiraRLController : public QObject
{
    Q_OBJECT
    QThread workerThread;
    QSet<QString> keysInProcess;
public:
    explicit JiraRLController(JiraRecModel *parent);
    ~JiraRLController();
    void start(const QStringList &keys);
public slots:
    void handleResults(const QString & result);
protected slots:
    void issueReaded(const QString &key);
signals:
    void operate(const QStringList &keys); // issue keys
};


#endif // JIRARLCONTROLLER_H
