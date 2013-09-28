#include "plans.h"
#include "project.h"
#include "settings.h"
#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QMessageBox>
#include <QHeaderView>
#include <QSettings>

PlanModel::PlanModel()
{
    loadDefinition("data/project.xml");
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
        if(role == Qt::SizeHintRole)
            return  QSize(fields[section].colSize, 19);
        if(role != Qt::DisplayRole)
            return QVariant();
        return fields[section].title;
    }
    else
    {
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

void PlanModel::loadDefinition(const QString &fileName)
{
	QXmlSimpleReader xmlReader;
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
    delete file;
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

int PlanModel::columnOfField(const QString& fieldName)
{
	for(int i=0; i<fields.count(); i++)
		if(!fields[i].title.compare(fieldName,Qt::CaseInsensitive))
			return i;
	return -1;
}

PrjItemModel *PlanModel::addPrjFile(const QString &fileName, bool readOnly)
{
	PrjItemModel *prj=new PrjItemModel(this);
    if(!prj->open(fileName, readOnly))
	{
		delete prj;
		return NULL;
	}
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
