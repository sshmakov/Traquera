#ifndef TQSERVICE_H
#define TQSERVICE_H

#include "tqservice_global.h"
#include <Windows.h>
#include "HttpExt.h"
#include "trkview.h"



class TQService: public QObject
{
private:
    TrkToolDB *db;
    TrkToolProject *prj;
public:
    TQService();
};

extern "C" {
TQSERVICESHARED_EXPORT BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO* pVer);
TQSERVICESHARED_EXPORT BOOL WINAPI TerminateExtension(DWORD dwFlags);
TQSERVICESHARED_EXPORT DWORD WINAPI HttpExtensionProc(LPEXTENSION_CONTROL_BLOCK lpECB);
}



#endif // TQSERVICE_H
