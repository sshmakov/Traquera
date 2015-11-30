#include "project.h"
#include "plans.h"
//#include "ttglobal.h"
#include <QtGui>
//#include <QSqlError>
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <comdef.h>

PrjModel::PrjModel(PrjItemModel *projectModel, QObject *parent)
	: 
	//prjdb(),
	//queryModel(parent), 
	//prjapp(parent)
	//userFilter(parent),
	dependFilter(parent)
{
	if(!projectModel)
		prjmodel = new PrjItemModel(parent);
	else
		prjmodel = projectModel;
	dependFilter.setSourceModel(prjmodel);
	model = &dependFilter;
	/*
	static int conn = 0;
	query = NULL;
	prjdb = QSqlDatabase::addDatabase("QSQLITE", "prj"+QString::number(++conn));
    prjdb.setDatabaseName(":memory:");
    if (!prjdb.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot create project database"),
			prjdb.lastError().text(), QMessageBox::Cancel);
        return;
    }
	QSqlQuery query(prjdb);
	query.exec("create table tasks(id int primary key, num int, name varchar(1024), scr varchar(1024), start datetime, finish datetime)");
	dependFilter.setSourceModel(&queryModel);
	userFilter.setSourceModel(&dependFilter);
	model = &userFilter;
	loadDefinition("data/project.xml");
	*/
}

bool PrjModel::openProject(const QString &prjfile)
{
	return prjmodel->open(prjfile);
}

void PrjModel::setParentColumn(int srcfield, const QString &dstfield)
{
	parentColumn = srcfield;
	int fnum=0;
	for(int i=0; i < prjmodel->fields.count(); i++)
	{
		if(!prjmodel->fields[i].title.compare(dstfield,Qt::CaseInsensitive))
		{
			fnum=i;
			break;
		}
	}
	dependFilter.setFilterKeyColumn(fnum);
}

void PrjModel::changedParent(const QModelIndex &index)
{
	if(!index.model())
		return;
	QModelIndex i= index.model()->index(index.row(),parentColumn);
	int scr = i.data().toInt();
    dependFilter.setFilterWildcard("*"+QString::number(scr)+"*");
}

int PrjModel::rowOfTask(int taskNum)
{
    if(!prjmodel)
        return -1;
    int col = prjmodel->colTaskNum();
    if(col<0)
        return -1;
    QModelIndex start = prjmodel->index(0,col);
    QModelIndexList list = prjmodel->match(start,Qt::DisplayRole,taskNum,1,Qt::MatchExactly);
    if(list.isEmpty())
        return -1;
    return list[0].row();
}

/*
void PrjModel::readCash()
{
	cash.blockSignals(true);
	cash.clear();
	PrjItemModel::
	for(int r=0; r<prjmodel.rowCount(); r++)
		for(int c=0; c<prjmodel.columnCount(); c++)
		{
			QStandardItem *item=new QStandardItem(prjmodel.d
	cash.blockSignals(false);
}
*/

// PrjItemModel ==================

PrjItemModel::PrjItemModel(QObject *parent)
    :QAbstractItemModel(parent)
{
    prj=0;
    tasks=0;
    rows=0;
    cols=0;
    outlineCol=-1;
    cash = new QStandardItemModel(this);
    isCashed = false;
    connect(&app,SIGNAL(exception(int,QString,QString,QString)),
            this,SLOT(slotException(int,QString,QString,QString)));
}


QModelIndex	PrjItemModel::index( int row, int column, const QModelIndex & /* parent */) const
{
	return createIndex(row, column);
}
QModelIndex PrjItemModel::parent(const QModelIndex & /* child */) const
{
    return QModelIndex();
}

QVariant PrjItemModel::fieldValue(int row, const QString &fieldName, int role) const
{
    if(!colByField.contains(fieldName))
        return QVariant();
    int col = colByField[fieldName];
    return cash->item(row,col)->data(role);
}

int PrjItemModel::columnCount(const QModelIndex & /* parent */) const
{
	return cols;
}

QVariant PrjItemModel::data(const QModelIndex & index, int role) const
{
    QStandardItem *item = cash->item(index.row(),index.column());
    if(!item)
        return QVariant();
    switch(role)
    {
    case Qt::FontRole:
        if(fieldValue(index.row(),"Summary",Qt::DisplayRole).toInt())
        {
            QFont f;
            f.setBold(true);
            return f;
        }
        return QVariant();
    case Qt::UserRole:
    case Qt::EditRole:
        return item->data(role);
    case Qt::DisplayRole:
    {
        int col=index.column();
        QVariant res=item->data(role);
        if(col == 1)
        {
            int o = fieldValue(index.row(), "OutlineLevel").toInt();
            if(o)
                res = QString("  ").repeated(o-1) + res.toString();
        }
        return res;
    }
    case Qt::TextAlignmentRole:
        switch (index.column())
        {
        case 0: return Qt::AlignLeft;
        }
        return QVariant();
    }
    return QVariant();
    /*
	QAxObject *task;
	if(!index.isValid())
		return QVariant();
	if(index.row() >= rows)
		return QVariant();
	task = tasks->querySubObject("item(int)",index.row()+1);
	if(!task)
		return QVariant();
	if(index.column() >= cols)
		return QVariant();
	QString s=fields[index.column()].field;
	res = task->property(s.toAscii().constData());
	//QStandardItem *item=new QStandardItem();
	//item->setData(res,role);
	cash->setData(index,res,role);
	return res;
    */
}

QVariant PrjItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::SizeHintRole)
        return  QSize(fields[section].colSize, 19);
    if(role != Qt::DisplayRole)
		return QVariant();
	if(orientation == Qt::Horizontal)
	{
		if(section <0 || section >=fields.count())
			return QVariant();
		return fields[section].title;
	}
	else
    {
        QStandardItem *item = cash->item(section,0);
        QVariant res=item->data(role);
        return res;

    }
    return QVariant();
}

int	PrjItemModel::rowCount(const QModelIndex & /* parent */) const
{
	return rows;
}

static void saveDoc(const QString &fileName, QAxObject *obj)
{
	QFile fileDoc(fileName);
	if(fileDoc.open(QIODevice::WriteOnly))
	{
		QString doc = obj->generateDocumentation();
		fileDoc.write(doc.toLocal8Bit());
	}
}

bool PrjItemModel::open(const QString &file, bool readOnly)
{
	prj=NULL;
	tasks=NULL;
    app.setControl("MSProject.Application"); // {36D27C48-A1E8-11D3-BA55-00C04F72F325}
    IDispatch *disp=0;
    app.queryInterface(IID_IDispatch, (void**)&disp);
    if(!disp)
        return false;
    disp->Release();
    QVariant v = app.property("Visible");
    bool vis = v.toBool();
	if(!vis || file.isEmpty())
		app.setProperty("Visible", true);

    char *method = "FileOpen(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, PjPoolOpen, QVariant, QVariant, QVariant, QVariant)";

	bool result;
	if(file.isEmpty())
		result = app.dynamicCall("FileOpen()").toBool();
	else
        result = app.dynamicCall(method, file, readOnly).toBool();
	if(!result)
	{
		if(file.isEmpty())
			return false;
		QAxObject *p = app.querySubObject("ActiveProject");

		if(p && !p->dynamicCall("FullName").toString().compare(file,Qt::CaseInsensitive))
			prj = p;
		else
		{
			QAxObject *prjs = app.querySubObject("Projects");
			if(!prjs)
				return false;

			for(int i=0; i<prjs->dynamicCall("Count").toInt(); i++)
			{
				QAxObject *p = prjs->querySubObject("Item(int)", i+1);
				if(!p)
					continue;
				QString pfile = p->dynamicCall("FullName").toString();
				if(!pfile.compare(file,Qt::CaseInsensitive))
				{
					prj = p;
					break;
				}
			}
			if(!prj)
				return false;
		}
	}
	else
		prj = app.querySubObject("ActiveProject");
    tasks = prj->querySubObject("Tasks");
#ifdef QT_DEBUG
	saveDoc("data/tasks.htm",tasks);
#endif
	prjName = prj->property("Name").toString();
	fileName = prj->property("FullName").toString();
    readOnly = prj->dynamicCall("ReadOnly").toBool();
    loadDefinition(app, pluginObject->dataDir.absoluteFilePath("project.xml"));
	rows = tasks->dynamicCall("Count").toInt();
	cols = fields.count();
	saveToMemory(tasks);
	isReadOnly = readOnly;
	if(isReadOnly)
	{
		delete tasks;
		tasks=0;
		delete prj;
		prj=0;
		app.clear();
	}
	reset();
	return true;
}

void PrjItemModel::saveToMemory(QAxObject *tasks)
{
    outlineCol = colByField.contains("OutlineLevel") ? colByField["OutlineLevel"] : -1;
	for(int r=0; r<rows; r++)
	{
		QAxObject *task;
		task = tasks->querySubObject("item(int)",r+1);
		if(!task)
			continue;
		readTask(r, task);
	}
//    if(colByField.contains("ParentTask"))
//    {
//        int pcol;
//        pcol = colByField["ParentTask"];
//        for(int r=0; r<rows; r++)
//        {

//        }
//    }
}


void PrjItemModel::loadDefinition(QAxObject &app, const QString &fileName)
{
//	QXmlSimpleReader xmlReader;
	QFile *file = new QFile(fileName);
	QXmlInputSource *source = new QXmlInputSource(file);
	QDomDocument dom;
	if(!dom.setContent(source,false))
		return;
	QDomElement doc = dom.documentElement();
	if(doc.isNull()) return;
	//QDomElement prj = doc.firstChildElement("project");
	//if(prj.isNull()) return;
	QDomElement tasks = doc.firstChildElement("tasks");
	if(tasks.isNull()) return;
	QDomElement flds = tasks.firstChildElement("fields");
	if(flds.isNull()) return;
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
	QDomElement field = flds.firstChildElement("field");
	for(int i=0; !field.isNull(); field = field.nextSiblingElement("field"), i++)
	{
		PrjField f;
        f.field = field.attribute("property");
		f.title = field.attribute("title");
        f.fieldId = field.attribute("const").toInt();
        if(f.field.isEmpty() && !f.fieldId && !f.title.isEmpty())
            f.fieldId = app.dynamicCall("FieldNameToFieldConstant(QVariant)",f.title).toInt();
        if(f.title.isEmpty())
        {
            if(f.fieldId)
                f.title = app.dynamicCall("FieldConstantToFieldName(QVariant)",f.fieldId).toString();
            else if(hc.contains(f.field.toUpper()))
                f.title = app.dynamicCall("FieldConstantToFieldName(QVariant)",hc[f.field.toUpper()]).toString();
            if(f.title.isEmpty())
                f.title = f.field;
        }
		f.type = field.attribute("type");
        if(!f.type.compare("SCR",Qt::CaseInsensitive))
            f.stdType = TQ::TQ_FIELD_TYPE_NUMBER;
        else if (!f.type.compare("%",Qt::CaseInsensitive))
            f.stdType = TQ::TQ_FIELD_TYPE_PERCENT;
        else if (!f.type.compare("min",Qt::CaseInsensitive))
            f.stdType = TQ::TQ_FIELD_TYPE_NUMBER;
        else
            f.stdType = TQ::TQ_FIELD_TYPE_STRING;
        f.display = field.attribute("display");
        f.format = field.attribute("format");
		f.internalField = "f"+QString::number(i);
        f.colSize = field.attribute("size", "50").toInt();
		fields.append(f);
		colByTitle[f.title]=i;
		colByField[f.field]=i;
	}
	delete file;
}

QString PrjItemModel::projectName()
{
	return prjName;
}

bool PrjItemModel::addTask(const PrjRecord &record)
{
	if(tasks && !tasks->isNull())
	{
        emit beginInsertRows(QModelIndex(),rows,rows);
		QAxObject *task = tasks->querySubObject("Add()");
		for(PrjRecord::const_iterator i=record.constBegin();
			i!=record.constEnd();
			++i)
		{
			int col;
//			if(byTitle)
                col= colByTitle.value(i.key(),-1);
//			else
//				col= colByField.value(i.key(),-1);
			if(col<0)
				continue;
			QString f = fields[col].field;
            if(!f.isEmpty())
                task->setProperty(f.toLocal8Bit().constData(),i.value());
            else
                task->dynamicCall("SetField(QVariant,QVariant)",fields[col].fieldId,i.value());
		}
		readTask(rows, task);
		rows++;
        emit endInsertRows();
		return true;
	}
	return false;
}

static QStandardItem * findPrevRow(QStandardItemModel *model, int &row, int column)
{
    if(!model)
        return 0;
    QStandardItem *item;
    while((item = model->item(--row,column)) == 0 && row > 0) ;
    return item;
}

void PrjItemModel::readTask(int row, QAxObject *task)
{
	for(int c=0; c<cols; c++)
	{
		QVariant res;
		//res = bag[fields[c].field];
		//res = task->dynamicCall(fields[c].field.toLocal8Bit().constData());
        if(!fields[c].field.isEmpty())
            res = task->property(fields[c].field.toLocal8Bit().constData());
        else
            res = task->dynamicCall("GetField(QVariant)",fields[c].fieldId);
		QStandardItem *item = new QStandardItem();
        item->setData(res, Qt::EditRole);
        const QString &type = fields[c].type;
        const QString &format = fields[c].format;
        if(type == "min")
            if(format == "days")
                res = QString(tr("%1 д")).arg(res.toDouble()/60.0/8.0,0,'g',3);
            else
                res = QString(tr("%1 ч")).arg(res.toDouble()/60.0,0,'g',3);
        if(type == "sec")
            res = QString(tr("%1 ч")).arg(res.toDouble()/3600.0,0,'g',3);
        if(type == "%")
            res = QString(tr("%1%")).arg(res.toInt());
        item->setData(res, Qt::DisplayRole);
        cash->setItem(row,c,item);
    }
    if(outlineCol >= 0 && row)
    {
        int level = cash->item(row,outlineCol)->data(Qt::EditRole).toInt();
        int prevRow = row;
        int parentTask = 0;
        QStandardItem *prevItem = findPrevRow(cash, prevRow, outlineCol);
        int prevLevel;
        if(prevItem)
            prevLevel = prevItem->data(Qt::EditRole).toInt();
        else
        {
            level = 1; // если нет предыдущей задачи, то это верхний уровень
        }
        if(level == 1) // верхний уровень задач
        {
            cash->item(row,0)->setData(0,Qt::UserRole);
        }
        else if(prevLevel == level) // такой же уровень, как у предыдущей задачи
        {
            parentTask = cash->item(prevRow,0)->data(Qt::UserRole).toInt();
            cash->item(row,0)->setData(parentTask,Qt::UserRole);
        }
        else if (prevLevel < level) // предыдущая задача является суммарной для текущей
        {
            parentTask = row;
            cash->item(row,0)->setData(parentTask,Qt::UserRole);
        }
        else // текущая задача выше предыдущей
        {
            parentTask = cash->item(prevRow,0)->data(Qt::UserRole).toInt(); // номер предыдущей задачи
            while(prevLevel >= level) // пока не найдем задачу, которая выше по уровню, чем текущая
            {
                parentTask = cash->item(parentTask-1,0)->data(Qt::UserRole).toInt();
                prevLevel = cash->item(parentTask-1,outlineCol)->data(Qt::EditRole).toInt();
            }
            cash->item(row,0)->setData(parentTask,Qt::UserRole);
        }

    }
}

int PrjItemModel::colTaskNum()
{
    if(colByField.contains("ID"))
        return colByField["ID"];
    return -1;
}

void PrjItemModel::slotException(int code, const QString &source, const QString &desc, const QString &help)
{
    pluginObject->emitError(QString(tr("ERROR: (%1) %2")).arg(code).arg(desc));
}


TaskTypeDef::TaskTypeDef(PrjItemModel *srcModel)
{
    model = srcModel;
}

QStringList TaskTypeDef::fieldNames() const
{
    QStringList res;
    foreach(const PrjField &f, model->fields)
        res.append(f.field);
    return res;
}

int TaskTypeDef::fieldNativeType(const QString &name) const
{
    foreach(const PrjField &f, model->fields)
    {
        if(!f.field.compare(name,Qt::CaseInsensitive))
            return f.stdType;
    }
    return 0;
}

bool TaskTypeDef::canFieldSubmit(const QString &name) const
{
    Q_UNUSED(name)
    return false;
}

bool TaskTypeDef::canFieldUpdate(const QString &name) const
{
    Q_UNUSED(name)
    return false;
}

/*
ChoiceList TaskTypeDef::choiceList(const QString &fieldName)
{
    Q_UNUSED(fieldName)
    return ChoiceList();
}
*/

QList<int> TaskTypeDef::fieldVids() const
{
    QList<int> res;
    int i=0;
    foreach(const PrjField &f, model->fields)
    {
        res.append(i++);
    }
    return res;
}

QString TaskTypeDef::fieldName(int vid) const
{
    return model->fields[vid].field;
}
