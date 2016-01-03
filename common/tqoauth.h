#ifndef TQOAUTH_H
#define TQOAUTH_H

#include <tqplugin_global.h>
#include <QObject>
#include <QtNetwork>

class TQOAuthPrivate;
class QNetworkReply;
class QNetworkAccessManager;

class TQPLUGIN_SHARED TQOAuth : public QObject
{
    Q_OBJECT
private:
    TQOAuthPrivate *d;
    //network
    QNetworkAccessManager *man;
    QList<QObject *> readyReplies;
    int timeOutSecs;
public:
    explicit TQOAuth(QObject *parent = 0);
    ~TQOAuth();
    QMap<QString, QString> getRequestToken(const QString &method, const QString &link, const QString &callback);
    QString getAccessToken(const QString &method, const QString &link, const QString &requestToken);
    QNetworkReply *signedGet(QNetworkRequest *request);
    bool signRequest(const QString &method, QNetworkRequest *request, const QString &token, const QMap<QString, QString> &pars = QMap<QString, QString>());


    void setConsumerKey(const QString &key);
    void setConsumerSecret(const QString &secret);
    void setSignatureMethod(const QString &method);
//    void setBaseUrl(const QString &url);
    void setRealm(const QString &realm);
    void setFoundUrl(const QUrl& url);

    QNetworkReply *sendWait(const QString& method, QNetworkRequest &request, const QByteArray &body = QByteArray());
    QNetworkReply *sendRequest(int operation, const QString &link, const QString &headers, const QByteArray &body = QByteArray());
    static QMap<QString, QString> parseBodyReply(QNetworkReply *reply);
    bool waitReply(QNetworkReply *reply);
    bool loadPrivateKey(const QString &fileName, const QString &filePassword);
//    QString parJoin(const QMap<QString, QString> &pars);
signals:
private slots:
    void replyFinished(QNetworkReply* reply);
    void queryAuthentication(QNetworkReply *reply, QAuthenticator*authenticator);
    void proxyAuthentication(const QNetworkProxy &proxy, QAuthenticator*authenticator);
public slots:
    
};

#endif // TQOAUTH_H
