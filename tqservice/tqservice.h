#ifndef TQSERVICE_H
#define TQSERVICE_H

#include "tqservice_global.h"
#include <Windows.h>
#include "HttpExt.h"
#include "trkview.h"
#include <QtCore>
#include <QtXml>

class TQSession;
class TQMesHandler;

class TQService: public QObject
{
    Q_OBJECT
private:
    QHash<QString, TQSession *> sessions;
    QFile fout;
    QTextStream out;
public:
    TQService(QObject *parent = 0);
//    QDomDocument process(const QString &action, const QDomDocument &request);
    QDomDocument processDev(const QString &action, QIODevice *dev);
    QDomDocument handle(TQMesHandler *handler, QIODevice *dev);
    QDomDocument errorDoc(int errorCode, const QString &errorDesc, const QString &errorDetail = QString());
};

class TQSession: public QObject
{
    Q_OBJECT
private:
    TQAbstractDB *db;
    QString dbType;
    QHash<QString, TQAbstractProject *> connectedProjects;
//    bool opened;
    QDateTime createTime, lastActivity;
    QString sid;
public:
    TQSession(QObject *parent = 0);
    ~TQSession();
    QString sessionID() const;
    QStringList projectList();
    QString login(const QString &user, const QString &password, const QString &project);
    QStringList projects();
    TQAbstractProject *project(const QString &pid) const;
    bool isOpened(const QString &pid) const;
    QString requestRecord(const QString &pid, int id);
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
    /*
    enum RequestType {
        LoginProject,
        GetRecords,
        GetQueryList,
        OpenQuery,
        GetNoteTitles,
        GetRecordDef
    };

    int requestType;
    */
    QString lastTag;
    QString sessionId, projectId;
    TQSession *session;
    TQAbstractProject *project;
    QStringList log;
    QDomDocument resultDoc;
    QDomElement resultRoot, resultBody;
    QString lastErrorString;

    TQMesHandler();

    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    //bool characters(const QString &str);
    //bool fatalError(const QXmlParseException &exception);
    //QString errorString() const;

    virtual bool startTQElement(const QString &namespaceURI, const QString &localName,
                           const QString &qName, const QXmlAttributes &attributes);
    virtual bool endTQElement(const QString &namespaceURI, const QString &localName,
                            const QString &qName);
    QDomDocument executeAll();
    virtual int executeTQ();
    virtual bool	error ( const QXmlParseException & exception );
    virtual bool	fatalError ( const QXmlParseException & exception );
    QString errorString () const;
    void addErrorString(const QString &text);


//    QString user;
//    QString password;
//    QString project;
//    QString recordIds;
//    int recordType;
};

class TQLoginHandler : public TQMesHandler
{
public:
    QString user;
    QString password;
    QString project;

    TQLoginHandler();
    bool startTQElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endTQElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    int executeTQ();
};

class TQProjectListHandler : public TQMesHandler
{
public:
    int executeTQ();
};

/*
class TQLoggedHandler : public TQMesHandler
{
public:
    TQLoggedHandler();
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
};
*/

class TQQueryHandler : public TQMesHandler
{
public:
    QString queryName;
    int recType;

    TQQueryHandler();
    bool startTQElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    int executeTQ();
};

class TQQueryListHandler : public TQMesHandler
{
public:
    int recType;

    TQQueryListHandler();
    bool startTQElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    int executeTQ();
};

class TQRecordDefReq : public TQMesHandler
{
public:
    int recType;

    TQRecordDefReq();
    bool startTQElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    int executeTQ();
};



#endif // TQSERVICE_H
