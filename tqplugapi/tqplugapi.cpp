#include "tqplugapi.h"
#include "ttglobal.h"


TQPlugAPI::TQPlugAPI()
{
}

static TTGlobal *ttGlobalObj=0;

//TQPLUGIN_SHARED TTGlobal *TTGlobal::global()
//{
//    if(!ttGlobal)
//        ttGlobal=new TTGlobal();
//    return ttGlobal;
//}

TTGlobal *ttglobal()
{
    if(!ttGlobalObj)
        ttGlobalObj = new TTGlobal();
    return ttGlobalObj;
}
