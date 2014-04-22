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
    lpECB->GetServerVariable(lpECB->ConnID, name.toLatin1().data(), NULL, &bufLen);
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

    QByteArray resXML = result.toByteArray();
    qint64 resSize = resXML.size();

    char pszHdr[200];
    static char szHeader[] =
            "Content-Length: %lu\r\n"
            "Content-type: application/xml\r\n"
            "Pragma: no-cache\r\n"
            "Expires: 0\r\n"
            "Cache-Control: no-cache\r\n"
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

}

const char *SoapNS = "http://schemas.xmlsoap.org/soap/envelope/";
const char *AppNS = "http://shmakov.ru/TQService/";
const char *XsiNS="http://www.w3.org/2001/XMLSchema-instance";
const char *XsdNS="http://www.w3.org/2001/XMLSchema";

QDomDocument TQService::process(const QString &action, const QDomDocument &request)
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

QDomDocument TQService::processDev(const QString &action, QIODevice *dev)
{
    TQMesHandler handler;
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QXmlInputSource xmlInputSource(dev);
    reader.parse(xmlInputSource);

    if(action == "Login")
    {
        TQSession *session = new TQSession(this);
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
        }
    }
    return errorDoc(-1, handler.log);
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
    root.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xmlns:xsd","http://www.w3.org/2001/XMLSchema");
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

TQSession::TQSession(QObject *parent)
{
    db=new TrkToolDB(this);
    db->dbmsUser = ""; //sets.value("dbmsUser").toString();
    db->dbmsPassword = ""; //sets.value("dbmsPassword").toString();
    db->dbmsName = "SHMAKOVTHINK\\SQLEXPRESS";
    //QScopedPointer<TrkToolProject> prj(db->openProject(argv[1],argv[2],argv[3],argv[4]));
}

TQSession::~TQSession()
{
    if(prj)
        delete prj;
    if(db)
        delete db;
}

bool TQSession::login(const QString &user, const QString &password, const QString &project)
{
    QString dbType = "LocalSQL"; //sets.value("dbmsType").toString(),
//            project = "RS-Bank V.6", //sets.value("project").toString(),
//            user = QString::fromLocal8Bit("Сергей"), //sets.value("user").toString(),
//            password = ""; //sets.value("password").toString();
    prj = db->openProject(dbType,project,user,password);
    if(prj && prj->isOpened())
    {
        opened = true;
        return true;
    }
    opened = false;
    return false;
}


bool TQSession::isOpened() const
{
    return opened;
}

QString TQSession::requestRecord(int id)
{
    QScopedPointer<TrkToolRecord> rec(prj->createRecordById(id));
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
    qint64 res=0;
    qint64 inBuf = qMin<qint64>(maxSize, qMax<qint64>(ecb->cbAvailable - readed, 0));
    if(inBuf)
    {
        LPBYTE bufPtr = ecb->lpbData + readed;
        qMemCopy(data, bufPtr, inBuf);
        readed += inBuf;
        data += inBuf;
        res += inBuf;
        maxSize -= inBuf;
        res += inBuf;
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
    return res;
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
}

bool TQMesHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
    if(namespaceURI == AppNS)
    {
        if(localName == "Login")
        {
            user = attributes.value("user");
            password = attributes.value("password");
            project = attributes.value("project");
        }
    }
    log += QString("start element ns: %1, localName: %2, qName: %3").arg(namespaceURI, localName, qName)
        + "\n";
    return true;
}

bool TQMesHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    log += QString("end element ns: %1, localName: %2, qName: %3").arg(namespaceURI, localName, qName)
        + "\n";
    return true;
}
