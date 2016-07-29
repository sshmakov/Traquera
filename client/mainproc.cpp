#include "mainproc.h"
#include "mainwindow.h"
#include "ttrecwindow.h"
#include "activefactory.h"

MainProc *mainProc;

// =========================================================================
MainProc::MainProc(MainWindow *win)
    : mainWin(win)
{
    mainProc = this;
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

QVariant MainProc::createActiveX(const QString &objectName, QObject *parent)
{
    QAxObject *obj = new ActiveXObject(objectName, parent);
    return obj->asVariant();
}

QAbstractMessageHandler *MainProc::messager()
{
    return messageHandler;
}

QXmlQuery MainProc::makeXmlQuery(TQViewController *controller) const
{
    QXmlQuery xquery;
    xquery.setMessageHandler(messageHandler);
    xquery.setNetworkAccessManager(ttglobal()->networkManager());

    if(controller)
    {
        QDomDocument xml("records");
        QDomElement root=xml.createElement("records");
        xml.appendChild(root);
        TQRecord *firstRecord = 0;
        foreach(QObject *obj, controller->selectedRecords())
        {
            TQRecord *rec = qobject_cast<TQRecord *>(obj);
            if(!rec)
                continue;
            if(!firstRecord)
                firstRecord = rec;
            rec->refresh();
            QDomDocument recxml = rec->toXML();
            root.appendChild(recxml);
        }

        QFile trackerXML(controller->project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
        trackerXML.open(QIODevice::ReadOnly);

        QString page;
        QByteArray baAll, baSingle;
        QBuffer bufAll, bufSingle;
        baAll= xml.toByteArray();
        bufAll.setData(baAll);
        bufAll.open(QIODevice::ReadOnly);

        if(firstRecord)
        {
            xml = firstRecord->toXML();
            baSingle = xml.toByteArray();
        }
        bufSingle.setData(baSingle);
        bufSingle.open(QIODevice::ReadOnly);

//        QFile xq(xqCodePath);
//        xq.open(QIODevice::ReadOnly);

        xquery.bindVariable("scrs",&bufAll);
        xquery.bindVariable("def",&trackerXML);
        xquery.bindVariable("scrdoc",&bufSingle);
    }
    return xquery;
}

