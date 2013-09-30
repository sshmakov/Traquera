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

#include "querypage.h"
#include "trkview.h"
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
//#include <Shlwapi.h>

QueryPage::QueryPage(QWidget *parent)
	:QWidget(parent)
    , tmodel(0)
    , planViewModel(this)
	, history()
    , itIsFolder(false)
{
    setupUi(this);
    initWidgets();
    qryFilterModel = new QSortFilterProxyModel(this);
    qryFilterModel->setFilterKeyColumn(-1);
    qryFilterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    detailsTimer = new QTimer(this);
    detailsTimer->setInterval(0);
    detailsTimer->setSingleShot(true);
    connect(detailsTimer,SIGNAL(timeout()),this,SLOT(updateDetails()));

    previewTimer = new QTimer(this);
    previewTimer->setInterval(0);
    previewTimer->setSingleShot(true);
    connect(previewTimer,SIGNAL(timeout()),this,SLOT(slotFilePreview()));

    QVBoxLayout * v = new QVBoxLayout(pageWithPreview);
    v->setContentsMargins(0, 0, 0, 0);
    v->setObjectName(QString::fromUtf8("verticalLayout"));
    previewWidget = new Preview(pageWithPreview);
    previewWidget->setObjectName("previewWidget");
    v->addWidget(previewWidget);
    stackedWidget->setCurrentIndex(0);

	planModel=NULL;
	curHistoryPos=0;
	isDefLoaded=false;
	//isInteractive=false;
	linkField="Id";
	projectTabWidget->hide();
    // !! tabPanels->resize(tabPanels->width(), 550);
    queryView->setModel(qryFilterModel);
	//queryView->setModel(&trkmodel.queryModel);
	//queryView->setModel(trkmodel.model);
    planTreeView->setModel(&planViewModel);
	//connect(queryView,SIGNAL(itemSelectionChanged()),

	connect(projectTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));

	QAction *a;
    a = new QAction(QString::fromLocal8Bit("Добавить в план"),this);
	connect(a,SIGNAL(triggered(bool)),this,SLOT(addScrTasks()));
	planTreeView->addAction(a);
    //planTreeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    planTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(planTreeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slotPlanContextMenuRequested(QPoint)));

    //connect(queryView,SIGNAL(activated(const QModelIndex &)),this,SLOT(recordOpen(const QModelIndex &)));
    connect(queryView,SIGNAL(activated(const QModelIndex &)),actionTest,SLOT(trigger()));
    connect(this, SIGNAL(openingModel(const TrkToolModel*)),&history,SLOT(openedModel(const TrkToolModel *)),Qt::DirectConnection);

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
    settings->setValue(Settings_ScrPlanView,planTreeView->header()->saveState());
}

void QueryPage::initWidgets()
{
    tabBar = new QTabBar(tabPlaceWidget);
    tabBar->addTab(QIcon(":/images/file.png"),tr("Текст"));
    tabBar->addTab(QIcon(":/images/plan.png"),tr("Планы"));
    tabBar->addTab(tr("Файлы"));

    QBoxLayout *lay = qobject_cast<QBoxLayout *>(tabPlaceWidget->layout());
    lay->insertWidget(0,tabBar);
    horizontalSpacer->changeSize(10,20,QSizePolicy::Expanding);
    subLay = new QStackedLayout(subViewWidget);

    planTreeView = new QTreeView(subViewWidget);
    planTreeView->setObjectName(QString::fromUtf8("planTreeView"));
    planTreeView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    planTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    planTreeView->setIndentation(5);
    subLay->addWidget(planTreeView);

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
    connect(tabBar,SIGNAL(currentChanged(int)),this,SLOT(slotTabChanged(int)));
    connect(webView_2->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    connect(webView_2->page(),SIGNAL(unsupportedContent(QNetworkReply*)),
            this,SLOT(slotUnsupportedContent(QNetworkReply*)));
    webView_2->page()->setForwardUnsupportedContent(true);
    webView_2->page()->mainFrame()->setUrl(QUrl("about:blank"));
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

void QueryPage::setQueryModel(TrkToolModel *model)
{
    if(tmodel)
    {
        if(((QObject*)tmodel)->parent() == this)
            tmodel->deleteLater();
        tmodel->disconnect(this);
        tmodel = 0;
    }
	tmodel = model;
    if(!tmodel->isSystemModel())
        tmodel->setParent(this);
    qryFilterModel->setSourceModel(tmodel);
    if(!isDefLoaded)
        loadDefinition();
    //queryView->setModel(model);
#ifdef DECORATOR
    fieldEdits.clear();
    decorator->fillEditPanels(this->tabPanels, tmodel->typeDef(),fieldEdits);
#endif
    filterFieldComboBox->clear();
    filterFieldComboBox->addItem(tr("Любое поле"),-1);
    QMap<QString, int> headers;
    for(int col=0; col<tmodel->columnCount(); col++)
    {
        headers.insert(tmodel->headerData(col, Qt::Horizontal).toString(),col);
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

void QueryPage::setPlanModel(PlanModel *newmodel)
{
	if(newmodel == planModel)
		return;
    planViewModel.setSourceModel(newmodel);
	/*
	if(newmodel)
		proxyModel.setFilterKeyColumn(newmodel->SCRfield);
		*/
	planModel=newmodel;
    if(planModel)
    {
        planModel->setFieldHeaders(planTreeView->header());
        QVariant value = settings->value(Settings_ScrPlanView);
        if(value.isValid() && value.type() == QVariant::ByteArray)
            planTreeView->header()->restoreState(value.toByteArray());
    }
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

void QueryPage::changedView(const QModelIndex &index, const QModelIndex &prev)
{
	if(prev.row() == index.row())
		return;
    QModelIndex qryIndex = mapIndexToModel(index);
    if(!qryIndex.isValid())
        return;
    TrkToolRecord *record = tmodel->at(qryIndex.row());
#ifdef DECORATOR
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
    detailsTimer->start(250);
    emit selectionRecordsChanged(selectedRecords());
}

void QueryPage::currentChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    detailsTimer->start(100);
    emit selectionRecordsChanged(selectedRecords());
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

void QueryPage::slotPlanContextMenuRequested(const QPoint &pos)
{
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    bool isTasksSelected=false;
    bool isGroupSelected=false;
    //QSet<PrjItemModel *>selectedPlans;
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

void QueryPage::populateJavaScriptWindowObject()
{
    webView_2->page()->mainFrame()->addToJavaScriptWindowObject("query", this);
    webView_2->page()->mainFrame()->addToJavaScriptWindowObject("global", TTGlobal::global());
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
	for(int i=0; i<tmodel->columnCount(); i++)
		if(!colname.compare(tmodel->headerData(i,Qt::Horizontal).toString(),
			Qt::CaseInsensitive))
			return i;
	return -1;
}

void QueryPage::headerChanged()
{
	if(isInteractive)
        settings->setValue(Settings_Grid, queryView->horizontalHeader()->saveState());
}

void QueryPage::initPopupMenu()
{
	QHeaderView *hv=queryView->horizontalHeader();
	for(int i=0; i<hv->count(); i++)
	{
		QString label = queryView->model()->headerData(i,Qt::Horizontal).toString().trimmed(); 
        QAction *action = new QAction(label,this);
		action->setCheckable(true);
		action->setChecked(!hv->isSectionHidden(i));
		connect(action,SIGNAL(toggled(bool)),this,SLOT(headerToggled(bool)));
		//hv->addAction(action);
		headerActions.append(action);
	}

    hv->addActions(headerActions);
	hv->setContextMenuPolicy(Qt::ActionsContextMenu);
}

void QueryPage::headerToggled(bool checked)
{
	QAction *a = qobject_cast<QAction*>(sender());
	if(a)
	{
		int i = headerActions.indexOf(a);
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
	const TrkToolModel *model = qobject_cast<const TrkToolModel *>(qryIndex.model());
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

/*
QString QueryPage::makeRecordsPage(const QModelIndexList &records, const QString& xqCodeFile)
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

QString QueryPage::makeRecordsPage(const QList<TrkToolRecord *> &records, const QString &xqCodeFile)
{
    QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    foreach( TrkToolRecord *rec, records)
    {
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
    QString page = makeRecordPage(qryIndex,"data/scr.xq");
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

void QueryPage::sendEmail(const QList<TrkToolRecord *> &records)
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
        subject = QString(tr("Запрос %1. %2")).arg((int)(records[0]->recordId())).arg(records[0]->value("Title").toString());
    else
    {
        subject = tr("Запросы ");
        for(int i=0; i<records.count(); i++)
        {
            const TrkToolRecord *rec = records[i];
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

/*
    QItemSelectionModel *is=planTreeView->selectionModel();
	QModelIndexList ii = is->selectedRows();
	for(int i=0; i<ii.count(); i++)
	{
		QModelIndex si = proxyModel.mapToSource(ii[i]);
		PrjItemModel *prj=planModel->prjModel(si);
		if(prj)
			addScrTask(prj);
	}
    */
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

void QueryPage::loadDefinition()
{
    decorator->loadViewDef(queryView);
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
    initPopupMenu();
    isDefLoaded = true;
}

void QueryPage::recordOpen(const QModelIndex & index)
{
	if(!index.isValid())
		return;
    /*
    const TrkToolModel *model = qobject_cast<const TrkToolModel *>(mapIndexToModel(index).model());
	if(!model)
		return;
	TrkToolRecord *rec = model->at(index.row());
    */
    TrkToolRecord *rec = currentRecord();
    if(!rec)
        return;
    ScrWidget *scr = new ScrWidget(rec->typeDef());
	scr->setRecord(rec);
	scr->show();
}

void QueryPage::openQuery(TrkToolProject *prj, const QString &queryName, int recType)
{
    itIsFolder = false;
    TrkToolModel *newmodel = prj->openQueryModel(queryName, recType);
	if(!newmodel)
		return;
	setQueryModel(newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    //TrkHistoryItem item;
    //item.isQuery = true;
    //item.projectName = prj->name;
    //item.prj = prj;
    //item.queryName = queryName;
    //item.recType = recType;
    //for(int i=history.count()-1; i>curHistoryPos; i--)
    //	history.removeLast();
    //history.append(item);
    curHistoryPos = history.rowCount()-1;
    emit changedQuery(prj->name, queryName);
}

void QueryPage::openIds(TrkToolProject *prj, const QString &ids, const QString &title, int recType)
{
    itIsFolder = false;
    QList<int> idlist = toIntList(ids);
    TrkToolModel *newmodel = prj->openIdsModel(idlist, recType);
	if(!newmodel)
		return;
	setQueryModel(newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);

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
    curHistoryPos = history.rowCount()-1;
    QString newTitle;
    if(title.isEmpty())
        newTitle = ids;
    else
        newTitle = title;
    emit changedQuery(prj->name, newTitle);
}

void QueryPage::openIds(TrkToolProject *prj, const QList<int> &idlist, const QString &title, int recType)
{
    itIsFolder = false;
    TrkToolModel *newmodel = prj->openIdsModel(idlist, recType);
    if(!newmodel)
        return;
    setQueryModel(newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    curHistoryPos = history.rowCount()-1;

    QString newTitle;
    if(title.isEmpty())
        newTitle = intListToString(idlist);
    else
        newTitle = title;
    emit changedQuery(prj->name, newTitle);
}

void QueryPage::openModel(TrkToolModel *newModel)
{
    setQueryModel(newModel);
    emit openingModel(newModel);
    emit modelChanged(newModel);
}

void QueryPage::openFolder(TrkToolProject *prj, const TTFolder &afolder, int recType)
{
    QList<int> idlist = afolder.folderContent();
    itIsFolder = true;
    folder = afolder;
    TrkToolModel *newmodel = prj->openIdsModel(idlist, recType, false);
    if(!newmodel)
        return;
    setQueryModel(newmodel);
    emit openingModel(newmodel);
    emit modelChanged(newmodel);
    curHistoryPos = history.rowCount()-1;
    QString ids = intListToString(idlist);
    emit changedQuery(prj->name, ids);

}

void QueryPage::openQuery(const QString &projectName, const QString &queryName, int recType)
{
	TrkToolProject *prj = TrkToolDB::getProject(projectName);
	if(!prj)
		return;
    openQuery(prj,queryName,recType);
}

void QueryPage::openHistoryItem(int pos)
{
    if(pos<0 || pos>=history.rowCount())
		return;
	const TrkHistoryItem &item = history[pos];
	if(item.isQuery)
	{
		TrkToolProject *prj = TrkToolDB::getProject(item.projectName);
		if(!prj)
			return;
        TrkToolModel *newmodel = prj->openQueryModel(item.queryName, item.rectype);
		if(!newmodel)
			return;
		setQueryModel(newmodel);
        emit changedQuery(item.projectName, item.queryName);
        curHistoryPos = pos;
	}
	else
	{
		TrkToolProject *prj = TrkToolDB::getProject(item.projectName);
		if(!prj)
			return;
        TrkToolModel *newmodel = prj->openIdsModel(stringToIntList(item.queryName), item.rectype);
		if(!newmodel)
			return;
		setQueryModel(newmodel);
        emit changedQuery(item.projectName, item.queryName);
        curHistoryPos = pos;
	}
}

void QueryPage::goBack()
{
	openHistoryItem(curHistoryPos-1);
}

void QueryPage::goForward()
{
	openHistoryItem(curHistoryPos+1);
}

void QueryPage::printPreview()
{
    QList<TrkToolRecord *> records = selectedRecords();
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


TrkToolRecord *QueryPage::recordOnIndex(const QModelIndex &index)
{
    QModelIndex f = index;
    const QAbstractItemModel *model = f.model();
    while(const QAbstractProxyModel *proxy =  qobject_cast<const QAbstractProxyModel *>(model))
    {
        f = proxy->mapToSource(f);
        model = f.model();
    }
    const TrkToolModel *trkmodel = qobject_cast<const TrkToolModel *>(model);
    if(!trkmodel)
        return 0;
    return trkmodel->at(f.row());
}

QModelIndexList QueryPage::selectedRows()
{
    QItemSelectionModel *is=queryView->selectionModel();
    return is->selectedRows();
}

QList<TrkToolRecord *> QueryPage::selectedRecords()
{
    QList<TrkToolRecord *> list;
    QModelIndexList ii = selectedRows();
    foreach(QModelIndex f, ii)
    {
        TrkToolRecord *rec = recordOnIndex(f);
        if(!rec)
            continue;
        list.append(rec);
    }
    return list;
}

TrkToolRecord *QueryPage::currentRecord()
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
    const TrkToolModel *trkmodel = qobject_cast<const TrkToolModel *>(model);
    if(!trkmodel)
        return NULL;
    return trkmodel->at(cur.row());
}

void QueryPage::on_actionAdd_Note_triggered()
{
    NoteDialog *nd=new NoteDialog();
    nd->titleEdit->addItems(tmodel->typeDef()->project()->noteTitles);
    if(settings->contains("LastNote"))
        nd->titleEdit->setEditText(settings->value("LastNote").toString());
    if(nd->exec())
    {
        settings->setValue("LastNote",nd->titleEdit->currentText());
        QList<TrkToolRecord *> records = selectedRecords();
        TrkToolRecord *rec;
        foreach(rec, records)
        {
            if(rec->updateBegin())
            {
                rec->appendNote(nd->titleEdit->currentText(), nd->noteEdit->toPlainText());
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
    if(text == filterString)
        return;
    filterString = text;
    if(text.isEmpty())
    {
        //actionFilter->setChecked(false);
        qryFilterModel->setFilterFixedString("");
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
        qryFilterModel->setFilterRegExp(stringToRegExp(text));
    else
        qryFilterModel->setFilterFixedString(text);
}

void QueryPage::appendId(int id)
{
    tmodel->appendRecordId(id);
}

void QueryPage::removeId(int id)
{
    tmodel->removeRecordId(id);
}

void QueryPage::setIdChecked(int id, bool checked)
{
    int row = tmodel->rowOfRecordId(id);
    Qt::CheckState state = checked ? Qt::Checked : Qt::Unchecked;
    tmodel->setData(tmodel->index(row,tmodel->idColumn()), state, Qt::CheckStateRole);
}

void QueryPage::refreshQuery()
{
    if(tmodel)
    {
        tmodel->refreshQuery();
    }
}

void QueryPage::openRecordId(int id)
{
    TrkToolRecord *rec = tmodel->project()->getRecordId(id);
    if(rec)
    {
        TTRecordWindow *win = new TTRecordWindow(this);
        win->setTypeDef(rec->typeDef());
        win->setRecord(rec);
        win->show();
    }
}

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
        qryFilterModel->setFilterKeyColumn(-1);
    else
        qryFilterModel->setFilterKeyColumn(filterFieldComboBox->itemData(index).toInt());
}


void QueryPage::updateDetails()
{
    //int linkCol=getColNum(linkField);
    QStringList keys;
    QList<TrkToolRecord *> selected = selectedRecords();
    foreach(TrkToolRecord *rec, selected)
    {
        QString key = QString::number(rec->recordId());
        if(!key.isEmpty())
            keys.append(key);
    }
    planViewModel.setFilterSCR(keys.join(","));
    planTreeView->expandAll();
    for(int r=0; r<planViewModel.rowCount(); r++)
        planTreeView->setFirstColumnSpanned(r,QModelIndex(),true);

    QModelIndex qryIndex = mapIndexToModel(queryView->currentIndex());
    if(!qryIndex.isValid())
        return;
    TrkToolRecord *record = tmodel->at(qryIndex.row());
#ifdef DECORATOR
    decorator->readValues(record, fieldEdits);
#endif
    drawNotes(qryIndex);

    QList<TrkToolFile> files = record->fileList();
    filesTable->clearContents();
    filesTable->setRowCount(0);
    TTFileIconProvider prov;
    int row=0;
    foreach(const TrkToolFile &file, files)
    {
        QFileInfo f(file.fileName);
        filesTable->insertRow(row);
        filesTable->setItem(row,0,new QTableWidgetItem(prov.icon(f),f.fileName()));
        filesTable->setItem(row,1,new QTableWidgetItem(file.createDateTime.toString(TT_DATETIME_FORMAT)));
        filesTable->setItem(row,2,new QTableWidgetItem(f.dir().path()));
        row++;
    }

    //emit selectionRecordsChanged(selectedRecords());
}

void QueryPage::on_actionCopyId_triggered()
{
    QStringList numList;
    QString numbers;
    foreach(const TrkToolRecord *rec, selectedRecords())
    {
        int id = rec->recordId();
        numList.append(QString::number(id));
    }
    if(numList.isEmpty())
        return;
    numbers = numList.join(", ");
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(numbers);
}

void QueryPage::on_actionSelectRecords_triggered()
{
    foreach(TrkToolRecord *rec, selectedRecords())
    {
        rec->setSelected(true);
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
    foreach(TrkToolRecord *rec, selectedRecords())
    {
        rec->setSelected(false);
    }
    /*
    foreach(QModelIndex index, queryView->selectionModel()->selectedRows())
        queryView->model()->setData(index,QVariant::fromValue<int>(Qt::Unchecked),Qt::CheckStateRole);
        */
}

void QueryPage::on_actionDeleteFromList_triggered()
{
    foreach(TrkToolRecord *rec, selectedRecords())
        tmodel->removeRecordId(rec->recordId());
}

void QueryPage::on_queryView_customContextMenuRequested(const QPoint &pos)
{
    // Контекстое меню списка запросов
    //queryView->setContextMenuPolicy(Qt::CustomContextMenu);
    QMenu menu;
    menu.addAction(actionCopyId);
    menu.addAction(actionSelectRecords);
    menu.addAction(actionDeselectRecords);
    menu.addAction(actionDeleteFromList);
    if(itIsFolder)
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
    if(!itIsFolder)
        return;
    foreach(TrkToolRecord *rec, selectedRecords())
    {
        int recId = rec->recordId();
        folder.deleteRecordId(recId);
        tmodel->removeRecordId(recId);
    }
}

void QueryPage::slotFilesTable_doubleClicked(const QModelIndex &index)
{
    previewTimer->stop();
    TrkToolRecord *rec = recordOnIndex(queryView->currentIndex());
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
    previewTimer->start(250);
}

void QueryPage::slotFilePreview()
{
    QModelIndex recIndex = queryView->currentIndex();
    QModelIndex fileIndex = filesTable->currentIndex();
    if(!recIndex.isValid() || !fileIndex.isValid())
        return;
    TrkToolRecord *rec = recordOnIndex(recIndex);
    if(!rec)
        return;
    QString fileName = filesTable->item(fileIndex.row(),0)->text();
    QString tempFile = QDir::temp().absoluteFilePath(fileName);
    rec->saveFile(fileIndex.row(), tempFile);
    previewFile(tempFile);
}

void QueryPage::on_actionTest_triggered()
{
    TrkToolRecord *rec = currentRecord();
    if(!rec)
        return;
    int id = rec->recordId();
    openRecordId(id);
}

void QueryPage::on_actionSelectTrigger_triggered()
{
    foreach(TrkToolRecord *rec, selectedRecords())
    {
        rec->setSelected(!rec->isSelected());
    }
    queryView->selectRow(queryView->currentIndex().row()+1);
}

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
