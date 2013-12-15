#ifndef MAIN_H
#define MAIN_H

#include <QObject>
#include <QtNetwork>
#include <QDomDocument>

class TrkToolDB;
class TrkToolProject;
class TrkToolRecord;
class TrkToolModel;

class MainClass: public QObject
{
    Q_OBJECT
protected:
    TrkToolDB *db;
    TrkToolProject *prj;
    TrkToolModel *model;
    int row;
    QTextStream sout;
    QSettings sets;
    QString prjName;
    QHash<QString, QString> fieldMap;
public:
    QNetworkAccessManager man;
    QNetworkReply *reply;
    MainClass(QObject *parent, char *iniFile);
    void start();
    void send(const QDomDocument &dom);
    QDomDocument recFieldsXml(TrkToolRecord *rec);
    bool isOk() const;
    QString fieldNameTranslate(const QString &name, int ftype);
signals:
    void startProcess();
    void next();
public slots:
    void process();
    void sendNextRecord();
    void finished();
    void onError(QNetworkReply::NetworkError error);
    void	finished ( QNetworkReply * reply );
};

#endif // MAIN_H
