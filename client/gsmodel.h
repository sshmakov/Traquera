#ifndef GSMODEL_H
#define GSMODEL_H

#include <QAbstractProxyModel>

typedef QList<int> IntList;

class SortGroupModel: public QAbstractProxyModel
{
    Q_OBJECT
    //Q_PROPERTY(int groupColumn READ groupColumn WRITE setGroupColumn)
    //Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn)
protected:
    QHash<int, IntList> groups;
    QList<QVariant> groupsOrder;
    int sortColumn;
    int sortRole;
    int groupColumn;
    int groupRole;
    Qt::SortOrder groupSortOrder;
    QList<int> sourceOrderedRows;
public:
    SortGroupModel(QObject *parent = 0);
    virtual QModelIndex	mapFromSource(const QModelIndex & sourceIndex ) const;
    virtual QModelIndex mapToSource(const QModelIndex & proxyIndex ) const;
    virtual void setSourceModel( QAbstractItemModel * sourceModel );
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder );
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &proxyIndex, int role) const;
    virtual bool hasChildren(const QModelIndex &parent) const;
    void setSortColumn(int column, Qt::SortOrder order = Qt::AscendingOrder );
    void setGroupColumn(int column, Qt::SortOrder order = Qt::AscendingOrder );
    void resetGroups();
    void resetSourceRows();
protected:
    bool rowLess(const int &row1, const int &row2);
};


#endif // GSMODEL_H
