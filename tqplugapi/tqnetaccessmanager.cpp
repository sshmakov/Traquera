#include "tqnetaccessmanager.h"
#include <ttglobal.h>
#include <QNetworkRequest>
#include <tqbase.h>

TQNetAccessManager::TQNetAccessManager(QObject *parent) :
    QNetworkAccessManager(parent)
{
}

TQNetAccessManager::TQNetAccessManager(QNetworkAccessManager *manager, QObject *parent)
    : QNetworkAccessManager(parent)
{
    setCache(manager->cache());
    setCookieJar(manager->cookieJar());
    setProxy(manager->proxy());
    setProxyFactory(manager->proxyFactory());
}

QNetworkReply *TQNetAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData)
{
    if(req.url().scheme() != "trq")
        return QNetworkAccessManager::createRequest(op, req, outgoingData);
    TQNetReply *reply = new TQNetReply();
    QString host = req.url().host();
    QString path = req.url().path();

    return reply;
}

//==================== TQNetReply ========================
class TQNetReplyPrivate
{
public:
    QByteArray content;
    QBuffer *buf;
    qint64 offset;
    QStringList units;
};

TQNetReply::TQNetReply(QObject *parent)
    : QNetworkReply(parent), d(new TQNetReplyPrivate)
{
    d->buf = 0;
//    ttglobal()->getRecord()
}

void TQNetReply::abort()
{
}

qint64 TQNetReply::bytesAvailable() const
{
    if(!d->buf)
        return 0;
    return d->buf->bytesAvailable();
}

bool TQNetReply::isSequential() const
{
    return true;
}

qint64 TQNetReply::readData(char *data, qint64 maxSize)
{
    if(!d->buf)
        return 0;
    return d->buf->read(data, maxSize);
}

void TQNetReply::processCommand(int command, bool error)
{

}

void TQNetReply::processListInfo(const QUrlInfo &urlInfo)
{

}

void TQNetReply::processData()
{

}

void TQNetReply::setContent(TQRecord *record)
{
    QDomDocument xml = record->toXML();
    d->content = xml.toByteArray(0);
    if(d->buf)
        delete d->buf;
    d->buf = new QBuffer(&d->content, this);
}

void TQNetReply::setListContent()
{
}
