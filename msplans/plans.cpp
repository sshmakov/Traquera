#include "plans.h"
#include "project.h"
//#include "settings.h"
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QMessageBox>
#include <QHeaderView>
#include <QSettings>
#include <QtGui>
#include "planfilesform.h"
#include "tqplanswidget.h"
#include "projectpage.h"
//#include <ttglobal.h>

Q_EXPORT_PLUGIN2("msprojectplans", PlansPlugin)

PlansPlugin *pluginObject = 0;

PlanModel::PlanModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    QString xml = pluginObject->dataDir.absoluteFilePath("project.xml");
    if(!loadDefinition(xml))
    {
        loadDefinition(":/msplans/data/project.xml");
    }
}

PlanModel::~PlanModel()
{
	while(!plans.isEmpty())
	{
		PrjItemModel *prj = plans.takeFirst();
		delete prj;
	}
}


int	PlanModel::columnCount(const QModelIndex & parent) const
{
	if(parent.isValid())
		return plans[parent.row()]->columnCount();
	else
		return fields.count();
}

int	PlanModel::rowCount(const QModelIndex & parent) const
{
	if(parent.isValid())
	{
		PrjItemModel *prj = plans[parent.row()];
		return prj->rowCount();
	}
	else
		return plans.count();
}

QVariant PlanModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
		return QVariant();
    PrjItemModel *prj = (PrjItemModel *)index.internalPointer();
    if(prj)
    {
        QModelIndex & pi = prj->index(index.row(), index.column());
        return pi.data(role);
    }
    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if(prj)
        {
            QModelIndex & pi = prj->index(index.row(), index.column());
            return pi.data(role);
        }
        else if(index.column()==0)
        {
            prj=plans[index.row()];
            return prj->prjName;
        }
        else if(role == Qt::DisplayRole)
            return QString();
        break;
    case Qt::FontRole:
        if(prj)
        {
            QModelIndex & pi = prj->index(index.row(), index.column());
            return pi.data(role);
        }
        QFont f;
        f.setBold(true);
        return f;
    }
    return QVariant();
}

QVariant PlanModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(section<0 || section >= fields.count())
            return QVariant();
//        if(role == Qt::SizeHintRole)
//            return  QSize(fields[section].colSize, 19);
        if(role != Qt::DisplayRole)
            return QVariant();
        return fields[section].title;
    }
    else
    {
        if(role == Qt::SizeHintRole)
            return 19;
        /*
        QModelIndex *index = this->index(section,0);
        if(!index->isValid())
            return QVariant();
        PrjItemModel *prj = (PrjItemModel *)index.internalPointer();
        switch(role)
        {
        case Qt::DisplayRole:
            if(prj)
            {
                QModelIndex & pi = prj->index(index.row(), index.column());
                return pi.data(role);
            }
            else if(index.column()==0)
            {
                return QVariant();
            }
            else
                return QString();
            break;
        case Qt::FontRole:
            if(prj)
            {
                QModelIndex & pi = prj->index(index.row(), index.column());
                return pi.data(role);
            }
            QFont f;
            f.setBold(true);
            return f;
        }
        */
    }
    return QVariant();
}

QModelIndex	PlanModel::index ( int row, int column, const QModelIndex & parent) const
{
	if(parent.isValid())
	{
		if(!parent.internalPointer())
		{
			void *plan=plans[parent.row()];
			return createIndex(row, column, plan);
		}
		else
			return QModelIndex();
	}
	else if(row<0 || row >=plans.count())
		return QModelIndex();
	else /*if(column)
		return QModelIndex();*/
	return createIndex(row, column, 0);
}

QModelIndex PlanModel::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();
	PrjItemModel *plan = (PrjItemModel *)child.internalPointer();
	if(!plan)
		return QModelIndex();
	int pRow = plans.indexOf(plan);
	if(pRow<0)
		return QModelIndex();
	return createIndex(pRow, 0, 0);
}

bool PlanModel::hasChildren(const QModelIndex & parent) const
{
	return !parent.internalPointer();
}

QSize PlanModel::span(const QModelIndex & index ) const
{
	if(index.column()!=0 || index.parent().isValid())
		return QSize(0,0);
	return QSize(10,1);
}

//-------------------

void PlanModel::addPrjModel(PrjItemModel *plan)
{
	emit layoutAboutToBeChanged();
    plans.append(plan);
	ScrPrj sPrj;
	scrByProject.append(sPrj);
	readScrSet(plan);
    connect(plan,SIGNAL(rowsInserted ( const QModelIndex &, int, int)),this,SLOT(planRowsInserted(QModelIndex,int,int)));
	emit layoutChanged();
}

void PlanModel::readScrSet(PrjItemModel *plan)
{
	int index = plans.indexOf(plan);
	if(index<0)
		return;
	//emit layoutAboutToBeChanged();
	ScrPrj sPrj;
	for(int row=0; row<plan->rowCount(); row++)
	{
   		QModelIndex ind=plan->index(row, SCRfield);
		QString scr = ind.data().toString();
		//QMessageBox::information(0,"scr",scr);
		ScrSet sSet = scrStringToSet(scr);
        sPrj.scrByRow[row] = sSet;
		sPrj.allScrs += sSet;
        int parentTaskID = plan->index(row,0).data(Qt::UserRole).toInt();
        while(parentTaskID > 0)
        {
            sPrj.scrByRow[parentTaskID-1].unite(sSet);
            parentTaskID = plan->index(parentTaskID-1,0).data(Qt::UserRole).toInt();
        }
    }
	scrByProject[index] = sPrj;
	//emit layoutChanged();
}

void PlanModel::readScrSetNewRec(PrjItemModel *plan, int row)
{
    int index = plans.indexOf(plan);
    if(index<0)
        return;
    //emit layoutAboutToBeChanged();
    ScrPrj sPrj = scrByProject[index];
    QModelIndex ind=plan->index(row, SCRfield);
    QString scr = ind.data().toString();
    //QMessageBox::information(0,"scr",scr);
    ScrSet sSet = scrStringToSet(scr);
    sPrj.scrByRow[row] = sSet;
    sPrj.allScrs += sSet;
    scrByProject[index] = sPrj;
    //emit layoutChanged();
}

void PlanModel::setFieldHeaders(QHeaderView *header)
{
    for(int i=0; i<fields.count(); i++)
    {
        const PrjField &f = fields[i];
        header->resizeSection(i,f.colSize);
    }
}

bool PlanModel::loadDefinition(const QString &fileName)
{
    //QXmlSimpleReader xmlReader;
    QFile file(fileName);
    if(!file.exists())
        return false;
    QXmlInputSource source(&file);
	QDomDocument dom;
    if(!dom.setContent(&source,false))
        return false;
	QDomElement doc = dom.documentElement();
    if(doc.isNull()) return false;
	//QDomElement prj = doc.firstChildElement("project");
	//if(prj.isNull()) return;
	QDomElement tasks = doc.firstChildElement("tasks");
    if(tasks.isNull()) return false;
	QDomElement flds = tasks.firstChildElement("fields");
    if(flds.isNull()) return false;
	/*
	QHash<QString, int> hc;
	QDomElement fconsts = tasks.firstChildElement("fconsts");
	for(QDomElement fc = fconsts.firstChildElement("field");
		!fc.isNull();
		fc = fc.nextSiblingElement("field"))
	{
		QString prop = fc.attribute("property");
		bool ok;
		uint cnum = fc.attribute("const").toUInt(&ok);
		if(ok)
			hc[prop.toUpper()] = cnum;
	}
	*/
    parentField = -1;

	QDomElement field = flds.firstChildElement("field");
	for(int i=0; !field.isNull(); field = field.nextSiblingElement("field"), i++)
	{
		PrjField f;
		f.field = field.attribute("property");
		f.title = field.attribute("title");
        f.fieldId = field.attribute("const").toInt();
        /*
        if(f.field.isEmpty() && !f.fieldId && !f.title.isEmpty())
            f.fieldId = app.dynamicCall("FieldNameToFieldConstant(QVariant)",f.title).toInt();
        if(f.title.isEmpty())
        {
            if(f.fieldId)
                f.title = app.dynamicCall("FieldConstantToFieldName(QVariant)",f.fieldId).toString();
            if(f.title.isEmpty())
                f.title = f.field;
        }
        */
        /*
		if(f.title.isEmpty() && hc.contains(f.field.toUpper()))
			f.title = app.dynamicCall("FieldConstantToFieldName(QVariant)",hc[f.field.toUpper()]).toString();
			//f.title = app.dynamicCall("FieldConstantToFieldName(QVariant)",QString("prjTask"+f.field.trimmed())).toString();
		*/
		if(f.title.isEmpty())
			f.title = f.field;
		f.type = field.attribute("type");
		if(!f.type.compare("SCR", Qt::CaseInsensitive))
            SCRfield = i;
        if(!f.type.compare("parent", Qt::CaseInsensitive))
            parentField = i;
		f.internalField = "f"+QString::number(i);
        f.colSize = field.attribute("size", "50").toInt();
        f.display = field.attribute("display");
        fields.append(f);
    }
//    delete file;
    return true;
}

void PlanModel::planChanged()
{
    PrjItemModel *prj = qobject_cast<PrjItemModel*>(sender());
    if(!prj)
        return;
    readScrSet(prj);
}

void PlanModel::planRowsInserted(const QModelIndex &parent, int start, int end)
{
    if(parent.isValid())
        return;
    PrjItemModel *prj = qobject_cast<PrjItemModel*>(sender());
    if(!prj)
        return;
    int parentRow = plans.indexOf(prj);
    if(parentRow <0)
        return;
    emit beginInsertRows(index(parentRow,0),start,end);
    for(int row=start; row<=end;row++)
        readScrSetNewRec(prj, row);
    emit endInsertRows();
}

int PlanModel::columnOfField(const QString& fieldName) const
{
	for(int i=0; i<fields.count(); i++)
		if(!fields[i].title.compare(fieldName,Qt::CaseInsensitive))
			return i;
    return -1;
}

QVariant PlanModel::fieldData(int row, const QString &fieldName, int role) const
{
    int col = columnOfField(fieldName);
    if(col<0)
        return QVariant();
    return index(row,col).data(role);
}

PrjItemModel *PlanModel::addPrjFile(const QString &fileName, bool readOnly)
{
	PrjItemModel *prj=new PrjItemModel(this);
    if(!prj->open(fileName, readOnly))
	{
        QMessageBox::critical(0, tr("Ошибка MSPlans"),
                              tr("Не открылся проект '%1'").arg(fileName));
        delete prj;
		return NULL;
	}
//    QMessageBox::information(0, tr("MSPlans"), tr("Открыт проект '%1'").arg(fileName));
    addPrjModel(prj);
	return prj;
}

bool PlanModel::closePrjModel(PrjItemModel *model)
{
	if(!plans.contains(model))
		return false;
	plans.takeAt(plans.indexOf(model));
	delete model;
	return true;
}

ScrSet PlanModel::listScr(int row, const QModelIndex & parent)
{
	if(parent.isValid())
	{
		ScrPrj &p = scrByProject[parent.row()];
		if(row>=0 && row<p.scrByRow.count())
			return p.scrByRow[row];
		return ScrSet();
	}
	else
	{
		ScrPrj &p = scrByProject[row];
		return p.allScrs;
	}
}

PrjItemModel *PlanModel::prjModel(const QModelIndex &index)
{
	if(!index.isValid())
		return NULL;
	PrjItemModel *plan = (PrjItemModel *)index.internalPointer();
	if(plan)
		return plan;
	return plans[index.row()];
}


PlansPlugin::PlansPlugin(QObject *parent)
    : QObject(parent)
{
    loadedPlans = 0;
    planFiles = 0;
    propWidget = 0;
    settings = 0;
    pluginObject = this;
}

PlansPlugin::~PlansPlugin()
{
    delete loadedPlans;
    delete planFiles;
    pluginObject = 0;
}

void PlansPlugin::initPlugin(QObject *obj, const QString &modulePath)
{
    globalObject = obj;
    mainWindow = 0;
    QMetaObject::invokeMethod(globalObject, "mainWindow", Qt::DirectConnection,
                                  Q_RETURN_ARG(QMainWindow *, mainWindow));
    QFileInfo fi(modulePath);
    pluginModule = fi.absoluteFilePath();
    QDir pDir;
    pDir = fi.absoluteDir();
    if(pDir.dirName().compare("debug",Qt::CaseInsensitive) == 0)
    {
        pDir = QDir(pDir.filePath(".."));
        pDir.makeAbsolute();
    }
    QString abs;
    pluginDir = pDir;
    abs = pDir.absolutePath();
    dataDir = QDir(pluginModule).filePath("data");
    abs = dataDir.absolutePath();
    initProjectModel();
    if(obj)
    {
        QSettings *s;
        if(QMetaObject::invokeMethod(globalObject,"settings",
                                     Q_RETURN_ARG(QSettings *, s)))
            settings = s;
    }
    loadSettings();
    QWidget *prop = getPropWidget();
    bool res;
    QMetaObject::invokeMethod(globalObject,"addPropWidget",
                              Q_RETURN_ARG(bool, res),
                              Q_ARG(QWidget *, prop))
                                 ;
}

void PlansPlugin::initProjectModel()
{
    loadedPlans = new PlanModel(this);
    planFiles = new PlanFilesModel(this);
    /*
    settings->beginGroup("Plans");
    QStringList keys=settings->childKeys();
    for(int i=0; i<keys.count(); i++)
    {
        QString prj = settings->value(keys[i]).toString();
        projects->addPlan(prj);
    }
    settings->endGroup();
    */
    connect(planFiles,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(saveSettings()));

    /*
    filesTableView->setModel(planFiles);
    QHeaderView *hv=filesTableView->horizontalHeader();
    hv->setResizeMode(QHeaderView::ResizeToContents);
    //hv->resizeSection(0, 200);
    //hv->resizeSection(2, 400);
    connect(projectAddBtn,SIGNAL(clicked()),this,SLOT(addPlanFile()));
    connect(projectOpenBtn,SIGNAL(clicked()),this,SLOT(addMSProjectFile()));
    connect(projectDelBtn,SIGNAL(clicked()),this,SLOT(delCurrentPlan()));
    connect(projectLoadBtn,SIGNAL(clicked()),this,SLOT(loadCurrentPlan()));
    connect(projectShowBtn,SIGNAL(clicked()),this,SLOT(showCurrentPlan()));
    */

    /*
    planListView->setModel(projects);
    //planListView->setModelColumn(1);
    QAction *a;
    a = new QAction(QString::fromLocal8Bit("Открыть план"),this);
    connect(a,SIGNAL(triggered(bool)),this,SLOT(openProject()));
    planListView->addAction(a);
    a = new QAction(QString::fromLocal8Bit("Открыть cвязанный план"),this);
    connect(a,SIGNAL(triggered()),this,SLOT(openLinkedProject()));
    planListView->addAction(a);
    planListView->setContextMenuPolicy(Qt::ActionsContextMenu);
    */
}

void PlansPlugin::addPlanFile()
{
    QString selectedFilter;
    QStringList fileNames= QFileDialog::getOpenFileNames(0,
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
        planFiles->addPlan(fileNames[f],false);
    }
    saveSettings();
    //((QStandardItemModel *)projects)->appendRow(new QStandardItem(fileName));
}

void PlansPlugin::delCurrentPlan()
{
    QItemSelectionModel *is = propWidget->getSelectionModel(); // =filesTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=ii.count()-1; i>=0; i--)
    {
        QString file = ii[i].data().toString();
        planFiles->removeRow(ii[i].row());
    }
    //filesTableView->clearSelection();
    saveSettings();
}

void PlansPlugin::loadCurrentPlan()
{
    QItemSelectionModel *is = propWidget->getSelectionModel(); // filesTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=0; i<ii.count(); i++)
    {
        planFiles->triggerLoadPlan(ii[i].row());
    }
}

void PlansPlugin::showCurrentPlan()
{
    QItemSelectionModel *is = propWidget->getSelectionModel(); // filesTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=0; i<ii.count(); i++)
    {
        int row = ii[i].row();
        PrjItemModel * model = planFiles->loadPlan(row);
        openPlanPage(model);
    }
}

void PlansPlugin::loadAllProjects()
{
}

void PlansPlugin::checkAutoProjects()
{
}

void PlansPlugin::addMSProjectFile()
{
    PrjItemModel *model = new PrjItemModel(this);
    if(model->open("",false))
    {
        planFiles->addPlan(model->fileName,model->isReadOnly);
        loadedPlans->addPrjModel(model);
        planFiles->models[model->fileName]=model;
        saveSettings();
    }
    else
        delete model;
}

void PlansPlugin::openPlanPage(PrjItemModel *model, int selectedTask)
{
    if(!mainWindow)
        return;
    ProjectPage *page = new ProjectPage(model);
    QMetaObject::invokeMethod(mainWindow,"addTab",
                              Q_ARG(QString, model->prjName),
                              Q_ARG(QWidget *, page));
}

void PlansPlugin::appendContextMenu(QMenu *menu)
{
}

void PlansPlugin::queryViewOpened(QWidget *widget, QTableView *view, const QString &recType)
{
    Q_UNUSED(view)
    if(recType == "scr")
    {
        TQPlansWidget *tab = new TQPlansWidget(widget);
        tab->setParentObject(widget);
        tab->setPlanModel(loadedPlans);
        connect(widget,SIGNAL(selectionRecordsChanged()),tab,SLOT(selectingRecordsChanged()));
        QString title = "Plans";
        QMetaObject::invokeMethod(widget,"addDetailTab",
                                  Q_ARG(QWidget *,tab),
                                  Q_ARG(QString,title));
    }
}

void PlansPlugin::recordOpened(QWidget *widget, QObject *record, const QString &recType)
{
}

QWidget *PlansPlugin::getPropWidget(QWidget *parentWidget)
{
    if(!propWidget)
    {
        PlanFilesForm *prop = new PlanFilesForm(parentWidget);
        prop->setModel(planFiles);
        connect(prop,SIGNAL(addBtn_clicked()),SLOT(addPlanFile()));
        connect(prop,SIGNAL(delBtn_clicked()),SLOT(delCurrentPlan()));
        connect(prop,SIGNAL(loadBtn_clicked()),SLOT(loadCurrentPlan()));
        connect(prop,SIGNAL(openMSP_clicked()),SLOT(addMSProjectFile()));
        connect(prop,SIGNAL(showBtn_clicked()),SLOT(showCurrentPlan()));
        propWidget = prop;
    }
    return propWidget;
    /*
    QWidget *prop = new QWidget(parentWindget);
    QGroupBox *groupBox_2 = new QGroupBox(prop);
    groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));

    QVBoxLayout *verticalLayout_3 = new QVBoxLayout(groupBox_2);
    verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
    verticalLayout_3->setContentsMargins(-1, 3, -1, 3);

    QGridLayout *gridLayout_2 = new QGridLayout();
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));

    filesTableView = new QTableView(groupBox_2);
    filesTableView->setObjectName(QString::fromUtf8("projectTableView"));
    filesTableView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    filesTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    filesTableView->setShowGrid(false);
    filesTableView->setCornerButtonEnabled(false);
    filesTableView->horizontalHeader()->setHighlightSections(false);
    filesTableView->horizontalHeader()->setMinimumSectionSize(5);
    filesTableView->horizontalHeader()->setStretchLastSection(true);
    filesTableView->verticalHeader()->setDefaultSectionSize(19);

    gridLayout_2->addWidget(filesTableView, 0, 0, 5, 1);

    QPushButton *projectOpenBtn = new QPushButton(groupBox_2);
    projectOpenBtn->setObjectName(QString::fromUtf8("projectOpenBtn"));

    gridLayout_2->addWidget(projectOpenBtn, 1, 1, 1, 1);

    QCheckBox *projectAutoLoad = new QCheckBox(groupBox_2);
    projectAutoLoad->setObjectName(QString::fromUtf8("projectAutoLoad"));
    projectAutoLoad->setEnabled(false);

    gridLayout_2->addWidget(projectAutoLoad, 5, 0, 1, 1);

    QPushButton *projectShowBtn = new QPushButton(groupBox_2);
    projectShowBtn->setObjectName(QString::fromUtf8("projectShowBtn"));

    gridLayout_2->addWidget(projectShowBtn, 4, 1, 1, 1);

    QPushButton *projectLoadBtn = new QPushButton(groupBox_2);
    projectLoadBtn->setObjectName(QString::fromUtf8("projectLoadBtn"));

    gridLayout_2->addWidget(projectLoadBtn, 3, 1, 1, 1);

    QPushButton *projectDelBtn = new QPushButton(groupBox_2);
    projectDelBtn->setObjectName(QString::fromUtf8("projectDelBtn"));

    gridLayout_2->addWidget(projectDelBtn, 2, 1, 1, 1);

    QPushButton *projectAddBtn = new QPushButton(groupBox_2);
    projectAddBtn->setObjectName(QString::fromUtf8("projectAddBtn"));

    gridLayout_2->addWidget(projectAddBtn, 0, 1, 1, 1);

    verticalLayout_3->addLayout(gridLayout_2);
    prop->setLayout(verticalLayout_3);


    projectOpenBtn->setText(QApplication::translate("MainWindow", "\320\230\320\267 MSProject...", 0, QApplication::UnicodeUTF8));
    projectAutoLoad->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\263\321\200\321\203\320\266\320\260\321\202\321\214 \320\262\321\201\320\265 \320\260\320\262\321\202\320\276\320\274\320\260\321\202\320\270\321\207\320\265\321\201\320\272\320\270 \320\277\321\200\320\270 \320\262\321\205\320\276\320\264\320\265", 0, QApplication::UnicodeUTF8));
    projectShowBtn->setText(QApplication::translate("MainWindow", "\320\236\321\202\320\272\321\200\321\213\321\202\321\214", 0, QApplication::UnicodeUTF8));
    projectLoadBtn->setText(QApplication::translate("MainWindow", "\320\227\320\260\320\263\321\200\321\203\320\267\320\270\321\202\321\214", 0, QApplication::UnicodeUTF8));
    projectDelBtn->setText(QApplication::translate("MainWindow", "\320\243\320\261\321\200\320\260\321\202\321\214", 0, QApplication::UnicodeUTF8));
    projectAddBtn->setText(QApplication::translate("MainWindow", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \321\204\320\260\320\271\320\273...", 0, QApplication::UnicodeUTF8));

    connect(projectAddBtn,SIGNAL(clicked()),this,SLOT(addPlanFile()));
    connect(projectOpenBtn,SIGNAL(clicked()),this,SLOT(addMSProjectFile()));
    connect(projectDelBtn,SIGNAL(clicked()),this,SLOT(delCurrentPlan()));
    connect(projectLoadBtn,SIGNAL(clicked()),this,SLOT(loadCurrentPlan()));
    connect(projectShowBtn,SIGNAL(clicked()),this,SLOT(showCurrentPlan()));

    return prop;
    */
}


//void PlansPlugin::setSettings(QSettings *settings)
//{
//    this->settings = settings;
//}

bool PlansPlugin::saveSettings()
{
    if(settings)
    {
        settings->beginGroup("Plans");
        {
            QStringList keys=settings->childKeys();
            for(int i=0; i<keys.count(); i++)
                settings->remove(keys[i]);
            for(int i=0; i<planFiles->plans.count(); i++)
            {
                settings->setValue(QString::number(i),planFiles->plans[i].file);
                settings->setValue(QString::number(i) + ".RO", planFiles->plans[i].readOnly);
            }
        }
        settings->endGroup();
        return true;
    }
    return false;
}

bool PlansPlugin::loadSettings()
{
    if(settings)
    {
        settings->beginGroup("Plans");
        {
            QStringList keys=settings->childKeys();
            QMap<QString, QString> map;
            for(int i=0; i<keys.count(); i++)
            {
                const QString &key = keys[i];
                if(!key.contains(".RO"))
                {
                    QString prj = settings->value(key).toString();
                    map.insert(prj, key);
                }
            }
            QMap<QString, QString>::iterator i;
            for(i = map.begin(); i != map.end(); i++)
            {
                QString prj = i.key();
                QString key = i.value();
                bool readOnly = settings->value(key+".RO",false).toBool();
                planFiles->addPlan(prj, readOnly);
            }
        }
        settings->endGroup();
        return true;
    }
    return false;
}

PrjItemModel *PlansPlugin::loadPrjFile(const QString &fileName, bool readOnly)
{
    return loadedPlans->addPrjFile(fileName, readOnly);
}

void PlansPlugin::emitError(const QString &msg)
{
    emit error("MSPlans",msg);
}

void PlansPlugin::slotPlanContextMenuRequested(const QPoint &pos)
{
    /* to TQPlansWidget
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
    */
}
