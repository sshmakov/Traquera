#include "projectpage.h"
#include "settings.h"
#include "mainwindow.h"
#include "ttglobal.h"
#include "ttutils.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QScrollArea>
#include <QFormLayout>
#include <QLineEdit>
#include <QSettings>
#include <QMessageBox>

ProjectPage::ProjectPage(PrjItemModel *projectModel, QWidget *parent)
    :QWidget(parent), prjmodel(projectModel,parent), trkmodel(0)
{
	setupUi(this);
    selectionTimer = new QTimer(this);
    selectionTimer->setInterval(0);
    selectionTimer->setSingleShot(true);
    connect(selectionTimer,SIGNAL(timeout()),this,SLOT(updateDetails()));
	isDefLoaded = false;
	isInteractive=false;
	colSCR=0;
	projectView->setModel(prjmodel.model);
	loadDefinition();
	//projectTabWidget->hide();
	//queryView->setModel(&trkmodel.queryModel);
	//queryView->setModel(model);
	//connect(queryView,SIGNAL(itemSelectionChanged()),
	connect(
		projectView->horizontalHeader(),
		SIGNAL(sectionMoved (int, int, int)),
		this, SLOT(headerChanged()));
	connect(
		projectView->horizontalHeader(),
		SIGNAL(sectionPressed (int)),
		this, SLOT(headerChanged()));
	connect(
		projectView->horizontalHeader(),
		SIGNAL(sectionResized (int, int, int)),
		this, SLOT(headerChanged()));

    //trkmodel = new TrkItemModel(TrkDb::defaultDb(),this);
    //scrTable->setModel(trkmodel);
	connect(
		projectView->selectionModel(), 
		SIGNAL(selectionChanged ( const QItemSelection & , const QItemSelection &  )),
		this,
		SLOT(selectionChanged ( const QItemSelection & , const QItemSelection &  )));
}

void ProjectPage::openProject(const QString& file)
{
	isInteractive=false;
	if(!prjmodel.openProject(file))
		return;
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

static int findColumn(QHeaderView *hv, const QString &label)
{
	QAbstractItemModel *model = hv->model();
	for(int i=0; i<model->columnCount(); i++)
	{
		if(!QString::compare(
			label.trimmed(),
			model->headerData(i,Qt::Horizontal).toString().trimmed()))
			return i;
	}
	return -1;
}

void ProjectPage::setParentColumn(QHeaderView *hv, const QString& srcFieldTitle, const QString& dstFieldTitle)
{
	isInteractive=false;
	int colSrc = findColumn(hv, srcFieldTitle);
	if(colSrc<0)
	{
		QMessageBox::critical(this,"Error","Не найдено поле-источник "+srcFieldTitle);
		return;
	}
	prjmodel.setParentColumn(colSrc, dstFieldTitle);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void ProjectPage::loadDefinition()
{
    QSettings *settings = ttglobal()->settings();
    QXmlSimpleReader xmlReader;
	QFile *file = new QFile("data/project.xml");
	QXmlInputSource *source = new QXmlInputSource(file);
	QDomDocument dom;
    if(dom.setContent(source,false))
    {
        QDomElement doc = dom.documentElement();
        if(!doc.isNull())
        {
            QHeaderView *hv=projectView->horizontalHeader();
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
                        QVariant coltype = col.attribute("type");
                        if(coltype.isValid() && coltype.toString() == "SCR")
                            colSCR = colNum;
                    }
                }
                for(; nextCol<hv->count(); nextCol++)
                {
                    int log=hv->logicalIndex(nextCol);
                    hv->hideSection(log);
                }
            }
            QVariant gridSet = settings->value("ProjectGrid");
            if(gridSet.isValid())
                hv->restoreState(gridSet.toByteArray());
            hv->setMovable(true);
            hv->setDefaultAlignment(Qt::AlignLeft);
            settings->setValue("ProjectGrid", hv->saveState());
        }
    }
    //decorator->loadViewDef(scrTable);
	/*
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
			inp->setGeometry(QRect(0, 0, 295, 92));
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
					PEditDef f;
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
	}
	*/
	isDefLoaded = true;
	delete source;
	delete file;
	initPopupMenu();
}

void ProjectPage::initPopupMenu()
{
	QHeaderView *hv=projectView->horizontalHeader();
	for(int i=0; i<hv->count(); i++)
	{
		QString label = projectView->model()->headerData(i,Qt::Horizontal).toString().trimmed(); 
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

void ProjectPage::headerToggled(bool checked)
{
	QAction *a = qobject_cast<QAction*>(sender());
	if(a)
	{
		int i = headerActions.indexOf(a);
		if(i>=0)
			projectView->horizontalHeader()->setSectionHidden(i,!checked);
	}
}

void ProjectPage::headerChanged()
{
	if(isInteractive)
    {
        QSettings *settings = ttglobal()->settings();
        settings->setValue("ProjectGrid", projectView->horizontalHeader()->saveState());
    }
}

QString ProjectPage::projectName()
{

	return prjmodel.prjmodel->projectName();
}

void ProjectPage::selectionChanged(const QItemSelection & /*selected*/, const QItemSelection & /*deselected*/)
{
    selectionTimer->start(250);
}

void ProjectPage::goToTask(int taskNum)
{
    int row = prjmodel.rowOfTask(taskNum);
    if(row>=0)
        projectView->selectRow(row);
}

void ProjectPage::updateDetails()
{
    QStringList scrs;

    QModelIndexList list=projectView->selectionModel()->selectedIndexes();
    for(int i=0; i<list.count(); i++)
    {
        QModelIndex index = list[i];
        QModelIndex scrIndex = index.sibling(index.row(),colSCR);
        if(scrIndex.isValid())
        {
            QString scrData = scrIndex.data().toString();
            scrs.append(scrData);
        }
    }
    if(trkmodel)
    {
        scrTable->setModel(0);
        delete trkmodel;
        trkmodel = 0;
    }
    TrkToolProject *prj = ttglobal()->mainWindow->currentProject();
    if(prj)
    {
        trkmodel = prj->openIdsModel(toIntList(scrs.join(", ")),TRK_SCR_TYPE,false);
        scrTable->setModel(trkmodel);
        scrTable->loadDefinition();
    }
}
