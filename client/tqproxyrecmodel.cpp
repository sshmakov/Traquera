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
        return QAbstractProxyModel::data(proxyIndex, role);
        /*
        QVariant v;
        QModelIndex sourceIndex = mapToSource(proxyIndex);
        v = sourceModel()->data(sourceIndex, role);
        return v;
        */
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
    if(recModel)
    {
        recModel->disconnect(this);
    }
    recModel = qobject_cast<TQRecModel *>(sourceModel);
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(_q_sourceDataChanged(QModelIndex,QModelIndex)));

    connect(sourceModel, SIGNAL(headerDataChanged(Qt::Orientation,int,int)),
            this, SLOT(_q_sourceHeaderDataChanged(Qt::Orientation,int,int)));

    connect(sourceModel, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(_q_sourceRowsAboutToBeInserted(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(_q_sourceRowsInserted(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int)),
            this, SLOT(_q_sourceColumnsAboutToBeInserted(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsInserted(QModelIndex,int,int)),
            this, SLOT(_q_sourceColumnsInserted(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(_q_sourceRowsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
            this, SLOT(_q_sourceRowsRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(_q_sourceColumnsAboutToBeRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(columnsRemoved(QModelIndex,int,int)),
            this, SLOT(_q_sourceColumnsRemoved(QModelIndex,int,int)));

    connect(sourceModel, SIGNAL(layoutAboutToBeChanged()),
            this, SLOT(_q_sourceLayoutAboutToBeChanged()));

    connect(sourceModel, SIGNAL(layoutChanged()),
            this, SLOT(_q_sourceLayoutChanged()));

    connect(sourceModel, SIGNAL(modelAboutToBeReset()), this, SLOT(_q_sourceAboutToBeReset()));
    connect(sourceModel, SIGNAL(modelReset()), this, SLOT(_q_sourceReset()));

//    d->_q_clearMapping();
    endResetModel();
}

void TQProxyRecModel::_q_sourceDataChanged(QModelIndex &topLeft, QModelIndex &bottomRight)
{
    emit dataChanged(topLeft, bottomRight);
}

void TQProxyRecModel::_q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last)
{
    emit headerDataChanged(orientation, first, last);
}

void TQProxyRecModel::_q_sourceRowsAboutToBeInserted(QModelIndex &parent, int first, int last)
{
    first = mapFromSource(recModel->index(first,0,parent)).row();
    last = mapFromSource(recModel->index(last,0,parent)).row();
    QModelIndex par = mapFromSource(parent);
    beginInsertRows(par, first, last);
}

void TQProxyRecModel::_q_sourceRowsInserted(QModelIndex &parent, int first, int last)
{
    endInsertRows();
}

void TQProxyRecModel::_q_sourceColumnsAboutToBeInserted(QModelIndex &parent, int first, int last)
{
    first = mapFromSource(recModel->index(first,0,parent)).column();
    last = mapFromSource(recModel->index(last,0,parent)).column();
    QModelIndex par = mapFromSource(parent);
    beginInsertColumns(par, first, last);
}

void TQProxyRecModel::_q_sourceColumnsInserted(QModelIndex &parent, int first, int last)
{
    endInsertColumns();
}

void TQProxyRecModel::_q_sourceRowsAboutToBeRemoved(QModelIndex &parent, int first, int last)
{
    first = mapFromSource(recModel->index(first,0,parent)).row();
    last = mapFromSource(recModel->index(last,0,parent)).row();
    QModelIndex par = mapFromSource(parent);
    beginRemoveRows(par, first,last);
}

void TQProxyRecModel::_q_sourceRowsRemoved(QModelIndex &parent, int first, int last)
{
    endRemoveRows();
}

void TQProxyRecModel::_q_sourceColumnsAboutToBeRemoved(QModelIndex &parent, int first, int last)
{
    first = mapFromSource(recModel->index(first,0,parent)).column();
    last = mapFromSource(recModel->index(last,0,parent)).column();
    QModelIndex par = mapFromSource(parent);
    beginRemoveColumns(par, first, last);
}

void TQProxyRecModel::_q_sourceColumnsRemoved(QModelIndex &parent, int first, int last)
{
    endRemoveColumns();
}

void TQProxyRecModel::_q_sourceLayoutAboutToBeChanged()
{
    emit layoutAboutToBeChanged();
}

void TQProxyRecModel::_q_sourceLayoutChanged()
{
    emit layoutChanged();
}

void TQProxyRecModel::_q_sourceAboutToBeReset()
{
    beginResetModel();
}

void TQProxyRecModel::_q_sourceReset()
{
    endResetModel();
}
