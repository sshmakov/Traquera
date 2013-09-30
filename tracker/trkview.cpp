#include "tracker.h"
#include "trkview.h"
#ifndef CONSOLE_APP
#include "settings.h"
#include "ttglobal.h"
#include "trkdecorator.h"
#include <QHeaderView>
#include "messager.h"
#endif
#include "ttutils.h"

#include <QSettings>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QXmlQuery>


bool isTrkOK(int result, bool show = true)
{
    QString error;
    switch(result)
    {
    case TRK_SUCCESS:
        return true;
    case TRK_E_VERSION_MISMATCH		       :    error = QObject::tr("TRK_E_VERSION_MISMATCH        "); break;
    case TRK_E_OUT_OF_MEMORY		 	   :    error = QObject::tr("TRK_E_OUT_OF_MEMORY			"); break;
    case TRK_E_BAD_HANDLE			       :    error = QObject::tr("TRK_E_BAD_HANDLE			"); break;
    case TRK_E_BAD_INPUT_POINTER		   :    error = QObject::tr("TRK_E_BAD_INPUT_POINTER		"); break;
    case TRK_E_BAD_INPUT_VALUE		       :    error = QObject::tr("TRK_E_BAD_INPUT_VALUE		"); break;
    case TRK_E_DATA_TRUNCATED		       :    error = QObject::tr("TRK_E_DATA_TRUNCATED		"); break;
    case TRK_E_NO_MORE_DATA			       :    error = QObject::tr("TRK_E_NO_MORE_DATA			"); break;
    case TRK_E_LIST_NOT_INITIALIZED	       :    error = QObject::tr("TRK_E_LIST_NOT_INITIALIZED	"); break;
    case TRK_E_END_OF_LIST			       :    error = QObject::tr("TRK_E_END_OF_LIST			"); break;
    case TRK_E_NOT_LOGGED_IN			   :    error = QObject::tr("TRK_E_NOT_LOGGED_IN			"); break;
    case TRK_E_SERVER_NOT_PREPARED	       :    error = QObject::tr("TRK_E_SERVER_NOT_PREPARED	"); break;
    case TRK_E_BAD_DATABASE_VERSION	       :    error = QObject::tr("TRK_E_BAD_DATABASE_VERSION	"); break;
    case TRK_E_UNABLE_TO_CONNECT		   :    error = QObject::tr("TRK_E_UNABLE_TO_CONNECT		"); break;
    case TRK_E_UNABLE_TO_DISCONNECT	       :    error = QObject::tr("TRK_E_UNABLE_TO_DISCONNECT	"); break;
    case TRK_E_UNABLE_TO_START_TIMER	   :    error = QObject::tr("TRK_E_UNABLE_TO_START_TIMER	"); break;
    case TRK_E_NO_DATA_SOURCES		       :    error = QObject::tr("TRK_E_NO_DATA_SOURCES		"); break;
    case TRK_E_NO_PROJECTS			       :    error = QObject::tr("TRK_E_NO_PROJECTS			"); break;
    case TRK_E_WRITE_FAILED			       :    error = QObject::tr("TRK_E_WRITE_FAILED			"); break;
    case TRK_E_PERMISSION_DENIED		   :    error = QObject::tr("TRK_E_PERMISSION_DENIED		"); break;
    case TRK_E_SET_FIELD_DENIED		       :    error = QObject::tr("TRK_E_SET_FIELD_DENIED		"); break;
    case TRK_E_ITEM_NOT_FOUND		       :    error = QObject::tr("TRK_E_ITEM_NOT_FOUND"); break;
    case TRK_E_CANNOT_ACCESS_DATABASE      :    error = QObject::tr("TRK_E_CANNOT_ACCESS_DATABASE"); break;
    case TRK_E_CANNOT_ACCESS_QUERY	       :    error = QObject::tr("TRK_E_CANNOT_ACCESS_QUERY	"); break;
    case TRK_E_CANNOT_ACCESS_INTRAY	       :    error = QObject::tr("TRK_E_CANNOT_ACCESS_INTRAY	"); break;
    case TRK_E_CANNOT_OPEN_FILE		       :    error = QObject::tr("TRK_E_CANNOT_OPEN_FILE		"); break;
    case TRK_E_INVALID_DBMS_TYPE		   :    error = QObject::tr("TRK_E_INVALID_DBMS_TYPE		"); break;
    case TRK_E_INVALID_RECORD_TYPE	       :    error = QObject::tr("TRK_E_INVALID_RECORD_TYPE	"); break;
    case TRK_E_INVALID_FIELD			   :    error = QObject::tr("TRK_E_INVALID_FIELD			"); break;
    case TRK_E_INVALID_CHOICE		       :    error = QObject::tr("TRK_E_INVALID_CHOICE		"); break;
    case TRK_E_INVALID_USER			       :    error = QObject::tr("TRK_E_INVALID_USER			"); break;
    case TRK_E_INVALID_SUBMITTER		   :    error = QObject::tr("TRK_E_INVALID_SUBMITTER		"); break;
    case TRK_E_INVALID_OWNER			   :    error = QObject::tr("TRK_E_INVALID_OWNER			"); break;
    case TRK_E_INVALID_DATE			       :    error = QObject::tr("TRK_E_INVALID_DATE			"); break;
    case TRK_E_INVALID_STORED_QUERY	       :    error = QObject::tr("TRK_E_INVALID_STORED_QUERY	"); break;
    case TRK_E_INVALID_MODE			       :    error = QObject::tr("TRK_E_INVALID_MODE			"); break;
    case TRK_E_INVALID_MESSAGE		       :    error = QObject::tr("TRK_E_INVALID_MESSAGE		"); break;
    case TRK_E_VALUE_OUT_OF_RANGE	       :    error = QObject::tr("TRK_E_VALUE_OUT_OF_RANGE	"); break;
    case TRK_E_WRONG_FIELD_TYPE		       :    error = QObject::tr("TRK_E_WRONG_FIELD_TYPE		  "); break;
    case TRK_E_NO_CURRENT_RECORD		   :    error = QObject::tr("TRK_E_NO_CURRENT_RECORD		  "); break;
    case TRK_E_NO_CURRENT_NOTE		       :    error = QObject::tr("TRK_E_NO_CURRENT_NOTE		  "); break;
    case TRK_E_NO_CURRENT_ATTACHED_FILE    :    error = QObject::tr("TRK_E_NO_CURRENT_ATTACHED_FILE"); break;
    case TRK_E_NO_CURRENT_ASSOCIATION      :    error = QObject::tr("TRK_E_NO_CURRENT_ASSOCIATION"); break;
    case TRK_E_NO_RECORD_BEGIN		       :    error = QObject::tr("TRK_E_NO_RECORD_BEGIN		"); break;
    case TRK_E_NO_MODULE				   :    error = QObject::tr("TRK_E_NO_MODULE				"); break;
    case TRK_E_USER_CANCELLED		       :    error = QObject::tr("TRK_E_USER_CANCELLED		"); break;
    case TRK_E_SEMAPHORE_TIMEOUT		   :    error = QObject::tr("TRK_E_SEMAPHORE_TIMEOUT		"); break;
    case TRK_E_SEMAPHORE_ERROR		       :    error = QObject::tr("TRK_E_SEMAPHORE_ERROR		"); break;
    case TRK_E_INVALID_SERVER_NAME	       :    error = QObject::tr("TRK_E_INVALID_SERVER_NAME	"); break;
    case TRK_E_NOT_LICENSED			       :    error = QObject::tr("TRK_E_NOT_LICENSED			"); break;
    case TRK_E_RECORD_LOCKED			   :    error = QObject::tr("TRK_E_RECORD_LOCKED			"); break;
    case TRK_E_RECORD_NOT_LOCKED		   :    error = QObject::tr("TRK_E_RECORD_NOT_LOCKED		"); break;
    case TRK_E_UNMATCHED_PARENS		       :    error = QObject::tr("TRK_E_UNMATCHED_PARENS		"); break;
    case TRK_E_NO_CURRENT_TRANSITION	   :    error = QObject::tr("TRK_E_NO_CURRENT_TRANSITION	"); break;
    case TRK_E_NO_CURRENT_RULE		       :    error = QObject::tr("TRK_E_NO_CURRENT_RULE		"); break;
    case TRK_E_UNKNOWN_RULE			       :    error = QObject::tr("TRK_E_UNKNOWN_RULE			"); break;
    case TRK_E_RULE_ASSERTION_FAILED	   :    error = QObject::tr("TRK_E_RULE_ASSERTION_FAILED	"); break;
    case TRK_E_ITEM_UNCHANGED		       :    error = QObject::tr("TRK_E_ITEM_UNCHANGED		"); break;
    case TRK_E_TRANSITION_NOT_ALLOWED      :    error = QObject::tr("TRK_E_TRANSITION_NOT_ALLOWED"); break;
    case TRK_E_NO_CURRENT_STYLESHEET	   :    error = QObject::tr("TRK_E_NO_CURRENT_STYLESHEET	"); break;
    case TRK_E_NO_CURRENT_FORM		       :    error = QObject::tr("TRK_E_NO_CURRENT_FORM		"); break;
    case TRK_E_NO_CURRENT_VALUE		       :    error = QObject::tr("TRK_E_NO_CURRENT_VALUE		"); break;
    case TRK_E_FORM_FIELD_ACCESS		   :    error = QObject::tr("TRK_E_FORM_FIELD_ACCESS		"); break;
    case TRK_E_INVALID_QBID_STRING	       :    error = QObject::tr("TRK_E_INVALID_QBID_STRING	"); break;
    case TRK_E_FORM_INVALID_FIELD	       :    error = QObject::tr("TRK_E_FORM_INVALID_FIELD	"); break;
    case TRK_E_PARTIAL_SUCCESS		       :    error = QObject::tr("TRK_E_PARTIAL_SUCCESS		"); break;
    }
    if(error.isEmpty())
        error = QString(QObject::tr("Ошибка Tracker %1")).arg(result);
#ifndef CONSOLE_APP

    if(show)
        TTGlobal::global()->showError(error);
#endif
    return false;
}


#ifndef CONSOLE_APP
// =================== TrkView ===============
TrkView::TrkView(QWidget *parent) : 
	QTableView(parent),
	trkmodel(parent)
{
	isInteractive = true;
	isDefLoaded = false;
}

void TrkView::headerChanged()
{
	if(isInteractive)
		settings->setValue("TrackerGrid", horizontalHeader()->saveState());
}

void TrkView::headerToggled(bool checked)
{
	QAction *a = qobject_cast<QAction*>(sender());
	if(a)
	{
		int i = headerActions.indexOf(a);
		if(i>=0)
			horizontalHeader()->setSectionHidden(i,!checked);
	}
}

void TrkView::initPopupMenu()
{
	QHeaderView *hv=horizontalHeader();
	for(int i=0; i<hv->count(); i++)
	{
		QString label = model()->headerData(i,Qt::Horizontal).toString().trimmed(); 
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

void TrkView::setQuery(int id, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQuery(id,trkdb);
	setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void TrkView::setQueryById(const QString& numbers, TrkDb *trkdb)
{
	isInteractive=false;
	trkmodel.setQueryById(numbers,trkdb);
	setModel(trkmodel.model);
	if(!isDefLoaded)
		loadDefinition();
	isInteractive=true;
}

void TrkView::loadDefinition()
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
	QHeaderView *hv=horizontalHeader();
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
				int colNum = findColumn(flabel);
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
    */

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
	*/
	isDefLoaded = true;
    //delete source;
    //delete file;
	initPopupMenu();
}

int TrkView::findColumn(const QString &label) const
{
	QHeaderView *hv = horizontalHeader();
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
#endif

// =============== TrkToolDB =================
TrkToolDB::TrkToolDB(QObject *parent)
	:QObject(parent), dbmsTypeList()
{
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
}

TrkToolDB::~TrkToolDB()
{
	TrkHandleFree(&handle);
}

QStringList TrkToolDB::dbmsTypes()
{
	if(dbmsTypeList.isEmpty())
        if(isTrkOK(TrkInitDBMSTypeList(handle)))
		{
			char buf[1024];
            while(TRK_SUCCESS == TrkGetNextDBMSType(handle, 1024, buf))
				dbmsTypeList.append(QString::fromLocal8Bit(buf));
		}
    return dbmsTypeList;
}

QStringList TrkToolDB::projects(const QString &dbmsType)
{
	if(!projectList.contains(dbmsType) || projectList[dbmsType].isEmpty())
	{
		TRK_UINT res;
		if(dbmsUser.isEmpty())
			res = TrkInitProjectList(handle, 
			dbmsType.toLocal8Bit().constData(), 
			dbmsName.toLocal8Bit().constData(), 
			"", 
			"", 
			TRK_USE_DEFAULT_DBMS_LOGIN);
		else
			res = TrkInitProjectList(handle, 
			dbmsType.toLocal8Bit().constData(), 
			dbmsName.toLocal8Bit().constData(), 
			dbmsUser.toLocal8Bit().constData(), 
			dbmsPassword.toLocal8Bit().constData(), 
			TRK_USE_SPECIFIED_DBMS_LOGIN);
        if(isTrkOK(res))
		{
			char buf[1024];
			QStringList list;
            while(TRK_SUCCESS == TrkGetNextProject(handle, 1024, buf))
				list.append(QString::fromLocal8Bit(buf));
			projectList[dbmsType] = list;
		}
	}
    return projectList[dbmsType];
}

TrkToolProject *TrkToolDB::openProject(
	const QString &dbmsType,
	const QString &projectName,
	const QString &user, 
	const QString &pass)
{
	TrkToolProject *prj = new TrkToolProject(this);
    prj->login(user, pass, projectName, dbmsType, dbmsName, dbmsUser, dbmsPassword);
    openedProjects[projectName]=prj;
	return prj;
}

 QHash<QString, TrkToolProject *> TrkToolDB::openedProjects;

TrkToolProject *TrkToolDB::getProject(const QString &projectName)
{
	return openedProjects[projectName];
}

// ============= TrkToolProject ==============
TrkToolProject::TrkToolProject(TrkToolDB *parent)
    : QObject(parent),
      qList()
    , theQueryModel(this)
    , opened(false)
    , selected()
{
    db=parent;
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
}

TrkToolProject::~TrkToolProject()
{
    close();
}

bool TrkToolProject::login(
		const QString &user, 
		const QString &pass,
		const QString &project, 
		const QString &dbmsType, 
		const QString &dbmsName, 
		const QString &dbmsUser, 
		const QString &dbmsPassword)
{
	TRK_UINT res;
	/*
	char dtype[256];
	strncpy(dtype,dbmsType.toLocal8Bit().constData(),sizeof(dtype));
	char dname[256];
	strncpy(dname,dbmsName.toLocal8Bit().constData(),sizeof(dname));
	char duser[256];
	strncpy(duser,dbmsUser.toLocal8Bit().constData(),sizeof(duser));
	char dpass[256];
	strncpy(dpass,dbmsPassword.toLocal8Bit().constData(),sizeof(dpass));
	char puser[256];
	strncpy(puser,user.toLocal8Bit().constData(),sizeof(puser));
	char ppass[256];
	strncpy(ppass,pass.toLocal8Bit().constData(),sizeof(ppass));
	char pname[256];
	strncpy(pname,project.toLocal8Bit().constData(),sizeof(pname));
	*/
	TRK_UINT mode;
	if(dbmsUser.isEmpty())
		mode = TRK_USE_DEFAULT_DBMS_LOGIN;
	else
		mode = TRK_USE_SPECIFIED_DBMS_LOGIN;
	res = TrkProjectLogin(handle,
        user.toLocal8Bit().constData(),
		pass.toLocal8Bit().constData(),
		project.toLocal8Bit().constData(),
		dbmsType.toLocal8Bit().constData(),
		dbmsName.toLocal8Bit().constData(), 
		dbmsUser.toLocal8Bit().constData(),
		dbmsPassword.toLocal8Bit().constData(), 
		mode);
    if(isTrkOK(res))
	{
		opened = true;
		name = project;
        this->user = user;
        readUserList();
        readQueryList();
		readDefs();
        readNoteTitles();
	}
    else
#ifdef CONSOLE_APP
        qDebug() << tr("Ошибка открытия проекта %1 (%2)").arg(project).arg(res);
#else
        QMessageBox::critical(0,tr("Ошибка подключения"),
                              tr("Ошибка открытия проекта %1 (%2)")
                              .arg(project).arg(res));
#endif
	return opened;
}

void TrkToolProject::close()
{
	TrkHandleFree(&handle);
	TrkHandleAlloc(TRK_VERSION_ID, &handle);
	recordTypes.clear();
	foreach(RecordTypeDef *p, recordDef)
		delete p;
    recordDef.clear();
	opened = false;
    db->openedProjects.remove(name);
}

const QStringList &TrkToolProject::queryList(TRK_RECORD_TYPE type) const
{
	const QStringList *list = qList[type];
	return *list;
}

bool TrkToolProject::readQueryList()
{
    bool rc = isTrkOK(TrkInitQueryNameList(handle));
	if(!rc)
		return false;
	char buf[256];
	TRK_RECORD_TYPE recType;
    while(TRK_SUCCESS == TrkGetNextQueryName(handle, 256, buf, &recType))
	{
		QString name(buf);
		if(!qList[recType])
			qList[recType] = new QStringList();
		qList[recType]->append(name);
	}
    initQueryModel();
	return rc;
}

bool TrkToolProject::readDefs()
{
	recordTypes.clear();
	//bool rc;
    if(!isTrkOK(TrkInitRecordTypeList(handle)))
		return false;
	TRK_RECORD_TYPE recType;
    while(TRK_SUCCESS == TrkGetNextRecordType(handle, &recType))
	{
		char buf[1024];
        isTrkOK(TrkGetRecordTypeName(handle, recType, 1024, buf));
		recordTypes[recType] = QString::fromLocal8Bit(buf);
	}
	QHash<TRK_RECORD_TYPE, QString>::const_iterator ri;
	for(ri = recordTypes.constBegin(); ri != recordTypes.constEnd(); ++ri)
	{
        if(!recordDef.contains(ri.key()))
        {
            recordDef[ri.key()] = new RecordTypeDef(this);
            recordDef[ri.key()]->recType = ri.key();
        }
        TrkIntDef def;
        if(isTrkOK(TrkInitFieldList(handle, ri.key())))
		{
            char bufname[1024];
			TRK_FIELD_TYPE fType;
            while(TRK_SUCCESS == TrkGetNextField(handle, 1024, bufname, &fType))
			{
				TRK_VID vid;
                TrkGetFieldVid(handle, bufname, ri.key(), &vid);
                QString fieldName = QString::fromLocal8Bit(bufname);
                TRK_BOOL nullValid=0;
                TrkGetFieldIsNullValid(handle,bufname,ri.key(), &nullValid);
                recordDef[ri.key()]->nameVids[fieldName] = vid;
                RecordTypeDef *rDef = recordDef[ri.key()];
                TrkFieldDef id(rDef);
                id.name = fieldName;
                id.fType = fType;
                id.nullable = nullValid;

                if(fType == TRK_FIELD_TYPE_NUMBER)
                {
                    TRK_UINT minV=0, maxV=INT_MAX;
                    if(TRK_SUCCESS == TrkGetFieldRange(handle,bufname,ri.key(),&minV,&maxV))
                    {
                        id.minValue = minV;
                        id.maxValue = maxV;
                    }
                    TRK_UINT v;
                    if(TRK_SUCCESS == TrkGetFieldDefaultNumericValue(handle,bufname,ri.key(),&v))
                        id.defaultValue = QVariant::fromValue<int>(v);
                    else
                        id.defaultValue = 0;
                }
                if(fType == TRK_FIELD_TYPE_STRING
                        || fType ==  TRK_FIELD_TYPE_SUBMITTER
                        || fType ==  TRK_FIELD_TYPE_OWNER
                        || fType ==  TRK_FIELD_TYPE_USER
                        || fType ==  TRK_FIELD_TYPE_STATE
                        || fType ==  TRK_FIELD_TYPE_CHOICE
                        )
                {
                    char buf[1024];
                    if(TRK_SUCCESS == TrkGetFieldDefaultStringValue(handle,bufname,ri.key(),sizeof(buf),buf))
                        id.defaultValue = QString::fromLocal8Bit(buf);
                    else
                        id.defaultValue = "";
                }
                //id.p_choiceList->clear();
                /*
                if((
                            (fType == TRK_FIELD_TYPE_CHOICE) || (fType == TRK_FIELD_TYPE_STATE)
                            )
                    && TRK_SUCCESS == TrkInitChoiceList(handle, bufname, ri.key()))
				{
					TrkToolChoice ch;
					char chname[1024];
					int order = 0;
					while(TRK_SUCCESS == TrkGetNextChoice(handle, sizeof(chname), chname))
					{
                        ch.displayText = QString::fromLocal8Bit(chname);
                        ch.fieldValue = ch.displayText;
                        //ch.order = order++;
                        //ch.weight = 0;
                        id.p_choiceList->append(ch);
					}
				}
                */
				def[vid] =  id;
//				if(!nameVids.contains(ri.key()))
//					nameVids[ri.key()] = NameVid();
//				nameVids[ri.key()][id.name] = vid;
            }
		}
        recordDef[ri.key()]->fieldDefs = def;
        //recordDef[ri.key()].rectype = ri.key();
	}
    return true;
}

void TrkToolProject::readUserList()
{
    userList.clear();
    if(isTrkOK(TrkInitUserList(handle)))
    {
        char userName[1024];
        //int order = 0;
        while(TRK_SUCCESS == TrkGetNextUser(handle, sizeof(userName), userName))
        {
            QString login = QString::fromLocal8Bit(userName);
            char fullName[1024];
            if(TRK_SUCCESS == TrkGetUserFullName(handle, userName, sizeof(fullName), fullName))
                userList.insert(login,QString::fromLocal8Bit(fullName));
            else
                userList.insert(login,login);
        }
    }
}

void TrkToolProject::readNoteTitles()
{
    noteTitles.clear();
    QXmlSimpleReader xmlReader;
    QFile *file = new QFile("data/tracker.xml");
    QXmlInputSource *source = new QXmlInputSource(file);
    QDomDocument dom;
    if(!dom.setContent(source,false))
        return;
    QDomElement doc = dom.documentElement();
    QDomElement notes = doc.firstChildElement("notes");
    if(!notes.isNull())
    {
        for(QDomElement title = notes.firstChildElement("title");
            !title.isNull();
            title = title.nextSiblingElement("title"))
        {
            noteTitles.append(title.text());
        }
    }

/*
    if(isTrkOK(TrkInitNoteTitleList(handle)))
    {
        char buf[1024];
        TRK_UINT unk;
        while(TRK_SUCCESS == TrkGetNextNoteTitle(handle, sizeof(buf), buf, &unk))
        {
            noteTitles.append(buf);
        }
    }
    */
}

void TrkToolProject::initQueryModel(TRK_RECORD_TYPE type)
{
    theQueryModel.clearRecords();
    //theQueryModel.headers << tr("Query Name");
	if(!qList.isEmpty() && qList.contains(type))
     {
        foreach(const QString &qryName, *qList[type])
        {
            TRK_UINT res=0;

            TrkGetQueryIsPublic(handle, qryName.toLocal8Bit().constData(), &res);
            theQueryModel.appendQry(qryName, (res!=0), type);
        }
    }
}


bool displayLessThan(const TrkToolChoice &c1, const TrkToolChoice &c2)
{
    return QString::compare(c1.displayText,c2.displayText,Qt::CaseInsensitive)<0;
}

ChoiceList *TrkToolProject::fieldChoiceList(const QString &name, TRK_RECORD_TYPE recType)
{
    ChoiceList *list = new ChoiceList();
    TRK_FIELD_TYPE fType = fieldType(name, recType);
    if((fType == TRK_FIELD_TYPE_CHOICE) || (fType == TRK_FIELD_TYPE_STATE))
    {
        if(isTrkOK(TrkInitChoiceList( handle, name.toLocal8Bit().constData(), recType)))
        {
            TrkToolChoice ch;
            char chname[1024];
            //int order = 0;
            while(TRK_SUCCESS == TrkGetNextChoice(handle, sizeof(chname), chname))
            {
                ch.displayText = QString::fromLocal8Bit(chname);
                ch.fieldValue = ch.displayText;
                //ch.order = order++;
                //ch.weight = 0;
                list->append(ch);
            }
        }
    }
    else if ((fType == TRK_FIELD_TYPE_SUBMITTER)
             || (fType == TRK_FIELD_TYPE_OWNER)
             || (fType == TRK_FIELD_TYPE_USER))
    {

        foreach(const QString &login, userList.keys())
        {
            TrkToolChoice ch;
            ch.fieldValue = login;
            ch.displayText = userList.value(login);
            list->append(ch);
        }
        qSort(list->begin(),list->end(),displayLessThan);
            /*
        if(isTrkOK(TrkInitUserList(handle)))
        {
            TrkToolChoice ch;
            char userName[1024];
            //int order = 0;
            while(TRK_SUCCESS == TrkGetNextUser(handle, sizeof(userName), userName))
            {
                ch.fieldValue = QString::fromLocal8Bit(userName);
                char fullName[1024];
                if(isTrkOK(TrkGetUserFullName(handle, userName, sizeof(fullName), fullName)))
                    ch.displayText = QString::fromLocal8Bit(fullName);
                else
                    ch.displayText = QString::fromLocal8Bit(userName);
                //ch.order = order++;
                //ch.weight = 0;
                list->append(ch);
            }
        }
        */
    }
    return list;
}

TRK_FIELD_TYPE TrkToolProject::fieldType(const QString &name, TRK_RECORD_TYPE recType)
{
    const RecordTypeDef *rdef = recordDef[recType];
    TRK_FIELD_TYPE fType = rdef->getFieldDef(name).fType;
    return fType;
}

TrkToolModel *TrkToolProject::selectedModel(TRK_RECORD_TYPE recType)
{
    if(!selectedModels.contains(recType))
    {
        QList<int> ids = selected[recType].toList();
        TrkToolModel *model = openIdsModel(ids,recType,false);
        selectedModels.insert(recType,model);
        return model;
    }
    return selectedModels[recType];
}

bool TrkToolProject::canFieldSubmit(const QString &name, TRK_RECORD_TYPE recType)
{
    TRK_VID vid = recordDef[recType]->nameVids[name];
    if(vid == VID_Id)
        return false;

    TRK_UINT rights;
    if(isTrkOK(TrkGetFieldSubmitRights(handle,name.toLocal8Bit().constData(),recType,&rights)))
        return rights;
    return false;
}

bool TrkToolProject::canFieldUpdate(const QString &name, TRK_RECORD_TYPE recType)
{
    TRK_VID vid = recordDef[recType]->nameVids[name];
    if(vid == VID_Id)
        return false;

    TRK_UINT rights=1;
    if(!isTrkOK(TrkGetFieldUpdateRights(handle,name.toLocal8Bit().constData(),recType,&rights)))
        rights=0;
    return rights;
}

QString TrkToolProject::userFullName(const QString &login)
{
    QString res;
    if(login.isEmpty())
        return res;
    else if(userList.contains(login))
        res = userList[login];
    else
    {
        char buf[256];
        buf[0]=0;
        if(TRK_SUCCESS == TrkGetUserFullName(handle,login.toLocal8Bit().constData(),sizeof(buf),buf))
            res = buf;
        if(res.isEmpty())
            return login;
    }
    return res;
}

TrkToolRecord *TrkToolProject::getRecordId(TRK_UINT id, TRK_RECORD_TYPE rectype)
{
    TrkToolRecord *rec=0;
    TRK_RECORD_HANDLE recHandle;
    if(!isTrkOK(TrkRecordHandleAlloc(handle, &recHandle)))
        return 0;
    if(isTrkOK(TrkGetSingleRecord(recHandle, id, rectype)))
    {
        rec = new TrkToolRecord(this, rectype);
        rec->readHandle(recHandle);
        connect(rec,SIGNAL(changed(int)),this,SIGNAL(recordChanged(int)));
    }
    TrkRecordHandleFree(&recHandle);
    return rec;
}

QAbstractItemModel *TrkToolProject::queryModel(TRK_RECORD_TYPE /* type */)
{
    return &theQueryModel;
}

#ifndef CONSOLE_APP
QAbstractItemModel *TrkToolProject::createProxyQueryModel(TrkQryFilter::Filter filter, QObject *parent, TRK_RECORD_TYPE /* type */)
{
    TrkQryFilter *proxy = new TrkQryFilter(parent);
    proxy->filter = filter;
    proxy->setSourceModel(&theQueryModel);
    return proxy;
}
#endif

TrkToolModel *TrkToolProject::openQueryModel(const QString &name, TRK_RECORD_TYPE type, bool emitEvent)
{
	TrkToolModel *model = new TrkToolModel(this, type, this);
	model->openQuery(name);
    if(emitEvent)
        emit openedModel(model);
    return model;
}

TrkToolModel *TrkToolProject::openRecentModel(int afterTransId, const QString &name, TRK_RECORD_TYPE type)
{
    TrkToolModel *model = new TrkToolModel(this, type, this);
    model->openQuery(name,afterTransId);
    return model;
}

TrkToolModel *TrkToolProject::openIdsModel(const QList<int> &ids, TRK_RECORD_TYPE type, bool emitEvent)
{
	TrkToolModel *model = new TrkToolModel(this, type, this);
	model->openIds(ids);
    if(emitEvent)
        emit openedModel(model);
    /*
    QList<int> foundIds;
    QString sids;
    for(int i=0; i<model->rowCount(); i++)
    {
        TRK_UINT id = model->rowId(i);
        foundIds.append(id);
        if(i)
            sids += ", ";
        sids += QString::number(id);
    }
    */

    return model;
}

NotesCol TrkToolProject::getNotes(int recId, TRK_RECORD_TYPE type) const
{
    NotesCol col;
    TRK_RECORD_HANDLE recHandle;
    if(!isTrkOK(TrkRecordHandleAlloc(handle, &recHandle)))
        return col;
    if(isTrkOK(TrkGetSingleRecord(recHandle, recId, type)))
        col = getNotes(recHandle);
	TrkRecordHandleFree(&recHandle);
    return col;
}

bool TrkToolProject::readNoteRec(TRK_NOTE_HANDLE noteHandle, TrkNote *note) const
{
    char buf[1024];
    if(isTrkOK(TrkGetNoteTitle(noteHandle, sizeof(buf), buf)))
        note->title = QString::fromLocal8Bit(buf);
    if(isTrkOK(TrkGetNoteAuthor(noteHandle, sizeof(buf), buf)))
        note->author = QString::fromLocal8Bit(buf);
    TRK_UINT len;
    if(isTrkOK(TrkGetNoteDataLength(noteHandle, &len) && len))
    {
        TRK_UINT remain = len;
        note->text = "";
        TRK_UINT res;
        while(remain)
        {
            //int blen = remain < sizeof(buf) ? remain : sizeof(buf);
            res = TrkGetNoteData(noteHandle, sizeof(buf), buf, &remain);
            note->text += QString::fromLocal8Bit(buf);
            if(res!=TRK_SUCCESS && res !=TRK_E_DATA_TRUNCATED)
                break;
        }
    }
    TRK_TIME time;
    if(isTrkOK(TrkGetNoteCreateTime(noteHandle, &time)))
        note->crdate = QDateTime::fromTime_t(time);
    if(isTrkOK(TrkGetNoteModifyTime(noteHandle, &time)))
        note->mddate = QDateTime::fromTime_t(time);
    TRK_UINT perm;
    if(isTrkOK(TrkGetNotePermissions(noteHandle,&perm)))
        note->perm = perm;
    return true;
}

NotesCol TrkToolProject::getNotes(TRK_RECORD_HANDLE recHandle) const
{
    NotesCol col;
    TrkScopeNoteHandle noteHandle(recHandle);
    if(noteHandle.isValid())
    {
        if(isTrkOK(TrkInitNoteList(*noteHandle)))
        {
            while(TRK_SUCCESS == TrkGetNextNote(*noteHandle))
            {
                TrkNote note;
                readNoteRec(*noteHandle, &note);
                col.append(note);
            }
        }
    }
    return col;
}

QString TrkToolProject::fieldVID2Name(TRK_RECORD_TYPE rectype, TRK_VID vid)
{
	char buf[1024];
	buf[0]=0;
    TrkGetFieldNameFromVid(handle,rectype, vid, sizeof(buf), buf);
	if(buf[0])
		return QString::fromLocal8Bit(buf);
	switch(vid)
	{
	case VID_InternalId:
		return "InternalId";
	case VID_Id:
		return "Id";
	case VID_Title:
		return "Title";
	case VID_Description:
		return "Description";
	case VID_Submitter:
		return "Submitter";
	case VID_SubmitDate:
		return "SubmitDate";
	case VID_Owner:
		return "Owner";
	}
	return "";
	/*
	VID_State = 10007,
	VID_CloseDate = 10008,
	VID_FirstTransactionId = 10010,
	VID_LastTransactionId = 10011,
	VID_LockUserId = 10012,
	VID_TypeId = 10013
	*/
	//return fields[rectype][vid].name;
}

TRK_VID TrkToolProject::fieldName2VID(TRK_RECORD_TYPE rectype, const QString &fname)
{
    NameVid & nv = recordDef[rectype]->nameVids;
    return nv[fname];
}

bool TrkToolProject::isSelectedId(TRK_UINT id, TRK_RECORD_TYPE recType) const
{
    return selected.contains(recType) && selected[recType].contains(id);
}

void TrkToolProject::setSelectedId(TRK_UINT id, bool value, TRK_RECORD_TYPE recType)
{
	if(value)
    {
        if(!selected[recType].contains(id))
        {
            selected[recType].insert(id);
            if(selectedModels.contains(recType))
                selectedModels[recType]->appendRecordId(id);
        }
    }
    else if(selected.contains(recType))
    {
        if(selectedModels.contains(recType))
            selectedModels[recType]->removeRecordId(id);
        selected[recType].remove(id);
    }
}

void TrkToolProject::clearSelected(TRK_RECORD_TYPE recType)
{
    selected[recType].clear();
    if(selectedModels.contains(recType))
        selectedModels[recType]->clearRecords();
}

// ============= TrkToolModel ===============

TrkToolModel::TrkToolModel(TrkToolProject *project, TRK_RECORD_TYPE type, QObject *parent)
	:
	prj(project),
	rectype(type),
    BaseRecModel<PTrkToolRecord>(parent),
    prevTransId(0)
    //, rset(parent)
{
    //char name[1024];
    //strncpy(name, prj->fields[rectype][VID_Id].name.toLocal8Bit().constData(), sizeof(name));
    idFieldName = prj->recordDef[rectype]->fieldName(VID_Id); //.toLocal8Bit().constData();
	QHash<TRK_VID, TrkFieldDef>::const_iterator fi;
    headers = prj->recordDef[rectype]->fieldNames();
    vids = prj->recordDef[rectype]->fieldIds();
    idCol = vids.indexOf(VID_Id);
    connect(project,SIGNAL(recordChanged(int)),this,SLOT(recordChanged(int)));
}

TrkToolModel::~TrkToolModel()
{
    clear();
}

bool TrkToolModel::openQuery(const QString &queryName, TRK_TRANSACTION_ID afterTransId)
{
    beginResetModel();
    //rset.clear();
    clearRecords();
    this->queryName = queryName;
    isQuery = true;
	/*
	QHash<TRK_VID, TrkFieldDef>::const_iterator fi;
	for(fi = prj->fields[rectype].constBegin(); fi!= prj->fields[rectype].constEnd(); ++fi)
	{
		rset.fields.append(fi.value().name);
	}
	*/
	bool rc;
	TRK_RECORD_HANDLE recHandle;
    rc = isTrkOK(TrkRecordHandleAlloc(prj->handle, &recHandle));
    bool res = rc;
    if(rc)
    {
        prevTransId = afterTransId;
        rc = isTrkOK(TrkQueryInitRecordList(recHandle, queryName.toLocal8Bit().constData(), prevTransId, &lastTransId));
        while(rc = (TRK_SUCCESS == (TrkGetNextRecord(recHandle))))
            appendRecord(recHandle);
        TrkRecordHandleFree(&recHandle);
    }
    endResetModel();
    return res;
}

bool TrkToolModel::openIds(const QList<int> &ids)
{
    beginResetModel();
	bool rc;
    QList <int> unique = uniqueIntList(ids);
	TRK_RECORD_HANDLE recHandle;
    rc = isTrkOK(TrkRecordHandleAlloc(prj->handle, &recHandle));
    if(rc)
    {
        prevTransId=0;
        //TRK_TRANSACTION_ID newTransId;
        foreach(int id, unique)
        {
            if(isTrkOK(TrkGetSingleRecord(recHandle, id, rectype)))
                appendRecord(recHandle);
        }
        queryName = intListToString(unique);
        isQuery = false;
        TrkRecordHandleFree(&recHandle);
    }
    endResetModel();
	return true;

}

bool TrkToolModel::appendRecord(TRK_RECORD_HANDLE recHandle)
{
	TRK_UINT Id;
    if(!isTrkOK(TrkGetNumericFieldValue(recHandle, idFieldName.toLocal8Bit().constData(), &Id)))
        return false;
    if(rowOfRecordId(Id)!=-1)
        return false;
	TrkToolRecord *rec = new TrkToolRecord(prj, rectype);
    rec->readHandle(recHandle); // no immediate read
	append(rec);
    rec->addLink();
    connect(rec,SIGNAL(changed(int)),this,SLOT(recordChanged(int)));
    return true;
}

void TrkToolModel::appendRecordId(TRK_UINT id)
{
    emit layoutAboutToBeChanged();
    TRK_RECORD_HANDLE recHandle;
    if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &recHandle)))
        return;
    if(isTrkOK(TrkGetSingleRecord(recHandle, id, rectype)))
        if(appendRecord(recHandle))
            addedIds.insert(id);
    TrkRecordHandleFree(&recHandle);
    emit layoutChanged();
}

void TrkToolModel::removeRecordId(TRK_UINT id)
{
    int r = rowOfRecordId(id);
    if(r==-1)
        return;
    beginRemoveRows(QModelIndex(),r,r);
    TrkToolRecord *rec = records.takeAt(r);
    //rec->disconnect(this);
    rec->removeLink(this);
    //delete rec;

    /*
    emit layoutAboutToBeChanged();
    int r;
    for(r=0; r<rowCount(); ++r)
    {
        if(rowId(r) == id)
        {
            TrkToolRecord *rec = records.takeAt(r);
            delete rec;
            break;
        }
    }
    emit layoutChanged();
    */
    endRemoveRows();
    addedIds.remove(id);
}

QVariant TrkToolModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(role == Qt::CheckStateRole && index.column()==idCol)
	{
		bool isSel = records[index.row()]->isSelected();
		Qt::CheckState state = isSel ? Qt::Checked : Qt::Unchecked;
		return QVariant::fromValue<int>(state);
	}
#ifndef CONSOLE_APP
    if(role == Qt::FontRole)
    {
        QFont font;
        if(addedIds.contains(records[index.row()]->recordId()))
           font.setItalic(true);
        return font;
    }
#endif
    return BaseRecModel::data(index, role);
}

bool TrkToolModel::setData(const QModelIndex & index, const QVariant & value, int role) 
{
    if(role == Qt::CheckStateRole && index.isValid() /*&& index.column()==idCol*/)
	{
		Qt::CheckState state = (Qt::CheckState)value.toInt();
		bool sel = (state == Qt::Checked);
		int row = index.row();
        if(!records[row]->isSelected() == sel)
        {
            records[row]->setSelected(sel);
            emit dataChanged(index, index);
        }
		return true;
	}
    return BaseRecModel::setData(index, value, role);
}

const RecordTypeDef *TrkToolModel::typeDef()
{
    if(!prj)
        return 0;
    if(!prj->recordDef.contains(rectype))
        return 0;
    return prj->recordDef[rectype];
}

void TrkToolModel::clearRecords()
{
    TrkToolRecord *rec;
    foreach(rec, records)
        if(rec)
            rec->removeLink(this);
    addedIds.clear();
    BaseRecModel::clearRecords();
}

QVariant TrkToolModel::displayColData(const PTrkToolRecord & rec, int col) const
{
	int vid = vids[col];
    return rec->value(vid,Qt::DisplayRole);
}

QVariant TrkToolModel::editColData(const PTrkToolRecord &rec, int col) const
{
    int vid = vids[col];
    return rec->value(vid,Qt::EditRole);
}

bool TrkToolModel::setEditColData(const PTrkToolRecord & rec, int col, const QVariant & value)
{
	int vid = vids[col];
	if(vid)
	{
		rec->values[vid] = value;
		return true;
	}
    return false;
}

void TrkToolModel::recordChanged(int id)
{
    int row = rowOfRecordId(id);
//    TrkToolRecord *rec =  qobject_cast<TrkToolRecord *>(sender());
//    if(!rec)
//        return;
//    int row = records.indexOf(rec);
    if(row<0)
        return;
    emit dataChanged(index(row,0),index(row,columnCount()-1));
}

/*
int	TrkToolModel::rowCount(const QModelIndex & parent) const
{
	if(parent.isValid())
		return 0;
	return rset.recCount();
}
*/

QDomDocument TrkToolModel::recordXml(int row) const
{
	TrkToolRecord * rec = at(row);
	return rec->toXML();
	/*
	QDomDocument xml("scr");
	QDomElement root=xml.createElement("scr");
	xml.appendChild(root);
	QDomElement flds =xml.createElement("fields");
	int cc = columnCount();
	//QSqlRecord rr = trkmodel.record(qryIndex);
	for(int c=0; c<cc; c++)
	{
		QDomElement f = xml.createElement("field");
		QString fname = headers[c];
		f.setAttribute("name", fname);
		QString fvalue = index(row, c).data().toString();
		QDomText v = xml.createTextNode(fvalue);
		f.appendChild(v);
		flds.appendChild(f);
	}
	root.appendChild(flds);
	QDomElement notes = xml.createElement("notes");
	NotesCol *col=prj->getNotes(rowId(row),rectype);
	for(int i=0; i<col->count(); i++)
	{
		QDomElement note = xml.createElement("note");
		const TrkNote &tn = col->value(i);
		note.setAttribute("title", tn.title);
		note.setAttribute("author", tn.author);
		note.setAttribute("createdate", tn.crdate.toString());
		QDomText v = xml.createTextNode(tn.text);
		note.appendChild(v);
		notes.appendChild(note);
	}
	delete col;
	root.appendChild(notes);
	return xml;
	*/
}

void TrkToolModel::refreshQuery()
{
    beginResetModel();
    QSet<int> a = addedIds;
    if(isQuery)
    {
        QString q = queryName;
        openQuery(q,prevTransId);
    }
    else
    {
        QList<int> ids = stringToIntList(queryName);
        openIds(ids);
    }
    foreach(int id, a)
        appendRecordId(id);
    endResetModel();
}

TRK_UINT TrkToolModel::rowId(int row) const
{
    //int col=vids.indexOf(VID_Id);
    if(idCol == -1)
		return 0;
    return index(row,idCol).data().toUInt();
}

int TrkToolModel::rowOfRecordId(int id) const
{
    for(int r=0; r<rowCount(); ++r)
        if(rowId(r) == id)
            return r;
    return -1;
}

Qt::ItemFlags TrkToolModel::flags ( const QModelIndex & index ) const
{
	Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	const TrkToolRecord *rec = at(index.row());
	if(!index.column())
		res |= Qt::ItemIsUserCheckable;
	if(rec->mode() == TrkToolRecord::View)
		return res;
	return res | Qt::ItemIsEditable;
}

QString TrkToolModel::getQueryName() const
{
    return queryName;
}

QList<int> TrkToolModel::getIdList() const
{
    QList<int> res;
    foreach(PTrkToolRecord rec, records)
    {
        uint id = rec->recordId(); // value(idFieldName).toUInt();
        res << id;
    }
    return res;
}

bool TrkToolModel::isSystemModel()
{
    foreach(const TrkToolModel *m, prj->selectedModels)
    {
        if(m==this)
            return true;
    }
    return false;
}



//====================== TrkToolRecordSet =====================

TrkToolRecordSet::TrkToolRecordSet(QObject * parent)
    : QObject(parent)
{
}

void TrkToolRecordSet::clear()
{
	records.clear();
}

int TrkToolRecordSet::recCount() const
{
	return records.count();
}

//====================== TrkToolRecord =====================


TrkToolRecord::TrkToolRecord(TrkToolProject *parent, TRK_RECORD_TYPE rtype)
    : QObject(0), prj(parent), rectype(rtype), recMode(View),
      lockHandle(0), fieldList(), links(0)
      //addedNotes()
{
    fieldList = prj->recordDef[rectype]->fieldNames();
}

TrkToolRecord::TrkToolRecord(const TrkToolRecord &src)
    : prj(src.prj), rectype(src.rectype), recMode(View),
      lockHandle(0), fieldList(), links(0)
      //addedNotes()
{
    fieldList = prj->recordDef[rectype]->fieldNames();
}


TrkToolRecord &TrkToolRecord::operator =(const TrkToolRecord &src)
{
    prj = src.prj;
    rectype = src.rectype;
    recMode = View;
    lockHandle = 0;
    fieldList.clear();
    links = 0;

    fieldList = prj->recordDef[rectype]->fieldNames();
    return *this;
}


TrkToolRecord::~TrkToolRecord()
{
    links=0;
	if(lockHandle)
	{
		TrkRecordHandleFree(&lockHandle);
		lockHandle=0;
	}
    //QObject::~QObject();
}

QVariant TrkToolRecord::value(const QString& fieldName, int role)
{
	TRK_VID vid = prj->fieldName2VID(rectype, fieldName);
    return value(vid, role);
}

QVariant TrkToolRecord::value(TRK_VID vid, int role)
{
    if(!values.contains(vid))
        readFullRecord();
    QVariant v = values[vid];
    if(role == Qt::DisplayRole)
    {
        TrkFieldDef fdef = fieldDef(vid);
        return fdef.valueToDisplay(v);
    }
//    if(role == Qt::DisplayRole && v.type() == QMetaType::QDateTime)
//        return QVariant(v.toDateTime().toString(TT_DATETIME_FORMAT));
    return v;
}

bool TrkToolRecord::insertBegin()
{
	if(recMode == Insert)
		return true;
	if(recMode == Edit)
		return false;
	if(!lockHandle)
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle,&lockHandle)))
			return false;
    if(!isTrkOK(TrkNewRecordBegin(lockHandle,rectype)))
		return false;
	recMode = Insert;
    emit changedState(recMode);
	return true;
}

bool TrkToolRecord::updateBegin()
{
	if(recMode == Edit)
		return true;
	if(recMode == Insert)
		return false;
	if(!lockHandle)
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle,&lockHandle)))
			return false;
    if(!isTrkOK(TrkGetSingleRecord(lockHandle, recordId(), rectype)))
		return false;
    readHandle(lockHandle,true);
    if(!isTrkOK(TrkUpdateRecordBegin(lockHandle)))
		return false;
	recMode = Edit;
    emit changedState(recMode);
	return true;
}

bool TrkToolRecord::commit()
{
	TRK_UINT res;
	switch(recMode)
	{
		case Edit:
			res = TrkUpdateRecordCommit(lockHandle, &lastTransaction);
            if(isTrkOK(res))
			{
                readHandle(lockHandle,true);
				TrkRecordHandleFree(&lockHandle);
				lockHandle=0;
				recMode = View;
                //addedNotes.clear();
                deletedNotes.clear();
                emit changedState(recMode);
                emit changed(recordId());
				return true;
			}
			return false;
		case Insert:
			res = TrkNewRecordCommit(lockHandle, &lastTransaction);
            if(isTrkOK(res))
			{
                readHandle(lockHandle,true);
                TrkRecordHandleFree(&lockHandle);
                lockHandle=0;
				recMode = View;
                //addedNotes.clear();
                deletedNotes.clear();
                emit changedState(recMode);
                emit changed(recordId());
                return true;
			}
			return false;
		default:
			return false;
	}
}

bool TrkToolRecord::cancel()
{
	if(!lockHandle)
	{
		recMode = View;
        emit changedState(recMode);
		return false;
	}
    if(recMode != View && isTrkOK(TrkRecordCancelTransaction(lockHandle)))
	{
		if(recMode == Edit)
            readHandle(lockHandle,true);
		TrkRecordHandleFree(&lockHandle);
		lockHandle=0;
        //addedNotes.clear();
        deletedNotes.clear();
		recMode = View;
        emit changedState(recMode);
        emit changed(recordId());
        return true;
	}
	return false;
}

void TrkToolRecord::readHandle(TRK_RECORD_HANDLE handle, bool force)
{
    readed = false;
    //char buf[1024];
    //TRK_VID v = prj->recordDef[rectype]->fieldVid(VID_Id);
    readFieldValue(handle, VID_Id);
    if(!force)
        return;
    QList<int> vids = prj->recordDef[rectype]->fieldIds();
    foreach(int vid, vids)
        readFieldValue(handle, vid);

//    QHash<TRK_VID, TrkFieldDef>::const_iterator fi;
//    for(fi = prj->recordDef[rectype]->fieldDefs.constBegin(); fi!= prj->recordDef[rectype]->fieldDefs.constEnd(); ++fi)
//	{
//        TRK_VID vid = fi.key();
//        readFieldValue(handle, vid);
//    }
}

void TrkToolRecord::readFieldValue(TRK_RECORD_HANDLE handle, TRK_VID vid)
{
    char buf[1024];
    TRK_FIELD_TYPE fType = prj->recordDef[rectype]->fieldType(vid);
    QString fname = prj->recordDef[rectype]->fieldName(vid);
    switch(fType)
    {
    case TRK_FIELD_TYPE_DATE:
        if(isTrkOK(TrkGetStringFieldValue(handle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
        {
            QDateTime dt = QDateTime::fromString(QString::fromLocal8Bit(buf),TT_DATETIME_FORMAT);
            values[vid] = QVariant::fromValue<QDateTime>(dt);
        }
        break;
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STATE:
    case TRK_FIELD_TYPE_STRING:
        if(isTrkOK(TrkGetStringFieldValue(handle, fname.toLocal8Bit().constData(), sizeof(buf), buf)))
            values[vid] = QVariant::fromValue<QString>(QString::fromLocal8Bit(buf));
        break;
    case TRK_FIELD_TYPE_NUMBER:
        TRK_UINT value;
        if(isTrkOK(TrkGetNumericFieldValue(handle, fname.toLocal8Bit().constData(), &value)))
            values[vid] = QVariant::fromValue<int>(value);
        break;
    }
}

void TrkToolRecord::readFullRecord()
{
    bool ch=!lockHandle;
    TRK_RECORD_HANDLE handle;
    if(ch)
    {
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
            return;
    }
    else
        handle=lockHandle;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype)))
    {
        readHandle(handle,true);
    }
    if(ch)
        TrkRecordHandleFree(&handle);
}

//void TrkToolRecord::needRecHandle()
//{
//    if(!recHandle)
//        isTrkOK(TrkRecordHandleAlloc(prj->handle, &recHandle));
//}

//void TrkToolRecord::freeRecHandle()
//{
//    if(recHandle)
//    {
//        TrkRecordHandleFree(&recHandle);
//        recHandle = 0;
//    }
//}

QList<TrkToolFile> TrkToolRecord::fileList()
{
    QList<TrkToolFile> res;
    bool ch=!lockHandle;
    TRK_RECORD_HANDLE handle;
    if(ch)
    {
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
            return res;
    }
    else
        handle=lockHandle;

    TRK_ATTFILE_HANDLE attHandle;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype))
            && isTrkOK(TrkAttachedFileHandleAlloc(handle,&attHandle))
            && isTrkOK(TrkInitAttachedFileList(attHandle)))
    {
        while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
        {
            TrkToolFile file;
            char buf[1024];
            if(TRK_SUCCESS == TrkGetAttachedFileName(attHandle,sizeof(buf),buf))
                file.fileName = QString::fromLocal8Bit(buf);
            TRK_TIME time;
            if(TRK_SUCCESS == TrkGetAttachedFileTime(attHandle, &time))
                file.createDateTime = QDateTime::fromTime_t(time);
            /*
            TRK_FILE_STORAGE_MODE mode;
            TRK_UINT sz;
            char buf1[1024],buf2[1024],buf3[1024],buf4[1024],buf5[1024];
            isTrkOK(TrkGetAttachedFileInfo(attHandle,buf1,buf2,buf3,buf4,buf5));
            */
            res.append(file);
        }
    }
    TrkAttachedFileHandleFree(&attHandle);
    if(ch)
        TrkRecordHandleFree(&handle);
    return res;
}

bool TrkToolRecord::saveFile(int fileIndex, const QString &dest)
{
    bool res=false;
    bool ch=!lockHandle;
    TRK_RECORD_HANDLE handle;
    if(ch)
    {
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
            return true;
    }
    else
        handle=lockHandle;
    TRK_ATTFILE_HANDLE attHandle;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype))
            && isTrkOK(TrkAttachedFileHandleAlloc(handle,&attHandle))
            && isTrkOK(TrkInitAttachedFileList(attHandle)))
    {
        while(TRK_SUCCESS == TrkGetNextAttachedFile(attHandle))
        {
            if(!fileIndex--)
            {
                res = isTrkOK(TrkExtractAttachedFile(attHandle,dest.toLocal8Bit().constData()));
                break;
            }
        }
    }
    TrkAttachedFileHandleFree(&attHandle);
    if(ch)
        TrkRecordHandleFree(&handle);
    return res;

}

void TrkToolRecord::refresh()
{
    if(lockHandle)
        readHandle(lockHandle);
    else
    {
        TrkScopeRecHandle recHandle(prj->handle);
        if(isTrkOK(recHandle.getRecord(recordId(), rectype)))
        {
            readHandle(*recHandle);
        }
    }
}

void TrkToolRecord::setValue(const QString& fieldName, const QVariant& value, int role)
{
    Q_UNUSED(role)
	if(recMode != Edit && recMode !=Insert)
		return;
	TRK_VID vid = prj->fieldName2VID(rectype, fieldName);
    const TrkFieldDef *def = prj->recordDef[rectype]->fieldVidDef(vid);
    QString s;
    QDateTime dt;
    switch(def->fType)
	{
    case TRK_FIELD_TYPE_DATE:
        dt = value.toDateTime();
        s = dt.toString(TT_DATETIME_FORMAT);
        if(isTrkOK(TrkSetStringFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), //buf
                                          s.toLocal8Bit().constData())))
        {
            readFieldValue(lockHandle, vid);
            //values[vid] = value;
            emit changed(recordId());
        }
        break;
        //case TRK_FIELD_TYPE_NONE:
	case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
	case TRK_FIELD_TYPE_OWNER:
	case TRK_FIELD_TYPE_USER:
	case TRK_FIELD_TYPE_ELAPSED_TIME:
	case TRK_FIELD_TYPE_STATE:
	case TRK_FIELD_TYPE_STRING:
        //char buf[1024];
        //strncpy(buf,value.toString().toLocal8Bit().constData(),sizeof(buf));
        s = value.toString();
        if(isTrkOK(TrkSetStringFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), //buf
                                                 s.toLocal8Bit().constData())))
        {
            readFieldValue(lockHandle, vid);
            //values[vid] = value;
            emit changed(recordId());
        }
		break;
	case TRK_FIELD_TYPE_NUMBER:
		TRK_UINT uint = value.toUInt();
        if(isTrkOK(TrkSetNumericFieldValue(lockHandle, fieldName.toLocal8Bit().constData(), uint)))
        {
            readFieldValue(lockHandle, vid);
            //values[vid] = QVariant::fromValue<int>(uint);
            emit changed(recordId());
        }
		break;
	}
}

QDomDocument TrkToolRecord::toXML()
{
	QDomDocument xml("scr");
	QDomElement root=xml.createElement("scr");
	xml.appendChild(root);
	QDomElement flds =xml.createElement("fields");
    QHash<TRK_VID, TrkFieldDef>::const_iterator fi;
    QList<TRK_VID> vids = prj->recordDef[rectype]->fieldVids();
    foreach(TRK_VID vid, vids) //fi = prj->recordDef[rectype]->fieldDefs.constBegin(); fi!= prj->recordDef[rectype]->fieldDefs.constEnd(); ++fi)
	{
        //QString fname = fi.value().name;
        //TRK_VID vid = fi.key();
        QString fname = prj->recordDef[rectype]->fieldName(vid);
        QVariant ftext = value(vid,Qt::DisplayRole);
        QVariant fvalue = value(vid,Qt::EditRole);

		QDomElement f = xml.createElement("field");
		//QString fname = headers[c];
		f.setAttribute("name", fname);
        f.setAttribute("value",fvalue.toString());
		//QString fvalue = index(row, c).data().toString();
        QDomText v = xml.createTextNode(ftext.toString());
		f.appendChild(v);
		flds.appendChild(f);
	}
	root.appendChild(flds);
	QDomElement desc = xml.createElement("Description");
	desc.setAttribute("name", prj->fieldVID2Name(rectype, VID_Description));
	QDomText v = xml.createTextNode(description());
	desc.appendChild(v);
	root.appendChild(desc);

    // fill <notes>
	QDomElement notes = xml.createElement("notes");

    NotesCol notesCol = getNotes();
//    if(lockHandle)
//        notesCol = prj->getNotes(lockHandle);
//    else
//        notesCol = prj->getNotes(recordId(),rectype);

    int index=0;
    foreach(const TrkNote &tn, notesCol)
    {
        QDomElement note = xml.createElement("note");
        //const TrkNote &tn = notesCol.at(i);
        note.setAttribute("title", tn.title);
        note.setAttribute("author", prj->userFullName(tn.author));
        note.setAttribute("cdatetime", tn.crdate.toString(Qt::ISODate));
        note.setAttribute("createdate", tn.crdate.toString(TT_DATETIME_FORMAT));
        note.setAttribute("mdatetime", tn.mddate.toString(Qt::ISODate));
        note.setAttribute("modifydate", tn.mddate.toString(TT_DATETIME_FORMAT));
        note.setAttribute("editable", QString(tn.perm?"true":"false"));
        note.setAttribute("index",index++);
        QDomText v = xml.createTextNode(tn.text);
        note.appendChild(v);
        notes.appendChild(note);
    }
    //delete col;
	root.appendChild(notes);

    // fill <history>
    QDomElement history = xml.createElement("history");
    index=0;
    foreach(const QString &item, historyList())
    {
        QDomElement change = xml.createElement("change");
        QStringList sections = item.split(QChar(' '));
        QString changeDate = sections[0];
        QString changeTime = sections[1];//item.left(10+1+5);
        QDateTime changeDateTime = QDateTime::fromString(changeDate + " " + changeTime, TT_DATETIME_FORMAT);
        QString changeAuthor = sections[2].mid(1,sections[2].length()-3);

        QString changeDesc = QStringList(sections.mid(5)).join(" ");
        change.setAttribute("author", changeAuthor);
        change.setAttribute("datetime", changeDateTime.toString(Qt::ISODate));
        change.setAttribute("createdate", changeDate + " " + changeTime);
        change.setAttribute("action", changeDesc);
        change.setAttribute("index",index++);
        QDomText v = xml.createTextNode(item);
        change.appendChild(v);
        history.appendChild(change);
    }
    root.appendChild(history);
    return xml;
}

QString TrkToolRecord::toHTML(const QString &xqCodeFile)
{
    QDomDocument xml=toXML();
    /*QDomDocument xml("records");
    QDomElement root=xml.createElement("records");
    xml.appendChild(root);
    root.appendChild(toXML());*/




    QFile xq(xqCodeFile);
    xq.open(QIODevice::ReadOnly);
    QFile trackerXML("data/tracker.xml");
    trackerXML.open(QIODevice::ReadOnly);
    QXmlQuery query;
#ifndef CONSOLE_APP
    query.setMessageHandler(sysMessager);
#endif


    QString page;
    //QString src=xml.toString();
    QByteArray ba=xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    query.bindVariable("scrdoc",&buf);
    query.bindVariable("def",&trackerXML);
    query.setQuery(&xq);
    //query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
    query.evaluateTo(&page);
#ifdef QT_DEBUG
    QFile testXml("!testEdit.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);

    QFile testRes("!testResult.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    return page;
}

QStringList TrkToolRecord::historyList() const
{
    TRK_RECORD_HANDLE handle;
    if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
        return QStringList();
    QStringList result;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype)))
    {
        char buf[1024];
        if(isTrkOK(TrkInitChangeList(handle)))
        {
            while(1)
            {
                TRK_UINT res = TrkGetNextChange(handle, sizeof(buf), buf);
                if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
                    break;
                result.append(QString::fromLocal8Bit(buf));
            }
        }
    }
    TrkRecordHandleFree(&handle);
    return result;
}


QString TrkToolRecord::description() const
{
	TRK_RECORD_HANDLE handle;
    if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
		return QString();
	QString result;
    if(isTrkOK(TrkGetSingleRecord(handle, recordId(), rectype)))
	{
		TRK_UINT remain;
        if(isTrkOK(TrkGetDescriptionDataLength(handle, &remain)))
		{
			char buf[1024];
			TRK_UINT res;
			while(remain)
			{
				res = TrkGetDescriptionData(handle,sizeof(buf),buf,&remain);
				if(res != TRK_SUCCESS && res != TRK_E_DATA_TRUNCATED)
					break;
				result += buf;
			}
		}
	}
	TrkRecordHandleFree(&handle);
	return result;
}

bool TrkToolRecord::setDescription(const QString &desc)
{
	if(recMode != Edit && recMode != Insert)
        return false;
	QByteArray loc = desc.toLocal8Bit();
    bool res = isTrkOK(TrkSetDescriptionData(lockHandle, loc.count(), loc.constData(), 0));
        emit changed(recordId());
    return res;
}

const QStringList & TrkToolRecord::fields() const
{
	return fieldList;
}

bool TrkToolRecord::isSelected() const
{
	return prj->isSelectedId(recordId());
}

void TrkToolRecord::setSelected(bool value)
{
    prj->setSelectedId(recordId(), value);
    emit changed(recordId());
}

bool TrkToolRecord::isFieldReadOnly(const QString &field) const
{
    if(mode() == View)
        return true;

    bool ro=true, ch=!lockHandle;
    TRK_RECORD_HANDLE handle;
    if(ch)
    {
        if(!isTrkOK(TrkRecordHandleAlloc(prj->handle, &handle)))
            return true;
    }
    else
        handle=lockHandle;

    TRK_ACCESS_MODE accMode;
    if(isTrkOK(TrkGetFieldAccessRights(handle,field.toLocal8Bit().constData(),&accMode)))
        ro = !accMode;
    if(ch)
        TrkRecordHandleFree(&handle);
    return ro;
}

static TRK_UINT Do_TrkSetNoteData(TRK_NOTE_HANDLE noteHandle, const QByteArray &data)
{
    return TrkSetNoteData(noteHandle, data.size(), data.constData(), 0);
}


bool TrkToolRecord::setNote(int index, const QString &title, const QString &text)
{
    if(!isEditing())
        return false;
    bool res=false;
    bool wasView = (mode()==View);
    if(wasView)
        if(!updateBegin())
            return false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            TrkGetNextNote(*noteHandle);
        res = isTrkOK(TrkSetNoteTitle(*noteHandle, title.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, text.toLocal8Bit()));
    }
    if(wasView)
        if(res)
            commit();
        else
            cancel();
    emit changed(recordId());
    return res;
}

bool TrkToolRecord::setNoteText(int index, const QString &text)
{
    if(!isEditing())
        return false;
    bool res=false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        for(int i=0; i<=index; ++i)
            TrkGetNextNote(*noteHandle);
        res = isTrkOK(Do_TrkSetNoteData(*noteHandle, text.toLocal8Bit()));
    }
    emit changed(recordId());
    return res;
}

bool TrkToolRecord::deleteNote(int index)
{
    if(!isEditing())
        return false;
    bool res=false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkInitNoteList(*noteHandle)))
    {
        int i=0;
        bool res;
        for(int i=0; i<=index; ++i)
            if(!isTrkOK(TrkGetNextNote(*noteHandle)))
                return false;
        res = isTrkOK(TrkDeleteNote(*noteHandle));
        if(res)
            deletedNotes.append(i);
    }
    emit changed(recordId());
    return res;
}

bool TrkToolRecord::appendNote(const QString &noteTitle, const QString &note)
{
    if(!isEditing())
        return false;
    bool res=false;
    bool wasView = (mode()==View);
    if(wasView)
        if(!updateBegin())
            return false;
    TrkScopeNoteHandle noteHandle(lockHandle);
    if(!noteHandle.isValid())
        return false;
    if(isTrkOK(TrkAddNewNote(*noteHandle)))
    {
        res = isTrkOK(TrkSetNoteTitle(*noteHandle, noteTitle.toLocal8Bit().constData()))
            && isTrkOK(Do_TrkSetNoteData(*noteHandle, note.toLocal8Bit()));
//        if(res)
//        {
//            TrkNote tmpNote;
//            project()->readNoteRec(*noteHandle,&tmpNote);
//            addedNotes.append(tmpNote);
//        }
    }
    if(wasView)
        if(res)
            commit();
        else
            cancel();
    emit changed(recordId());
    return res;
}

void TrkToolRecord::addLink()
{
    links++;
}

void TrkToolRecord::removeLink(const QObject *receiver)
{
    if(!this)
        return;
    if(receiver)
        disconnect(receiver);
    if(--links <= 0)
        deleteLater();
}

NotesCol TrkToolRecord::getNotes() const
{
    NotesCol res;
    if(lockHandle)
        res = prj->getNotes(lockHandle);
    else
        res = prj->getNotes(recordId(),rectype);
    //res.append(addedNotes);
    return res;
}


//================== TrkHistory =======================

TrkHistory::TrkHistory(QObject *parent)
    : BaseRecModel(parent),
      prj(0)
{
    headers << tr("пїЅпїЅпїЅпїЅпїЅпїЅпїЅ");
    headers << tr("пїЅпїЅпїЅ");
    headers << tr("пїЅпїЅпїЅпїЅпїЅпїЅ");
    headers << tr("пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ");
    headers << tr("пїЅпїЅпїЅпїЅпїЅ");
}

TrkHistory::~TrkHistory()
{
}

void TrkHistory::setProject(TrkToolProject *project)
{
    if(prj!=project)
    {
        clearRecords();
        if(prj)
            prj->disconnect(this);
        prj = project;
        if(prj)
            connect(prj,SIGNAL(openedModel(const TrkToolModel*)),this,SLOT(openedModel(const TrkToolModel*)));
    }
}

void TrkHistory::setUnique(bool value)
{
    unique = value;
}

void TrkHistory::removeLast()
{
    records.removeLast();
}

QVariant TrkHistory::displayColData(const TrkHistoryItem &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.queryName;
    case 1:
        return rec.isQuery;
    case 2:
        return rec.foundIds;
    case 3:
        return QVariant((uint)rec.rectype);
    case 4:
        return rec.createDateTime;
    }
    return QVariant();
}

void TrkHistory::openedModel(const TrkToolModel *model)
{
    TrkHistoryItem item;
    bool isNew=true;
    QString qName = model->getQueryName();
    int i;
    if(unique)
        for(i = 0; i < records.count(); i++)
        {
            const TrkHistoryItem &fi = records[i];
            if(fi.projectName == model->prj->name
                    && fi.queryName == qName)
            {
                isNew = false;
                break;
            }
        }
    if(!isNew)
        item = records[i];
    else
    {
        item.projectName = model->prj->name;
        item.queryName = model->getQueryName();
    }
    item.foundIds = intListToString(model->getIdList());
    item.isQuery = model->isQuery;
    item.rectype = model->rectype;
    item.createDateTime = QDateTime::currentDateTime();
    if(isNew)
        append(item);
    else
        records[i]=item;
}

// ======================= TrkToolQryModel ====================================

TrkToolQryModel::TrkToolQryModel(QObject *parent)
    : BaseRecModel(parent)
{
    headers
            << tr("пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ")
            << tr("пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ")
            << tr("пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ");
}

void TrkToolQryModel::appendQry(const QString &queryName, bool isPublic, TRK_UINT rectype)
{
    TrkQuery query;
    query.qryName = queryName;
    query.isPublic = isPublic;
    query.qryTypeId = rectype;
    append(query);
}

QVariant TrkToolQryModel::displayColData(const TrkQuery &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.qryName;
    case 1:
        return rec.isPublic;
    case 2:
         return rec.qryTypeId;
    }
    return QVariant();
}

#ifndef CONSOLE_APP
//TrkQryFilter

TrkQryFilter::TrkQryFilter(QObject *parent)
    : QSortFilterProxyModel(parent),
      filter(All)
{
}

void TrkQryFilter::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    switch(filter)
    {
    case All:
        setFilterRegExp("");
        setFilterKeyColumn(1);
        break;
    case UserOnly:
        setFilterRegExp("false");
        setFilterKeyColumn(1);
        break;
    case PublicOnly:
        setFilterRegExp("true");
        setFilterKeyColumn(1);
        break;
    }
    sort(0);
}
#endif

const ChoiceList &TrkFieldDef::choiceList() const
{
    if(!isChoice())
        return *p_choiceList;

    if(recDef && !p_choiceList->count())
    {
        const ChoiceList &list = recDef->choiceList(name);
        p_choiceList->clear();
        if(isNullable())
        {
            TrkToolChoice ch;
            ch.displayText="";
            ch.fieldValue=QVariant();
            p_choiceList->append(ch); // leak 24 bytes
        }
        p_choiceList->append(list);// leak 24 bytes
    }
    return *p_choiceList;
}

QStringList TrkFieldDef::choiceStringList(bool isDisplayText) const
{
    QStringList list;
    //const ChoiceList &chList = choiceList();
    foreach(const TrkToolChoice &item, choiceList())
    {
        if(isDisplayText)
            list.append(item.displayText);
        else
            list.append(item.fieldValue.toString());
    }
    return list;
}

bool TrkFieldDef::canSubmit()
{
    return isValid() && recDef->canFieldSubmit(name);
}

bool TrkFieldDef::canUpdate()
{
    return isValid() && recDef->canFieldUpdate(name);
}

QString TrkFieldDef::valueToDisplay(const QVariant &value) const
{
    QDateTime dt;
    switch(fType)
    {
    case TRK_FIELD_TYPE_DATE:
            dt = value.toDateTime();
            return dt.toString(TT_DATETIME_FORMAT);
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_STATE:
    {
        const ChoiceList &list = choiceList();
        foreach(const TrkToolChoice &c, list)
        {
            if(c.fieldValue == value)
                return c.displayText;
        }
        return "";
    }
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STRING:
    case TRK_FIELD_TYPE_NUMBER:
        return value.toString();
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    {
        return recDef->project()->userFullName(value.toString());
    }

    }
    return value.toString();
}

QVariant TrkFieldDef::displayToValue(const QString &text) const
{
    QDateTime dt;
    switch(fType)
    {
    case TRK_FIELD_TYPE_DATE:
        dt = QDateTime::fromString(TT_DATETIME_FORMAT);
        return QVariant(dt);
    //case TRK_FIELD_TYPE_NONE:
    case TRK_FIELD_TYPE_CHOICE:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_USER:
    case TRK_FIELD_TYPE_STATE:
    {
        const ChoiceList &list = choiceList();
        foreach(const TrkToolChoice &c, list)
        {
            if(c.displayText.compare(text,Qt::CaseInsensitive) == 0)
                return c.fieldValue;
        }
        return QVariant();
    }
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_STRING:
        return text;
    case TRK_FIELD_TYPE_NUMBER:
        bool ok;
        int v = text.toInt(&ok);
        if(!ok)
            return QVariant();
        return v;

    }
    return text;
}

/*
const ChoiceList *TrkFieldDef::userList()
{
    if(!isUser())
        return NULL
}
*/


bool RecordTypeDef::canFieldSubmit(const QString &name) const
{
    return prj->canFieldSubmit(name, recType);
}

bool RecordTypeDef::canFieldUpdate(const QString &name) const
{
    return prj->canFieldUpdate(name, recType);
}

const ChoiceList &RecordTypeDef::choiceList(const QString &fieldName)
{
    TRK_VID vid = nameVids.contains(fieldName) ? nameVids[fieldName] : -1;
    if(vid == -1)
        return emptyChoices;
    return *prj->fieldChoiceList(fieldName, recType);
}

TrkScopeRecHandle::TrkScopeRecHandle(TRK_HANDLE prjHandle)
    :handle(0)
{
    TrkRecordHandleAlloc(prjHandle, &handle);
}

TrkScopeRecHandle::~TrkScopeRecHandle()
{
    TrkRecordHandleFree(&handle);
}

int TrkScopeRecHandle::getRecord(TRK_UINT id, TRK_RECORD_TYPE rectype)
{
    return TrkGetSingleRecord(handle, id, rectype);
}


TrkScopeNoteHandle::TrkScopeNoteHandle(TRK_RECORD_HANDLE recHandle)
{
    handle = 0;
    TrkNoteHandleAlloc(recHandle, &handle);
}

TrkScopeNoteHandle::~TrkScopeNoteHandle()
{
    TrkNoteHandleFree(&handle);
}
