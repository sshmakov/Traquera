#include "mainproc.h"
#include "mainwindow.h"
#include "ttrecwindow.h"


// =========================================================================
MainProc::MainProc(MainWindow *win)
    : mainWin(win)
{
}

QMainWindow *MainProc::mainWindow()
{
    return mainWin;
}

TQAbstractProject *MainProc::currentProject()
{
    return mainWin->currentProject();
}

bool MainProc::insertViewTab(QWidget *view, QWidget *tab, const QString &title)
{
    QueryPage *page = qobject_cast<QueryPage *>(view);
    if(page)
    {
        page->addDetailTab(tab, title, QIcon());
        return true;
    }
    TTRecordWindow *editor = qobject_cast<TTRecordWindow *>(view);
    if(editor)
    {
        editor->addDetailTab(tab, title, QIcon());
        return true;
    }
    return false;
}

bool MainProc::addPropWidget(QWidget *widget)
{
    return mainWin->addPropWidget(widget);
}

