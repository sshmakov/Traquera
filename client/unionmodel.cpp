#include "unionmodel.h"
#include <QMutexLocker>
#include <QFont>

UnionModel::UnionModel(QObject *parent) :
    QAbstractProxyModel(parent),models(),maxColCount(0)
{
}

QModelIndex UnionModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();
    const QAbstractItemModel *model = sourceIndex.model();
    if(models.indexOf(model)==-1)
        return QModelIndex();
    QModelIndex p, sp = sourceIndex.parent();
    if(sp.isValid())
        p = mapFromSource(sp);
    if(p.isValid())
        p.internalId();
    return index(sourceIndex.row(),sourceIndex.column(),p);
    //return createIndex(sourceIndex.row(),sourceIndex.column(),model);
}

QModelIndex UnionModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return QModelIndex();
    QModelIndex p = proxyIndex.parent();
    if(p.isValid())
    {
        QModelIndex sp = mapToSource(p);
        const MapInfo &m = info.value(proxyIndex.internalId());
        QAbstractItemModel *model = m.model;
        return model->index(proxyIndex.row(),proxyIndex.column(),sp);
    }
    return QModelIndex();
    /*
    QAbstractItemModel *model = (QAbstractItemModel *)proxyIndex.internalPointer();
    if(!model)
        return QModelIndex();
    return model->index(proxyIndex.row());
    */
}


QVariant UnionModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid())
        return QVariant();
    int id = proxyIndex.internalId();
    if(!info.contains(id))
        return QVariant();
    const MapInfo &m = info[id];
    if(m.parentId<0)
    {
        if(proxyIndex.column()==0 && proxyIndex.row()<titles.count())
        {
            switch(role)
            {
            case Qt::DisplayRole:
                return titles[proxyIndex.row()];
            case Qt::FontRole:
                QFont boldFont;
                if(m.model == selModel)
                    boldFont.setBold(true);
                return boldFont;
            }
        }
        return QVariant();
    }
    QModelIndex si = mapToSource(proxyIndex);
    return si.data(role);
}

QVariant UnionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole || section)
        return QVariant();
    return "header";
}

Qt::ItemFlags UnionModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    int id = index.internalId();
    if(!info.contains(id))
        return 0;
    const MapInfo &m = info[id];
    if(m.parentId<0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    QModelIndex si = mapToSource(index);
    return si.flags();
}

QModelIndex UnionModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();
    int infoIndex = child.internalId();
    const MapInfo &m = info[infoIndex];
    if(m.parentId<0)
        return QModelIndex();
    return createIndex(info[m.parentId].row,0,info[m.parentId].id);
    /*
    int parentId = child.internalId();
    if(!parentId)
        return QModelIndex();
    int grandId = pids[parentId-1];
    int parentRow = pRows[parentId-1];
    return createIndex(parentRow,0,grandId+1);
    */
    /*
    QAbstractItemModel *model = (QAbstractItemModel *)child.internalPointer();
    if(!model)
        return QModelIndex();
    int r = models.indexOf(model);
    if(r==-1)
        return QModelIndex();
    QModelIndex sIndex = mapToSource(child);
    if(!sIndex.isValid())
        return QModelIndex();
    QModelIndex sParent = model->parent(sIndex);
    if(!sParent.isValid())
        return QModelIndex();
    return createIndex(sParent.row(),sParent.column());
    */
}

bool UnionModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return models.count()>0;
    int id = parent.internalId();
    const MapInfo &m = info[id];
    QModelIndex sp = mapToSource(parent);
    return m.model->hasChildren(sp);
}

QModelIndex UnionModel::index(int row, int column, const QModelIndex &parent) const
{
    QMutexLocker lock(&mutex);
    if(!parent.isValid())
    {
        if(row<0 || row >= models.count() || column!=0)
            return QModelIndex();
        MapInfo m2 = findInfo(row,parent);
        if(!m2.isValid())
            return QModelIndex();
        return createIndex(row,column,m2.id);
        /*
        return createIndex(row,column,-1);
        */
    }
    const MapInfo &p = info[parent.internalId()];
    MapInfo m = findInfo(row,parent);
    if(!m.isValid())
    {
        m.id = info.count();
        m.parentId = p.id;
        m.row = row;
        m.model = p.model;
        info.insert(m.id,m);
    }
    return createIndex(row,column,m.id);
    /*
    if(p)
    pids << parent.internalId()-1;
    pRows << parent.row();
    int parentId = pids.count()-1;
    return createIndex(row,column,parentId+1);
    */
}

int UnionModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return models.count();
    int id = parent.internalId();
    const MapInfo &m = info[id];
    QModelIndex sp = mapToSource(parent);
    return m.model->rowCount(sp);
}

int UnionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return maxColCount;
}

bool UnionModel::insertRows(int row, int count, const QModelIndex &parent)
{
    bool res = false;
    beginInsertRows(parent,row,row+count);
    if(parent.isValid())
    {
        QAbstractItemModel *model = sourceModel(parent);
        if(model)
            res = model->insertRows(row, count, mapToSource(parent));
    }
    endInsertRows();
    return res;
}

bool UnionModel::removeRows(int row, int count, const QModelIndex &parent)
{
    bool res = false;
    beginRemoveRows(parent,row,row+count);
    if(parent.isValid())
    {
        QAbstractItemModel *model = sourceModel(parent);
        if(model)
            res = model->removeRows(row, count, mapToSource(parent));
    }
    endRemoveRows();
    return res;
}

bool UnionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    QAbstractItemModel *model = sourceModel(index);
    if(!model)
        return false;
    QModelIndex sIndex = mapToSource(index);
    return model->setData(sIndex,value,role);
}

QMimeData *UnionModel::mimeData(const QModelIndexList &indexes) const
{
    QAbstractItemModel  *sModel = 0;
    foreach(QModelIndex index, indexes)
    {
        QAbstractItemModel *model = sourceModel(index);
        if(!model)
            continue;
        if(!sModel)
        {
            sModel = model;
            continue;
        }
        if(model != sModel)
            return 0;
    }
    QModelIndexList sourceIndexes;
    foreach(QModelIndex index, indexes)
    {
        QModelIndex sIndex = mapToSource(index);
        sourceIndexes.append(sIndex);
    }
    return sModel->mimeData(sourceIndexes);
}

QStringList UnionModel::mimeTypes() const
{
    QStringList res;
    foreach(const QAbstractItemModel *model,models)
    {
        res.append(model->mimeTypes());
    }
    res.removeDuplicates();
    return res;
}

bool UnionModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    QModelIndex proxyIndex;
    if(row < 0 && column <0)
        proxyIndex = parent;
    else
        proxyIndex = index(row,column,parent);
    QAbstractItemModel *model = sourceModel(proxyIndex);
    if(!model)
        return false;
    QModelIndex sourceIndex = mapToSource(proxyIndex);
    if(row < 0 && column <0)
        return model->dropMimeData(data,action,row,column,sourceIndex);
    else
        return model->dropMimeData(data,action,sourceIndex.row(),sourceIndex.column(),sourceIndex.parent());
}

void UnionModel::appendSourceModel(QAbstractItemModel *model, const QString &title)
{
    QMutexLocker lock(&mutex);
//    emit layoutAboutToBeChanged();
    int cols = model->columnCount();
    if(maxColCount < cols)
        maxColCount = cols;
    MapInfo m;
    m.id = info.count();
    m.parentId = -1;
    m.row = models.count();
    m.model = model;
    info.insert(m.id,m);
    models << model;
    titles << title;
    connect(model,SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(do_dataChanged(QModelIndex,QModelIndex)));
    connect(model,SIGNAL(headerDataChanged(Qt::Orientation,int,int)),this,SLOT(do_headerDataChanged(Qt::Orientation,int,int)));
    connect(model,SIGNAL(layoutAboutToBeChanged()),this,SLOT(do_layoutAboutToBeChanged()));
    connect(model,SIGNAL(layoutChanged()),this,SLOT(do_layoutChanged()));
    connect(model,SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),this,SLOT(do_rowsAboutToBeInserted(QModelIndex,int,int)));
    connect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(do_rowsInserted()));
    connect(model,SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),this,SLOT(do_rowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(model,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(do_rowsRemoved()));
    emit layoutChanged();
}

void UnionModel::removeSourceModel(QAbstractItemModel *model)
{
    QMutexLocker lock(&mutex);
    //emit layoutAboutToBeChanged();
    int row = models.indexOf(model);
    if(row == -1)
        return;
    foreach(MapInfo m, info)
    {
        if(m.model == model)
        {
            info.remove(m.id);
            models.removeAt(m.row);
            titles.removeAt(m.row);
            disconnect(model);
            break;
        }
    }
    emit layoutChanged();
}

QAbstractItemModel *UnionModel::sourceModel(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid())
        return 0;
    int pid = proxyIndex.internalId();

    if(!info.contains(pid))
        return 0;
    const MapInfo &m = info.value(pid);
    return m.model;
    /*
    if(!proxyIndex.isValid())
        return 0;
    QModelIndex i, p = proxyIndex;
    do
    {
        i = p;
        p = i.parent();
    } while(p.isValid());
    return models[i.row()];
    */
}

void UnionModel::setSelectedModel(QAbstractItemModel *model)
{
    int row = models.indexOf(model);
    if(row == -1)
        selModel = 0;
    else
        selModel = model;
    emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()));
}

void UnionModel::setMaxColCount(int value)
{
    emit layoutAboutToBeChanged();
    maxColCount = value;
    emit layoutChanged();
}

void UnionModel::clear()
{
    emit layoutAboutToBeChanged();
    models.clear();
    titles.clear();
    maxColCount = 1;
    info.clear();
    emit layoutChanged();
}

MapInfo UnionModel::findInfo(int row, const QModelIndex &parent) const
{
    int pid = -1;
    if(parent.isValid())
        pid = parent.internalId();
    foreach(const MapInfo &m, info)
        if(m.row == row && m.parentId == pid)
        {
            return m;
        }
    return MapInfo();
}
