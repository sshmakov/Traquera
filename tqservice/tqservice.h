#ifndef TQSERVICE_H
#define TQSERVICE_H

#include "tqservice_global.h"
#include <Windows.h>
#include "HttpExt.h"
#include "trkview.h"
#include <QtCore>
#include <QtXml>

class TQSession;

class TQService: public QObject
{
    Q_OBJECT
private:
    QHash<QString, TQSession *> sessions;
public:
    TQService(QObject *parent = 0);
    QDomDocument process(const QString &action, const QDomDocument &request);
    QDomDocument processDev(const QString &action, QIODevice *dev);
    QDomDocument errorDoc(int errorCode, const QString &errorDesc, const QString &errorDetail = QString());
};

class TQSession: public QObject
{
    Q_OBJECT
private:
    TrkToolDB *db;
    TrkToolProject *prj;
    bool opened;
public:
    TQSession(QObject *parent = 0);
    ~TQSession();
    bool login(const QString &user, const QString &password, const QString &project);
    bool isOpened() const;
    QString requestRecord(int id);
};

class TQHttpRequestDevice: public QIODevice
{
    Q_OBJECT
private:
    EXTENSION_CONTROL_BLOCK *ecb;
    qint64 readed;
public:
    TQHttpRequestDevice(EXTENSION_CONTROL_BLOCK *lpECB, QObject *parent = 0);
    ~TQHttpRequestDevice();
    bool isSequential () const;
    qint64 bytesAvailable() const;
protected:
    virtual qint64	readData ( char * data, qint64 maxSize );
    virtual qint64	writeData (const char *data, qint64 maxSize );
};

extern "C" {
TQSERVICESHARED_EXPORT BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO* pVer);
TQSERVICESHARED_EXPORT BOOL WINAPI TerminateExtension(DWORD dwFlags);
TQSERVICESHARED_EXPORT DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB);
}

class TQMesHandler : public QXmlDefaultHandler
{
public:
    TQMesHandler();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    //bool characters(const QString &str);
    //bool fatalError(const QXmlParseException &exception);
    //QString errorString() const;

    QString user;
    QString password;
    QString project;

    QString log;
};

#endif // TQSERVICE_H
