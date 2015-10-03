#include "tqoauth.h"
#include <QtNetwork>
#include <QCoreApplication>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <QFile>
#include <io.h>
//#include "ttglobal.h"
//#include <openssl/applink.c>

//class Signer
//{
class TQOAuthPrivate
{
public:
    QString realm;
    QString consumer_key;
    QString consumer_secret;
    QString signature_method;
    QString timestamp;
    QString nonce;
    QString version;
    QString signature;
    QString baseUrl;
    QString callbackUrl;
    QString verifier;

    QString request_token;
    QString access_token;
    QString token_secret;
    QString callback_confirm;

    bool isPrivateKeyLoaded;

    TQOAuthPrivate()
        : isPrivateKeyLoaded(false)
    {
        SSL_load_error_strings();
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
    }


    int sn2nid(const QString& sn)
    {
        return OBJ_sn2nid(sn.toLocal8Bit().constData());
    }

    int method2op(const QString &method)
    {
        if(method == "GET")
            return QNetworkAccessManager::GetOperation;
        if(method == "POST")
            return QNetworkAccessManager::PostOperation;
        if(method == "HEAD")
            return QNetworkAccessManager::HeadOperation;
        else if(method == "DELETE")
            return QNetworkAccessManager::DeleteOperation;
        else if(method == "PUT")
            return QNetworkAccessManager::PutOperation;
        return -1;
    }


public:
    RSA *rsa_public, *rsa_private;
//    int nid;

// Generate private key:
// ...without passphrase:
//    openssl genrsa -out mykey.pem 1024
// ... with passphrase:
//    openssl genrsa -des3 -out mykey.pem 1024


    bool setPrivateKey(const QString &key, const QString &pempass = QString())
    {
        isPrivateKeyLoaded = false;
        BIO *mem = BIO_new(BIO_s_mem());
        BIO_puts(mem, key.toAscii().constData());
        rsa_private = PEM_read_bio_RSAPrivateKey(mem, NULL, 0, pempass.toLocal8Bit().data());
        BIO_free(mem);
        if (rsa_private == NULL)
            return false;
        isPrivateKeyLoaded = true;
        return true;
    }


    bool loadPrivateKey(const QString &keyFile, const QString &pempass = QString())
    {
        isPrivateKeyLoaded = false;
        FILE *priv_key_file=NULL;
        priv_key_file=fopen(keyFile.toAscii(), "rb");
        if(priv_key_file)
        {
            rsa_private=PEM_read_RSAPrivateKey(priv_key_file, NULL, 0, pempass.toLocal8Bit().data());
            if (rsa_private == NULL)
                return false;
            fclose(priv_key_file);
            isPrivateKeyLoaded = true;
            return true;
        }
        else
            return false;
    }

    QByteArray signRSA(const QByteArray &buf)
    {
        int nid = sn2nid(signature_method);

        unsigned char sigret[2048];
        unsigned int siglen=0;

        unsigned char digest[2048];
        unsigned int dlen;
        const EVP_MD *evp_md;
        EVP_MD_CTX md;
        qDebug() << "NID for" << signature_method << "is" << nid;
        nid = NID_sha1;
        qDebug() << "reset to" << nid;
        if ((evp_md = EVP_get_digestbynid(nid)) == NULL)
            return false;
        EVP_DigestInit(&md, evp_md);
        EVP_DigestUpdate(&md, buf.constData(), buf.size());
        EVP_DigestFinal(&md, digest, &dlen);

        if(RSA_sign(nid, digest, dlen, sigret, &siglen, rsa_private))
            return QByteArray((char*)sigret, siglen);
        else
            return QByteArray();
    }


    // Extract public key:
    //  openssl rsa -in mykey.pem -pubout -out mykey.pub

    bool loadPublicKey(const QString &keyFile)
    {
        FILE *pub_key_file = fopen(keyFile.toLocal8Bit(),"rb");
        if(!pub_key_file )
            return false;

        //    BIO *mem = BIO_new(BIO_s_mem());
        //    BIO_puts (mem, key.toAscii().data());
        rsa_public=PEM_read_RSA_PUBKEY(pub_key_file, NULL, NULL, NULL);
        fclose(pub_key_file);
        if (rsa_public == NULL)
            return false;
        return true;
    }

    bool verifyRSA(const QByteArray &text, const QByteArray &sign)
    {
        int nid = sn2nid(signature_method);
        //    QByteArray a=QByteArray::fromBase64(sign.toAscii());

        unsigned char digest[2048];
        unsigned int dlen;
        const EVP_MD *evp_md;
        EVP_MD_CTX md;
        if ((evp_md = EVP_get_digestbynid(nid)) == NULL) return false;
        EVP_DigestInit(&md, evp_md);
        EVP_DigestUpdate(&md, (unsigned char*)text.data(), text.size());
        EVP_DigestFinal(&md, digest, &dlen);

        return RSA_verify(nid, digest, dlen, (unsigned char*)sign.data(), sign.length(), rsa_public);
    }

    void dumpRequest(QNetworkRequest *request)
    {
        if(!request)
            return;
        qDebug() << "-- Request --------";
        qDebug() << "URL:" << request->url();
        foreach(const QByteArray &h, request->rawHeaderList())
            qDebug() << h << ": " << request->rawHeader(h);
    }

    void dumpReply(QNetworkReply *reply)
    {
        if(!reply)
            return;
        qDebug() << "-- Reply ----------" << reply->error();
        qDebug() << "Error: " << reply->error();
        qDebug() << "Headers";
        foreach(QNetworkReply::RawHeaderPair pair, reply->rawHeaderPairs())
            qDebug() << QUrl::fromPercentEncoding(pair.first) << ":" << QUrl::fromPercentEncoding(pair.second);
//        qDebug() << "--";
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug() << "";
            qDebug() << reply->readAll();
        }
        qDebug() << "===";
    }

/*
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QString privKey, pubKey, pass;
    if(argc<6)
    {
        qWarning("Too fee parameters");
        return -1;
    }
    CRYPTO_malloc_init();
    privKey = argv[1];
    pass = argv[2];
    if(!setPrivateKey(privKey, pass))
    {
        qWarning("Can't set private.key");
        return -1;
    }
    QFile fin(argv[3]);
    fin.open(QFile::ReadOnly);
    QByteArray buf = fin.readAll();
    QByteArray res = signRSA(buf);
    if(res.isEmpty())
    {
        qWarning("Can't set sign");
        return -1;
    }
    QFile fout(argv[4]);
    fout.open(QFile::WriteOnly);
    fout.write(res.toBase64());
    pubKey = argv[5];
//    QFile fpub(pubKey);
//    fpub.open(QFile::ReadOnly);
//    QByteArray pub = fpub.readAll();
    if(!setPublicKey(pubKey))
    {
        qWarning("Can't set public.key");
        return -1;
    }
    if(!verifyRSA(buf, res))
    {
        qWarning("Can't set verify");
        return -1;
    }
    qWarning("Ok!");
    //    return a.exec();
    qWarning("End!");
    return 0;
}
*/

};

//============== TQOAuth =========================
TQOAuth::TQOAuth(QObject *parent) :
    QObject(parent),
    d(new TQOAuthPrivate())
{
//    man =  ttglobal()->networkManager();// new QNetworkAccessManager(this);
    man =  new QNetworkAccessManager(this);
    connect(man, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
    timeOutSecs = 10;
//    d->realm="http%3A%2F%2Fapi.twitter.com%2F";
    d->consumer_key="1q2w3e4r";
    d->signature_method="RSA-SHA1";
//    d->timestamp="137131200";
    d->nonce="4572616e48616d6d65724c61686176";
    d->version="1.0";
    d->signature="wOJIO9A2W5mFwDgiDvZbTSMK%2FPY%3D";
    d->baseUrl = "http://rt.allrecall.com:8081/";
    d->callbackUrl = "oob";
}

TQOAuth::~TQOAuth()
{
    delete d;
}

QNetworkReply *TQOAuth::sendRequest(int operation, const QString &link, const QString &headers, const QByteArray &body)
{
    QUrl url(link);

    QNetworkRequest req;
    req.setUrl(url);
    foreach(QString line, headers.split("\n"))
    {
        int pos = line.indexOf(": ");
        req.setRawHeader(line.left(pos).toUtf8(),line.mid(pos+2).toUtf8());
    }
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    d->dumpRequest(&req);
    QNetworkReply *reply;
    switch(operation)
    {
    case QNetworkAccessManager::HeadOperation:
        reply = man->head(req);
        break;
    case QNetworkAccessManager::GetOperation:
        reply = man->get(req);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = man->put(req, body);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = man->post(req, body);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = man->deleteResource(req);
        break;
    case QNetworkAccessManager::CustomOperation:
        return 0;
    }
//    QScopedPointer<QNetworkReply> reply(man->get(req));
    QDateTime endTime = QDateTime::currentDateTime().addSecs(timeOutSecs);
    while(!readyReplies.contains(reply))
    {        
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
        if(QDateTime::currentDateTime() > endTime)
            break;
//        {
//            reply->abort();
//            readyReplies.removeAll(reply.data());
//            return reply.take();
//        }
    }
    readyReplies.removeAll(reply);

    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug(reply->errorString().toLocal8Bit().constData());
        return reply;
    }

//    QByteArray text = reply->readAll();
//    QString s(text.constData());
//    qDebug(text.constData());

    return reply;
}

QMap<QString, QString> TQOAuth::parseBodyReply(QNetworkReply *reply)
{
    QMap< QString, QString> result;
    QByteArray arr = reply->readAll();
    QList<QByteArray> items = arr.split('&');
    foreach(const QByteArray &item, items)
    {
        int pos = item.indexOf("=");
        if(pos<0)
            result.insert(QUrl::fromPercentEncoding(item),"");
        else
        {
            QString param = QUrl::fromPercentEncoding(item.left(pos));
            QString value = QUrl::fromPercentEncoding(item.mid(pos+1));
            result.insert(param, value);
        }
    }
    return result;
}

bool TQOAuth::waitReply(QNetworkReply *reply)
{
    QDateTime endTime = QDateTime::currentDateTime().addSecs(timeOutSecs);
    while(!readyReplies.contains(reply))
    {
        if(QDateTime::currentDateTime() > endTime)
            return false;
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
    }
    readyReplies.removeAll(reply);
    return true;
}


class ParItem: public QPair<QString, QString>
{
public:
    ParItem(QString first, QString second)
        : QPair(first, second)
    {

    }
    ParItem &operator = (const ParItem &src)
    {
        first = src.first;
        second = src.second;
        return *this;
    }

};


//typedef QPair<QString, QString> ParItem;
//typedef QList< QPair<QString, QString> > ParList;
typedef QList< ParItem > ParList;
/*
class ParList : public QList< QPair<QString, QString> >
{

};
*/

static QString parJoin(const ParList &parList)
{
    QByteArray res;
    foreach(const ParItem &item, parList)
    {
        if(!res.isEmpty())
            res += "&";
        res += QUrl::toPercentEncoding(item.first) + "=" + QUrl::toPercentEncoding(item.second);
    }
    return res.constData();
}

static bool parLessThan(const ParItem &i1, const ParItem &i2)
{
    return i1.first < i2.first
            || (i1.first == i2.first && i1.second < i2.second);
}

QMap< QString, QString> TQOAuth::getRequestToken(const QString &method, const QString &link, const QString &callback)
{
    QMap< QString, QString> result;
    uint timestamp = QDateTime::currentDateTime().toTime_t();
    QString nonce = QUuid::createUuid().toString();

    ParList oauthPars, allPars;
    oauthPars
            << ParItem("oauth_consumer_key", d->consumer_key)
            << ParItem("oauth_signature_method", d->signature_method)
            << ParItem("oauth_timestamp",QString::number(timestamp))
            << ParItem("oauth_nonce",nonce)
            << ParItem("oauth_version","1.0")
            << ParItem("oauth_callback", callback);
/*
    oauthPars.clear();
    oauthPars
            << ParItem("oauth_consumer_key", "9djdj82h48djs9d2")
            << ParItem("oauth_token", "kkk9d7dh3k39sjv7")
            << ParItem("oauth_signature_method", "HMAC-SHA1")
            << ParItem("oauth_timestamp", "137131201")
            << ParItem("oauth_nonce", "7d8f3e4a")
//            << ParItem("oauth_signature", "djosJKDKJSD8743243%2Fjdk33klY%3D")
               ;
*/
    QUrl url(link);

    QString scheme = url.scheme().toLower();
    QString portS;
    int port = url.port();
    if(scheme == "http" && port == 80)
        port = -1;
    else if(scheme == "https" && port == 443)
        port = -1;
    portS = port == -1 ? "" : QString(":%1").arg(port);
    QString path = url.path().toLower();
    QString baseUrl = QString("%1://%2%3%4")
            .arg(scheme,url.host(),portS,path);

    allPars = oauthPars;

    QList<QPair<QByteArray, QByteArray>> urlPars = url.encodedQueryItems();
    QPair<QByteArray, QByteArray>  pair;
    foreach(pair, urlPars)
        allPars.append(ParItem(QString::fromAscii(pair.first),QString::fromAscii(pair.second)));

    qSort(allPars.begin(), allPars.end(), parLessThan);

    QString join  = parJoin(allPars);
    qDebug() << "Join:" << join;
    QString encMethod = QUrl::toPercentEncoding(method).constData();
    QString encBaseUrl = QUrl::toPercentEncoding(baseUrl).constData();
    QString baseString = QString("%1&%2&%3")
            .arg(encMethod, encBaseUrl, QUrl::toPercentEncoding(join).constData());

//    "Authorization: OAuth realm=%1,"
//    " oauth_consumer_key=%2,"
//    " oauth_signature_method=%3,"
//    " oauth_timestamp=%4,"
//    " oauth_nonce=%5,"
//    " oauth_version=%6,"
//    " oauth_signature=%7";

/*
Ќам необходимо выполн€ть HMAC-SHA1 (или RSA-SHA1) с ключем, который формируетс€ так:
urlencode("<oauth_consumer_secret>&<oauth_token_secret>") Ч (на данном этапе oauth_token_secret еще не получен, поэтому он будет пустым)
и базовой строкой, котора€ составл€етс€ следующим образом:
"<метод запроса>&<urlencode(адрес запроса)>&<urlencode(key_sort(параметры запроса))>",
чтобы было пон€тнее приведу пример:
GET&http3A%2F2Fapi.twitter.com%2Frequest_token&oauth_consumer_key%3Ddpf43f3p2l4k3l03%26oauth_nonce%3Dkllo9940pd9333jh%26oauth_signature_method%3DHMAC-SHA1%26oauth_timestamp%3D1191242096%26oauth_version%3D1.0
*/

//    QString rsaKey = QUrl::toPercentEncoding(d->consumer_secret);

//    baseString = "aaaa";
    if(!d->loadPrivateKey("../test/mykey.pem", QString()))
        qDebug("Error load privateKey");
//    d->setPrivateKey(rsaKey);
    QByteArray baseBuf = baseString.toAscii();
    QByteArray sign = d->signRSA(baseBuf);
    QString sign64 = sign.toBase64();
    qDebug() << "Base:" << baseString;
    qDebug() << "Sign:" << sign64;
    oauthPars.append(ParItem("oauth_signature",
                             QUrl::toPercentEncoding(sign64)));

    QString headers = QString("Authorization: OAuth realm=\"%1\"").arg(d->realm);
    foreach(const ParItem &par, oauthPars)
    {
        headers += ", " + par.first + "=\"" + par.second + "\"";
    }
    int op = d->method2op(method);
    if(op == -1)
        return result;
    QNetworkReply *reply = sendRequest(op, link, headers);
    if(!reply)
        return result;
    d->dumpReply(reply);
    result = parseBodyReply(reply);
//    qDebug() << "Body: " << QUrl::fromPercentEncoding(arr);
    d->request_token = result.value("oauth_token");
    d->token_secret = result.value("oauth_token_secret");
    d->callback_confirm = result.value("oauth_callback_confirmed");
    return result;
}

QString TQOAuth::getAccessToken(const QString &method, const QString &link, const QString &requestToken)
{
    QNetworkRequest req;
    req.setUrl(QUrl(link));
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QMap<QString, QString> pars;
    if(!d->verifier.isEmpty())
        pars["oauth_verifier"] = d->verifier;
    if(!signRequest(method, &req, requestToken, pars))
        return 0;

    d->dumpRequest(&req);
    QScopedPointer<QNetworkReply> reply(sendWait(method, req));
    d->dumpReply(reply.data());
    QMap<QString, QString> res = parseBodyReply(reply.data());
    d->access_token = res.value("oauth_token");
    return d->access_token;
}

/*
oauth_consumer_key: The Consumer Key.
oauth_token: The Access Token.
oauth_signature_method: The signature method the Consumer used to sign the request.
oauth_signature: The signature as defined in Signing Requests.
oauth_timestamp: As defined in Nonce and Timestamp.
oauth_nonce: As defined in Nonce and Timestamp.
oauth_version: OPTIONAL. If present, value MUST be 1.0. Service Providers MUST assume the protocol version to be 1.0 if this parameter is not present. Service ProvidersТ response to non-1.0 value is left undefined.
Additional parameters: Any additional parameters, as defined by the Service Provider.
*/

QNetworkReply *TQOAuth::signedGet(QNetworkRequest *request)
{
    QMap<QString, QString> pars;
//    if(!d->verifier.isEmpty())
//        pars["oauth_verifier"] = d->verifier;
    if(!signRequest("GET", request, d->access_token, pars))
        return 0;

    d->dumpRequest(request);
    QNetworkReply *reply;
    reply = sendWait("GET", *request);
    d->dumpReply(reply);
//    if(reply->error() != QNetworkReply::NoError)
//    {
//        qDebug(reply->errorString().toLocal8Bit().constData());
//        return reply;
//    }
    return reply;
}

bool TQOAuth::signRequest(const QString &method, QNetworkRequest *request, const QString &token, const QMap<QString, QString> &pars)
{
    uint timestamp = QDateTime::currentDateTime().toTime_t();
    QString nonce = QUuid::createUuid().toString().replace(QChar('{'),"").replace(QChar('}'),"");

    QUrl url = request->url();
//    ParList reqPars;
//    QList<QByteArray> rawHeaders = request->rawHeaderList();

    ParList oauthPars, allPars;
    oauthPars
            << ParItem("oauth_consumer_key", d->consumer_key)
            << ParItem("oauth_signature_method", d->signature_method)
            << ParItem("oauth_timestamp",QString::number(timestamp))
            << ParItem("oauth_nonce",nonce)
            << ParItem("oauth_version","1.0")
            << ParItem("oauth_token", token);

    foreach(QString key, pars.keys())
        oauthPars  << ParItem(key, pars.value(key));

    QString scheme = url.scheme().toLower();
    QString portS;
    int port = url.port();
    if(scheme == "http" && port == 80)
        port = -1;
    else if(scheme == "https" && port == 443)
        port = -1;
    portS = port == -1 ? "" : QString(":%1").arg(port);
    QString path = url.path().toLower();
    QString baseUrl = QString("%1://%2%3%4")
            .arg(scheme,url.host(),portS,path);

    allPars = oauthPars;

    QList<QPair<QByteArray, QByteArray>> urlPars = url.encodedQueryItems();
    QPair<QByteArray, QByteArray>  pair;
    foreach(pair, urlPars)
        allPars.append(ParItem(QString::fromAscii(pair.first),QString::fromAscii(pair.second)));

    qSort(allPars.begin(), allPars.end(), parLessThan);

    QString join  = parJoin(allPars);
    qDebug() << "Join:" << join;
    QString encMethod = QUrl::toPercentEncoding(method).constData();
    QString encBaseUrl = QUrl::toPercentEncoding(baseUrl).constData();
    QString baseString = QString("%1&%2&%3")
            .arg(encMethod, encBaseUrl, QUrl::toPercentEncoding(join).constData());

    if(!d->loadPrivateKey("../test/mykey.pem", QString()))
    {
        qDebug("Error load privateKey");
        return false;
    }
    QByteArray baseBuf = baseString.toAscii();
    QByteArray sign = d->signRSA(baseBuf);
    QString sign64 = sign.toBase64();
    qDebug() << "Base:" << baseString;
    qDebug() << "Sign:" << sign64;
    if(sign.isEmpty())
        return false;
    oauthPars.append(ParItem("oauth_signature",
                             QUrl::toPercentEncoding(sign64)));


    QString aHeader = QString("OAuth realm=\"%1\"").arg(d->realm);
    foreach(const ParItem &par, oauthPars)
    {
        aHeader += ", " + par.first + "=\"" + par.second + "\"";
    }
    request->setRawHeader("Authorization",aHeader.toLatin1());
    return true;
}

void TQOAuth::setConsumerKey(const QString &key)
{
    d->consumer_key = key;
}

void TQOAuth::setConsumerSecret(const QString &secret)
{
    d->consumer_secret = secret;
}

void TQOAuth::setSignatureMethod(const QString &method)
{
    d->signature_method = method;
}

void TQOAuth::setBaseUrl(const QString &url)
{
    d->baseUrl = url;
}

void TQOAuth::setRealm(const QString &realm)
{
    d->realm = realm;
}

void TQOAuth::setFoundUrl(const QUrl &url)
{
    qDebug() << "Fount URL:" << url.toString();
    d->request_token = url.queryItemValue("oauth_token");
    d->verifier = url.queryItemValue("oauth_verifier");
}

QNetworkReply *TQOAuth::sendWait(const QString &method, QNetworkRequest &request, const QByteArray &body)
{
    QNetworkReply *reply;
    int op = d->method2op(method);
    switch(op)
    {
    case -1:
        return 0;
    case QNetworkAccessManager::HeadOperation:
        reply = man->head(request);
        break;
    case QNetworkAccessManager::GetOperation:
        reply = man->get(request);
        break;
    case QNetworkAccessManager::PutOperation:
        reply = man->put(request, body);
        break;
    case QNetworkAccessManager::PostOperation:
        reply = man->post(request, body);
        break;
    case QNetworkAccessManager::DeleteOperation:
        reply = man->deleteResource(request);
        break;
    case QNetworkAccessManager::CustomOperation:
        return 0;
    }
    waitReply(reply);
    return reply;
}

void TQOAuth::replyFinished(QNetworkReply *reply)
{
    readyReplies.append(reply);
}
