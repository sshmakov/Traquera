#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QScrollArea>
#include <QFormLayout>
#include <QLineEdit>
#include <QSettings>
#include <QString>
#include <QMenu>
#include <QChar>
#include <QXmlQuery>
#include <QUrl>
#include <QSqlRecord>
#include <QBuffer>
#include <QAxObject>
#include <QMessageBox>
#include <QTextStream>
#include <QPrintPreviewDialog>
#include <QShortcut>
#include <QClipboard>
#include <QtGui>
#include <QtWebKit>
#include <QScriptEngineDebugger>

#include "querypage.h"
//#include "trkview.h"
#include "settings.h"
//#include "scrwidg.h"
#include "notedialog.h"
#include "messager.h"
#include "ttutils.h"
#include "ttfolders.h"
#include "ttrecwindow.h"
#include "ttglobal.h"
#include "ttfileiconprovider.h"
#include "preview.h"
#include "cliputil.h"
#include "tqcolsdialog.h"
#include "tqproxyrecmodel.h"
#include <QAxScriptManager>
#include <filespage.h>
#include <tqqueryviewcontroller.h>
#include <tqdebug.h>
#include "queryfields.h"
//#include <Shlwapi.h>
#include <mainproc.h>


struct DetailPages
{
    QString title;
    QIcon icon;
    QWidget *topWidget;
    QWidget *pageWidget;
    QObject *titleObj;
    int tabIndex;
};

class QueryPagePrivate
{
public:
    bool isInteractive;
    bool isDefLoaded;
//	PlanModel *planModel;
//    PlanProxyModel planViewModel;
    TQProxyRecModel *proxy;
    QSortFilterProxyModel *qryFilterModel;
    QString linkField;
    TQHistory history;
    //QList<TrkHistoryItem> history;
    int curHistoryPos;
    QString filterString;
    TTFolder folder;
    bool itIsFolder;
    TQAbstractProject *modelProject;
    QString xqPageFile;
//    QStringList xqList;
    TQRecModel *tmodel;
    QList<QAction*> headerActions;
    QTimer *detailsTimer;
    QTimer *previewTimer;
    QList<DetailPages> pages;
    QMap<int, int> topIndex;
    QMap<int, int> pageIndex;
    TQQueryViewController *controller;
    QueryFields *fieldsPanel;
    TTGlobal *globalObj;
    QList<QAction*> recordActions;
    /*QXmlQuery loadXQ(const QString &filePath)
    {
        QFile xq(filePath);
        xq.open(QIODevice::ReadOnly);

        QXmlQuery query;
        query.setMessageHandler(sysMessager);
        if(globalObj)
            query.setNetworkAccessManager(globalObj->networkManager());

        query.setQuery(&xq);
        return query;
    }
    */
};

QueryPage::QueryPage(QWidget *parent)
    :QWidget(parent), d(new QueryPagePrivate())
{
    d->controller = new TQQueryViewController(this);
    connect(this,SIGNAL(selectedRecordsChanged()),d->controller,SLOT(onSelectedRecordsChanged()));
    connect(d->controller,SIGNAL(detailTabTitleChanged(QWidget*,QString)),this,SLOT(slotTabTitleChanged(QWidget*,QString)));
    QApplication *app = qApp;
    connect(app, SIGNAL(aboutToQuit()), SLOT(onAppQuit()));
    d->globalObj = ttglobal();
    d->modelProject = 0;
    d->tmodel = 0;
    d->itIsFolder = false;
    d->isInteractive = true;
    setupUi(this);
    initWidgets();
    d->proxy = new TQProxyRecModel(this);
    d->qryFilterModel = new QSortFilterProxyModel(this);
    d->qryFilterModel->setSourceModel(d->proxy);
    d->qryFilterModel->setFilterKeyColumn(-1);
    d->qryFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    d->qryFilterModel->setSortRole(Qt::EditRole);
    d->detailsTimer = new QTimer(this);
    d->detailsTimer->setInterval(200);
    d->detailsTimer->setSingleShot(true);
    connect(d->detailsTimer,SIGNAL(timeout()),this,SLOT(updateDetails()));

    d->previewTimer = new QTimer(this);
    d->previewTimer->setInterval(200);
    d->previewTimer->setSingleShot(true);
    connect(d->previewTimer,SIGNAL(timeout()),this,SLOT(slotFilePreview()));

    d->curHistoryPos=0;
    d->isDefLoaded=false;
    d->linkField="Id";
    queryView->setModel(d->qryFilterModel);

    d->fieldsPanel = new QueryFields(this);
    projectTabWidget->addTab(d->fieldsPanel, tr("Поля"));
    d->fieldsPanel->setViewController(d->controller, TQRecord::View);
    projectTabWidget->hide();

    connect(queryView,SIGNAL(activated(const QModelIndex &)),actionTest,SLOT(trigger()));

    QShortcut *shortcut;


    shortcut = new QShortcut(QKeySequence(QKeySequence::InsertParagraphSeparator),filterComboBox,0,0,Qt::WidgetShortcut);
    connect(shortcut,SIGNAL(activated()),this,SLOT(newFilterString()));

    shortcut = new QShortcut(QKeySequence(Qt::Key_Enter),filterComboBox,0,0,Qt::WidgetShortcut);
    connect(shortcut,SIGNAL(activated()),this,SLOT(newFilterString()));

    /*
    filesTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    filesTable->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
    */
    queryView->addAction(actionSelectTrigger);
    queryView->addAction(actionDeleteFromList);
    tabBar->setCurrentIndex(0);
    subViewWidget->hide();
    splitter->setChildrenCollapsible(true);
}

QueryPage::~QueryPage()
{
    delete d;
}

void QueryPage::initWidgets()
{
    tabBar = new QTabBar(tabPlaceWidget);
    tabBar->addTab(QIcon(":/images/file.png"),tr("Текст"));
//    tabBar->addTab(tr("Файлы"));

    QBoxLayout *lay = qobject_cast<QBoxLayout *>(tabPlaceWidget->layout());
    lay->insertWidget(0,tabBar);
//    horizontalSpacer->changeSize(10,20,QSizePolicy::Expanding);
    topStackLay = new QStackedLayout(subViewWidget);

    pageLayout = new QVBoxLayout(pageWithPreview);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setObjectName(QString::fromUtf8("verticalLayout"));


//    filesPage = new FilesPage(subViewWidget);

//    addDetailWidgets(0, filesPage, tr("Файлы"));

    stackedWidget->setCurrentIndex(0);

    connect(tabBar,SIGNAL(currentChanged(int)),this,SLOT(slotTabChanged(int)));
    connect(webView_2->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    connect(webView_2->page(),SIGNAL(unsupportedContent(QNetworkReply*)),
            this,SLOT(slotUnsupportedContent(QNetworkReply*)));
    webView_2->page()->setForwardUnsupportedContent(true);
    webView_2->page()->mainFrame()->setUrl(QUrl("about:blank"));
    ttglobal()->emitViewOpened(this, controller());

    QAction *copyAction = webView_2->pageAction(QWebPage::Copy);
#ifdef Q_WS_WIN
    copyAction->setShortcuts(QList<QKeySequence>()
                             << QKeySequence("Ctrl+Insert")
                             << QKeySequence("Ctrl+C")
                             );
#else
    copyAction->setShortcut(QKeySequence::Copy);
#endif
    webView_2->addAction(copyAction);
}

void QueryPage::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
{
    DetailPages item;
    item.title = title;
    item.icon = icon;
    item.topWidget = 0;
    item.pageWidget = tab;
    d->pages.append(item);
    int index = d->pages.size()-1;
    doAddTab(index);

    /*
    tabBar->addTab(icon, title);
    int i = topStackLay->addWidget(tab);
    */
}

void QueryPage::addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon)
{
    DetailPages item;
    item.title = title;
    item.topWidget = topWidget;
    item.pageWidget = pageWidget;
    d->pages.append(item);
    int index = d->pages.size()-1;
    doAddTab(index);
}

static QMetaMethod notifyMethod(QObject *object, const char *property)
{
    const QMetaObject *meta = object->metaObject();
    int index = meta->indexOfProperty(property);
    QMetaProperty prop = meta->property(index);
    return prop.notifySignal();
}


static QByteArray notifySignature(QObject *object, const char *property)
{
    QMetaMethod method = notifyMethod(object, property);
    return QByteArray(method.signature());
}

static bool isMethodValid(const QMetaMethod &method)
{
    return !QByteArray(method.signature()).isEmpty();
}

void QueryPage::doAddTab(int pageIndex)
{
    if(d->pages.size()<pageIndex+1 || pageIndex<0)
        return;
    DetailPages item = d->pages.value(pageIndex);
    item.titleObj = item.topWidget ? item.topWidget : item.pageWidget;
    if(!item.titleObj)
        return;
    QString title = item.title;
    if(title.isEmpty())
        title = item.titleObj->property("title").toString();
    if(title.isEmpty())
        title = tr("Инфо %1").arg(pageIndex);
    item.tabIndex = tabBar->addTab(item.icon, title);
    if(item.topWidget)
    {
        int i = topStackLay->addWidget(item.topWidget);
        d->topIndex.insert(item.tabIndex, i);
    }
    if(item.pageWidget)
    {
        int i = stackedWidget->addWidget(item.pageWidget);
        d->pageIndex.insert(item.tabIndex,i);
    }
    d->pages[pageIndex] = item;
    QMetaMethod titleSignal = notifyMethod(item.titleObj,"title");
    if(isMethodValid(titleSignal))
    {
        int i = this->metaObject()->indexOfSlot("slotTabTitleChanged()");
        QMetaMethod slot = this->metaObject()->method(i);
        connect(item.titleObj, titleSignal, this, slot);
    }
    /*
    QByteArray titleSignal = notifySignature(item.titleObj,"title");
    if(!titleSignal.isEmpty())
    {
        connect(item.titleObj, titleSignal.constData(), this, SLOT(slotTabTitleChanged()));
    }
    */
}

bool QueryPage::hasMarked()
{
    return !markedRecords().isEmpty();
}

TQAbstractProject *QueryPage::project() const
{
    return d->modelProject;
}

TQViewController *QueryPage::controller()
{
    return d->controller;
}

QAbstractItemView *QueryPage::tableView() const
{
    return queryView;
}

void QueryPage::closeTab(int index)
{
	QWidget *w = projectTabWidget->widget(index);
	if(!w)
		return;
	delete w;
	if(!projectTabWidget->count())
		projectTabWidget->hide();
}

void QueryPage::setQueryModel(TQAbstractProject *prj, TQRecModel *model)
{
    TQAbstractProject *oldPrj = d->modelProject;
    if(d->tmodel)
    {
        if(((QObject*)d->tmodel)->parent() == this)
            d->tmodel->deleteLater();
        d->tmodel->disconnect(this);
        d->tmodel = 0;
    }
    d->tmodel = model;
    d->modelProject = prj;
    d->xqPageFile = ttglobal()->optionDefaultValue(TQOPTION_VIEW_TEMPLATE).toString();
//    d->xqFile = "data/scr.xq";
    if(prj)
    {
//        TQScopeSettings sets(d->modelProject->projectSettings());
        d->xqPageFile = prj->optionValue(TQOPTION_VIEW_TEMPLATE).toString();
//        d->xqFile = sets->value("RecordTemplate", d->xqFile).toString();
    }
    if(!d->tmodel->isSystemModel())
        d->tmodel->setParent(this);
//    d->proxy->setSourceModel(d->tmodel);
    d->qryFilterModel->setSourceModel(d->tmodel);
    loadDefinition();
#ifdef DECORATOR
    fieldEdits.clear();
    decorator->fillEditPanels(this->tabPanels, tmodel->typeDef(),fieldEdits);
#endif
    filterFieldComboBox->clear();
    filterFieldComboBox->addItem(tr("Любое поле"),-1);
    QMap<QString, int> headers;
    for(int col=0; col<d->tmodel->columnCount(); col++)
    {
        headers.insert(d->tmodel->headerData(col, Qt::Horizontal).toString(),col);
    }
    QMapIterator<QString, int> hi(headers);
    while (hi.hasNext()) {
        hi.next();
        filterFieldComboBox->addItem(hi.key(),hi.value());
    }
    filterFieldComboBox->setCurrentIndex(0);
	connect(
		queryView->selectionModel(), 
		SIGNAL(selectionChanged ( const QItemSelection & , const QItemSelection &  )),
		this,
		SLOT(selectionChanged ( const QItemSelection & , const QItemSelection &  )));
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this,
                SLOT(currentChanged ( const QModelIndex & , const QModelIndex &  )));
    connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionCountChanged(int,int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionDoubleClicked (int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionEntered(int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionHandleDoubleClicked(int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionMoved (int, int, int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionPressed (int)),
		this, SLOT(headerChanged()));
	connect(
		queryView->horizontalHeader(),
		SIGNAL(sectionResized (int, int, int)),
		this, SLOT(headerChanged()));
    connect(
        queryView->horizontalHeader(),
        SIGNAL(customContextMenuRequested(QPoint)),
        this, SLOT(showHeaderPopupMenu(QPoint)));
    queryView->selectRow(0);
    queryView->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
//    initPopupMenu();
}

QModelIndex QueryPage::mapIndexToModel(const QModelIndex &index) const
{
    QModelIndex qryIndex;
    const QAbstractItemModel *m=index.model();
    qryIndex = index;
    while(const QAbstractProxyModel *p=qobject_cast<const QAbstractProxyModel *>(m))
    {
        qryIndex = p->mapToSource(qryIndex);
        m=qryIndex.model();
    }
    return qryIndex;
}

/*void QueryPage::changedView(const QModelIndex &index, const QModelIndex &prev)
{
	if(prev.row() == index.row())
		return;
    QModelIndex qryIndex = mapIndexToModel(index);
    if(!qryIndex.isValid())
        return;
#ifdef DECORATOR
    TQRecord *record = tmodel->at(qryIndex.row());
    decorator->readValues(record, fieldEdits);
#endif

    drawNotes(qryIndex);
}*/

void QueryPage::selectionChanged(const QItemSelection & /* selected */, const QItemSelection & /*deselected*/)
{
    d->detailsTimer->start(250);
    emit selectedRecordsChanged();
}

void QueryPage::currentChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    d->detailsTimer->start(250);
    emit selectedRecordsChanged();
}

void QueryPage::slotTabChanged(int index)
{
    if(!index)
    {
        subViewWidget->setVisible(false);
        stackedWidget->setCurrentWidget(pageWithWeb);
        return;
    }
    int topIndex = d->topIndex.value(index,-1);
    int pageIndex = d->pageIndex.value(index, -1);
    if(topIndex >= 0)
    {
        topStackLay->setCurrentIndex(topIndex);
        subViewWidget->setVisible(true);
    }
    else
        subViewWidget->setVisible(false);
    if(pageIndex >= 0)
    {
//        stackedWidget->setCurrentWidget(pageWithPreview);
        stackedWidget->setCurrentIndex(pageIndex);
    }
    else
        stackedWidget->setCurrentWidget(pageWithWeb);
}

void QueryPage::slotTabTitleChanged()
{
    QObject *obj = sender();
    foreach(const DetailPages &item, d->pages)
    {
        if(obj == item.titleObj)
        {
            QVariant v = item.titleObj->property("title");
            QString title = v.toString();
            if(!title.isEmpty())
                tabBar->setTabText(item.tabIndex,title);
            return;
        }
    }
}

void QueryPage::slotTabTitleChanged(QWidget* widget, const QString & newTitle)
{
    foreach(const DetailPages &item, d->pages)
    {
        if(widget == item.titleObj)
        {
            if(!newTitle.isEmpty())
                tabBar->setTabText(item.tabIndex,newTitle);
            return;
        }
    }
}

void QueryPage::populateJavaScriptWindowObject()
{
    QWebFrame *frame = webView_2->page()->mainFrame();
    if(!frame)
        return;
    frame->addToJavaScriptWindowObject("query", this);
    frame->addToJavaScriptWindowObject("view", this);
    frame->addToJavaScriptWindowObject("global", ttglobal());
}

void QueryPage::slotUnsupportedContent(QNetworkReply *reply)
{
    ttglobal()->showError(reply->errorString());
    delete reply;
}

void QueryPage::columnVisibleToggle()
{
}

void QueryPage::showHeaderPopupMenu(const QPoint &pos)
{
    QMenu menu;
    QHeaderView *hv=queryView->horizontalHeader();
    QAction *action;
    action = new QAction(tr("Настройка столбцов..."),&menu);
    connect(action,SIGNAL(triggered()),this,SLOT(execColumnsEditor()));
    menu.addAction(action);
    int section = hv->logicalIndexAt(pos);
    if(section >= 0)
    {
        QString label = hv->model()->headerData(section,Qt::Horizontal).toString().trimmed();
        action = new QAction(tr("Скрыть столбец '%1'").arg(label),&menu);
        action->setProperty("HeaderPos", section);
        connect(action,SIGNAL(triggered()),this,SLOT(headerHide()));
        menu.addAction(action);
    }
    menu.addSeparator();
    QMenu *subMenu = menu.addMenu(tr("Столбцы"));

    QHash<QString, int> fieldPos;
    QStringList fields = recordTypeDef()->fieldNames();
    QStringList labels;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        fieldPos[label] = i;
        labels << label;
    }
    fields.sort();
    foreach(const QString fieldName, fields)
    {
        int section = fieldPos.value(fieldName, -1);
        QAction *action = new QAction(fieldName,&menu);
        action->setCheckable(true);
        action->setChecked(section != -1 && !hv->isSectionHidden(section));
        action->setProperty("HeaderPos", section);
        connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
        subMenu->addAction(action);
    }
    menu.exec(queryView->mapToGlobal(pos));
}

void QueryPage::recordActionTriggered(QAction *action)
{
    if(d->recordActions.contains(action))
        project()->onActionTriggered(controller(), action);

}

void QueryPage::resetPlanFilter()
{
	QItemSelectionModel *sm = queryView->selectionModel();
	emit selectionChanged(sm->selection(), sm->selection());
}

int QueryPage::getColNum(const QString &colname)
{
    for(int i=0; i<d->tmodel->columnCount(); i++)
        if(!colname.compare(d->tmodel->headerData(i,Qt::Horizontal).toString(),
			Qt::CaseInsensitive))
			return i;
	return -1;
}

void QueryPage::headerChanged()
{
    if(d->isInteractive)
    {
        decorator->saveState(this);
    }
}

void QueryPage::initPopupMenu()
{
	QHeaderView *hv=queryView->horizontalHeader();
    QList<QAction *> headerActions;
    QHash<QString, int> fieldPos;
    QStringList fields = recordTypeDef()->fieldNames();
    QStringList labels;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        fieldPos[label] = i;
        labels << label;
    }
    fields.sort();
    foreach(const QString fieldName, fields)
    {
        int pos = fieldPos.value(fieldName, -1);
        QAction *action = new QAction(fieldName,this);
        action->setCheckable(true);
        action->setChecked(pos != -1 && !hv->isSectionHidden(pos));
        action->setProperty("HeaderPos", pos);
        connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
        headerActions.append(action);
    }


/*    QList<QAction *> headerActions;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = projectView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        QAction *action = new QAction(label,this);
        action->setCheckable(true);
        action->setChecked(!hv->isSectionHidden(i));
        action->setProperty("HeaderPos")
        connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
        //hv->addAction(action);
        headerActions.append(action);
    }
    hv->addActions(headerActions);
*/

//    QStringList names = recordTypeDef()->fieldNames();
//    names.sort();
//    foreach(const QString &fieldName, names)
//    {
//        QAction *action = new QAction(fieldName, this);
//        connect(action,SIGNAL(triggered()),this,SLOT(columnVisibleToggle()));
//        if(hv->logicalIndex(0))
//        hv->addAction(action);
//    }

    QAction *action = new QAction(tr("Настройка столбцов..."),this);
    connect(action,SIGNAL(triggered()),this,SLOT(execColumnsEditor()));
//    hv->addAction(action);
    headerActions.append(action);
    hv->addActions(headerActions);
    hv->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void QueryPage::headerToggled(bool checked)
{
	QAction *a = qobject_cast<QAction*>(sender());
	if(a)
	{
        bool ok;
        int i = a->property("HeaderPos").toInt(&ok);
        if(!ok)
            return;
		if(i>=0)
            if(checked)
            {
                queryView->horizontalHeader()->showSection(i);
                queryView->horizontalHeader()->resizeSection(i,queryView->horizontalHeader()->defaultSectionSize());
            }
            else
            {
                queryView->horizontalHeader()->hideSection(i);
            }
	}
    headerChanged();
}

void QueryPage::headerHide()
{
    headerToggled(false);
}

static QString escape(const QString& s)
{
	QString res= s;
	res.replace("&","&amp;");
	res.replace("<","&lt;");
	res.replace(">","&gt;");
	res.replace("\"","&quote;");
	return res;
}

/*
QString QueryPage::makeRecordPage(const QModelIndex &qryIndex, const QString& xqCodeFile)
{
	const TQRecModel *model = qobject_cast<const TQRecModel *>(qryIndex.model());
	if(!model)
		return "";
	QDomDocument xml = model->recordXml(qryIndex.row());

#ifdef QT_DEBUG
    QFile testXml("!testRec.xml");
	testXml.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream textOut(&testXml);
	xml.save(textOut,4);
#endif

	QFile xq(xqCodeFile);
	xq.open(QIODevice::ReadOnly);

	QXmlQuery query;
    query.setMessageHandler(sysMessager);

	QString page;
	QByteArray ba=xml.toByteArray();
	QBuffer buf;
	buf.setData(ba);
	buf.open(QIODevice::ReadOnly);
	query.bindVariable("scrdoc",&buf);
	query.setQuery(&xq);
	query.evaluateTo(&page);
	return page;
	
}

QString QueryPage::makeRecordsPage(const QObjectList &records, const QString &xqCodeFile)
{
    QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    foreach(QObject *obj, records)
    {
        TQRecord *rec = qobject_cast<TQRecord *>(obj);
        if(!rec)
            continue;
        rec->refresh();
        QDomDocument recxml = rec->toXML();
        root.appendChild(recxml);
    }

#ifdef QT_DEBUG
    QFile testXml("!testRec.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);
#endif


    QFile xq(xqCodeFile);
    xq.open(QIODevice::ReadOnly);
    QFile trackerXML(project()->optionValue(TQOPTION_GROUP_FIELDS).toString()); // "data/tracker.xml");
    trackerXML.open(QIODevice::ReadOnly);
    QXmlQuery query;
    query.setMessageHandler(sysMessager);


    QString page;
    //QString src=xml.toString();
    QByteArray ba=xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    query.bindVariable("scrs",&buf);
    query.bindVariable("def",&trackerXML);
    query.setQuery(&xq);
    //query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
    query.evaluateTo(&page);
#ifdef QT_DEBUG
    QFile testRes("!testResult.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    return page;

}
*/
/*
QString QueryPage::makeRecordPage(const TQRecord *record, QXmlQuery xquery)
{
    QDomDocument xml = record->toXML();

#ifdef QT_DEBUG
    QFile testXml("!testRec.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);
#endif

    QString page;
    QByteArray ba = xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    xquery.bindVariable("scrdoc",&buf);
    xquery.evaluateTo(&page);
    return page;
}
*/

#define XQGLOBAL

QString QueryPage::makeRecordsPage(const QObjectList &records, const QString &xqCodePath)
{
    QXmlQuery xquery;
    QString page;
#ifdef XQGLOBAL
    page = ttglobal()->mainProc()->makeXmlQuery(&xquery, xqCodePath, records);

#else
    QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    TQRecord *firstRecord = 0;
    foreach(QObject *obj, records)
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

#ifdef QT_DEBUG
    QFile testXml("!testRec.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);
#endif

    QFile groupXML(project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
    bool isGroupXMLOpened = groupXML.open(QIODevice::ReadOnly);

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

    xquery.setMessageHandler(mainProc->messageHandler);
    if(d->globalObj)
        xquery.setNetworkAccessManager(d->globalObj->networkManager());

    xquery.bindVariable("scrs",&bufAll);
    if(isGroupXMLOpened)
        xquery.bindVariable("def",&groupXML);
    xquery.bindVariable("scrdoc",&bufSingle);
    QFile xq(xqCodePath);
    xq.open(QIODevice::ReadOnly);
    xquery.setQuery(&xq);

    xquery.evaluateTo(&page);

#endif
    return page;
}

void QueryPage::drawNotes()
{
    QUrl baseUrl = QUrl::fromUserInput(d->xqPageFile);
    QString page;
    TQRecord *record = currentRecord();
//    page = makeRecordPage(qryIndex, d->xqPageFile);

//    const TQRecModel *model = qobject_cast<const TQRecModel *>(qryIndex.model());
    if(record)
    {
        if(d->detailsTimer->isActive())
            return;
        page = makeRecordsPage(QObjectList() << record, d->xqPageFile);
    }
#ifdef TQ_QT_DEBUG
    QFile testRes("!testResult.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    if(d->detailsTimer->isActive())
        return;
    webView_2->setHtml(page, baseUrl);
}

void QueryPage::sendEmail(const QObjectList &records)
{
    if(!records.count())
        return;
    QString page = makeRecordsPage(records,"data/email.xq");
#ifdef QT_DEBUG
    return;
#endif
	QAxObject app;
	app.setControl("Outlook.Application");
	if(app.isNull())
	{
		QMessageBox::critical(this,tr("Ошибка"),
			tr("Нет доступа к Outlook"));
		return;
	}
	QVariant itemType = app.property("olMailItem");
	if(!itemType.isValid())
		itemType = 0;
	QAxObject *mail = app.querySubObject("CreateItem(QVariant)",itemType);
	if(!mail)
	{
		QMessageBox::critical(this,tr("Ошибка"),
			tr("Не создается письмо в Outlook"));
		return;
	}
	QVariant format = app.property("olFormatHTML");
	if(!format.isValid())
		format=2;
	mail->setProperty("BodyFormat",format);
	mail->setProperty("HTMLBody",page);
    QString subject;
    if(records.count()==1)
    {
        TQRecord *rec = qobject_cast<TQRecord *>(records[0]);
        subject = QString(tr("Запрос %1. %2")).arg((int)(rec->recordId())).arg(rec->value("Title").toString());
    }
    else
    {
        subject = tr("Запросы ");
        for(int i=0; i<records.count(); i++)
        {
            const TQRecord *rec = qobject_cast<TQRecord *>(records[i]);
            if(i)
                subject += ", ";
            subject += QString("%1").arg(rec->recordId());
        }
    }
    mail->setProperty("Subject", subject);

	mail->dynamicCall("Display(QVariant)", true);
}

/*
void QueryPage::sendEmail3(const QObjectList &records)
{
    if(!records.count())
        return;
    QString json = "[";
    int index=0;
    foreach(QObject *obj, records)
    {
        TQRecord *rec = qobject_cast<TQRecord *>(obj);
        if(index++)
            json += ", ";
        json += rec->toJSON();
    }
    json += "]";

    QAxScriptManager *man = ttglobal()->newAxScriptManager();
    QString scriptFileName = project()->optionValue(TQOPTION_EMAIL_SCRIPT).toString();
    QAxScript * script = man->load(scriptFileName,"email");
//    QAxScript * script = man->load("C:/gits/traquera/client/data/email3.js","email");
//    QStringList funcs = script->functions();
//    script->call("test", QList<QVariant>());
    delete man;
}
*/

void QueryPage::sendEmail2(const QObjectList &records)
{
    if(!records.count())
        return;
    TQRecord *rec = qobject_cast<TQRecord*>(records[0]);
    if(!rec)
        return;
    TQAbstractProject *prj = rec->project();
    QString fileName = prj->optionValue(TQOPTION_EMAIL_SCRIPT).toString();
    if(fileName.isEmpty())
        fileName = "data/email.js";
    QScopedPointer<QScriptEngine>engine(ttglobal()->newScriptEngine());
    QScriptEngineDebugger debugger;
    debugger.attachTo(engine.data());
    QFile js(fileName);
    js.open(QFile::ReadOnly);
    QByteArray buf = js.readAll();
    QString script = QString::fromUtf8(buf.constData());
    QScriptValue recs = engine->toScriptValue(records);
    engine->globalObject().setProperty("records", recs);
    QScriptValue qObj = engine->newQObject(this);
    engine->globalObject().setProperty("query", qObj);
    engine->evaluate(script,fileName);
}

void QueryPage::loadDefinition()
{
    decorator->loadViewDef(this);
    d->isDefLoaded = true;
}

/*
void QueryPage::recordOpen(const QModelIndex & index)
{
	if(!index.isValid())
		return;
    TQRecord *rec = currentRecord();
    if(!rec)
        return;
    ScrWidget *scr = new ScrWidget(rec->typeDef());
	scr->setRecord(rec);
	scr->show();
}
*/

void QueryPage::openQuery(TQAbstractProject *prj, const QString &queryName, int recType)
{
    d->itIsFolder = false;
    TQRecModel *newmodel = prj->openQuery(queryName, recType);
	if(!newmodel)
		return;
    setQueryModel(prj,newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();
    emit changedQuery(prj->projectName(), queryName);
    tqInfo() << tr("Открыта выборка \"%1\"").arg(queryName);
}

void QueryPage::openIds(TQAbstractProject *prj, const QString &ids, const QString &title, int recType)
{
    d->itIsFolder = false;
    //QList<int> idlist = toIntList(ids);
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->query(ids, recType));
	if(!newmodel)
		return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();


    QString newTitle;
    if(title.isEmpty())
        newTitle = ids;
    else
        newTitle = title;
    emit changedQuery(prj->projectName(), newTitle);
    tqInfo() << tr("Открыт список запросов %1").arg(ids);
}

void QueryPage::openIds(TQAbstractProject *prj, const QList<int> &idlist, const QString &title, int recType)
{
    d->itIsFolder = false;
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->queryIds(idlist, recType));
    if(!newmodel)
        return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();

    QString newTitle;
    if(title.isEmpty())
        newTitle = intListToString(idlist);
    else
        newTitle = title;
    emit changedQuery(prj->projectName(), newTitle);
    tqInfo() << tr("Открыт список запросов %1").arg(intListToString(idlist));
}

void QueryPage::openModel(TQAbstractProject *prj, QAbstractItemModel *newModel)
{
    setQueryModel(prj, qobject_cast<TQRecModel *>(newModel));
    emit openingModel(newModel);
    emit modelChanged(newModel);
    addHistoryPoint();
}

void QueryPage::openFolder(TQAbstractProject *prj, const TTFolder &afolder, int recType)
{
    QList<int> idlist = afolder.folderContent();
    d->itIsFolder = true;
    d->folder = afolder;
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->queryIds(idlist, recType, false));
    if(!newmodel)
        return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    QString ids = intListToString(idlist);
    emit changedQuery(prj->projectName(), ids);
    addHistoryPoint();
    tqInfo() << tr("Открыта папка \"%1\"").arg(afolder.title);

}

void QueryPage::openQuery(const QString &projectName, const QString &queryName, int recType)
{
    TQAbstractProject *prj = TQAbstractDB::getProject(projectName);
	if(!prj)
		return;
    openQuery(prj,queryName,recType);
}

void QueryPage::openHistoryItem(int pos)
{

    if(pos<0 || pos>=d->history.rowCount())
		return;
    const TQHistoryItem &item = d->history[pos];
	if(item.isQuery)
	{
        TQAbstractProject *prj = TQAbstractDB::getProject(item.projectName);
        if(!prj)
			return;
        TQRecModel *newmodel = prj->openQuery(item.queryName, item.rectype);
		if(!newmodel)
			return;
        setQueryModel(prj, newmodel);
        appendIds(stringToIntList(item.addedIds));
        removeIds(stringToIntList(item.removedIds));
        emit changedQuery(item.projectName, item.queryName);
        d->curHistoryPos = pos;
	}
	else
	{
        TQAbstractProject *prj = TQAbstractDB::getProject(item.projectName);
		if(!prj)
			return;
        TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->queryIds(stringToIntList(item.queryName), item.rectype));
		if(!newmodel)
			return;
        setQueryModel(prj, newmodel);
        appendIds(stringToIntList(item.addedIds));
        removeIds(stringToIntList(item.removedIds));
        emit changedQuery(item.projectName, item.queryName);
        d->curHistoryPos = pos;
	}
}

void QueryPage::goBack()
{
    openHistoryItem(d->curHistoryPos-1);
}

void QueryPage::goForward()
{
    openHistoryItem(d->curHistoryPos+1);
}

void QueryPage::printPreview()
{
    QObjectList records = selectedRecords();
    if(records.isEmpty())
        return;
    QString xqfile = project()->optionValue(TQOPTION_PRINT_TEMPLATE).toString();
    QString page = makeRecordsPage(records, xqfile);
    QWebView web(this);
    QUrl baseUrl = QUrl::fromUserInput(xqfile);
    web.setHtml(page, baseUrl);
#ifdef QT_DEBUG
    QFile testRes("!testPrint.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    QPrintPreviewDialog ppd;
    connect(&ppd,SIGNAL(paintRequested(QPrinter*)),&web,SLOT(print(QPrinter*)));
    ppd.exec();

}


TQRecord *QueryPage::recordOnIndex(const QModelIndex &index)
{
    QModelIndex f = index;
    const QAbstractItemModel *model = f.model();
    while(const QAbstractProxyModel *proxy =  qobject_cast<const QAbstractProxyModel *>(model))
    {
        f = proxy->mapToSource(f);
        model = f.model();
    }
    const TQRecModel *trkmodel = qobject_cast<const TQRecModel *>(model);
    if(!trkmodel)
        return 0;
    return trkmodel->recordInRow(f.row());
}

const TQAbstractRecordTypeDef *QueryPage::recordTypeDef() const
{
    if(!d->tmodel)
        return 0;
    return d->tmodel->typeDef();
}

int QueryPage::recordType() const
{
    if(!d->tmodel)
        return 0;
    const TQAbstractRecordTypeDef *recDef = d->tmodel->typeDef();
    if(!recDef)
        return 0;
    return recDef->recordType();
}

void QueryPage::setRecordsChecked(const QString &ids, bool flag)
{
    QList<int> nums = stringToIntList(ids);

    foreach(int id, nums)
        setIdChecked(id, flag);
}

QModelIndexList QueryPage::selectedRows()
{
    QItemSelectionModel *is=queryView->selectionModel();
    return is->selectedRows();
}

QObjectList QueryPage::selectedRecords()
{
    QObjectList list;
    QModelIndexList ii = selectedRows();
    foreach(QModelIndex f, ii)
    {
        TQRecord *rec = recordOnIndex(f);
        if(!rec)
            continue;
        list.append(rec);
    }
    return list;
}

TQRecordList QueryPage::selectedTQRecords()
{
    TQRecordList list;
    QModelIndexList ii = selectedRows();
    foreach(QModelIndex f, ii)
    {
        TQRecord *rec = recordOnIndex(f);
        if(!rec)
            continue;
        list.append(rec);
    }
    return list;
}

QList<int> QueryPage::selectedIds()
{
    QList<int> list;
    QModelIndexList ii = selectedRows();
    foreach(QModelIndex f, ii)
    {
        TQRecord *rec = recordOnIndex(f);
        if(!rec)
            continue;
        list.append(rec->recordId());
    }
    return list;
}

QObjectList QueryPage::allRecords()
{
    QObjectList list;
    for(int r=0; r<queryView->model()->rowCount(); r++)
    {
        QModelIndex index = queryView->model()->index(r,0);
        TQRecord *rec = recordOnIndex(index);
        list << rec;
    }
    return list;
}

QObjectList QueryPage::markedRecords()
{
    QObjectList list;
    for(int r=0; r<queryView->model()->rowCount(); r++)
    {
        QModelIndex index = queryView->model()->index(r,0);
        TQRecord *rec = recordOnIndex(index);
        if(rec->isSelected())
            list << rec;
    }
    return list;
}

//TQAbstractProject *QueryPage::currentProject()
//{
//    return d->modelProject;
//}

TQRecord *QueryPage::currentRecord()
{
    QModelIndex cur = queryView->currentIndex();
    if(!cur.isValid())
        return NULL;
    if(!queryView->selectionModel()->isSelected(cur))
        return NULL;
    const QAbstractItemModel *model = cur.model();
    while(const QAbstractProxyModel *proxy =  qobject_cast<const QAbstractProxyModel *>(model))
    {
        cur = proxy->mapToSource(cur);
        model = cur.model();
    }
    const TQRecModel *trkmodel = qobject_cast<const TQRecModel *>(model);
    if(!trkmodel)
        return NULL;
    return trkmodel->recordInRow(cur.row());
}

void QueryPage::on_actionAdd_Note_triggered()
{
    NoteDialog *nd=new NoteDialog();
    const TQAbstractRecordTypeDef *def = d->tmodel->typeDef();
    nd->titleEdit->addItems(def->noteTitleList());
//    nd->titleEdit->addItems(((const TQAbstractRecordTypeDef*)tmodel->typeDef())->project()->noteTitleList());
    QSettings *settings = ttglobal()->settings();
    if(settings->contains("LastNote"))
        nd->titleEdit->setEditText(settings->value("LastNote").toString());
    if(nd->exec())
    {
        settings->setValue("LastNote",nd->titleEdit->currentText());
        QObjectList records = selectedRecords();
        TQRecord *rec;
        QList<int> successIds, rejectIds;
        foreach(QObject *obj, records)
        {
            rec = qobject_cast<TQRecord *>(obj);
            if(!rec)
                continue;
            int id = rec->recordId();
            if(rec->updateBegin())
            {
                if(rec->addNote(nd->titleEdit->currentText(), nd->noteEdit->toPlainText()) != -1 && rec->commit())
                    successIds << id;
                else
                {
                    rec->cancel();
                    rejectIds << id;
                }
            }
            else
            {
                rec->cancel();
                rejectIds << id;
            }
        }
        if(successIds.size())
            tqInfo() << tr("В запрос(-ы) %1 добавлен комментарий \"%2\":\n%3")
                        .arg(intListToString(successIds), nd->titleEdit->currentText(), nd->noteEdit->toPlainText());
        if(rejectIds.size())
            tqInfo() << tr("В запрос(-ы) %1 не удалось добавить комментарий \"%2\":\n%3")
                        .arg(intListToString(rejectIds), nd->titleEdit->currentText(), nd->noteEdit->toPlainText());
    }
    delete nd;
}

static QRegExp stringToRegExp(const QString &s)
{
    QString res;
    QStringList list = s.split(",");
    foreach(QString str, list)
    {
        str = str.trimmed();
        if(str.isEmpty())
            continue;
        if(!res.isEmpty())
            res += '|';
        res += QRegExp::escape(str);
    }
    return QRegExp("("+res+")");
}

void QueryPage::newFilterString()
{
    newFilterString(filterComboBox->currentText().trimmed());
}

void QueryPage::newFilterString(const QString &text)
{
    if(text == d->filterString)
        return;
    d->filterString = text;
    if(text.isEmpty())
    {
        //actionFilter->setChecked(false);
        d->qryFilterModel->setFilterFixedString("");
        if(filterButton->isChecked())
            filterButton->setChecked(false);
        return;
    }
    int index = filterComboBox->findText(text, static_cast<Qt::MatchFlags> ( Qt::MatchExactly ));
    if(index == -1)
        filterComboBox->insertItem(0,text);
    else
        filterComboBox->setItemText(index,text);
    if(!filterButton->isChecked())
        filterButton->setChecked(true);
    if(text.contains(','))
        d->qryFilterModel->setFilterRegExp(stringToRegExp(text));
    else
        d->qryFilterModel->setFilterFixedString(text);
}

void QueryPage::appendId(int id)
{
    appendIds(QList<int>() << id);
}

void QueryPage::appendIds(const QList<int> &ids)
{
    d->tmodel->appendRecordIds(ids);
    updateHistoryPoint();
    tqInfo() << tr("Добавлены в список запросы %1").arg((intListToString(ids)));
}

void QueryPage::removeIds(const QList<int> &ids)
{
    d->tmodel->removeRecordIds(ids);
    updateHistoryPoint();
    tqInfo() << tr("Удалены из списка запросы %1").arg((intListToString(ids)));
}

void QueryPage::setIdChecked(int id, bool checked)
{
    int row = d->tmodel->rowOfRecordId(id);
    Qt::CheckState state = checked ? Qt::Checked : Qt::Unchecked;
    d->tmodel->setData(d->tmodel->index(row,d->tmodel->idColumn()), state, Qt::CheckStateRole);
}

void QueryPage::refreshQuery()
{
    if(d->tmodel)
    {
        d->tmodel->refreshQuery();
    }
}

void QueryPage::openRecordId(int id)
{
    TQRecord *rec = d->tmodel->project()->createRecordById(id,d->tmodel->project()->defaultRecType());
    if(rec)
    {
        TTRecordWindow *win = new TTRecordWindow();
//        win->setRecordTypeDef(rec->typeDef());
        win->setRecord(rec);
        win->show();
    }
    tqInfo() << tr("Открыт запрос %1").arg((intListToString(QList<int>() << id)));
}

/*
void QueryPage::previewFile(const QString &filePath)
{
    stackedWidget->setCurrentWidget(pageWithPreview);
    previewWidget->setSourceFile(filePath);
}
*/

void QueryPage::popupScrMenu(int id)
{
//    QMenu menu;
//    QAction
    //    menu.addAction(tr("Добавить #$1 в список").arg(id),this,SLOT(appendId(int))
}

void QueryPage::execColumnsEditor()
{
    QHeaderView *hv=queryView->horizontalHeader();
    QScopedPointer<TQColsDialog> dlg(new TQColsDialog(this));
    dlg->exec(hv);
}

void QueryPage::on_actionFilter_toggled(bool arg1)
{
    if(arg1)
        newFilterString();
    else
        newFilterString("");
}

void QueryPage::on_filterFieldComboBox_currentIndexChanged(int index)
{
    if(index == 0 || index == -1)
        d->qryFilterModel->setFilterKeyColumn(-1);
    else
        d->qryFilterModel->setFilterKeyColumn(filterFieldComboBox->itemData(index).toInt());
}


extern void notesDrawning(QueryPage *page)
{
    page->drawNotes();
}

void QueryPage::updateDetails()
{
    //int linkCol=getColNum(linkField);
    d->detailsTimer->stop();
    QStringList keys;
    QObjectList selected = selectedRecords();
    foreach(QObject *obj, selected)
    {
        TQRecord *rec = qobject_cast<TQRecord *>(obj);
        QString key = QString::number(rec->recordId());
        if(!key.isEmpty())
            keys.append(key);
    }
//    QModelIndex qryIndex = mapIndexToModel(queryView->currentIndex());
//    if(!qryIndex.isValid())
//        return;
//    const TQAbstractRecordTypeDef *def = record->typeDef();
    TQRecord *record = currentRecord(); // d->tmodel->at(qryIndex.row());
#ifdef DECORATOR
    if(record)
        decorator->readValues(record, fieldEdits);
#endif
    if(d->detailsTimer->isActive())
        return;
    QueryPage *page = this;
//    QFuture<void> future = QtConcurrent::run(notesDrawning, page);
    drawNotes();


//    filesPage->setRecord(record);
    d->controller->emitCurrentRecordChanged(record);

    /*
    QList<TQToolFile> files = record->fileList();
    filesTable->clearContents();
    filesTable->setRowCount(0);
    TTFileIconProvider prov;
    int row=0;
    foreach(const TQToolFile &file, files)
    {
        QFileInfo f(file.fileName);
        filesTable->insertRow(row);
        filesTable->setItem(row,0,new QTableWidgetItem(prov.icon(f),f.fileName()));
        filesTable->setItem(row,1,new QTableWidgetItem(file.createDateTime.toString(def->dateTimeFormat())));
        filesTable->setItem(row,2,new QTableWidgetItem(f.dir().path()));
        row++;
    }
    */
    //emit selectionRecordsChanged(selectedRecords());
}

void QueryPage::on_actionCopyId_triggered()
{
    QStringList numList;
    QString numbers;
    foreach(QObject *obj, selectedRecords())
    {
        const TQRecord *rec = (TQRecord *)obj;
        int id = rec->recordId();
        numList.append(QString::number(id));
    }
    if(numList.isEmpty())
        return;
    numbers = numList.join(", ");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(numbers);
}

void QueryPage::on_actionCopyTable_triggered()
{
    QHeaderView *hv=queryView->horizontalHeader();
    QHash<int, QString> titles;
    //QStringList labels;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        if(!hv->isSectionHidden(i))
            titles[hv->visualIndex(i)] = label;
    }
    QStringList labels;
    foreach(const QString &label, titles)
        labels << label;
    QString html = copyRecordsToHTMLTable(selectedRecords(), labels);
    QMimeData *data = new QMimeData();
    data->setHtml(html);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(data);
}

void QueryPage::on_actionCopyRecords_triggered()
{
    QObjectList records = selectedRecords();
    if(records.isEmpty())
        return;
    QString page = makeRecordsPage(records,"data/print.xq");
    QMimeData *data = new QMimeData();
    data->setHtml(page);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(data);
}

void QueryPage::on_actionSelectRecords_triggered()
{
    foreach(QObject *rec, selectedRecords())
    {
        ((TQRecord *)rec)->setSelected(true);
    }
}

void QueryPage::on_actionDeselectRecords_triggered()
{
    foreach(QObject *obj, selectedRecords())
    {
        TQRecord *rec = (TQRecord *)obj;
        rec->setSelected(false);
    }
}

void QueryPage::on_actionCopyMarkedId_triggered()
{
    QStringList numList;
    QString numbers;
    foreach(QObject *obj, markedRecords())
    {
        TQRecord *rec = (TQRecord *)obj;
        int id = rec->recordId();
        numList.append(QString::number(id));
    }
    if(numList.isEmpty())
        return;
    numbers = numList.join(", ");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(numbers);
}

void QueryPage::on_actionCopyMarkedTable_triggered()
{
    QHeaderView *hv=queryView->horizontalHeader();
    QHash<int, QString> titles;
    //QStringList labels;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        if(!hv->isSectionHidden(i))
            titles[hv->visualIndex(i)] = label;
    }
    QStringList labels;
    foreach(const QString &label, titles)
        labels << label;
    QString html = copyRecordsToHTMLTable(markedRecords(), labels);
    QMimeData *data = new QMimeData();
    data->setHtml(html);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(data);
}


void QueryPage::on_actionCopyMarkedRecords_triggered()
{
    QObjectList records = markedRecords();
    if(records.isEmpty())
        return;
    QString page = makeRecordsPage(records,"data/print.xq");
    QMimeData *data = new QMimeData();
    data->setHtml(page);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setMimeData(data);
}

void QueryPage::on_actionDeleteMarked_triggered()
{
    QList<int> ids;
    foreach(QObject *obj, markedRecords())
    {
        TQRecord *rec = qobject_cast<TQRecord*>(obj);
        if(rec)
            ids << rec->recordId();
    }
    removeIds(ids);
}

void QueryPage::on_actionSelectMarked_triggered()
{

}

void QueryPage::on_actionDeleteFromList_triggered()
{
    QList<int> ids;
    foreach(QObject *obj, selectedRecords())
    {
        TQRecord *rec = qobject_cast<TQRecord*>(obj);
        if(rec)
            ids << rec->recordId();
    }
    removeIds(ids);
}

void QueryPage::on_queryView_customContextMenuRequested(const QPoint &pos)
{
    // Контекстое меню списка запросов
    //queryView->setContextMenuPolicy(Qt::CustomContextMenu);
    QMenu menu;
    menu.addAction(actionCopyId);
    menu.addAction(actionCopyTable);
    menu.addAction(actionCopyRecords);
    menu.addAction(actionSelectRecords);
    menu.addAction(actionDeselectRecords);
    if(hasMarked())
    {
        menu.addSeparator();
        QMenu *subMenu = menu.addMenu(tr("Отмеченные запросы"));
        subMenu->addAction(actionCopyMarkedId);
        subMenu->addAction(actionCopyMarkedTable);
        subMenu->addAction(actionCopyMarkedRecords);
        subMenu->addSeparator();
        subMenu->addAction(actionDeleteMarked);
    }
    menu.addSeparator();
    menu.addAction(actionDeleteFromList);
    if(d->itIsFolder)
    {
        menu.addSeparator();
        menu.addAction(actionDeleteFromFolder);
    }
    menu.addSeparator();
    //queryView->addAction(actionCopyTable);
    menu.addAction(actionAdd_Note);
    menu.addAction(actionTest);

    d->recordActions.clear();
    TQRecordList recs = selectedTQRecords();
    QList<QAction*> actions = project()->actions(recs);
    if(!actions.isEmpty())
    {
        menu.addSeparator();
        foreach(QAction *a, actions)
        {
            menu.addAction(a);
            d->recordActions.append(a);
        }
        connect(&menu, SIGNAL(triggered(QAction*)), SLOT(recordActionTriggered(QAction*)));
    }
    menu.exec(queryView->mapToGlobal(pos));
}

void QueryPage::on_actionDeleteFromFolder_triggered()
{
    if(!d->itIsFolder)
        return;
    QList<int> ids;
    foreach(QObject *obj, selectedRecords())
    {
        TQRecord *rec = qobject_cast<TQRecord*>(obj);
        if(rec)
            ids << rec->recordId();
    }
    removeIds(ids);
    foreach(int id, ids)
        d->folder.deleteRecordId(id);
}

void QueryPage::openFileSystem(const QModelIndex &index)
{
    /*
    QModelIndex fileIndex = index;
    if(!fileIndex.isValid())
        fileIndex = filesTable->currentIndex();
    d->previewTimer->stop();
    TQRecord *rec = recordOnIndex(queryView->currentIndex());
    if(!rec || !fileIndex.isValid())
        return;
    QString fileName = filesTable->item(fileIndex.row(),0)->text();
    QString tempFile = QDir::temp().absoluteFilePath(fileName);
    rec->saveFile(fileIndex.row(), tempFile);
    tempFile = QDir::toNativeSeparators(tempFile);
    QUrl url = QUrl::fromLocalFile(tempFile);
    QDesktopServices::openUrl(url);
    */
}

void QueryPage::slotFilesTable_pressed(const QModelIndex &index)
{
    d->previewTimer->start(100);
}

void QueryPage::slotFilePreview()
{
    /*
    QModelIndex recIndex = queryView->currentIndex();
    QTableWidgetItem *fileItem = filesTable->currentItem();
    if(recIndex.isValid() && fileItem && fileItem->isSelected())
    {
        TQRecord *rec = recordOnIndex(recIndex);
        if(rec)
        {
            QString fileName = fileItem->text();
            QString tempFile = QDir::temp().absoluteFilePath(fileName);
            if(rec->saveFile(fileItem->row(), tempFile))
                previewFile(tempFile);
            return;
        }
    }
    previewWidget->clear();
    */
}

void QueryPage::slotCurrentFileChanged()
{
    /*
    QTableWidgetItem *fileItem = filesTable->currentItem();
    if(!fileItem || !fileItem->isSelected())
        previewWidget->clear();
        */
}

void QueryPage::on_actionTest_triggered()
{
    TQRecord *rec = currentRecord();
    if(!rec)
        return;
    int id = rec->recordId();
    openRecordId(id);
}

void QueryPage::on_actionSelectTrigger_triggered()
{
    foreach(QObject *obj, selectedRecords())
    {
        TQRecord * rec = (TQRecord *)obj;
        rec->setSelected(!rec->isSelected());
    }
    queryView->selectRow(queryView->currentIndex().row()+1);
}

void QueryPage::addHistoryPoint()
{
    TQHistoryItem item;
    item.projectName = d->modelProject->projectName();
    if(d->tmodel)
    {
        item.isQuery = d->tmodel->isQueryType();
        item.queryName = d->tmodel->queryName();
        item.rectype = d->tmodel->recordType();
        item.foundIds = intListToString(d->tmodel->getIdList());
        item.addedIds = intListToString(d->tmodel->addedIdList());
        item.removedIds = intListToString(d->tmodel->deletedIdList());
    }
    d->history.append(item);
    d->curHistoryPos = d->history.rowCount()-1;
}

void QueryPage::updateHistoryPoint()
{
    if(!d->history.rowCount())
        addHistoryPoint();
    else
    {
        int r = d->history.rowCount()-1;
        TQHistoryItem item = d->history.at(r);
        item.projectName = d->modelProject->projectName();
        if(d->tmodel)
        {
            item.addedIds = intListToString(d->tmodel->addedIdList());
            item.removedIds = intListToString(d->tmodel->deletedIdList());
        }
        d->history.removeRow(r);
        d->history.append(item);
    }
}

void QueryPage::on_toolButton_clicked()
{
    QString newFile = QFileDialog::getOpenFileName(this,
                                 tr("Выберите файл шаблона страницы"),
                                 d->xqPageFile,
                                 tr("XQuery (*.xq);;Все файлы (*.*)"));
    if(newFile.isEmpty())
        return;
    d->xqPageFile = newFile;
    updateDetails();
}

void QueryPage::onAppQuit()
{
    d->detailsTimer->stop();
    d->previewTimer->stop();
}
