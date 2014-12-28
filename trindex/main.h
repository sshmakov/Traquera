#ifndef MAIN_H
#define MAIN_H

#include <QObject>
#include <QList>
#include <QtNetwork>
#include <QDomDocument>

class TQAbstractDB;
class TQAbstractProject;
class TrkToolDB;
class TrkToolProject;
class TrkToolRecord;
class TrkToolModel;

class MainClass: public QObject
{
    Q_OBJECT
protected:
    TQAbstractDB *db;
    TQAbstractProject *prj;
    TrkToolModel *model;
    QList<int> list;
    int rowCount;
    int row;
    QTextStream sout;
    QSettings sets;
    QString prjName;
    QString uniqueField;
    QHash<QString, QString> fieldMap;
    QUrl url;
    QFile localFile;
    bool inFile;
public:
    QNetworkAccessManager man;
    QNetworkReply *reply;
    MainClass(QObject *parent, char *iniFile);
    void start();
    void send(const QDomDocument &dom);
    void write(const QDomDocument &dom);
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
