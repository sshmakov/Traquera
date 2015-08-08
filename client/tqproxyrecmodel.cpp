#include "tqproxyrecmodel.h"

TQProxyRecModel::TQProxyRecModel(QObject *parent) :
    QAbstractProxyModel(parent), isActual(false), recModel(0)
{
}

void TQProxyRecModel::actualize()
{
//    if(!sourceModel())
//    {
//        actual = false;
//        return;
//    }
    //    parentRows = sourceModel()->rowCount()
}

int TQProxyRecModel::rowCount(const QModelIndex &parent) const
{
    if(!recModel)
        return 0;
    QModelIndex sourceParent = mapToSource(parent);
    return recModel->rowCount(sourceParent);
}

int TQProxyRecModel::columnCount(const QModelIndex &parent) const
{
    if(!recModel)
        return 0;
    QModelIndex sourceParent = mapToSource(parent);
    return recModel->columnCount(sourceParent)+1;
}

QVariant TQProxyRecModel::data(const QModelIndex &proxyIndex, int role) const
{
    if(!proxyIndex.isValid())
        return QVariant();
    if(!recModel)
        return QAbstractProxyModel::data(proxyIndex, role);
    if(proxyIndex.column() == 0)
    {
//        QModelIndex sourceIndex = mapToSource(index(proxyIndex.row(), 1, proxyIndex.parent()));
//        if(!sourceIndex.isValid())
//            return QVariant();
        TQRecord *rec = recModel->recordInRow(proxyIndex.row());
        if(!rec || !rec->isValid())
            return QVariant();
        switch(role)
        {
        case Qt::DisplayRole:
            return QVariant(QString(rec->hasFiles() ? "F" : " "));
        }
//        if(role == Qt::DecorationRole)
    }
    else
    {
        QVariant v;
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        v = sourceModel()->data(sourceIndex, role);
        return v;
    }
//    recModel->index(proxy)
    return QVariant();
}

Qt::ItemFlags TQProxyRecModel::flags(const QModelIndex &index) const
{
    if(index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return QAbstractProxyModel::flags(index);
}

QVariant TQProxyRecModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(!recModel)
        return QVariant();
    if(orientation != Qt::Horizontal)
        return QAbstractProxyModel::headerData(section, orientation, role);
    if(section!=0)
        return recModel->headerData(section-1, orientation, role);
    if(role != Qt::DisplayRole)
        return QVariant();
    return "F";
}

QModelIndex TQProxyRecModel::index(int row, int column, const QModelIndex &parent) const
{
    return createIndex(row, column);
}

QModelIndex TQProxyRecModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if(!sourceIndex.isValid() || !recModel)
        return QModelIndex();
    QModelIndex parent = sourceIndex.parent();
    if(parent.isValid())
        parent = mapFromSource(parent);
    return index(sourceIndex.row(), sourceIndex.column()+1, parent);
}

QModelIndex TQProxyRecModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if(!proxyIndex.isValid() || !recModel)
        return QModelIndex();
    QModelIndex parent = proxyIndex.parent();
    if(proxyIndex.column() == 0)
        return recModel->index(proxyIndex.row(), 0, parent);
    if(parent.isValid())
        parent = mapToSource(parent);
    return recModel->index(proxyIndex.row(), proxyIndex.column()-1, parent);
}

QModelIndex TQProxyRecModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

void TQProxyRecModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    beginResetModel();
    recModel = qobject_cast<TQRecModel *>(sourceModel);
    QAbstractProxyModel::setSourceModel(sourceModel);
    endResetModel();
}
