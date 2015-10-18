#ifndef MAINPROC_H
#define MAINPROC_H
#include <ttglobal.h>
#include <QMainWindow>
#include <tqbase.h>

class MainWindow;
class MainProc: public TTMainProc
{
public:
    MainWindow *mainWin;

    MainProc(MainWindow *win);
    virtual QMainWindow *mainWindow();
    virtual TQAbstractProject *currentProject();
    virtual bool insertViewTab(QWidget *view, QWidget *tab, const QString &title);
    virtual bool addPropWidget(QWidget *widget);

};

#endif // MAINPROC_H
