#include "planfiles.h"
#include "mainwindow.h"
#include <QFileInfo>



// ======== PlanFilesModel
/*
PlanFilesModel::PlanFilesModel(MainWindow *parent)
: QAbstractItemModel(parent)
{
    mainWindow = parent;
}
*/

PlanFilesModel::PlanFilesModel(PlansPlugin *main)
    : QAbstractItemModel(main)
{
    plugin = main;
}


int	PlanFilesModel::columnCount(const QModelIndex & /* parent */) const
{
    return 3;
}

QVariant PlanFilesModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid() || index.row()<0 || index.row()>=plans.count())
        return QVariant();
    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case 0:
            return plans[index.row()].name;
        case 1:
            return "";
//            return plans[index.row()].readOnly?
//                    QString(tr("чтение"))
//                    : QString(tr("запись"));
        case 2:
            return plans[index.row()].file;
        }
        break;
    case Qt::EditRole:
        switch(index.column())
        {
        case 0:
            return plans[index.row()].name;
        case 1:
            return plans[index.row()].readOnly;
        case 2:
            return plans[index.row()].file;
        }
        break;
    case Qt::CheckStateRole:
        if(index.column()==0)
            return models.contains(plans[index.row()].file) ? Qt::Checked : Qt::Unchecked;
        else if(index.column()==1)
            return plans[index.row()].readOnly  ? Qt::Checked : Qt::Unchecked;
        else
            return QVariant();
    }
    return QVariant();
}

bool PlanFilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 1)
        if(role == Qt::EditRole)
        {
            plans[index.row()].readOnly = value.toBool();
            emit dataChanged(index,index);
            return true;
        }
        else if(role == Qt::CheckStateRole)
        {
            plans[index.row()].readOnly = (value == Qt::Checked);
            emit dataChanged(index,index);
            return true;
        }
    return false;
}

Qt::ItemFlags PlanFilesModel::flags(const QModelIndex &index) const
{
    switch(index.column())
    {
    case 1:
        return QAbstractItemModel::flags(index) | Qt::ItemIsUserCheckable;
    }
    return QAbstractItemModel::flags(index);
}


QVariant PlanFilesModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal)
    {
        if(role==Qt::DisplayRole)
            return QString::number(section+1);
        return QVariant();
    }
    if(role==Qt::DisplayRole)
    {
        switch(section)
        {
        case 0:
            return QString(tr("План"));
        case 1:
            return QString(tr("RO"));
        case 2:
            return QString(tr("Путь"));
        }
    }
    else if(role==Qt::TextAlignmentRole)
        return Qt::AlignLeft;
    return QVariant();
}

int	PlanFilesModel::rowCount(const QModelIndex & /* parent */) const
{
    return plans.count();
}

QModelIndex	PlanFilesModel::index( int row, int column, const QModelIndex & /* parent */) const
{
    return createIndex(row, column);
}

QModelIndex PlanFilesModel::parent(const QModelIndex & /* child */) const
{
    return QModelIndex();
}

int PlanFilesModel::addPlan(const QString & plan, bool readOnly)
{
    Plan p;
    p.file=plan;
    p.loaded=false;
    p.name=QFileInfo(plan).completeBaseName();
    p.readOnly = readOnly;
    plans.append(p);
    emit layoutChanged();
    return plans.count()-1;
}

bool PlanFilesModel::removeRows ( int row, int count, const QModelIndex & /* parent */)
{
    for(int i=row+count-1; i>=row; i--)
        plans.removeAt(i);
    emit layoutChanged();
    return true;
}

void PlanFilesModel::loadPlan(int row)
{
    PrjItemModel *model;
    QString fileName = plans[row].file;
    if(!models.contains(fileName))
    {
        //model = mainWindow->planModel.addPrjFile(fileName, plans[row].readOnly);
        model = plugin->loadPrjFile(fileName, plans[row].readOnly);
        if(model)
        {
            models[fileName]=model;
            plans[row].loaded=true;
        }
    }
    else
    {
        model = models.value(fileName);
        //mainWindow->planModel.closePrjModel(model);
        plugin->loadedPlans->closePrjModel(model);
        plans[row].loaded=false;
        models.remove(fileName);
    }
    emit dataChanged(index(row,0),index(0,columnCount()-1));
}
