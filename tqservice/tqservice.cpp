#include <QtCore>
#include "tqservice.h"
#include "HttpExt.h"

TQService *service=0;
QMutex reqMutex;

static const char szDescription[] = "TQService";


#define REG_PATH TEXT("Software\\Test\\MyISAPI")
#define REG_ISAPI_THREADS_POOL TEXT("ISAPIThreadsPool")
#define REG_ISAPI_QUEUE_TIMEOUT TEXT("ISAPIQueueTimeout")
#define REG_ISAPI_QUEUE_SIZE TEXT("ISAPIQueueSize")

#define DefaultTimeout 60000
#define DefaultSize 60
#define DefaultPool 5

#define VERSION_MINOR 0
#define VERSION_MAJOR 1

//const char szDescription[]="Test ISAPI Extension";

#define EventSource TEXT("Test.MyISAPI")

//код ответа http
static const char szRespOK[]="HTTP/1.1 200 OK";




//TQSERVICESHARED_EXPORT BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO* pVer);
//TQSERVICESHARED_EXPORT BOOL WINAPI TerminateExtension(DWORD dwFlags);
//TQSERVICESHARED_EXPORT DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB);

extern "C" TQSERVICESHARED_EXPORT BOOL __stdcall  GetExtensionVersion( __out HSE_VERSION_INFO  *pVer )
//TQSERVICESHARED_EXPORT BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO* pVer)
{
    QMutexLocker locker(&reqMutex);
    if(!service)
        service = new TQService();
    //устанавливает версию
    pVer->dwExtensionVersion=MAKELONG(0,1);
    //копируем описание
    memcpy(pVer->lpszExtensionDesc, szDescription, sizeof(szDescription));
    return TRUE;
}

extern "C" TQSERVICESHARED_EXPORT    BOOL  WINAPI   TerminateExtension( DWORD dwFlags )
//TQSERVICESHARED_EXPORT BOOL WINAPI TerminateExtension(DWORD dwFlags)
{
    QMutexLocker locker(&reqMutex);
    if(service)
    {
        TQService *s = service;
        service = 0;
        delete s;
    }
    return TRUE;
}

void ExtensionThreadProc(LPVOID);

QString serverVariable(const EXTENSION_CONTROL_BLOCK *lpECB, const QString &name)
{
    DWORD bufLen = 0;
    QByteArray arr = name.toLatin1();
    char *varName = arr.data();
    bool res = lpECB->GetServerVariable(lpECB->ConnID, varName, NULL, &bufLen);
    int error;
    if(!res && ERROR_INSUFFICIENT_BUFFER != (error = GetLastError()))
        return QString();
    if(!bufLen)
        return QString();
    QScopedArrayPointer<char> buf(new char[bufLen]);
    if(lpECB->GetServerVariable(lpECB->ConnID, name.toLatin1().data(), buf.data(), &bufLen))
        return QString(buf.data());
    return QString();
}

BOOL WINAPI GetServerVariable(
   HCONN hConn,
   LPSTR lpszVariableName,
   LPVOID lpvBuffer,
   LPDWORD lpdwSizeofBuffer
);

extern "C" TQSERVICESHARED_EXPORT DWORD WINAPI   HttpExtensionProc( __in EXTENSION_CONTROL_BLOCK *lpECB )
//TQSERVICESHARED_EXPORT DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB)
{
    QMutexLocker locker(&reqMutex);
    //PostQueuedCompletionStatus(hIOPort,-1,NULL,(LPOVERLAPPED)lpECB);

    TQHttpRequestDevice dev(lpECB);
    QString action = serverVariable(lpECB,"HTTP_SOAPAction");
    if(action.size()>=2 && action.at(0) == '"' && action.at(action.size()-1) == '"')
        action = action.mid(1, action.size()-2);

    QDomDocument result = service->processDev(action,&dev);

//    QDomDocument dom;
//    QXmlSimpleReader xmlReader;
//     QXmlInputSource *source = new QXmlInputSource(file);
//    dom.setContent(&dev);
//    QDomDocument result = service->process(action,dom);

    QString resString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
            + result.toString();
    QByteArray resXML = resString.toUtf8();
    qint64 resSize = resXML.size();

    char pszHdr[400];

    static char szHeader[] =
            "Content-Length: %lu\r\n"
            "Content-type: text/xml; charset=UTF-8\r\n"
            "Pragma: no-cache\r\n"
            "Expires: 0\r\n"
            "Cache-Control: no-cache\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Headers: SOAPAction, Content-type\r\n"
            "\r\n";
    sprintf(pszHdr, szHeader, resSize); //create header
    HSE_SEND_HEADER_EX_INFO hInfo;
    hInfo.pszStatus = szRespOK;
    hInfo.pszHeader = pszHdr;
    hInfo.cchStatus = sizeof(szRespOK) - 1;
    hInfo.cchHeader = strlen(pszHdr);
    //используем KeepAlive http/1.1
    hInfo.fKeepConn = TRUE;
    //отправляем заголовок
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_APPEND_LOG_PARAMETER, &hInfo, NULL, NULL);
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &hInfo, NULL, NULL);
    //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, pszHdr, NULL, NULL);

    qint64 sended = 0;
    while(sended < resSize)
    {
        DWORD bufLen = qMin<qint64>(16*1024, resSize - sended);
        char * buf = resXML.data() + sended;
        if(!lpECB->WriteClient(lpECB->ConnID, buf, &bufLen, HSE_IO_SYNC))
            break;
        if(!bufLen)
            break;
        sended += bufLen;
    }
    DWORD dwStatus = HSE_STATUS_SUCCESS;
    //уведомляем об успешном завершении обработки
    //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwStatus, NULL, NULL);

    return HSE_STATUS_SUCCESS;

#ifdef _SAMPLE_
    //указатель на буфер
    char *pAll = NULL;
    //размер буфера
    DWORD dwAll;
    DWORD dwFlag, dwNull;
    //параметры a и b
    __int64 ia, ib;
    //флаги наличия парметров
    int iParamFlg = 0;
    //текущая позиция в строке запроса
    char *ppos = lpECB->lpszQueryString;
    char *pend = strchr(ppos, 0);
    ppos--;
    while(ppos && ppos + 3 < pend)
    {
        ppos++;
        char cPar = *ppos++;
        //ищем строчку вида "X="
        if(*ppos++ == '=')
        {
            switch(cPar)
            {
            case 'a':
                ia = atoi(ppos);
                iParamFlg |= 0x01;
                break;
            case 'b':
                ib = atoi(ppos);
                iParamFlg |= 0x02;
            }
        }
        //ищем следующий параметр.
        ppos = strchr(ppos, '&');
    }
    if(iParamFlg == 3) //оба параметра
    {
        QString rec = service->requestRecord(ia);
        pAll = new char[rec.length()+1];
        strcpy(pAll,rec.toLocal8Bit().constData());


        //        pAll = new char[21];
        //        __int64 res = ia*ib;
        //        sprintf(pAll, "%I64i", res);
        dwAll = strlen(pAll);
    }
    else
    {
        dwAll = 0;
        //определим требуемый размер буфера
        lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get size
        pAll = new char[dwAll+1];
        //получим переменную
        lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get data
        pAll[dwAll] = 0;
        dwAll = strlen(pAll);
    }
    char pszHdr[200];
    static char szHeader[] = "Content-Length: %lu\r\nContent-type: text/plain\r\nPragma: no-cache\r\nExpires: 0\r\nCache-Control: no-cache\r\n\r\n";
    //формируем заголовок
    sprintf(pszHdr, szHeader, dwAll); //create header
    HSE_SEND_HEADER_EX_INFO hInfo;
    hInfo.pszStatus = szRespOK;
    hInfo.pszHeader = pszHdr;
    hInfo.cchStatus = sizeof(szRespOK) - 1;
    hInfo.cchHeader = strlen(pszHdr);
    //используем KeepAlive http/1.1
    hInfo.fKeepConn = TRUE;
    //отправляем заголовок
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_APPEND_LOG_PARAMETER, &hInfo, NULL, NULL);
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &hInfo, NULL, NULL);
    //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, pszHdr, NULL, NULL);
    //отправляем данные
    lpECB->WriteClient(lpECB->ConnID, pAll, &dwAll, HSE_IO_SYNC);
    DWORD dwStatus = HSE_STATUS_SUCCESS;
    //уведомляем об успешном завершении обработки
    //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwStatus, NULL, NULL);
    delete [] pAll;
    //освобождаем семафор

    return HSE_STATUS_SUCCESS;
#endif
}

void ExtensionThreadProc(LPVOID)
{
    /*
  DWORD dwFlag, dwNull;
  OVERLAPPED * pParam;
  while(::GetQueuedCompletionStatus(hIOPort, &dwFlag, &dwNull, &pParam, INFINITE) && dwFlag != NULL)
  {
    //преобразуем указатель
    LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK)pParam;
    //параметры a и b
    __int64 ia, ib;
    //флаги наличия парметров
    int iParamFlg = 0;
    //текущая позиция в строке запроса
    char *ppos = lpECB->lpszQueryString;
    char *pend = strchr(ppos, 0);
    ppos--;
    while(ppos && ppos + 3 < pend)
    {
      ppos++;
      char cPar = *ppos++;
      //ищем строчку вида "X="
      if(*ppos++ == '=')
      {
        switch(cPar)
        {
          case 'a':
              ia = atoi(ppos);
              iParamFlg |= 0x01;
              break;
          case 'b':
              ib = atoi(ppos);
              iParamFlg |= 0x02;
        }
      }
      //ищем следующий параметр.
      ppos = strchr(ppos, '&');
    }
    //указатель на буфер
    char *pAll = NULL;
    //размер буфера
    DWORD dwAll;
    if(iParamFlg == 3) //оба параметра
    {
      pAll = new char[21];
      __int64 res = ia*ib;
      sprintf(pAll, "%I64i", res);
      dwAll = strlen(pAll);
    }
    else
    {
      dwAll = 0;
      //определим требуемый размер буфера
      lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get size
      pAll = new char[dwAll];
      //получим переменную
      lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get data
    }
    char pszHdr[200];
    static char szHeader[] = "Content-Length: %lu\r\nContent-type: text/plain\r\nPragma: no-cache\r\nExpires: 0\r\nCache-Control: no-cache\r\n\r\n";
    //формируем заголовок
    sprintf(pszHdr, szHeader, dwAll); //create header
    HSE_SEND_HEADER_EX_INFO hInfo;
    hInfo.pszStatus = szRespOK;
    hInfo.pszHeader = pszHdr;
    hInfo.cchStatus = sizeof(szRespOK) - 1;
    hInfo.cchHeader = strlen(pszHdr);
    //используем KeepAlive http/1.1
    hInfo.fKeepConn = TRUE;
    //отправляем заголовок
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &hInfo, NULL, NULL);
    //отправляем данные
    lpECB->WriteClient(lpECB->ConnID, pAll, &dwAll, HSE_IO_SYNC);
    DWORD dwStatus = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
    //уведомляем об успешном завершении обработки
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwStatus, NULL, NULL);
    delete [] pAll;
    //освобождаем семафор
    ::ReleaseSemaphore(hSemaphore, 1, NULL);
  }
  _endthread();
  */
}

static QString tagLocalName(const QDomElement &node, const QString & ns = QString())
{
    QString local = node.localName();
    if(local.isEmpty())
    {
        QString tag = node.tagName();
        QString prefix = node.prefix();
        if(tag.indexOf(prefix)==0)
            return tag.left(prefix.size());
    }
    return local;
}

//================  TQService =====================

TQService::TQService(QObject *parent)
    : QObject(parent)
{
    fout.open(stdout,QIODevice::WriteOnly);
    out.setDevice(&fout);
}

const char *SoapNS = "http://schemas.xmlsoap.org/soap/envelope/";
const char *AppNS = "http://shmakov.ru/TQService/";
const char *XsiNS="http://www.w3.org/2001/XMLSchema-instance";
const char *XsdNS="http://www.w3.org/2001/XMLSchema";

/*QDomDocument TQService::process(const QString &action, const QDomDocument &request)
{
    QDomElement header, body;

    header = request.firstChildElement().firstChildElement();
    QString name = header.localName();
    if(name.isEmpty())
        name = header.nodeName();
    if(QString::compare(name, "body", Qt::CaseInsensitive) == 0)
    {
        body= header;
        header = QDomElement();
    }
    else
        body = header.nextSiblingElement();
    if(action == "Login")
    {

        if(body.isNull())
            return errorDoc(-1, "No body element after "+header.nodeName());

//        QDomNodeList list = body.elementsByTagNameNS("tqs:Login",AppNS);
//        if(list.isEmpty())
//            return errorDoc(-1, "No login elements");
//        QDomElement login = list.at(0).toElement();
        QDomElement login = body.firstChildElement();
        if(login.isNull())
            return errorDoc(-1, "Null login element");
        if(QString::compare(login.localName(),"login", Qt::CaseInsensitive))
            return errorDoc(-1, QString("[Node: %1, Tag: %2, Local: %3, Prefix: %4] - isn't login element ")
                            .arg(login.nodeName(), login.tagName(), login.localName(), login.prefix()));
        QString user = login.attribute("user");
        QString pass = login.attribute("password");
        QString project = login.attribute("project");
        if(user.isEmpty() || project.isEmpty())
            return errorDoc(-1, "User and project must be filled");

        QDomDocument xml;
        QDomElement root= xml.createElementNS(SoapNS, "soapenv:Envelope");
        xml.appendChild(root);
        QDomElement body= xml.createElement("soapenv:Body");
        root.appendChild(body);
        QDomElement loginResult = xml.createElementNS(AppNS, "LoginResult");
        loginResult.appendChild(request.documentElement());
        body.appendChild(loginResult);
        return xml;
    }
    else
        return errorDoc(-1, "Unknown action");
}
*/

QDomDocument TQService::processDev(const QString &action, QIODevice *dev)
{
    out << action;
    if(action == "Login")
    {
        TQLoginHandler handler;
        return handle(&handler,dev);

        /*TQSession *session = new TQSession(this);
        if(session->login(handler.user,handler.password,handler.project))
        {
            QString sid = QUuid::createUuid().toString();
            sessions[sid] = session;
            QDomDocument xml;
            QDomElement root= xml.createElementNS(SoapNS, "soapenv:Envelope");
            root.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
            root.setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
            xml.appendChild(root);

            QDomElement body= xml.createElement("soapenv:Body");
            root.appendChild(body);
            QDomElement res= xml.createElement("session");
            res.setAttribute("id",sid);
            body.appendChild(res);
            return xml;
        }
        else
        {
            delete session;
            return errorDoc(-1, "Not connected to project");
        }*/
    }
    else if (action == "GetProjects")
    {
        TQProjectListHandler handler;
        return handle(&handler,dev);
    }
    else if (action == "Query")
    {
        TQQueryHandler handler;
        return handle(&handler,dev);
    }
    else if (action == "QueryList")
    {
        TQQueryListHandler handler;
        return handle(&handler,dev);
    }
    else if (action == "RecordDef")
    {
        TQRecordDefReq handler;
        return handle(&handler,dev);
    }
    return errorDoc(-1, "Unknown action");
}

QDomDocument TQService::handle(TQMesHandler *handler, QIODevice *dev)
{
    QXmlInputSource xmlInputSource(dev);
    QXmlSimpleReader reader;
    reader.setContentHandler(handler);
    reader.setErrorHandler(handler);

    reader.parse(xmlInputSource);
    return handler->executeAll();
}

QDomDocument TQService::errorDoc(int errorCode, const QString &errorDesc, const QString &errorDetail)
{
    /*
     * <SOAP-ENV:Fault>
      <faultcode xsi:type="xsd:string">SOAP-ENV:53</faultcode>
      <faultstring xsi:type="xsd:string">Authorization error</faultstring>
      <detail xsi:type="xsd:string"/>
      </SOAP-ENV:Fault>
     */

    QDomDocument xml;
    QDomElement root= xml.createElementNS(SoapNS, "soapenv:Envelope");
    root.setAttribute("xmlns:xsi", XsiNS /*"http://www.w3.org/2001/XMLSchema-instance"*/);
    root.setAttribute("xmlns:xsd", XsdNS /*"http://www.w3.org/2001/XMLSchema"*/);
    xml.appendChild(root);

    QDomElement body= xml.createElement("soapenv:Body");
    root.appendChild(body);

    QDomElement fault= xml.createElement("soapenv:Fault");
    body.appendChild(fault);

    QDomElement faultcode= xml.createElement("faultcode");
    faultcode.setAttribute("xsi:type","xsd:string");
    faultcode.appendChild(xml.createTextNode(QString::number(errorCode)));
    fault.appendChild(faultcode);

    QDomElement faultstring= xml.createElement("faultstring");
    faultstring.setAttribute("xsi:type","xsd:string");
    faultstring.appendChild(xml.createTextNode(errorDesc));
    fault.appendChild(faultstring);

    QDomElement detail = xml.createElement("detail");
    detail.setAttribute("xsi:type","xsd:string");
    detail.appendChild(xml.createTextNode(errorDetail));
    //faultcode.setNodeValue(QString::number(errorCode));
    fault.appendChild(detail);

    return xml;
}

static QHash<QString, TQSession *> sessionsPool;

TQSession::TQSession(QObject *parent)
{
    db = TQAbstractDB::createDbClass("pvcs", this); // new TrkToolDB(this);
    dbType = "LocalSQL";
    db->setDbmsType(dbType);
    db->setDbmsServer("SHMAKOVTHINK\\SQLEXPRESS");
    db->setDbmsUser("", //sets.value("dbmsUser").toString(),
                    "" //sets.value("dbmsPassword").toString();
                    );
//    db->dbmsUser = "";
//    db->dbmsPassword = "";
//    db->dbmsName = "SHMAKOVTHINK\\SQLEXPRESS";
    createTime = QDateTime::currentDateTimeUtc();
    lastActivity = createTime;
    sid = QUuid::createUuid().toString();
    sessionsPool.insert(sid, this);
    //QScopedPointer<TrkToolProject> prj(db->openProject(argv[1],argv[2],argv[3],argv[4]));
}

TQSession::~TQSession()
{
    if(!connectedProjects.isEmpty())
    {
        foreach(TQAbstractProject *prj, connectedProjects)
            delete prj;
        connectedProjects.clear();
    }
    if(db)
        delete db;
    sessionsPool.remove(sid);
}

QString TQSession::sessionID() const
{
    return sid;
}

QStringList TQSession::projectList()
{
    return db->projects(dbType);
}

QString TQSession::login(const QString &user, const QString &password, const QString &project)
{
     //sets.value("dbmsType").toString(),
//            project = "RS-Bank V.6", //sets.value("project").toString(),
//            user = QString::fromLocal8Bit("Сергей"), //sets.value("user").toString(),
//            password = ""; //sets.value("password").toString();
    TQAbstractProject *prj = db->openProject(project,user,password);
    if(prj)
        if(prj->isOpened())
        {
            QString pid = QUuid::createUuid().toString();
            connectedProjects[pid] = prj;
            return pid;
        }
        else
            delete prj;
    return QString();
}

QStringList TQSession::projects()
{
    return db->projects(dbType);
}

TQAbstractProject *TQSession::project(const QString &pid) const
{
    return connectedProjects.value(pid,0);
}


bool TQSession::isOpened(const QString &pid) const
{
    TQAbstractProject *prj = project(pid);
    return prj && prj->isOpened();
}

QString TQSession::requestRecord(const QString &pid, int id)
{
    TQAbstractProject *prj = project(pid);
    if(!prj)
        return QString();
    QScopedPointer<TQRecord> rec(prj->createRecordById(id, prj->defaultRecType()));
    if(rec.isNull())
        return "empty";
    return rec->toXML().toString();
}

// ==================== TQReadRequestDevice ======================
TQHttpRequestDevice::TQHttpRequestDevice(EXTENSION_CONTROL_BLOCK *lpECB, QObject *parent)
    :QIODevice(parent)
{
    ecb = lpECB;
    readed = 0;
}

TQHttpRequestDevice::~TQHttpRequestDevice()
{
}

bool TQHttpRequestDevice::isSequential() const
{
    return true;
}

qint64 TQHttpRequestDevice::readData(char *data, qint64 maxSize)
{
    qint64 resReaded=0;
    qint64 inBuf = qMin<qint64>(maxSize, qMax<qint64>(ecb->cbAvailable - readed, 0));
    if(inBuf)
    {
        LPBYTE bufPtr = ecb->lpbData + readed;
        qMemCopy(data, bufPtr, inBuf);
        readed += inBuf;
        data += inBuf;
        resReaded += inBuf;
        maxSize -= inBuf;
    }
    while(maxSize)
    {
        DWORD size = qMin<qint64>(maxSize, 48*1024);
        BOOL res = ecb->ReadClient(ecb->ConnID, data, &size);
        if(!res)
            break;
        maxSize -= size;
        res += size;
        data += size;
        readed += size;
        if(!size)
            break;
    }
    return resReaded;
}

qint64 TQHttpRequestDevice::writeData(const char *data, qint64 maxSize)
{
    qint64 res=0;
    char * buf = (char *)data;
    while(maxSize)
    {
        DWORD size = qMin<qint64>(maxSize, 16*1024);
        BOOL res = ecb->WriteClient(ecb->ConnID, buf, &size, HSE_IO_SYNC);
        if(!res)
            break;
        maxSize -= size;
        res += size;
        buf += size;
        if(!size)
            break;
    }
    return res;
}

qint64 TQHttpRequestDevice::bytesAvailable() const
{
    return ecb->cbTotalBytes - readed + QIODevice::bytesAvailable();
}

TQMesHandler::TQMesHandler()
{
    session = 0;
    resultDoc = QDomDocument();
    resultRoot = resultDoc.createElementNS(SoapNS, "soapenv:Envelope");
    //resultRoot.setAttribute("xmlns:soapenv","http://schemas.xmlsoap.org/soap/envelope/");
    resultRoot.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    resultRoot.setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
    resultDoc.appendChild(resultRoot);

    resultBody = resultDoc.createElement("soapenv:Body");
    resultRoot.appendChild(resultBody);
}

bool TQMesHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    log << QString("start element ns: %1, localName: %2, qName: %3").arg(namespaceURI, localName, qName)
        + "\n";
    if(namespaceURI == AppNS || namespaceURI.isEmpty())
    {
        if(localName == "Session")
        {
            //requestType = LoginProject;
            sessionId = attributes.value("sessionID");
            session = sessionsPool.value(sessionId,0);
            if(!session)
            {
                addErrorString(QString("Not found sessionID '%1'").arg(sessionId));
                QString s("Registered:") ;
                foreach(const QString &sid, sessionsPool.keys())
                    s += " " + sid;
                addErrorString(s);
            }
            return true;
        }
        else if(localName == "Project")
        {
            projectId = attributes.value("projectID");
            if(session)
                project = session->project(projectId);
            else
                project = 0;
            if(!project)
                addErrorString(QString("Not found projectID '%1'").arg(projectId));
            return true;
        }
    }
    return startTQElement(namespaceURI, localName, qName, attributes);
    /*
        else if(localName == "GetRecords")
        {
            requestType = GetRecords;
            project = attributes.value("project");
            recordIds = attributes.value("recordIDs");
            recordType = attributes.value("recordType").toInt();
        }
        else if(localName == "GetQueryList")
        {
            requestType = GetQueryList;
            project = attributes.value("project");
        }
        else if(localName == "OpenQuery")
        {
            requestType = OpenQuery;
            project = attributes.value("project");
        }
        else if(localName == "GetNoteTitles")
        {
            requestType = GetNoteTitles;
        }
        else if(localName == "GetRecordDef")
        {
            requestType = GetRecordDef;
        }
    }
    return true;
    */
}

bool TQMesHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    log << QString("end element ns: %1, localName: %2, qName: %3").arg(namespaceURI, localName, qName)
        + "\n";
    return endTQElement(namespaceURI, localName, qName);
}

bool TQMesHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    return true;
}

bool TQMesHandler::endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    return true;
}

QDomDocument TQMesHandler::executeAll()
{
    int res = executeTQ();
    if(res)
        return service->errorDoc(res,"Execute error",errorString());
    return resultDoc;
}

int TQMesHandler::executeTQ()
{
    return 0;
}

bool TQMesHandler::error(const QXmlParseException &exception)
{
    lastErrorString += QString("Error in %1:%2 %3/r/n").arg(exception.lineNumber(),exception.columnNumber()).arg(exception.message());
    return true;
}

bool TQMesHandler::fatalError(const QXmlParseException &exception)
{
    lastErrorString += QString("Fatal error in %1:%2 %3/r/n")
            .arg(exception.lineNumber())
            .arg(exception.columnNumber())
            .arg(exception.message());
    return true;
}

QString TQMesHandler::errorString() const
{
    return lastErrorString;
}

void TQMesHandler::addErrorString(const QString &text)
{
    lastErrorString += text;
}


TQLoginHandler::TQLoginHandler()
{
}

bool TQLoginHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    if(localName == "Login")
    {
        //requestType = LoginProject;
        user = attributes.value("user");
        password = attributes.value("password");
        project = attributes.value("project");
    }
    return true;
}

bool TQLoginHandler::endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    return true;
}

int TQLoginHandler::executeTQ()
{
    bool newSess = !session;
    if(newSess)
        session = new TQSession();
    QString pid = session->login(user,password,project);
    TQAbstractProject *prj = 0;
    if(!pid.isEmpty())
        prj = session->project(pid);
    if(prj)
    {
        QDomElement res = resultDoc.createElement("LoginResult");
        res.setAttribute("result","true");
        QString projName = prj->projectName();
        res.setAttribute("project",projName);
//        QDomElement sess = resultDoc.createElement("Session");
        res.setAttribute("sessionID",session->sessionID());
//        resultBody.appendChild(res);
//        QDomElement prjNode = resultDoc.createElement("Project");
        res.setAttribute("projectID",pid);
        resultBody.appendChild(res);
        return 0;
    }
    if(newSess)
    {
        delete session;
        session = 0;
    }
    return 10;
}


int TQProjectListHandler::executeTQ()
{
    if(!session)
        return 10;
    QDomElement res = resultDoc.createElement("ProjectList");
    resultBody.appendChild(res);
    foreach(const QString &p, session->projects())
    {
        QDomElement res = resultDoc.createElement("Project");
        res.setAttribute("name",p);
        resultBody.appendChild(res);
    }
    return 0;
}


TQQueryHandler::TQQueryHandler()
{
}

bool TQQueryHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    if(localName == "Query")
    {
        queryName = attributes.value("queryName");
        QString r = attributes.value("recordType");
        if(r.isEmpty())
            recType = 1;
        else
            recType = r.toInt();
    }
    return true;
}

int TQQueryHandler::executeTQ()
{
    if(!session || !project)
        return 12;
    if(queryName.isEmpty())
        return 13;
    QList<int> ids = project->getQueryIds(queryName, recType);
    foreach(int id, ids)
    {
        QDomElement res = resultDoc.createElement("Record");
        res.setAttribute("id",id);
        resultBody.appendChild(res);
    }
    return 0;
}


TQQueryListHandler::TQQueryListHandler()
{
}

bool TQQueryListHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    if(localName.compare("QueryList",Qt::CaseInsensitive) == 0)
    {
        QString r = attributes.value("recordType");
        if(r.isEmpty())
            recType = 1;
        else
            recType = r.toInt();
    }
    return true;
}

int TQQueryListHandler::executeTQ()
{
    if(!session || !project)
        return 12;
    QAbstractItemModel *model = project->queryModel(recType);
    QDomElement res = resultDoc.createElement("QueryList");
    for(int r=0; r<model->rowCount(); r++)
    {
        QString name = model->index(r,0).data().toString();
        QString pub = model->index(r,1).data().toString();
        QDomElement q = resultDoc.createElement("Query");
        q.setAttribute("name",name);
        q.setAttribute("public",pub);
        res.appendChild(q);
    }
    resultBody.appendChild(res);
    return 0;
}


TQRecordDefReq::TQRecordDefReq()
{
}

bool TQRecordDefReq::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    if(localName.compare("RecordDef",Qt::CaseInsensitive) == 0)
    {
        QString r = attributes.value("recordType");
        if(r.isEmpty())
            recType = 1;
        else
            recType = r.toInt();
    }
    return true;
}

int TQRecordDefReq::executeTQ()
{
    if(!session || !project)
        return 12;
    QDomDocument defDoc = project->recordTypeDefDoc(recType);
    QDomElement res = resultDoc.createElement("RecordDef");
    res.setAttribute("recordType",recType);
    resultBody.appendChild(res);
    resultBody.appendChild(defDoc.documentElement());
    return 0;
}
