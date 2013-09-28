#include "gsmodel.h"

#define NO_PARENT ((int)-1)
#define SOURCE_PARENT ((int)-2)

SortGroupModel::SortGroupModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
    sortColumn = -1;
    groupColumn = -1;
    sortRole = groupRole = Qt::DisplayRole;
}

QModelIndex SortGroupModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(!sourceIndex.isValid())
        return QModelIndex();
    if(sourceIndex.parent().isValid())
        return createIndex(sourceIndex.row(), sourceIndex.column(), SOURCE_PARENT);
    QModelIndex groupIndex = sourceIndex.sibling(sourceIndex.row(),groupColumn);
    QVariant groupValue = groupIndex.data();
    int groupRow = groupsOrder.indexOf(groupValue);
    if(groupRow<0)
        return QModelIndex();
    const IntList &sourceRows = groups[groupRow];
    int proxyChildRow = sourceRows.indexOf(sourceIndex.row());
    if(proxyChildRow<0)
        return QModelIndex();
    return createIndex(proxyChildRow, sourceIndex.column(), groupRow);
}

QModelIndex SortGroupModel::mapToSource(const QModelIndex &proxyIndex) const
{
    QModelIndex parent = proxyIndex.sibling(proxyIndex.row(),0).parent();
    if(parent.isValid())
    {
        int groupRow = parent.row();
        //QVariant groupValue = groups.keys()[groupRow];
        const IntList &sourceRows = groups[groupRow];
        int sr = sourceRows[proxyIndex.row()];
        return sourceModel()->index(sr, proxyIndex.column());
    }
    return QModelIndex();
}

void SortGroupModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    emit layoutAboutToBeChanged();
    QAbstractProxyModel::setSourceModel(sourceModel);
    resetSourceRows();
    resetGroups();
    emit layoutChanged();
}

void SortGroupModel::resetGroups()
{
    groups.clear();
    if(groupColumn >=0 )
    foreach(int row, sourceOrderedRows)
    {
        int groupRow;
        QVariant groupValue = sourceModel()->index(row, groupColumn).data(groupRole);
        groupRow = groupsOrder.indexOf(groupValue);
        if(groupRow<0)
        {
            groupsOrder.append(groupValue);
            groupRow = groupsOrder.count()-1;
            groups[groupRow] = IntList();
        }
        IntList &sourceRows = groups[groupRow];
        sourceRows.append(row);
    }
}

void SortGroupModel::resetSourceRows()
{
    QMultiMap<QString, int> rows;
    for(int srow=0; srow<sourceModel()->rowCount(); srow++)
        rows.insert(sourceModel()->index(srow, sortColumn).data(sortRole).toString(),srow);
    sourceOrderedRows.clear();
    sourceOrderedRows.append(rows.values());
}

void SortGroupModel::sort(int column, Qt::SortOrder /* order */)
{
    sortColumn = column;
    //QAbstractProxyModel::sort(column, order);
    resetSourceRows();
    resetGroups();
}

QModelIndex SortGroupModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();
    int grIndex = child.internalId();
    if(grIndex == NO_PARENT)
        return QModelIndex();
    if(grIndex == SOURCE_PARENT)
        return mapFromSource(mapToSource(child).parent());
    return createIndex(grIndex, 0, grIndex);
}

QModelIndex SortGroupModel::index(int row, int column, const QModelIndex &parent) const
{
    if(parent.isValid())
    {
        int grIndex = parent.internalId();
        if(grIndex != NO_PARENT)
            return QModelIndex();
        QVariant grValue = parent.data(groupRole);
        grIndex = groupsOrder.indexOf(grValue);
        return createIndex(row, column, grIndex);
    }
    else
        return createIndex(row, column, NO_PARENT);
}

int SortGroupModel::rowCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return groupsOrder.count();
    int grIndex = parent.internalId();
    if(grIndex == NO_PARENT)
        return groups[parent.row()].count();
    return sourceModel()->rowCount(mapToSource(parent));
}

int SortGroupModel::columnCount(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return sourceModel()->columnCount(); // return 1;
    int grIndex = parent.internalId();
    if(grIndex == NO_PARENT)
        return sourceModel()->columnCount();
    return sourceModel()->columnCount(mapToSource(parent));
}

QVariant SortGroupModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid())
        return QVariant();
    int grIndex = proxyIndex.internalId();
    if(grIndex == NO_PARENT)
        if(role == Qt::DisplayRole)
            return groupsOrder[proxyIndex.row()];
        else
            return QVariant();
    else
        return QAbstractProxyModel::data(proxyIndex, role);
}

bool SortGroupModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return false;
    int grIndex = parent.internalId();
    if(grIndex == NO_PARENT)
        return true;
    else
        return QAbstractProxyModel::hasChildren(parent);
}

void SortGroupModel::setSortColumn(int column, Qt::SortOrder order)
{
    sort(column, order);
}

void SortGroupModel::setGroupColumn(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();
    groupColumn = column;
    groupSortOrder = order;
    resetGroups();
    emit layoutChanged();
}
