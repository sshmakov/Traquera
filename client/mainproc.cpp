#include "mainproc.h"
#include "mainwindow.h"
#include "ttrecwindow.h"
#include "activefactory.h"
#include "ttdelegate.h"

#include <QtCore>
#include <axscriptable.h>

MainProc *mainProc;

class MainProcPrivate
{
public:
    TQEditorFactory *factory;
    MainProcPrivate()
    {
        factory = new TQEditorFactory();
        QItemEditorCreatorBase *creator;
        creator = new QStandardItemEditorCreator<TQDateTimeFieldEdit>();
        factory->registerEditor(QVariant::DateTime, creator);
    }
    ~MainProcPrivate()
    {
        delete factory;
    }
};

// =========================================================================
MainProc::MainProc(MainWindow *win)
    : mainWin(win), d(new MainProcPrivate())
{
    mainProc = this;
}

MainProc::~MainProc()
{
    delete d;
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
    QAxObject *obj;
    if(objectName.isEmpty())
        obj = new ActiveXObject(parent);
    else
        obj = new ActiveXObject(objectName, parent);
    if(obj)
        obj->connect(obj,SIGNAL(exception(int, const QString &, const QString &, const QString &)),
                                //"_exception(int,QString,QString,QString)",
                                mainWin,SLOT(onAxException(int,QString,QString,QString)));
    return obj->asVariant();
}

QObject *MainProc::createActiveXObj(const QString &objectName, QObject *parent)
{
    QAxObject *obj;
    if(objectName.isEmpty())
        return 0;
    obj = new ActiveXObject(objectName, parent);
    if(obj)
        obj->connect(obj,SIGNAL(exception(int, const QString &, const QString &, const QString &)),
                                //"_exception(int,QString,QString,QString)",
                                mainWin,SLOT(onAxException(int,QString,QString,QString)));
    return obj;
}

QAbstractMessageHandler *MainProc::messager() const
{
    return messageHandler;
}

QString MainProc::makeXMLController(QXmlQuery *xquery, const QString &xqCodePath, TQViewController *controller) const
{
    QString page;
    xquery->setMessageHandler(messager());
    QNetworkAccessManager *man = ttglobal()->networkManager();
    if(man)
        xquery->setNetworkAccessManager(man);

    TQRecord *firstRecord = 0;
    QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    if(controller)
    {
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

        QFile groupXML(controller->project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
        bool isGroupXMLOpened = groupXML.open(QIODevice::ReadOnly);
        if(isGroupXMLOpened)
            xquery->bindVariable("def",&groupXML);

    }
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

    xquery->bindVariable("scrs",&bufAll);
    xquery->bindVariable("scrdoc",&bufSingle);
    QFile xq(xqCodePath);
    xq.open(QIODevice::ReadOnly);
    xquery->setQuery(&xq, xqCodePath);

    xquery->evaluateTo(&page);
    return page;
}

QString MainProc::makeXmlQuery(QXmlQuery *xquery, const QString &xqCodePath, const QObjectList &records) const
{
    QString page;
    xquery->setMessageHandler(messager());
    QNetworkAccessManager *man = ttglobal()->networkManager();
    if(man)
        xquery->setNetworkAccessManager(man);

    TQRecord *firstRecord = 0;
    QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    for(int i = 0; i < records.size(); i++)
    {
        QObject *obj = records.value(i);
        TQRecord *rec = qobject_cast<TQRecord *>(obj);
        if(!rec)
            continue;
        if(!firstRecord)
            firstRecord = rec;
        if(!rec->isEditing())
            rec->refresh();
        QDomDocument recxml = rec->toXML();
        root.appendChild(recxml);
    }
    QByteArray baAll, baSingle;
    QBuffer bufAll, bufSingle;
    baAll= xml.toByteArray();
    bufAll.setData(baAll);
    bufAll.open(QIODevice::ReadOnly);
    xquery->bindVariable("scrs",&bufAll);

    if(firstRecord)
    {
        QDomDocument xml = firstRecord->toXML();
        baSingle = xml.toByteArray();
        if(!baSingle.isEmpty())
        {
            bufSingle.setData(baSingle);
            bufSingle.open(QIODevice::ReadOnly);
            xquery->bindVariable("scrdoc",&bufSingle);
        }
        QFile groupXML(firstRecord->project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
        bool isGroupXMLOpened = groupXML.open(QIODevice::ReadOnly);
        if(isGroupXMLOpened)
            xquery->bindVariable("def",&groupXML);
    }

    QFile xq(xqCodePath);
    xq.open(QIODevice::ReadOnly);
    xquery->setQuery(&xq, xqCodePath);

    xquery->evaluateTo(&page);
    return page;
}

QString MainProc::makeXmlQuery(QXmlQuery *xquery, const QString &xqCodePath, TQRecord *record) const
{
    QString page;
    xquery->setMessageHandler(messager());
    QNetworkAccessManager *man = ttglobal()->networkManager();
    if(man)
        xquery->setNetworkAccessManager(man);

    QByteArray baSingle;
    QBuffer bufSingle;
    QDomDocument xml;
    if(record)
    {
        QFile groupXML(record->project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
        bool isGroupXMLOpened = groupXML.open(QIODevice::ReadOnly);
        if(isGroupXMLOpened)
            xquery->bindVariable("def",&groupXML);
        xml = record->toXML();
        baSingle = xml.toByteArray();
        bufSingle.setData(baSingle);
        bufSingle.open(QIODevice::ReadOnly);
        xquery->bindVariable("scrdoc",&bufSingle);
    }
    else
    {
        return QString("<html><head/><body/></html>");
    }
    QFile xq(xqCodePath);
    xq.open(QIODevice::ReadOnly);
    xquery->setQuery(&xq, xqCodePath);

    xquery->evaluateTo(&page);
    return page;
}

TQEditorFactory *MainProc::fieldEditorFactory() const
{
    return d->factory;
}

void MainProc::populateScriptEngine(QScriptEngine *engine)
{
    AxScriptable::initEngine(engine);
}

