#include "tqplanswidget.h"
#include <QtCore>
#include <QtGui>
#include <QtXml>
//#include "ttutils.h"
//#include "settings.h"
//#include "ttglobal.h"
#include <tqviewcontroller.h>

TQPlansWidget::TQPlansWidget(QWidget *parent) :
    TQPluginWidget(parent),
    planViewModel(this),isSetsLoaded(false)
{
    initWidgets();
}

void TQPlansWidget::initWidgets()
{
    QVBoxLayout *lay = new QVBoxLayout;
    //subLay = new QStackedLayout(subViewWidget);

    planTreeView = new QTreeView(this);
    planTreeView->setObjectName(QString::fromUtf8("planTreeView"));
    planTreeView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    planTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    planTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    //planTreeView->setAlternatingRowColors(true);
    planTreeView->setIndentation(5);
    lay->addWidget(planTreeView);
    lay->setContentsMargins(0,0,0,0);
    setLayout(lay);
    connect(planTreeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenuRequested(QPoint)));
    connect(planTreeView->header(),SIGNAL(sectionMoved(int,int,int)),SLOT(headerChanged()));
    connect(planTreeView->header(),SIGNAL(sectionResized(int,int,int)),SLOT(headerChanged()));
    detailsTimer = new QTimer(this);
    detailsTimer->setInterval(0);
    detailsTimer->setSingleShot(true);
    connect(detailsTimer,SIGNAL(timeout()),this,SLOT(filterForRecords()));
    planTreeView->setModel(&planViewModel);
}

void TQPlansWidget::contextMenuRequested(const QPoint &pos)
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
        //PrjItemModel *prj=loadedPlanModel->prjModel(si);
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
        menu.addAction(tr("Открыть план"),this,SLOT(showCurrentPlan()));
    }
    QPoint gPos = planTreeView->mapToGlobal(pos);
    menu.exec(gPos);
}

void TQPlansWidget::selectingRecordsChanged()
{
//    QObjectList records;
//    QMetaObject::invokeMethod(parentObject, "selectedRecords", Qt::DirectConnection,
//                              Q_RETURN_ARG(QObjectList,records));
    curRecords = controller->selectedRecords();
    detailsTimer->start(250);
}

void TQPlansWidget::recordsChanged(QObjectList records)
{
    curRecords = records;
    detailsTimer->start(250);
}

int TQPlansWidget::getColNum(const QString &colname, const QAbstractItemModel *model)
{
    for(int i=0; i<model->columnCount(); i++)
        if(!colname.compare(model->headerData(i,Qt::Horizontal).toString(),
            Qt::CaseInsensitive))
            return i;
    return -1;
}

void TQPlansWidget::addScrTask(PrjItemModel *prj)
{
    //QModelIndexList selectedRows;
    QObjectList records;
    if(!QMetaObject::invokeMethod(parentObject,"selectedRecords",Qt::DirectConnection,
                                  Q_RETURN_ARG(QObjectList, records)))
        return;
    //QXmlSimpleReader xmlReader;
    QString fileName = pluginObject->dataDir.absoluteFilePath("scr2prj.xml");
    QFile file(fileName);
    QXmlInputSource *source = new QXmlInputSource(&file);
    QDomDocument dom;
    if(!dom.setContent(source,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull()) return;
    QDomElement fields = doc.firstChildElement("fields");

    //QItemSelectionModel *is=queryView->selectionModel();
    //QModelIndexList ii = is->selectedRows();
    foreach(QObject *obj, records)
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
            QVariant val;
            if(QMetaObject::invokeMethod(obj,"value",Qt::DirectConnection,
                                         Q_RETURN_ARG(QVariant, val),
                                         Q_ARG(QString, src)))
                rec[ftitle] = val;
        }
        prj->addTask(rec);
    }

    /*
    for(QModelIndexList::const_iterator i=selectedRows.constBegin();
        i!=selectedRows.constEnd();
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
            int col = getColNum(src,i->model());
            if(col<0)
                continue;
            QModelIndex srcIndex = i->sibling(i->row(),col);
            QVariant val = srcIndex.data();
            rec[ftitle] = val;
        }
        prj->addTask(rec);
    }
    */
}

void TQPlansWidget::addScrTasks()
{
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    QSet<PrjItemModel *>selectedPlans;
    foreach(const QModelIndex &i, list)
    {
        QModelIndex si = planViewModel.mapToSource(i);
        PrjItemModel *prj=loadedPlanModel->prjModel(si);
        if(prj)
          selectedPlans.insert(prj);
    }
    foreach(PrjItemModel *prj, selectedPlans)
        addScrTask(prj);

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

static QString my_intListToString(const QList<int> &arr)
{
    QStringList numbers;
    foreach(int i, arr)
        numbers.append(QString::number(i));
    return numbers.join(", ");
}


void TQPlansWidget::copyScrFromTasks()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    QString slist = my_intListToString(res.toList());
    QApplication::clipboard()->setText(slist);
}

void TQPlansWidget::showScrFromTasks()
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

void TQPlansWidget::filterForRecords()
{
    QStringList keys;
    //QList<TrkToolRecord *> selected = selectedRecords();
    foreach(QObject *obj, curRecords)
    {
        //TrkToolRecord *rec = qobject_cast<TrkToolRecord *>(obj);
        //if(!rec)
        //    continue;
        int id;
        if(!QMetaObject::invokeMethod(obj,"recordId", Qt::DirectConnection,
                                      Q_RETURN_ARG(int, id)))
            continue;
        QString key = QString::number(id);
        if(!key.isEmpty())
            keys.append(key);
    }
    planViewModel.setFilterSCR(keys.join(","));
    planTreeView->expandAll();
    for(int r=0; r<planViewModel.rowCount(); r++)
        planTreeView->setFirstColumnSpanned(r,QModelIndex(),true);
}

void TQPlansWidget::showCurrentTaskInPlan()
{
    QModelIndex index = planTreeView->currentIndex();
    QModelIndex si = planViewModel.mapToSource(index);
    PrjItemModel *prj=loadedPlanModel->prjModel(si);
    if(prj)
    {
        bool ok = false;
        int taskId = loadedPlanModel->fieldData(si.row(),"ID").toInt(&ok);
        if(!ok || taskId<0)
            taskId=0;
        pluginObject->openPlanPage(prj,taskId);
    }
}

void TQPlansWidget::showCurrentPlan()
{
    QModelIndex index = planTreeView->currentIndex();
    QModelIndex si = planViewModel.mapToSource(index);
    PrjItemModel *prj = loadedPlanModel->prjModel(si);
    if(prj)
        pluginObject->openPlanPage(prj);
}

void TQPlansWidget::updateDetailWindows()
{
    QMainWindow *m = pluginObject->mainWindow;
}

void TQPlansWidget::slotCheckPlannedIds()
{
    QString ids = selectedSCRs().join(", ");
    QMetaObject::invokeMethod(parentObject,"setRecordsChecked",Qt::AutoConnection,
                              Q_ARG(QString, ids),
                              Q_ARG(bool, true));
}

void TQPlansWidget::slotCheckNoPlannedIds()
{
    QList<int> viewIds;
    if(!QMetaObject::invokeMethod(parentObject,"selectedIds",Qt::DirectConnection,
                              Q_RETURN_ARG(QList<int>, viewIds)))
        return;
    QList<int> planIds = selectedSCRIds();
    QStringList list;
    foreach(int id, viewIds)
    {
        if(!planIds.contains(id))
            list.append(QString::number(id));
    }
    QMetaObject::invokeMethod(parentObject,"setRecordsChecked",Qt::AutoConnection,
                              Q_ARG(QString, list.join(", ")),
                              Q_ARG(bool, true));
}

void TQPlansWidget::headerChanged()
{
    if(isSetsLoaded)
        pluginObject->settings->setValue(Settings_ScrPlanView,planTreeView->header()->saveState());
}

void TQPlansWidget::setPlanModel(PlanModel *newmodel)
{
    isSetsLoaded = false;
    if(newmodel == loadedPlanModel)
        return;
    planViewModel.setSourceModel(newmodel);
    /*
    if(newmodel)
        proxyModel.setFilterKeyColumn(newmodel->SCRfield);
        */
    loadedPlanModel=newmodel;
    if(loadedPlanModel)
    {
        loadedPlanModel->setFieldHeaders(planTreeView->header());
        QVariant value = pluginObject->settings->value(Settings_ScrPlanView);
        if(value.isValid() && value.type() == QVariant::ByteArray)
            planTreeView->header()->restoreState(value.toByteArray());
    }
    isSetsLoaded = true;
}

void TQPlansWidget::setParentObject(QObject *obj)
{
    parentObject = obj;
}

QStringList TQPlansWidget::selectedSCRs()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    QStringList numbers;
    foreach(int i, res)
    {
        numbers.append(QString::number(i));
    }
    return numbers;
}

QList<int> TQPlansWidget::selectedSCRIds()
{
    ScrSet res;
    QItemSelectionModel *sm = planTreeView->selectionModel();
    QModelIndexList list = sm->selectedRows();
    foreach(const QModelIndex &i, list)
    {
        res += planViewModel.taskScrSet(i);
    }
    return res.toList();
}
