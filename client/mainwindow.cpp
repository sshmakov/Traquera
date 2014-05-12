#include "mainwindow.h"
//#include "planfiles.h"
#include "database.h"
#include "tracker.h"
#include "querypage.h"
//#include "projectpage.h"
//#include "dialog.h"
#include "trklogin.h"
#include "filterpage.h"
#include "idinput.h"
#include "gsmodel.h"
#include "modifypanel.h"
#include "messager.h"
#include "ttutils.h"
#include "ttglobal.h"
#include "unionmodel.h"
#include "ttfolders.h"

#include <QtGui>
#include <QtSql>
#include <QtXml>
#include <QVariant>
#include <QSize>
#include <QAction>
#include <QPrintPreviewDialog>
#include <QShortcut>
#include "preview.h"
//#include "planfilesform.h"
#include <QNetworkRequest>
#include <QtWebKit>
#include "ttrecwindow.h"

extern int uniqueAlbumId;
extern int uniqueArtistId;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
    statusLine = new QLabel(this);
    am = new QNetworkAccessManager(this);
    connect(am,SIGNAL(finished(QNetworkReply*)),SLOT(finishedSearch(QNetworkReply*)));
    //solrUrl = "http://localhost:8983/solr/collection1/select?q=%1&fl=Id_i&wt=xml&defType=edismax&qf=Description_t+note_txt&stopwords=true&lowercaseOperators=true";
    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(false);
    progressBar->setMaximumHeight(10);
    progressBar->setMaximumWidth(100);
    progressBar->setValue(50);
    progressBar->setVisible(false);
    progressLevel = 0;
    statusBar()->addPermanentWidget(progressBar);
    statusBar()->addPermanentWidget(statusLine);
    //splitterMain->setStretchFactor(1,2);
    //QSizePolicy policy = tabWidget->sizePolicy();
    //policy.setHorizontalStretch(1);
    //tabWidget->setSizePolicy(policy);
    ttglobal()->mainWindow = this;
    sysMessager = new Messager(this);
    //toolBox->setCurrentIndex(0);
    journal = 0;
    //setWindowTitle("TraQuera");
#ifdef DECORATOR
    decorator = new TrkDecorator(this);
#endif
    trkdb = new TrkToolDB(this);
	trkproject = 0;
    selectionTimer = new QTimer(this);
    selectionTimer->setSingleShot(true);
    selectionTimer->setInterval(0);
    connect(selectionTimer,SIGNAL(timeout()),this,SLOT(refreshSelection()));
    ttglobal()->loadPlugins();
	initProjectModel();
    makeMenus();
    treeModel = new UnionModel(this);
    treeView->setModel(treeModel);
    //btnQryId->setMenu(menuId);

    //file = albumDetails;
    //readAlbumData();

	connect(actionOpen_Tracker,SIGNAL(activated()),this,SLOT(readQueries()));
	//connect(actionOpen_Project,SIGNAL(activated()),this,SLOT(openProject()));
    //connect(actionOpen_Linked_Project,SIGNAL(activated()),this,SLOT(openLinkedPlan()));

    //connect(actionQueryByIdClip,SIGNAL(activated()),this,SLOT(openQueryById()));

	//connect(actionRead_Filters,SIGNAL(activated()),this,SLOT(readFilters()));
    connect(treeView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(on_actionOpen_Query_triggered()));
    //connect(queriesView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openQuery(QModelIndex)));
    //connect(planListView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(openProject(QModelIndex)));

    tabWidget->setTabsClosable(false);
    QToolButton *closeBtn = new QToolButton(this);
    closeBtn->setIcon(QIcon(":/images/closetab_1.png"));
    closeBtn->setShortcut(QKeySequence(tr("Ctrl+F4","Tab|Close")));
    closeBtn->setAutoRaise(true);
    tabWidget->setCornerWidget(closeBtn);

	connect(tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(closeBtn,SIGNAL(clicked()),this,SLOT(closeCurTab()));

    //createConnection();
	//trkdb = new TrkDb();
	/*
	trkdb = TrkConnect::connectTracker();
	if(!trkdb)
		return;
	tabWidget->setCurrentIndex(tabWidget->insertTab(0, new QueryPage(), "Query"));
	*/
	loadSettings();
	connect(readProjectsBtn, SIGNAL(clicked()),this,SLOT(readProjects()));
	connect(connectButton, SIGNAL(clicked()),this,SLOT(connectTracker()));
	connect(trustedUserBox, SIGNAL(stateChanged(int)), this, SLOT(trustChanged(int)));
    //readFilters();
    readModifications();
    setupToolbar();
    treeView->addAction(actionOpen_Query);
    //Preview *prev = new Preview(this);
    //prev->setSourceFile("C:/1/Doc1.doc");
}

const QString MainWindowGeometry = "MainWindowGeometry";
const QString MainWindowState = "MainWindowState";

MainWindow::~MainWindow()
{
    if(journal)
        delete journal;
     /*
    if(trkproject)
        delete trkproject;
    if(trkdb)
        delete trkdb;
        */
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ttglobal()->settings()->setValue(MainWindowState, saveState());
    ttglobal()->settings()->setValue(MainWindowGeometry, saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings *settings = ttglobal()->settings();
    dbmsEdit->setText(settings->value("TrackerDBMSType",dbmsEdit->text()).toString());
    serverEdit->setText(settings->value("TrackerSQLServer",serverEdit->text()).toString());
	sqlUserEdit->setText(settings->value("TrackerSQLUser",sqlUserEdit->text()).toString());
	sqlPassEdit->setText(settings->value("TrackerSQLPass",sqlPassEdit->text()).toString());
	trustedUserBox->setChecked(settings->value("TrackerSQL",trustedUserBox->isChecked()).toBool());
	trustChanged(trustedUserBox->checkState());
	projectEdit->setText(settings->value("TrackerProject",projectEdit->text()).toString());
	userEdit->setText(settings->value("TrackerUser",userEdit->text()).toString());
    /*
	settings->beginGroup("Plans");
	{
		QStringList keys=settings->childKeys();
		for(int i=0; i<keys.count(); i++)
		{
            const QString &key = keys[i];
            if(!key.contains(".RO"))
            {
                QString prj = settings->value(key).toString();
                bool readOnly = settings->value(key+".RO",false).toBool();
                projects->addPlan(prj, readOnly);
            }
		}
	}
	settings->endGroup();
    */
    if(settings->contains(MainWindowGeometry))
        restoreGeometry(settings->value(MainWindowGeometry).toByteArray());
    if(settings->contains(MainWindowState))
        restoreState(settings->value(MainWindowState).toByteArray());
//    solrUrl = settings->value("TrackerIndex", solrUrl).toString();
}

void MainWindow::saveSettings()
{
    QSettings *settings = ttglobal()->settings();
    settings->setValue("TrackerDBMSType",dbmsEdit->text());
    settings->setValue("TrackerSQLServer",serverEdit->text());
	settings->setValue("TrackerSQLUser",sqlUserEdit->text());
	settings->setValue("TrackerSQLPass",sqlPassEdit->text());
	settings->setValue("TrackerSQL",trustedUserBox->isChecked());
	settings->setValue("TrackerProject",projectEdit->text());
	settings->setValue("TrackerUser",userEdit->text());
//    settings->setValue("TrackerIndex", solrUrl);
    /*
	settings->beginGroup("Plans");
	{
		QStringList keys=settings->childKeys();
		for(int i=0; i<keys.count(); i++)
			settings->remove(keys[i]);
		for(int i=0; i<projects->plans.count(); i++)
        {
			settings->setValue(QString::number(i),projects->plans[i].file);
            settings->setValue(QString::number(i) + ".RO", projects->plans[i].readOnly);
        }
	}
    settings->endGroup();
    */
}

void MainWindow::showProgressBar()
{
    progressBar->setVisible(true);
    progressLevel++;
}

void MainWindow::hideProgressBar()
{
    if(--progressLevel <= 0)
        progressBar->setVisible(false);
}

void MainWindow::makeMenus()
{
    QMenu *res;
    res = new QMenu(this);
	res->addAction(actionOpen_Query);
	res->addAction(actionOpen_QueryInNew);
    menuQueryList = res;

    res = new QMenu(this);
    res->addAction(actionQueryById);
    res->addAction(actionQueryByIdClip);
    menuId = res;
}

void MainWindow::setupToolbar()
{
    connect(actionSendEmail, SIGNAL(activated()), this, SLOT(sendEmail()));
    openIdEdit = new QComboBox(this);
    openIdEdit->setEditable(true);
    openIdEdit->setAutoCompletion(false);
    openIdEdit->setInsertPolicy(QComboBox::NoInsert);
    openIdEdit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Maximum);

    //Добавляем строку поиска по Id
    toolBar_2->addSeparator();
    QLabel *idLabel = new QLabel(tr("&Id или текст: "),this);
    toolBar_2->addWidget(idLabel);
    toolBar_2->addWidget(openIdEdit);
    idLabel->setBuddy(openIdEdit);
    // Добавляем кнопку открытия
    QToolButton *tb = new QToolButton(this);
    tb->setText(tr("Открыть"));
    tb->setPopupMode(QToolButton::MenuButtonPopup);
    toolBar_2->addWidget(tb);
    connect(tb,SIGNAL(clicked()),this,SLOT(idEntered()));
    // К кнопке добавляем popup-меню
    QMenu *tbMenu =new QMenu(tb);
    QAction *a = new QAction(tr("Открыть в новой вкладке"),tb);
    connect(a,SIGNAL(triggered()),this,SLOT(idEnteredNewPage()));
    tbMenu->addAction(a);
    a = new QAction(tr("Добавить"),tb);
    connect(a,SIGNAL(triggered()),this,SLOT(slotAppendRecordsId()));
    //tbMenu->addAction(a);
    QToolButton *add = new QToolButton(this);
    add->setDefaultAction(a);
    toolBar_2->addWidget(add);
    a = new QAction(tr("Убрать"),tb);
    connect(a,SIGNAL(triggered()),this,SLOT(slotRemoveRecordsId()));
    //tbMenu->addAction(a);
    QToolButton *del = new QToolButton(this);
    del->setDefaultAction(a);
    toolBar_2->addWidget(del);
    tb->setMenu(tbMenu);
    QShortcut *enter;
    enter = new QShortcut(QKeySequence(QKeySequence::InsertParagraphSeparator),openIdEdit,0,0,Qt::WidgetShortcut);
    connect(enter,SIGNAL(activated()),this,SLOT(idEntered()));

    /*
    enter = new QShortcut(QKeySequence(Qt::Key_Return),openIdEdit,0,0,Qt::WidgetShortcut);
    connect(enter,SIGNAL(activated()),this,SLOT(idEntered()));
    */
    enter = new QShortcut(QKeySequence(Qt::Key_Enter),openIdEdit,0,0,Qt::WidgetShortcut);
    connect(enter,SIGNAL(activated()),this,SLOT(idEntered()));

    //connect(tabWidget,SIGNAL(currentChanged(int)),this
    openIdEdit->setContextMenuPolicy(Qt::ActionsContextMenu);
    openIdEdit->addAction(actionPasteNumbers);
    openIdEdit->addAction(actionOpenIds);
}

void MainWindow::sendEmail()
{
	QueryPage *qpage = qobject_cast<QueryPage *>(tabWidget->currentWidget());
	if(!qpage)
		return;
    //QItemSelectionModel *is=qpage->queryView->selectionModel();
    //QModelIndexList ii = is->selectedRows();
    qpage->sendEmail(qpage->selectedRecords());
}

void MainWindow::setDbmsType()
{
    QObject *s=sender();
    QAction *a=qobject_cast<QAction*>(s);
    if(a)
    {
        dbmsEdit->setText(a->text());
    }
}

void MainWindow::setProject()
{
	QObject *s=sender();
	QAction *a=qobject_cast<QAction*>(s);
	if(a)
	{
		projectEdit->setText(a->text());
	}
}

void MainWindow::trustChanged(int State)
{
	if(State == Qt::Checked)
	{
		sqlUserEdit->setEnabled(false);
		sqlPassEdit->setEnabled(false);
	}
	else
	{
		sqlUserEdit->setEnabled(true);
		sqlPassEdit->setEnabled(true);
    }
}

void MainWindow::saveIdsToList(const QString &list)
{
    int index = openIdEdit->findText(list,Qt::MatchExactly);
    if(index != -1)
        openIdEdit->removeItem(index);
    openIdEdit->insertItem(0,list);
    openIdEdit->setCurrentIndex(-1);
    openIdEdit->clearEditText();
}

void MainWindow::idEntered()
{
    QString s = openIdEdit->currentText().trimmed();
    if(s.isEmpty())
        return;
    bool ok;
    s.left(1).toInt(&ok);
    if(ok)
        openQueryById(s);
    else
        findTrkRecords(s);
    saveIdsToList(s);
}

void MainWindow::idEnteredNewPage()
{
    QString s = openIdEdit->currentText().trimmed();
    if(s.isEmpty())
        return;
    bool ok;
    s.left(1).toInt(&ok);
    if(ok)
        openQueryById(s,false);
    else
        findTrkRecords(s,false);
    saveIdsToList(s);
}

void MainWindow::slotAppendRecordsId()
{
    QueryPage *page = curQueryPage();
    if(!page)
        return;
    QString s = openIdEdit->currentText().trimmed();
    QList<int> list = stringToIntList(s);
    foreach(int id, list)
        page->appendId(id);
    saveIdsToList(s);
}

void MainWindow::slotRemoveRecordsId()
{
    QueryPage *page = curQueryPage();
    if(!page)
        return;
    QString s = openIdEdit->currentText().trimmed();
    QList<int> list = stringToIntList(s);
    foreach(int id, list)
        page->removeId(id);
    saveIdsToList(s);
}

void MainWindow::applyChanges()
{
    QueryPage *qpage = curQueryPage();
    if(!qpage)
        return;
    lastChanges = modifyPanel->changes();
    if(lastChanges.isEmpty())
        return;
    foreach(QObject *obj,qpage->selectedRecords())
    {
        TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(obj);
        if(!rec)
            continue;
        if(rec->updateBegin())
        {
            foreach(const QString &fieldName, lastChanges.keys())
                rec->setValue(fieldName, lastChanges.value(fieldName), Qt::EditRole);
            if(!rec->commit())
                rec->cancel();
        }
    }
    modifyPanel->resetAll();
    modifyPanel->selectedRecordsChanged(qpage->selectedRecords());
}

void MainWindow::repeatLastChanges()
{
    modifyPanel->setChanges(lastChanges);
}

void MainWindow::slotOpenRecordsClicked(ScrSet res)
{
    openQueryById(res.toList(),intListToString(res.toList()),false);
}

QString MainWindow::getTrkConnectString()
{
	QString dsn = "Driver={SQL Server};Server="+serverEdit->text()+";";
	if(trustedUserBox->isChecked())
		dsn += "Trusted_Connection=yes";
	else
		dsn += "User Id=" + sqlUserEdit->text()+";"
			+ "Password=" + sqlPassEdit->text()+";";
	return dsn;
}

void MainWindow::readDbms()
{
    QStringList dbmsList = trkdb->dbmsTypes();
	QMenu *menu=new QMenu();
    for(int i=0; i<dbmsList.count(); i++)
	{
        menu->addAction(dbmsList[i],this, SLOT(setDbmsType()));
	}
	menu->popup(QCursor::pos());
    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
}

void MainWindow::readProjects()
{
    trkdb->dbmsName = serverEdit->text().trimmed();
    trkdb->dbmsUser = sqlUserEdit->text();
    trkdb->dbmsPassword = sqlPassEdit->text();
    QStringList projects = trkdb->projects(dbmsEdit->text());
    QMenu *menu=new QMenu();
    for(int i = 0; i< projects.count(); i++)
    {
        menu->addAction(projects[i],this, SLOT(setProject()));
    }
    menu->popup(QCursor::pos());
    connect(menu,SIGNAL(aboutToHide()),this,SLOT(deleteTheObject()));
}

void MainWindow::deleteTheObject()
{
    sender()->deleteLater();
}

void MainWindow::curSelectionChanged()
{
    selectionTimer->start(50);
}

void MainWindow::refreshSelection()
{
    QueryPage *page = curQueryPage();
    if(!page)
        return;
    modifyPanel->setModel(page->tmodel);
    modifyPanel->selectedRecordsChanged(page->selectedRecords());
    calcCountRecords();
}

void MainWindow::finishedSearch(QNetworkReply *reply)
{
    if(reply->error())
    {
        ttglobal()->showError("No connect to index");
    }
    else
    {
        QString query(reply->request().rawHeader("QueryString").constData());
        bool reuse = QVariant(reply->request().rawHeader("ReuseWindow")).toBool();
        if(query.isEmpty())
            query = tr("Search");
        QXmlInputSource source(reply);
        QDomDocument dom;
        if(!dom.setContent(&source,false))
            ttglobal()->showError("Query error");
        else
        {
            QDomElement resDoc = dom.documentElement();
            if(!resDoc.isNull())
            {
                QDomElement result = resDoc.firstChildElement("result");
                ScrSet ids;
                for(QDomElement doc = result.firstChildElement("doc");
                    !doc.isNull();
                    doc = doc.nextSiblingElement("doc"))
                {
                    QDomElement idNode = doc.firstChildElement("int");
                    QString s = idNode.text();
                    bool ok;
                    int id = s.toInt(&ok);
                    if(!ok)
                        continue;
                    ids << id;
                }
                openQueryById(ids.toList(),query,reuse);
            }
        }
    }
    hideProgressBar();
}

void MainWindow::addPropWidget(QWidget *widget)
{
    verticalLayout->addWidget(widget);
}

void MainWindow::calcCountRecords()
{
    QString s;
    QueryPage *page = curQueryPage();
    if(page)
    {
        s = QString(tr("Запросов: %1")).arg(page->queryView->model()->rowCount());
        QObjectList list = page->selectedRecords();
        if(list.count())
            s = QString(tr("Выделено: %1 ")).arg(list.count()) + s;
    }
    statusLine->setText(s);
}

void MainWindow::connectTracker()
{
    QString dbType = dbmsEdit->text().trimmed();
    QString server = serverEdit->text().trimmed();
	QString user = userEdit->text();
	QString project = projectEdit->text().trimmed();
	QString password = passwordEdit->text().trimmed();
	QString dbmsUser = sqlUserEdit->text();
	QString dbmsPass = sqlPassEdit->text();
	
	if(!trkdb)
		trkdb = new TrkToolDB(this);
    trkdb->dbmsUser = dbmsUser;
    trkdb->dbmsPassword = dbmsPass;
    trkdb->dbmsName = server;
    //const QStringList *dbmsTypes = trkdb->dbmsTypes();
	//return;


    if(trkproject)
    {
        delete trkproject;
        trkproject = NULL;
    }
    if(!journal)
    {
        journal = new TrkHistory(this);
        journal->setUnique(true);
        journalView->setModel(journal);
    }
    journal->clearRecords();

    trkproject=trkdb->openProject(
                dbType,
                project,
                user,
                password);
    if(trkproject && trkproject->isOpened())
    {
        saveSettings();
        journal->setProject(trkproject);
        readQueries();
        //connectButton->setDisabled(true);
    }
	/*
	if(!trkdb)
		trkdb=new TrkDb;
	if(trkdb->openProject(getTrkConnectString(),project,user))
		saveSettings();
		*/
}


void MainWindow::readQueries()
{
    //QAbstractItemModel *qryModel = trkproject->createProxyQueryModel(TrkQryFilter::All, this);
    //queriesView->setModel(qryModel);

    /*
    SortGroupModel *model = new SortGroupModel(this);
    model->setGroupColumn(1);
    model->setSourceModel(qryModel);
    queriesView->setModel(model);
    queriesView->setModelColumn(0);
    */

    treeModel->clear();

    TTFolderModel *folders = new TTFolderModel(this);
    QSqlDatabase db = ttglobal()->userDatabase();
    folders->setDatabaseTable(db,"folders","user");
    treeModel->appendSourceModel(folders,tr("Личные папки"));

    QAbstractItemModel *userModel = trkproject->createProxyQueryModel(TrkQryFilter::UserOnly, this);
    //queriesView->setModel(userModel);
    treeModel->appendSourceModel(userModel,tr("Личные выборки"));

    QAbstractItemModel *publicModel = trkproject->createProxyQueryModel(TrkQryFilter::PublicOnly, this);
    treeModel->appendSourceModel(publicModel,tr("Общие выборки"));

    treeModel->setMaxColCount(1);

	/*
	QSqlQueryModel *m= new QSqlQueryModel(this);
	trkmodels.append(m);
	m->setQuery(trkdb->queryQueries,trkdb->db);  
	//queriesView->ins
	queriesView->setModel(m);
	queriesView->setModelColumn(4);
	*/
}

QString minTitle(const QString &title)
{
    if(title.size()>35)
        return title.left(27)+"...";
    return title;
}

void MainWindow::openQuery(const QModelIndex &index, bool reusePage)
{
    if(!index.isValid() || !index.model())
		return;
	//int id = index.data(Qt::UserRole).toInt();
	QString title = index.data(Qt::DisplayRole).toString().trimmed();

    QueryPage *page=NULL; // = qobject_cast<QueryPage *>(w);
    if(reusePage)
    {
        QObject *w = tabWidget->currentWidget();
        page = qobject_cast<QueryPage *>(w);
    }
    if(page)
        tabWidget->setTabText(tabWidget->currentIndex(),minTitle(title));
    else
        page=createNewPage(minTitle(title));

	//QAbstractItemModel *model = trkproject->openQueryModel(title);
	page->openQuery(trkproject, title);
	//page->setQueryModel(model);
	//page->setQuery(id, trkdb);
	// page->setPlanModel(&planModel);
	/*
	QString s = trkdb->getQueryById(id);
	page->setQuery(s,trkdb->db);
	*/
	/*

		QMessageBox::critical(0, "Select",
		s, QMessageBox::Ok);
	QSqlQueryModel *m = new QSqlQueryModel(this);
	trkmodels.append(m);
	m->setQuery(s,trkdb.db);  
	ui.queryView->setModel(m);
    */
}

void MainWindow::showCurrentPlan()
{
    QItemSelectionModel *is=projectTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=0; i<ii.count(); i++)
    {
        showPlan(ii[i]);
    }
}

void MainWindow::openQueryByIdClip()
{
	QString numbers=QApplication::clipboard()->text().simplified();
	//QObject *w = tabWidget->currentWidget();
    openQueryById(numbers);
}

void MainWindow::openQueryById(const QString &numbers, bool reusePage)
{
    QueryPage *page=NULL; // = qobject_cast<QueryPage *>(w);
    QObject *w = tabWidget->currentWidget();
    page = qobject_cast<QueryPage *>(w);
    if(page && reusePage)
        tabWidget->setTabText(tabWidget->currentIndex(),minTitle(numbers));
    else
        page = createNewPage(minTitle(numbers));
    showProgressBar();
    page->openIds(trkproject, numbers, numbers);
    hideProgressBar();
}

void MainWindow::openQueryById(const QList<int> &idList, const QString &title, bool reusePage)
{
    QueryPage *page = curQueryPage(); // = qobject_cast<QueryPage *>(w);
    QString newTitle = title;
    if(newTitle.isEmpty())
        newTitle = intListToString(idList);
    if(!page || !reusePage)
        page = createNewPage(minTitle(newTitle));
    else
        tabWidget->setTabText(tabWidget->currentIndex(),minTitle(newTitle));
    showProgressBar();
    page->openIds(trkproject,idList,title);
    hideProgressBar();
}

QueryPage *MainWindow::createNewPage(const QString &title)
{
	QueryPage *page=new QueryPage();
    tabWidget->setCurrentIndex(tabWidget->insertTab(tabWidget->count()-1, page, QIcon(":/images/trackerTable.png"), title));
    //page->setPlanModel(&planModel);
	connect(page,SIGNAL(changedQuery(QString,QString)),this,SLOT(on_changedQuery(QString,QString)));
    connect(page,SIGNAL(selectionRecordsChanged()),this,SLOT(curSelectionChanged()));
    connect(page,SIGNAL(openRecordsClicked(ScrSet)),this,SLOT(slotOpenRecordsClicked(ScrSet)));
    //connect(page,SIGNAL(showTaskInPlanClicked(QString,int)),this,SLOT(showPlanTask(QString,int)));
    //connect(page,SIGNAL(selectionRecordsChanged(QList<TrkToolRecord*>)),this,SLOT(refreshSelection()));
    return page;
}

/* to plugin
ProjectPage *MainWindow::openPlanPage(const QString& fileName)
{
    for(int i=0; i<tabWidget->count(); i++)
    {
        ProjectPage *p = qobject_cast<ProjectPage*>(tabWidget->widget(i));
        if(p && p->prjmodel.prjmodel->fileName == fileName)
        {
            tabWidget->setCurrentWidget(p);
            return p;
        }
    }
    PrjItemModel *model;
    if(!projects->models.contains(fileName))
        return 0;
    model = projects->models[fileName];
    ProjectPage *page;
    page=new ProjectPage(model,this);
    int newtab=tabWidget->insertTab(tabWidget->count()-1, page, QIcon(":/images/plan.png"), "Project");
    tabWidget->setCurrentIndex(newtab);
    tabWidget->show();
    QString prjName = page->projectName();
    if(!prjName.isEmpty())
        tabWidget->setTabText(newtab,prjName);
    return page;

}
*/

void MainWindow::findTrkRecords(const QString &line, bool reuse)
{
    QFile file("data/init.xml");
    QXmlInputSource source(&file); // new???
    QDomDocument dom;
    if(!dom.setContent(&source,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull()) return;
    QDomElement trindex = doc.firstChildElement("trindex");
    if(trindex.isNull())
        return;
    QString href = trindex.attribute("solr","http://localhost:8983/solr/collection1/select");
    if(href.isEmpty())
        return;
    QDomElement params = trindex.firstChildElement("params");
    QUrl solrUrl(href);
    QString prj = trkproject->name;
    prj.replace(" ","_");
    QDomNamedNodeMap pmap = params.attributes();
    for(int i=0; i<pmap.count(); i++)
    {
        QDomNode n = pmap.item(i);
        QString value = n.nodeValue();
        value.replace("{project}",prj);
        solrUrl.addQueryItem(n.nodeName(), value);
    }
    QString queryText = line;
    queryText.replace(' ',"+");
    solrUrl.addQueryItem("q",queryText);

    QNetworkRequest req;
    req.setUrl(solrUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    req.setRawHeader("QueryString", line.toLocal8Bit().constData());
    req.setRawHeader("ReuseWindow", QVariant(reuse).toString().toLocal8Bit().constData());
    showProgressBar();
    am->post(req,line.toLocal8Bit().constData());
}

QueryPage *MainWindow::curQueryPage()
{
    return qobject_cast<QueryPage *>(tabWidget->currentWidget());
}

//void MainWindow::showPlan(bool linked)
//{
//	QItemSelectionModel *is=projectTableView->selectionModel();
//	QModelIndexList ii = is->selectedRows();
//	for(int i=0; i<ii.count(); i++)
//	{
//        showPlan(ii[i],linked);
//	}
//}

void MainWindow::showPlan(const QModelIndex &index)
{
    return;
    /* for plan plugin
     *
	QString fileName;
	Plan & plan = projects->plans[index.row()];
	fileName = plan.file;
    openPlanPage(fileName);
    */
}

/* to plugin
void MainWindow::showPlanTask(const QString &fileName, int taskNum)
{
    ProjectPage *page = openPlanPage(fileName);
    if(page && taskNum)
        page->goToTask(taskNum);
}
*/

//void MainWindow::openLinkedPlan()
//{
//	QObject *w = tabWidget->currentWidget();
//	QueryPage *qpage = qobject_cast<QueryPage *>(w);
//	if(!qpage)
//		return;
//    showPlan(true);
//}


void MainWindow::readFilters()
{
    //toolBox->addItem(new FilterPage(this), tr("Фильтры"));
}

void MainWindow::readModifications()
{
    //toolBox->addItem(modifyPanel, tr("Свойства"));
    modifyPanel = new ModifyPanel(this);
    connect(modifyPanel,SIGNAL(applyButtonPressed()),this,SLOT(applyChanges()));
    connect(modifyPanel,SIGNAL(repeatButtonClicked()),this,SLOT(repeatLastChanges()));
    dockPropsContents->layout()->addWidget(modifyPanel);
}

void MainWindow::closeTab(int index)
{
    QObject *w;
    w = tabWidget->widget(index);
	QueryPage *qpage = qobject_cast<QueryPage *>(w);
    //ProjectPage *ppage = qobject_cast<ProjectPage *>(w);
    if(!qpage //&& !ppage
            )
		return;
	delete w;
    //tabWidget->removeTab(index);
}

void MainWindow::closeCurTab()
{
    int index = tabWidget->currentIndex();
    closeTab(index);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("О программе"),
            tr("Работает..."));
}

void MainWindow::initProjectModel()
{
    /*
     *  for plan plugin
     *
    projects = new PlanFilesModel(this);
    connect(projects,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(saveSettings()));
	projectTableView->setModel(projects);
    QHeaderView *hv=projectTableView->horizontalHeader();
    hv->setResizeMode(QHeaderView::ResizeToContents);
    connect(projectAddBtn,SIGNAL(clicked()),this,SLOT(addPlanFile()));
    connect(projectOpenBtn,SIGNAL(clicked()),this,SLOT(addMSProjectFile()));
    connect(projectDelBtn,SIGNAL(clicked()),this,SLOT(delCurrentPlan()));
    connect(projectLoadBtn,SIGNAL(clicked()),this,SLOT(loadCurrentPlan()));
    connect(projectShowBtn,SIGNAL(clicked()),this,SLOT(showCurrentPlan()));
    */
}

void MainWindow::loadPlugins()
{
    /* to TTGlobal
    planPlugin = new PlansPlugin(this);
    if(!QMetaObject::invokeMethod(planPlugin, "setGlobalObject",
                                  Q_ARG(QObject *,ttglobal())
                                  ))
        return;
    if(!QMetaObject::invokeMethod(planPlugin, "initPlugin"))
        return;
    QWidget *prop=0;
    if(!QMetaObject::invokeMethod(planPlugin, "getPropWidget",
                                  Q_RETURN_ARG(QWidget *,prop),
                                  Q_ARG(QWidget *, this)))
        return;
    verticalLayout->addWidget(prop);
    */
}

/*
 * for plan plugin
 *
void MainWindow::addPlanFile()
{
	QString selectedFilter;
	QStringList fileNames= QFileDialog::getOpenFileNames(this,
		tr("Добавить план"), 
		".", 
		tr(
			"Проекты Microsoft Project (*.mpp);;"
			"Книги Microsoft Excel (*.xls)"
		),
		&selectedFilter
		);
	for(int f=0; f<fileNames.count(); f++)
	{
        projects->addPlan(fileNames[f],false);
	}
	saveSettings();
	//((QStandardItemModel *)projects)->appendRow(new QStandardItem(fileName));
}

void MainWindow::delCurrentPlan()
{
	QItemSelectionModel *is=projectTableView->selectionModel();
	QModelIndexList ii = is->selectedRows();
	for(int i=ii.count()-1; i>=0; i--)
	{
		QString file = ii[i].data().toString();
		projects->removeRow(ii[i].row());
	}
	projectTableView->clearSelection();
	saveSettings();
}

void MainWindow::loadCurrentPlan()
{
	QItemSelectionModel *is=projectTableView->selectionModel();
	QModelIndexList ii = is->selectedRows();
	for(int i=0; i<ii.count(); i++)
	{
		projects->loadPlan(ii[i].row());
	}
}

void MainWindow::loadAllProjects()
{
}

void MainWindow::checkAutoProjects()
{
}

void MainWindow::addMSProjectFile()
{
	PrjItemModel *model = new PrjItemModel(this);
    if(model->open("",false))
	{
        projects->addPlan(model->fileName,model->isReadOnly);
		planModel.addPrjModel(model);
		projects->models[model->fileName]=model;
        saveSettings();
	}
	else
		delete model;
}
*/

/*
QMenu MainWindow::planMenu(bool forLink)
{
	QMenu res(this);
	for(int i=0; i<projects->plans.count(); i++)
	{
		Plan &plan = projects->plans[i];
		QAction *a=res.addAction(plan.file);
		a->setData(QVariant(i));
		if(forLink)
			connect(a,SIGNAL(activated()),this, SLOT(openLinkedProject()));
		else
			connect(a,SIGNAL(activated()),this, SLOT(openProject()));
	}
	return res;
}
*/

void MainWindow::on_actionPrevious_triggered()
{
    QObject *w = tabWidget->currentWidget();
    QueryPage *qpage = qobject_cast<QueryPage *>(w);
    if(!qpage)
        return;
	qpage->goBack();
}

void MainWindow::openCurItem(bool reuse)
{
    /*
    if(queriesView->isVisible())
    {
        QItemSelectionModel *selmodel = queriesView->selectionModel();
        QModelIndexList list = selmodel->selectedIndexes();
        if(!list.count())
            return;
        openQuery(list.at(0), true);
        return;
    }
    */
    if(treeView->isVisible())
    {
        QModelIndex index = treeView->currentIndex();
        if(!index.isValid())
            return;
        bool isMainGroup = !index.parent().isValid();
        QAbstractItemModel *model = treeModel->sourceModel(index);
        TrkQryFilter *qryModel = qobject_cast<TrkQryFilter*>(model);
        TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
        if(qryModel && !isMainGroup)
        {
            openQuery(index, reuse);
            return;
        }
        if(folderModel && !isMainGroup)
        {
            index = treeModel->mapToSource(index);
            if(!index.isValid())
                return;

            QueryPage *page = curQueryPage(); // = qobject_cast<QueryPage *>(w);
            QString folderName = index.sibling(index.row(),0).data().toString();
            int folderId = index.sibling(index.row(),1).data().toInt();
            if(folderName.isEmpty())
                folderName = tr("Папка");
            if(!page || !reuse)
                page = createNewPage(folderName);
            page->openFolder(trkproject,folderModel->folder(index));
            tabWidget->setTabText(tabWidget->currentIndex(),folderName);
        }
    }
}

void MainWindow::on_actionOpen_Query_triggered()
{
    openCurItem(true);
}
void MainWindow::on_queriesView_customContextMenuRequested(const QPoint &pos)
{
    //menuQueryList->popup(queriesView->mapToGlobal(pos));
}

void MainWindow::on_actionOpen_QueryInNew_triggered()
{
    openCurItem(false);
    /*
    QItemSelectionModel *selmodel = queriesView->selectionModel();
    QModelIndexList list = selmodel->selectedIndexes();
    if(!list.count())
        return;
    openQuery(list.at(0), false);
    */
}

void MainWindow::on_changedQuery(const QString & /*projectName*/, const QString & queryName)
{
    QueryPage *qpage = qobject_cast<QueryPage *>(sender());
    if(!qpage)
        return;
    int ind = tabWidget->indexOf(qpage);
    QString newTitle = minTitle(queryName);
    tabWidget->setTabText(ind, newTitle);
}

void MainWindow::on_actionForward_triggered()
{
    QObject *w = tabWidget->currentWidget();
    QueryPage *qpage = qobject_cast<QueryPage *>(w);
    if(!qpage)
        return;
    qpage->goForward();
}

void MainWindow::on_actionQueryById_triggered()
{
    IdInput *id;
    id = new IdInput();
    if(id->exec())
    {
        QString ids =id->getData();
        openQueryById(ids);
    }
}

void MainWindow::on_actionPrint_Preview_triggered()
{
    QObject *w = tabWidget->currentWidget();
    QueryPage *qpage = qobject_cast<QueryPage *>(w);
    if(!qpage)
        return;
    qpage->printPreview();
}

void MainWindow::on_btnDBMS_clicked()
{
    readDbms();
}

void MainWindow::on_journalView_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;
    const TrkHistory *model = qobject_cast<const TrkHistory *>(index.model());
    if(!model)
        return;
    const TrkHistoryItem &item = model->at(index.row());
    QueryPage *qpage = curQueryPage();
    TrkToolProject *prj = trkdb->getProject(item.projectName);
    if(!prj)
        return;
    if(!qpage)
        qpage = createNewPage(item.queryName);
    if(item.isQuery)
        qpage->openQuery(prj, item.queryName, item.rectype);
    else
        qpage->openIds(prj, item.queryName,  item.queryName, item.rectype);
    tabWidget->setTabText(tabWidget->currentIndex(),item.queryName);

}

void MainWindow::on_tabWidget_currentChanged(QWidget *arg1)
{
    QueryPage *qpage = qobject_cast<QueryPage *>(arg1);
    if(!qpage)
        return;
    modifyPanel->setQueryPage(qpage);
}

void MainWindow::on_actionPasteNumbers_triggered()
{
    QClipboard *clip = QApplication::clipboard();
    QString clipText = clip->text();
    if(clipText.isEmpty())
    {
        const QString CSVFORMAT = "application/x-qt-windows-mime;value=\"Csv\"";
        if(clip->mimeData()->hasFormat(CSVFORMAT))
        {
            QByteArray data = clip->mimeData()->data(CSVFORMAT);
            clipText = QString::fromAscii(data.constData());
        }
        else
            return;
    }
    QList<int> list = uniqueIntList(stringToIntList(clipText));
    if(list.isEmpty())
        return;
    QLineEdit *le = openIdEdit->lineEdit();
    if(le)
        le->insert(intListToString(list) + " ");
}

void MainWindow::on_actionNewQueryTab_triggered()
{
    QueryPage *qpage = curQueryPage();
    QueryPage *newpage = createNewPage(tr("Новая вкладка"));
    if(!qpage)
        return;
    //newpage->openIds(qpage->tmodel->prj);
}

void MainWindow::on_actionCloseTab_triggered()
{
    QueryPage *qpage = curQueryPage();
    if(!qpage)
        return;
    qpage->close();
}

void MainWindow::on_actionOpenSelected_triggered()
{
    QueryPage *newpage = createNewPage(tr("Отмеченные"));
    newpage->openModel(trkproject->selectedModel());

}

void MainWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    QPoint gPos = treeView->mapToGlobal(pos);
    QueryPage *qpage = curQueryPage();
    QModelIndex index = treeView->indexAt(pos);
    if(!index.isValid())
        return;
    bool isMainGroup = !index.parent().isValid();
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TrkQryFilter *qryModel = qobject_cast<TrkQryFilter*>(model);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    QMenu menu;
    if(!isMainGroup)
    {
        menu.addAction(actionOpen_Query);
        menu.addAction(actionOpen_QueryInNew);
        menu.addSeparator();
    }
    if(folderModel)
    {
        if(!isMainGroup)
        {
            menu.addAction(actionAddToFolder);
            menu.addAction(actionEditContents);
            menu.addSeparator();
        }
        menu.addAction(actionNewFolder);
        menu.addAction(actionDeleteFolder);
        menu.addAction(actionRenameFolder);
    }
    menu.exec(gPos);
}

void MainWindow::on_actionAddToFolder_triggered()
{
    QueryPage *qpage = curQueryPage();
    if(!qpage)
        return;
    QModelIndex index = treeView->currentIndex();
    if(!index.isValid())
        return;
    bool isMainGroup = !index.parent().isValid();
    if(isMainGroup)
        return;
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    if(!folderModel)
        return;
    QObjectList records = qpage->selectedRecords();
    foreach(const QObject *obj, records)
    {
        const TrkToolRecord *rec = (TrkToolRecord *)obj;
        folderModel->addRecordId(treeModel->mapToSource(index),rec->recordId());
    }
}

void MainWindow::on_actionNewFolder_triggered()
{
    QModelIndex index = treeView->currentIndex();
    if(!index.isValid())
        return;
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    if(!folderModel)
        return;
    bool ok;
    QString title = QInputDialog::getText(this,
                                          tr("Создание новой папки"),
                                          tr("Название новой папки"),
                                          QLineEdit::Normal,
                                          "",
                                          &ok);
    if(!ok)
        return;
    QModelIndex fIndex = treeModel->mapToSource(index);
    QPersistentModelIndex pIndex(fIndex);
    if(!folderModel->insertRow(0,fIndex))
        return;
    QModelIndex nIndex = folderModel->index(0,0,pIndex);
    treeView->setCurrentIndex(treeModel->mapFromSource(nIndex));
    if(!title.isEmpty())
        folderModel->setData(nIndex,title);

    /*
    QPersistentModelIndex pIndex(index);
    if(!treeModel->insertRow(0,index))
        return;
    if(title.isEmpty())
        return;
    QModelIndex nIndex = treeModel->index(0,0,pIndex);
    treeModel->setData(nIndex,title);
    treeView->setCurrentIndex(nIndex);
    */
}

void MainWindow::on_actionDeleteFolder_triggered()
{
    QModelIndex index = treeView->currentIndex();
    if(!index.isValid())
        return;
    bool isMainGroup = !index.parent().isValid();
    if(isMainGroup)
        return;
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    if(!folderModel)
        return;
    treeModel->removeRow(index.row(),index.parent());
}

void MainWindow::on_actionRenameFolder_triggered()
{
    QModelIndex index = treeView->currentIndex();
    if(!index.isValid())
        return;
    bool isMainGroup = !index.parent().isValid();
    if(isMainGroup)
        return;
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    if(!folderModel)
        return;
    bool ok;
    QString title = QInputDialog::getText(this,
                                          tr("Переименовать папку"),
                                          tr("Новое название"),
                                          QLineEdit::Normal,
                                          index.data().toString(),
                                          &ok);
    if(!ok)
        return;
    if(title.isEmpty())
        return;
    treeModel->setData(index,title);
}

void MainWindow::on_actionOpenIds_triggered()
{
    idEntered();
}

void MainWindow::on_dockProps_visibilityChanged(bool visible)
{
    actionViewModify->setChecked(visible);
}

void MainWindow::on_dockQueries_visibilityChanged(bool visible)
{
    actionViewQueriesFolders->setChecked(visible);
}

void MainWindow::on_dockWidget_visibilityChanged(bool visible)
{
    actionViewHistory->setChecked(visible);
}

void MainWindow::on_actionViewQueriesFolders_triggered(bool checked)
{
    dockQueries->setVisible(checked);
}

void MainWindow::on_actionViewHistory_triggered(bool checked)
{
    dockWidget->setVisible(checked);
}

void MainWindow::on_actionViewModify_triggered(bool checked)
{
    dockProps->setVisible(checked);
}

void MainWindow::on_actionRefresh_Query_triggered()
{
    QueryPage *page = curQueryPage();
    if(page)
        page->refreshQuery();
}

void MainWindow::on_actionEditContents_triggered()
{
    QModelIndex index = treeView->currentIndex();
    if(!index.isValid())
        return;
    bool isMainGroup = !index.parent().isValid();
    if(isMainGroup)
        return;
    QAbstractItemModel *model = treeModel->sourceModel(index);
    TTFolderModel *folderModel = qobject_cast<TTFolderModel*>(model);
    if(!folderModel)
        return;
    QModelIndex fIndex = treeModel->mapToSource(index);
    IdInput *dlg = new IdInput(this);
    dlg->setData(intListToString(folderModel->folderContent(fIndex)));
    if(dlg->exec())
    {
        QString res = dlg->getData();
        folderModel->setFolderContent(fIndex,stringToIntList(res));
    }
    delete dlg;
}

/*
 * for plan plugin
 *
void MainWindow::on_actionPlansDialog_triggered()
{
    PlanFilesForm::execute(this, projects);
}
*/

void MainWindow::on_actionNewRequest_triggered()
{
    TrkToolRecord *rec = currentProject()->newRecord();
    TTRecordWindow *win = new TTRecordWindow();
    win->setTypeDef(rec->typeDef());
    win->setRecord(rec);
    win->show();
}
