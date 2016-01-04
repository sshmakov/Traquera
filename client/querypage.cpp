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
#include "scrwidg.h"
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
//#include <Shlwapi.h>

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
    QString xqFile;
    TQRecModel *tmodel;
    QList<QAction*> headerActions;
    QTimer *detailsTimer;
    QTimer *previewTimer;
};

QueryPage::QueryPage(QWidget *parent)
    :QWidget(parent), d(new QueryPagePrivate())
{
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
    d->detailsTimer->setInterval(0);
    d->detailsTimer->setSingleShot(true);
    connect(d->detailsTimer,SIGNAL(timeout()),this,SLOT(updateDetails()));

    d->previewTimer = new QTimer(this);
    d->previewTimer->setInterval(0);
    d->previewTimer->setSingleShot(true);
    connect(d->previewTimer,SIGNAL(timeout()),this,SLOT(slotFilePreview()));

    QVBoxLayout * v = new QVBoxLayout(pageWithPreview);
    v->setContentsMargins(0, 0, 0, 0);
    v->setObjectName(QString::fromUtf8("verticalLayout"));

    previewWidget = new MasterPreview(pageWithPreview);
    previewWidget->setObjectName("previewWidget");
    v->addWidget(previewWidget);

    stackedWidget->setCurrentIndex(0);

//	planModel=NULL;
    d->curHistoryPos=0;
    d->isDefLoaded=false;
	//isInteractive=false;
    d->linkField="Id";
    queryView->setModel(d->qryFilterModel);

    projectTabWidget->hide();
    /* to plugin
    projectTabWidget->hide();
    planTreeView->setModel(&planViewModel);
	connect(projectTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));

	QAction *a;
    a = new QAction(QString::fromLocal8Bit("Добавить в план"),this);
	connect(a,SIGNAL(triggered(bool)),this,SLOT(addScrTasks()));
	planTreeView->addAction(a);

    planTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(planTreeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slotPlanContextMenuRequested(QPoint)));
    */

    //connect(queryView,SIGNAL(activated(const QModelIndex &)),this,SLOT(recordOpen(const QModelIndex &)));
    connect(queryView,SIGNAL(activated(const QModelIndex &)),actionTest,SLOT(trigger()));
//    connect(this, SIGNAL(openingModel(const QAbstractItemModel*)),&history,SLOT(openedModel(const QAbstractItemModel *)),Qt::DirectConnection);

    QShortcut *shortcut;


    shortcut = new QShortcut(QKeySequence(QKeySequence::InsertParagraphSeparator),filterComboBox,0,0,Qt::WidgetShortcut);
    connect(shortcut,SIGNAL(activated()),this,SLOT(newFilterString()));

    shortcut = new QShortcut(QKeySequence(Qt::Key_Enter),filterComboBox,0,0,Qt::WidgetShortcut);
    connect(shortcut,SIGNAL(activated()),this,SLOT(newFilterString()));

    //connect(filterComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(newFilterString(QString)));
    filesTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    filesTable->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);
    queryView->addAction(actionSelectTrigger);
    queryView->addAction(actionDeleteFromList);
    tabBar->setCurrentIndex(0);
    subViewWidget->hide();
    splitter->setChildrenCollapsible(true);

}

QueryPage::~QueryPage()
{
    delete d;
//    QSettings *settings = ttglobal()->settings();
//    settings->setValue(Settings_ScrPlanView,planTreeView->header()->saveState());
}

void QueryPage::initWidgets()
{
    tabBar = new QTabBar(tabPlaceWidget);
    tabBar->addTab(QIcon(":/images/file.png"),tr("Текст"));
    //tabBar->addTab(QIcon(":/images/plan.png"),tr("Планы"));
    tabBar->addTab(tr("Файлы"));

    QBoxLayout *lay = qobject_cast<QBoxLayout *>(tabPlaceWidget->layout());
    lay->insertWidget(0,tabBar);
    horizontalSpacer->changeSize(10,20,QSizePolicy::Expanding);
    subLay = new QStackedLayout(subViewWidget);

//    planTreeView = new QTreeView(subViewWidget);
//    planTreeView->setObjectName(QString::fromUtf8("planTreeView"));
//    planTreeView->setSelectionMode(QAbstractItemView::ContiguousSelection);
//    planTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
//    planTreeView->setIndentation(5);
//    subLay->addWidget(planTreeView);

    filesTable = new QTableWidget(subViewWidget);
    filesTable->setObjectName("filesTable");
    filesTable->setColumnCount(3);
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    filesTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    filesTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    filesTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    filesTable->setObjectName(QString::fromUtf8("filesTable"));
    filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    filesTable->setTabKeyNavigation(false);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    filesTable->horizontalHeader()->setHighlightSections(false);
    filesTable->horizontalHeader()->setStretchLastSection(true);
    filesTable->verticalHeader()->setVisible(false);
    filesTable->verticalHeader()->setDefaultSectionSize(19);
    filesTable->horizontalHeaderItem(0)->setText(tr("Файл"));
    filesTable->horizontalHeaderItem(1)->setText(tr("Изменен"));
    filesTable->horizontalHeaderItem(2)->setText(tr("Путь"));
    subLay->addWidget(filesTable);
    connect(filesTable,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(slotFilesTable_doubleClicked(QModelIndex)));
    connect(filesTable,SIGNAL(clicked(QModelIndex)),this,SLOT(slotFilesTable_pressed(QModelIndex)));
    connect(filesTable,SIGNAL(itemSelectionChanged()),SLOT(slotCurrentFileChanged()));
    connect(tabBar,SIGNAL(currentChanged(int)),this,SLOT(slotTabChanged(int)));
    connect(webView_2->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    connect(webView_2->page(),SIGNAL(unsupportedContent(QNetworkReply*)),
            this,SLOT(slotUnsupportedContent(QNetworkReply*)));
    webView_2->page()->setForwardUnsupportedContent(true);
    webView_2->page()->mainFrame()->setUrl(QUrl("about:blank"));
    ttglobal()->queryViewOpened(this,queryView);

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
    initPopupMenu();
}

void QueryPage::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
{
    tabBar->addTab(icon, title);
    subLay->addWidget(tab);
}

bool QueryPage::hasMarked()
{
    return !markedRecords().isEmpty();
}

TQAbstractProject *QueryPage::project() const
{
    return d->modelProject;
}

void QueryPage::closeTab(int index)
{
	QWidget *w = projectTabWidget->widget(index);
	if(!w)
		return;
	delete w;
	if(!projectTabWidget->count())
		projectTabWidget->hide();
	//projectTabWidget->removeTab(index);
}

/*
void QueryPage::setQuery(int id, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQuery(id,trkdb);
	queryView->setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void QueryPage::setQueryById(const QString& numbers, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQueryById(numbers,trkdb);
	queryView->setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}
*/

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
    d->xqFile = "data/scr.xq";
    if(prj)
    {
        TQScopeSettings sets(d->modelProject->projectSettings());
        d->xqFile = sets->value("RecordTemplate", d->xqFile).toString();
    }
    if(!d->tmodel->isSystemModel())
        d->tmodel->setParent(this);
    d->proxy->setSourceModel(d->tmodel);
//    d->qryFilterModel->setSourceModel(d->tmodel);
//    queryView->setModel(d->tmodel);
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
    /*
	connect(
		queryView->selectionModel(), 
		SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this, 
		SLOT(changedView(const QModelIndex &, const QModelIndex &)));
        */
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
    queryView->selectRow(0);
}

/*
void QueryPage::setPlanModel(PlanModel *newmodel)
{
	if(newmodel == planModel)
		return;
    planViewModel.setSourceModel(newmodel);
	planModel=newmodel;
    if(planModel)
    {
        planModel->setFieldHeaders(planTreeView->header());
        QSettings *settings = ttglobal()->settings();
        QVariant value = settings->value(Settings_ScrPlanView);
        if(value.isValid() && value.type() == QVariant::ByteArray)
            planTreeView->header()->restoreState(value.toByteArray());
    }
}
*/

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

void QueryPage::changedView(const QModelIndex &index, const QModelIndex &prev)
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

    /*
	for(QList<EditDef>::const_iterator i=fieldEdits.constBegin();
		i<fieldEdits.constEnd(); i++)
	{
		if(QLineEdit *ed=qobject_cast<QLineEdit *>(i->edit))
		{
			ed->setText(qryIndex.sibling(qryIndex.row(), i->fieldcol).data().toString());
			ed->setCursorPosition(0);
		}
	}
    */
    drawNotes(qryIndex);
}

void QueryPage::selectionChanged(const QItemSelection & /* selected */, const QItemSelection & /*deselected*/)
{
    /*
	int linkCol=getColNum(linkField);
	QStringList keys;
	QModelIndexList list=selected.indexes();
	for(int i=0; i<list.count(); i++)
	{
		QModelIndex index = list[i];
		QModelIndex link = index.sibling(index.row(),linkCol);
		QString key = link.data().toString();
		if(!key.isEmpty())
			keys.append(key);
	}
	//proxyModel.setFilterRegExp(keys.join("|"));
	proxyModel.setFilterSCR(keys.join(","));
	planTreeView->expandAll();
    */
    d->detailsTimer->start(250);
    emit selectionRecordsChanged();
}

void QueryPage::currentChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if(d->detailsTimer->isActive())
        return;
    d->detailsTimer->start(100);
    emit selectionRecordsChanged();
}

void QueryPage::slotTabChanged(int index)
{
    subViewWidget->setVisible(index != 0);
    if(index)
        subLay->setCurrentIndex(index-1);
    if(index && subLay->currentWidget() == filesTable)
        stackedWidget->setCurrentWidget(pageWithPreview);
    else
    {
        stackedWidget->setCurrentWidget(pageWithWeb);
        previewWidget->clear();
    }
}

/* to plugin
void QueryPage::slotPlanContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    bool isTasksSelected=false;
    bool isGroupSelected=false;
    foreach(const QModelIndex &i, list)
    {
        if(i.parent().isValid())
            isTasksSelected = true;
        else
            isGroupSelected = true;
        QModelIndex si = planViewModel.mapToSource(i);
        //PrjItemModel *prj=planModel->prjModel(si);
        //selectedPlans.insert(prj);
    }
    QMenu menu;
    if(isTasksSelected || isGroupSelected)
    {
        menu.addAction(tr("Копировать номера запросов из задач"),this,SLOT(copyScrFromTasks()));
        menu.addAction(tr("Показать запросы из задач"),this,SLOT(showScrFromTasks()));
    }
    if(isTasksSelected)
    {
        menu.addAction(tr("Отметить запланированные запросы"),this,SLOT(slotCheckPlannedIds()));
        menu.addAction(tr("Отметить незапланированные запросы"),this,SLOT(slotCheckNoPlannedIds()));
        menu.addSeparator();
        menu.addAction(tr("Перейти на задачу в плане"),this,SLOT(showCurrentTaskInPlan()));

    }
    if(isTasksSelected && isGroupSelected)
    {
        menu.addSeparator();
    }
    if(isGroupSelected)
    {
        menu.addAction(tr("Добавить выделенные запросы в план"),this,SLOT(addScrTasks()));
    }
    QPoint gPos = planTreeView->mapToGlobal(pos);
    menu.exec(gPos);
}
*/

void QueryPage::populateJavaScriptWindowObject()
{
    webView_2->page()->mainFrame()->addToJavaScriptWindowObject("query", this);
    webView_2->page()->mainFrame()->addToJavaScriptWindowObject("global", ttglobal());
}

void QueryPage::slotUnsupportedContent(QNetworkReply *reply)
{
    ttglobal()->showError(reply->errorString());
    delete reply;
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
//        QSettings *settings = ttglobal()->settings();
//        settings->setValue(Settings_Grid, queryView->horizontalHeader()->saveState());
    }
}

void QueryPage::initPopupMenu()
{
	QHeaderView *hv=queryView->horizontalHeader();

    QAction *action = new QAction(tr("Настройка столбцов..."),this);
    connect(action,SIGNAL(triggered()),this,SLOT(execColumnsEditor()));
    hv->addAction(action);

    /*
    QHash<QString, int> fieldPos;
    QStringList labels;
    for(int i=0; i<hv->count(); i++)
    {
        QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed();
        fieldPos[label] = i;
        labels << label;
    }
    labels.sort();
    for(int i=0; i<labels.count(); i++)
	{
        QString label = labels[i];
        int pos = fieldPos[label];
        QAction *action = new QAction(label,this);
		action->setCheckable(true);
        action->setChecked(!hv->isSectionHidden(pos));
        action->setProperty("HeaderPos", pos);
		connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
		//hv->addAction(action);
		headerActions.append(action);
	}
    hv->addActions(headerActions);
    */

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
        //int i = headerActions.indexOf(a);
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

static QString escape(const QString& s)
{
	QString res= s;
	res.replace("&","&amp;");
	res.replace("<","&lt;");
	res.replace(">","&gt;");
	res.replace("\"","&quote;");
	return res;
}

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
	//QString src=xml.toString();
	QByteArray ba=xml.toByteArray();
	QBuffer buf;
	buf.setData(ba);
	buf.open(QIODevice::ReadOnly);
	query.bindVariable("scrdoc",&buf);
	query.setQuery(&xq);
	//query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
	query.evaluateTo(&page);
	return page;
	
}

/*QString QueryPage::makeRecordsPage(const QModelIndexList &records, const QString& xqCodeFile)
{
    QDomDocument xml("scr");
    QDomElement root=xml.createElement("scrs");
    xml.appendChild(root);
    for(int ii=0; ii<records.count(); ii++)
    {
        QModelIndex rec;
        rec = mapIndexToModel(records[ii]);
        const TrkToolModel *model = qobject_cast<const TrkToolModel *>(rec.model());
        if(model)
        {
            QDomDocument scr = model->recordXml(rec.row());
            root.appendChild(scr);
        }
    }

    QFile testXml("!testPrint.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);
    QFile xq(xqCodeFile);
    xq.open(QIODevice::ReadOnly);

    QXmlQuery query;

    QString page;
    //QString src=xml.toString();
    QByteArray ba=xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    query.bindVariable("scrdoc",&buf);
    query.setQuery(&xq);
    //query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
    query.evaluateTo(&page);
    return page;
}
*/

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
    QFile trackerXML("data/tracker.xml");
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

void QueryPage::drawNotes(const QModelIndex &qryIndex)
{
    QString base = qApp->applicationDirPath()+"/data/";
    QString page = makeRecordPage(qryIndex, d->xqFile);
#ifdef QT_DEBUG
    QFile testRes("!testResult.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    //QUrl baseURL = QUrl::fromLocalFile(base);
    //QString u = baseURL.toString();
    webView_2->setHtml(page,QUrl(base));
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


	/*
    Dim olApp As Outlook.Application
    Dim objMail As MailItem
    Set olApp = Outlook.Application
    'Create mail item
    Set objMail = olApp.CreateItem(olMailItem)

    With objMail
       'Set body format to HTML
       .BodyFormat = olFormatHTML
       .HTMLBody = "<HTML><H2>The body of this message will appear in HTML.</H2><BODY>Type the message text here. </BODY></HTML>"
       .Display
    End With
    */
}


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
    QAxScript * script = man->load("C:/gits/traquera/client/data/email3.js","email");
//    QStringList funcs = script->functions();
//    script->call("test", QList<QVariant>());
    delete man;
}

void QueryPage::sendEmail2(const QObjectList &records)
{
    if(!records.count())
        return;
    QScopedPointer<QScriptEngine>engine(ttglobal()->newScriptEngine());
    QScriptEngineDebugger debugger;
    debugger.attachTo(engine.data());
    QString fileName("data/email.js");
//    QString fileName(":/js/email.js");
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

/* to plugin
void QueryPage::addScrTask(PrjItemModel *prj)
{
	QXmlSimpleReader xmlReader;
	QFile file("data/scr2prj.xml");
	QXmlInputSource *source = new QXmlInputSource(&file);
	QDomDocument dom;
	if(!dom.setContent(source,false))
		return;
	QDomElement doc = dom.documentElement();
	if(doc.isNull()) return;
	QDomElement fields = doc.firstChildElement("fields");

	QItemSelectionModel *is=queryView->selectionModel();
	QModelIndexList ii = is->selectedRows();
	for(QModelIndexList::const_iterator i=ii.constBegin();
		i!=ii.constEnd();
		++i)
	{
		PrjRecord rec;

		for(QDomElement field = fields.firstChildElement("field");
			!field.isNull();
			field = field.nextSiblingElement("field"))
		{
			QString ftitle = field.attribute("title");
			QString src = field.attribute("src");
			if(ftitle.isEmpty() || src.isEmpty())
				continue;
			int col = getColNum(src);
			if(col<0)
				continue;
			QModelIndex srcIndex = i->sibling(i->row(),col);
			QVariant val = srcIndex.data();
			rec[ftitle] = val;
		}
		prj->addTask(rec);
	}
}

void QueryPage::addScrTasks()
{
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    QSet<PrjItemModel *>selectedPlans;
    foreach(const QModelIndex &i, list)
    {
        QModelIndex si = planViewModel.mapToSource(i);
        PrjItemModel *prj=planModel->prjModel(si);
        if(prj)
          selectedPlans.insert(prj);
    }
    foreach(PrjItemModel *prj, selectedPlans)
    {
        addScrTask(prj);
    }

}

void QueryPage::copyScrFromTasks()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    QString slist = intListToString(res.toList());
    QApplication::clipboard()->setText(slist);
}

void QueryPage::showScrFromTasks()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    emit openRecordsClicked(res);
}
*/

void QueryPage::loadDefinition()
{
    decorator->loadViewDef(this);
	/*
	QXmlSimpleReader xmlReader;
	QFile *file = new QFile("data/tracker.xml");
	QXmlInputSource *source = new QXmlInputSource(file);
	QDomDocument dom;
	if(!dom.setContent(source,false))
		return;
	QDomElement doc = dom.documentElement();
	if(doc.isNull()) return;
	QHeaderView *hv=queryView->horizontalHeader();
	QVariant gridSet = settings->value("TrackerGrid");
	bool isGridRestored=false;
	if(gridSet.isValid()) 
		isGridRestored = hv->restoreState(gridSet.toByteArray());
	if(!isGridRestored)
	{
		QDomElement grid = doc.firstChildElement("grid");
		if(!grid.isNull())
		{
			int nextCol=0;
			for(QDomElement col = grid.firstChildElement("col");
				!col.isNull();
				col = col.nextSiblingElement("col"))
			{
				QString flabel = col.attribute("field");
				int colNum = findColumn(hv, flabel);
				if(colNum>=0)
				{
					int from = hv->visualIndex(colNum);
					hv->moveSection(from, nextCol++);
					bool ok;
					int size = col.attribute("size").toInt(&ok);
					if(ok)
						hv->resizeSection(colNum,size);
				}
			}
			for(; nextCol<hv->count(); nextCol++)
			{
				int log=hv->logicalIndex(nextCol);
				hv->hideSection(log);
			}
		}
	}
	hv->setMovable(true);
	hv->setDefaultAlignment(Qt::AlignLeft);
	settings->setValue("TrackerGrid", hv->saveState());
	QDomElement panels = doc.firstChildElement("panels");
	if(!panels.isNull())
	{
		QStringList used;
		for(QDomElement panel = panels.firstChildElement("panel");
			!panel.isNull();
			panel = panel.nextSiblingElement("panel"))
		{
			QWidget *p = new QWidget();
			QHBoxLayout *hbox = new QHBoxLayout(p);
			//p->setLayout(hbox);
			QScrollArea *scr = new QScrollArea(p);
			scr->setWidgetResizable(true);
			QWidget *inp = new QWidget();
			inp->setGeometry(QRect(0, 0, 295, 200));
			QGridLayout *grid=new QGridLayout(inp);
			QFormLayout *lay = new QFormLayout();
			int fields=0;
			int gridcol=0;
			//scr->setWidget(inp);
			if(panel.attribute("type") != "other")
			{
				for(QDomElement field = panel.firstChildElement("field");
					!field.isNull();
					field = field.nextSiblingElement("field"))
				{
					EditDef f;
					f.edit = new QLineEdit(inp);
					f.title = field.attribute("name");
					f.fieldcol = getColNum(f.title);
					if(f.fieldcol == -1)
						continue;
					if(fields && !(fields % 4))
					{
						grid->addLayout(lay,0,gridcol++,1,1);
						lay = new QFormLayout();
					}
					fields++;
					lay->addRow(f.title,f.edit);
					fieldEdits.append(f);
					used.append(f.title);
				}
			}
			else
			{
				QHash<int, TrkField>::ConstIterator ff;
				for(ff = trkmodel.trkdb->flds.constBegin();
					ff != trkmodel.trkdb->flds.constEnd();
					ff++)
				{
					QString fname = ff.value().fldLabel.trimmed();
					if(!used.contains(fname))
					{
						EditDef f;
						f.edit = new QLineEdit();
						f.title = fname;
						f.fieldcol = getColNum(f.title);
						if(f.fieldcol == -1)
							continue;
						if(fields && !(fields % 4))
						{
							grid->addLayout(lay,0,gridcol++);
							lay = new QFormLayout();
						}
						fields++;
						lay->addRow(f.title,f.edit);
						fieldEdits.append(f);
						used.append(f.title);
					}
				}
			}
			grid->addLayout(lay,0,gridcol);
			scr->setWidget(inp);
			scr->setFrameShape(QFrame::NoFrame);
			hbox->addWidget(scr);
			hbox->setContentsMargins(2,2,2,2);
			tabPanels->addTab(p, panel.attribute("title"));
			//scr->setWidget(p);
		}
		isDefLoaded = true;
	}
	isDefLoaded = true;
	delete source;
	delete file;
	*/
    d->isDefLoaded = true;
}

void QueryPage::recordOpen(const QModelIndex & index)
{
	if(!index.isValid())
		return;
    /*
    const TrkToolModel *model = qobject_cast<const TrkToolModel *>(mapIndexToModel(index).model());
	if(!model)
		return;
    TQRecord *rec = model->at(index.row());
    */
    TQRecord *rec = currentRecord();
    if(!rec)
        return;
    ScrWidget *scr = new ScrWidget(rec->typeDef());
	scr->setRecord(rec);
	scr->show();
}

void QueryPage::openQuery(TQAbstractProject *prj, const QString &queryName, int recType)
{
    d->itIsFolder = false;
    TQRecModel *newmodel = prj->openQueryModel(queryName, recType);
	if(!newmodel)
		return;
    setQueryModel(prj,newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();
    //TrkHistoryItem item;
    //item.isQuery = true;
    //item.projectName = prj->name;
    //item.prj = prj;
    //item.queryName = queryName;
    //item.recType = recType;
    //for(int i=history.count()-1; i>curHistoryPos; i--)
    //	history.removeLast();
    //history.append(item);
//    curHistoryPos = history.rowCount()-1;
    emit changedQuery(prj->projectName(), queryName);
}

void QueryPage::openIds(TQAbstractProject *prj, const QString &ids, const QString &title, int recType)
{
    d->itIsFolder = false;
    QList<int> idlist = toIntList(ids);
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->openIdsModel(idlist, recType));
	if(!newmodel)
		return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();

    /*
	TrkHistoryItem item;
	item.isQuery = false;
    //item.projectName = prj->name;
    //item.prj = prj;
	item.queryName = ids;
    item.rectype = recType;
    item.foundIds = intListToString(idlist);
    for(int i=history.rowCount()-1; i>curHistoryPos; i--)
		history.removeLast();
	history.append(item);
    */
//    curHistoryPos = history.rowCount()-1;
    QString newTitle;
    if(title.isEmpty())
        newTitle = ids;
    else
        newTitle = title;
    emit changedQuery(prj->projectName(), newTitle);
}

void QueryPage::openIds(TQAbstractProject *prj, const QList<int> &idlist, const QString &title, int recType)
{
    d->itIsFolder = false;
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->openIdsModel(idlist, recType));
    if(!newmodel)
        return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    addHistoryPoint();
//    curHistoryPos = history.rowCount()-1;

    QString newTitle;
    if(title.isEmpty())
        newTitle = intListToString(idlist);
    else
        newTitle = title;
    emit changedQuery(prj->projectName(), newTitle);
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
    TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->openIdsModel(idlist, recType, false));
    if(!newmodel)
        return;
    setQueryModel(prj, newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    QString ids = intListToString(idlist);
    emit changedQuery(prj->projectName(), ids);
    addHistoryPoint();

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
        TQRecModel *newmodel = prj->openQueryModel(item.queryName, item.rectype);
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
        TQRecModel *newmodel = qobject_cast<TQRecModel *>(prj->openIdsModel(stringToIntList(item.queryName), item.rectype));
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
    QString page = makeRecordsPage(records,"data/print.xq");
    QWebView web(this);
    web.setHtml(page);
#ifdef QT_DEBUG
    QFile testRes("!testPrint.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    QPrintPreviewDialog ppd;
    connect(&ppd,SIGNAL(paintRequested(QPrinter*)),&web,SLOT(print(QPrinter*)));
    ppd.exec();

    // old
    /*
    QPrintPreviewDialog *ppd = new QPrintPreviewDialog();
    connect(ppd,SIGNAL(paintRequested(QPrinter*)),webView_2,SLOT(print(QPrinter*)));
    ppd->exec();
    delete ppd;
    */
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
    return trkmodel->at(f.row());
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

TQAbstractProject *QueryPage::currentProject()
{
    return d->modelProject;
}

TQRecord *QueryPage::currentRecord()
{
    QModelIndex cur = queryView->currentIndex();
    if(!cur.isValid())
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
    return trkmodel->at(cur.row());
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
        foreach(QObject *obj, records)
        {
            rec = qobject_cast<TQRecord *>(obj);
            if(rec->updateBegin())
            {
                rec->addNote(nd->titleEdit->currentText(), nd->noteEdit->toPlainText());
                rec->commit();
            }
        }
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
}

void QueryPage::removeIds(const QList<int> &ids)
{
    d->tmodel->removeRecordIds(ids);
    updateHistoryPoint();
}

//void QueryPage::appendId(int id)
//{
//    tmodel->appendRecordId(id);
//}

//void QueryPage::removeId(int id)
//{
//    tmodel->removeRecordId(id);
//}

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
        win->setTypeDef(rec->typeDef());
        win->setRecord(rec);
        win->show();
    }
}

/* to plugin
void QueryPage::showCurrentTaskInPlan()
{
    QModelIndex cur = planTreeView->currentIndex();
    if(!cur.isValid())
        return;
    QModelIndex taskIndex = cur.sibling(cur.row(),0);
    int task = taskIndex.data().toInt();
    QModelIndex planIndex = planViewModel.mapToSource(cur);
    PrjItemModel *model = planModel->prjModel(planIndex);
    emit showTaskInPlanClicked(model->fileName,task);
}
*/

void QueryPage::previewFile(const QString &filePath)
{
    stackedWidget->setCurrentWidget(pageWithPreview);
    previewWidget->setSourceFile(filePath);
}

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
    /* to plugin
    planViewModel.setFilterSCR(keys.join(","));
    planTreeView->expandAll();
    for(int r=0; r<planViewModel.rowCount(); r++)
        planTreeView->setFirstColumnSpanned(r,QModelIndex(),true);
    */
    QModelIndex qryIndex = mapIndexToModel(queryView->currentIndex());
    if(!qryIndex.isValid())
        return;
    TQRecord *record = d->tmodel->at(qryIndex.row());
    const TQAbstractRecordTypeDef *def = record->typeDef();
#ifdef DECORATOR
    decorator->readValues(record, fieldEdits);
#endif
    drawNotes(qryIndex);

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

    // old
    /*
    QPrintPreviewDialog *ppd = new QPrintPreviewDialog();
    connect(ppd,SIGNAL(paintRequested(QPrinter*)),webView_2,SLOT(print(QPrinter*)));
    ppd->exec();
    delete ppd;
    */
}

void QueryPage::on_actionSelectRecords_triggered()
{
    foreach(QObject *rec, selectedRecords())
    {
        ((TQRecord *)rec)->setSelected(true);
    }
    /*
    foreach(QModelIndex index, queryView->selectionModel()->selectedRows())
    {
        QModelIndex srcIndex = mapIndexToModel(index);
        srcIndex = srcIndex.sibling(srcIndex.row(), tmodel->idColumn());
        tmodel->setData(srcIndex,QVariant::fromValue<int>(Qt::Checked),Qt::CheckStateRole);
    }
    */
}

void QueryPage::on_actionDeselectRecords_triggered()
{
    foreach(QObject *obj, selectedRecords())
    {
        TQRecord *rec = (TQRecord *)obj;
        rec->setSelected(false);
    }
    /*
    foreach(QModelIndex index, queryView->selectionModel()->selectedRows())
        queryView->model()->setData(index,QVariant::fromValue<int>(Qt::Unchecked),Qt::CheckStateRole);
        */
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

void QueryPage::slotFilesTable_doubleClicked(const QModelIndex &index)
{
    d->previewTimer->stop();
    TQRecord *rec = recordOnIndex(queryView->currentIndex());
    if(!rec || !index.isValid())
        return;
    QString fileName = filesTable->item(index.row(),0)->text();
    QString tempFile = QDir::temp().absoluteFilePath(fileName);
    rec->saveFile(index.row(), tempFile);
    tempFile = QDir::toNativeSeparators(tempFile);
    //QUrl url = QUrl::fromUserInput(tempFile);
    QUrl url = QUrl::fromLocalFile(tempFile);
    //tempFile = url.toString();
    QDesktopServices::openUrl(url);
//    wchar_t *buf = new wchar_t[tempFile.length()+1];
//    tempFile.toWCharArray(buf);
//    ShellExecuteW(0,"open",buf,0,0,SW_SHOWNORMAL);
//    delete buf;
    /*
    QString newFile = QFileDialog::getSaveFileName(this,
                                                   tr("Сохранить файл"),
                                                   fileName);
    if(newFile.isEmpty())
        return;
    rec->saveFile(index.row(), newFile);
    */
}

void QueryPage::slotFilesTable_pressed(const QModelIndex &index)
{
    d->previewTimer->start(100);
}

void QueryPage::slotFilePreview()
{
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
}

void QueryPage::slotCurrentFileChanged()
{
    QTableWidgetItem *fileItem = filesTable->currentItem();
    if(!fileItem || !fileItem->isSelected())
        previewWidget->clear();
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

/* to plugin
void QueryPage::slotCheckPlannedIds()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    foreach(int id, res)
        setIdChecked(id,true);
}

void QueryPage::slotCheckNoPlannedIds()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    for(int row=0; row<tmodel->rowCount(); row++)
    {
        int id = tmodel->rowId(row);
        if(!res.contains(id))
            setIdChecked(id,true);
    }
}
*/


void QueryPage::addHistoryPoint()
{
    TQHistoryItem item;
    item.projectName = d->modelProject->projectName();
    if(d->tmodel)
    {
        item.isQuery = d->tmodel->isQuery();
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
//            item.isQuery = tmodel->isQuery();
//            item.queryName = tmodel->queryName();
//            item.rectype = tmodel->recordType();
//            item.foundIds = intListToString(tmodel->getIdList());
            item.addedIds = intListToString(d->tmodel->addedIdList());
            item.removedIds = intListToString(d->tmodel->deletedIdList());
        }
        d->history.removeRow(r);
        d->history.append(item);
    }
}
