#include <QtCore>
#include "tqservice.h"
#include "HttpExt.h"
#include <QtXml>

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

//��� ������ http
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
    //������������� ������
    pVer->dwExtensionVersion=MAKELONG(0,1);
    //�������� ��������
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

extern "C" TQSERVICESHARED_EXPORT DWORD WINAPI   HttpExtensionProc( __in EXTENSION_CONTROL_BLOCK *lpECB )
//TQSERVICESHARED_EXPORT DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB)
{
    QMutexLocker locker(&reqMutex);
    //PostQueuedCompletionStatus(hIOPort,-1,NULL,(LPOVERLAPPED)lpECB);

    DWORD dwFlag, dwNull;
      //��������� a � b
      __int64 ia, ib;
      //����� ������� ���������
      int iParamFlg = 0;
      //������� ������� � ������ �������
      char *ppos = lpECB->lpszQueryString;
      char *pend = strchr(ppos, 0);
      ppos--;
      while(ppos && ppos + 3 < pend)
      {
        ppos++;
        char cPar = *ppos++;
        //���� ������� ���� "X="
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
        //���� ��������� ��������.
        ppos = strchr(ppos, '&');
      }
      //��������� �� �����
      char *pAll = NULL;
      //������ ������
      DWORD dwAll;
      if(iParamFlg == 3) //��� ���������
      {
        pAll = new char[21];
        __int64 res = ia*ib;
        sprintf(pAll, "%I64i", res);
        dwAll = strlen(pAll);
      }
      else
      {
        dwAll = 0;
        //��������� ��������� ������ ������
        lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get size
        pAll = new char[dwAll+1];
        //������� ����������
        lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get data
        pAll[dwAll] = 0;
        dwAll = strlen(pAll);
      }
      char pszHdr[200];
      static char szHeader[] = "Content-Length: %lu\r\nContent-type: text/plain\r\nPragma: no-cache\r\nExpires: 0\r\nCache-Control: no-cache\r\n\r\n";
      //��������� ���������
      sprintf(pszHdr, szHeader, dwAll); //create header
      HSE_SEND_HEADER_EX_INFO hInfo;
      hInfo.pszStatus = szRespOK;
      hInfo.pszHeader = pszHdr;
      hInfo.cchStatus = sizeof(szRespOK) - 1;
      hInfo.cchHeader = strlen(pszHdr);
      //���������� KeepAlive http/1.1
      hInfo.fKeepConn = TRUE;
      //���������� ���������
      lpECB->ServerSupportFunction(lpECB->ConnID, HSE_APPEND_LOG_PARAMETER, &hInfo, NULL, NULL);
      lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &hInfo, NULL, NULL);
      //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER, pszHdr, NULL, NULL);
      //���������� ������
      lpECB->WriteClient(lpECB->ConnID, pAll, &dwAll, HSE_IO_SYNC);
      DWORD dwStatus = HSE_STATUS_SUCCESS;
      //���������� �� �������� ���������� ���������
      //lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwStatus, NULL, NULL);
      delete [] pAll;
      //����������� �������

    return HSE_STATUS_SUCCESS;
}

void ExtensionThreadProc(LPVOID)
{
    /*
  DWORD dwFlag, dwNull;
  OVERLAPPED * pParam;
  while(::GetQueuedCompletionStatus(hIOPort, &dwFlag, &dwNull, &pParam, INFINITE) && dwFlag != NULL)
  {
    //����������� ���������
    LPEXTENSION_CONTROL_BLOCK lpECB = (LPEXTENSION_CONTROL_BLOCK)pParam;
    //��������� a � b
    __int64 ia, ib;
    //����� ������� ���������
    int iParamFlg = 0;
    //������� ������� � ������ �������
    char *ppos = lpECB->lpszQueryString;
    char *pend = strchr(ppos, 0);
    ppos--;
    while(ppos && ppos + 3 < pend)
    {
      ppos++;
      char cPar = *ppos++;
      //���� ������� ���� "X="
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
      //���� ��������� ��������.
      ppos = strchr(ppos, '&');
    }
    //��������� �� �����
    char *pAll = NULL;
    //������ ������
    DWORD dwAll;
    if(iParamFlg == 3) //��� ���������
    {
      pAll = new char[21];
      __int64 res = ia*ib;
      sprintf(pAll, "%I64i", res);
      dwAll = strlen(pAll);
    }
    else
    {
      dwAll = 0;
      //��������� ��������� ������ ������
      lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get size
      pAll = new char[dwAll];
      //������� ����������
      lpECB->GetServerVariable(lpECB->ConnID, "ALL_RAW", pAll, (unsigned long *)&dwAll); //get data
    }
    char pszHdr[200];
    static char szHeader[] = "Content-Length: %lu\r\nContent-type: text/plain\r\nPragma: no-cache\r\nExpires: 0\r\nCache-Control: no-cache\r\n\r\n";
    //��������� ���������
    sprintf(pszHdr, szHeader, dwAll); //create header
    HSE_SEND_HEADER_EX_INFO hInfo;
    hInfo.pszStatus = szRespOK;
    hInfo.pszHeader = pszHdr;
    hInfo.cchStatus = sizeof(szRespOK) - 1;
    hInfo.cchHeader = strlen(pszHdr);
    //���������� KeepAlive http/1.1
    hInfo.fKeepConn = TRUE;
    //���������� ���������
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &hInfo, NULL, NULL);
    //���������� ������
    lpECB->WriteClient(lpECB->ConnID, pAll, &dwAll, HSE_IO_SYNC);
    DWORD dwStatus = HSE_STATUS_SUCCESS_AND_KEEP_CONN;
    //���������� �� �������� ���������� ���������
    lpECB->ServerSupportFunction(lpECB->ConnID, HSE_REQ_DONE_WITH_SESSION, &dwStatus, NULL, NULL);
    delete [] pAll;
    //����������� �������
    ::ReleaseSemaphore(hSemaphore, 1, NULL);
  }
  _endthread();
  */
}

//================  TQService =====================

TQService::TQService()
{

    db=new TrkToolDB(this);
    db->dbmsUser = ""; //sets.value("dbmsUser").toString();
    db->dbmsPassword = ""; //sets.value("dbmsPassword").toString();
    db->dbmsName = "SHMAKOVTHINK\\SQLEXPRESS";
    //QScopedPointer<TrkToolProject> prj(db->openProject(argv[1],argv[2],argv[3],argv[4]));
    QString
            dbType = "LocalSQL", //sets.value("dbmsType").toString(),
            project = "RS-Bank V.6", //sets.value("project").toString(),
            user = "������", //sets.value("user").toString(),
            password = ""; //sets.value("password").toString();
    prj = db->openProject(dbType,project,user,password);
}

QString TQService::requestRecord(int id)
{
    QScopedPointer<TrkToolRecord> rec(prj->getRecordId(id));
    return rec->toXML().toString();
}
