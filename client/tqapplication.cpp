#include "tqapplication.h"
#include <tqdebug.h>


TQApplication::TQApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{

}

bool TQApplication::notify(QObject *obj, QEvent *ev)
{
    try {
        return QApplication::notify(obj, ev);
    }
    catch(...) {
        tqError() << "Exception";
        return false;
    }
}

