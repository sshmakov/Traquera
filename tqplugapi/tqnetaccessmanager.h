#ifndef TQNETACCESSMANAGER_H
#define TQNETACCESSMANAGER_H

#include <QtNetwork>

class TQNetAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit TQNetAccessManager(QObject *parent = 0);
    explicit TQNetAccessManager(QNetworkAccessManager *manager, QObject *parent);
    
protected:
    virtual QNetworkReply *	createRequest ( Operation op, const QNetworkRequest & req, QIODevice * outgoingData = 0 );

signals:
    
public slots:
    
};

class TQRecord;
class TQNetReplyPrivate;

class TQNetReply : public QNetworkReply
{
    Q_OBJECT
private:
    TQNetReplyPrivate *d;
protected:
    TQNetReply(QObject *parent = 0);
    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;

protected:
    qint64 readData(char *data, qint64 maxSize);

private slots:
    void processCommand(int command, bool error);
    void processListInfo(const QUrlInfo &urlInfo);
    void processData();

private:
    void setContent(TQRecord *record);
    void setListContent();

    friend class TQNetAccessManager;
};

#endif // TQNETACCESSMANAGER_H
