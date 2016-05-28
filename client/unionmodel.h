#ifndef UNIONMODEL_H
#define UNIONMODEL_H

#include <QAbstractProxyModel>
#include <QtCore>

class QMutex;

class MapInfo
{
public:
    int id;
    int parentId;
    int row;
    QAbstractItemModel *model;
    QList<int> childrens; //list of Id, ordered
    MapInfo(): id(-1), parentId(-1), row(-1), model(0), childrens() {}
    MapInfo(const MapInfo &src): id(src.id), parentId(src.parentId), model(src.model), childrens(src.childrens), row(src.row) {}
    bool isValid() { return id>=0; }
};


class UnionModel : public QAbstractProxyModel
{
    Q_OBJECT
protected:
    QList<QAbstractItemModel *>models;
    QStringList titles;
    int maxColCount;
    QAbstractItemModel *selModel;
    mutable QHash<int, MapInfo> info;
    mutable QMutex mutex;
//    QSignalMapper mapper;
public:
    explicit UnionModel(QObject *parent = 0);
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual QVariant	data ( const QModelIndex & proxyIndex, int role = Qt::DisplayRole ) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual bool hasChildren(const QModelIndex &parent) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual QStringList mimeTypes() const;
    virtual bool dropMimeData(const QMimeData *data,
         Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QModelIndex appendSourceModel(QAbstractItemModel *model, const QString &title);
    void removeSourceModel(QAbstractItemModel *model);
    QAbstractItemModel *sourceModel(const QModelIndex &proxyIndex) const;
    QAbstractItemModel *sourceModel(int index) const;
    QList<QAbstractItemModel *> sourceModels() const;
    int sourceModelIndex(QAbstractItemModel *model) const;
    virtual void setSelectedModel(QAbstractItemModel *models);
    void setMaxColCount(int value);
    void clear();
    //Qt::DropActions supportedDragActions () const;
private:
    MapInfo findInfo(int row, const QModelIndex &parent) const;
signals:
    
private slots:

    void	do_columnsAboutToBeInserted ( const QModelIndex & parent, int start, int end )
    { beginInsertColumns(mapFromSource(parent), start, end); }
    void	do_columnsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationColumn )
    { beginMoveColumns(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationColumn); }
    void	do_columnsAboutToBeRemoved ( const QModelIndex & parent, int start, int end )
    { beginRemoveColumns(mapFromSource(parent), start, end); }
    void	do_columnsInserted ()
    { endInsertColumns(); }
    void	do_columnsMoved ()
    { endMoveColumns(); }
    void	do_columnsRemoved ()
    { endRemoveColumns(); }

    void	do_dataChanged ( const QModelIndex & topLeft, const QModelIndex & bottomRight )
    { emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight)); }
    void	do_headerDataChanged ( Qt::Orientation orientation, int first, int last )
    { emit headerDataChanged(orientation, first, last); }
    void	do_layoutAboutToBeChanged ()
    { emit layoutAboutToBeChanged(); }
    void	do_layoutChanged ()
    { emit layoutChanged(); }

    void	do_modelAboutToBeReset ()
    { beginResetModel(); }
    void	do_modelReset ()
    { endResetModel(); }

    void	do_rowsAboutToBeInserted ( const QModelIndex & parent, int start, int end );

    void	do_rowsAboutToBeMoved ( const QModelIndex & sourceParent, int sourceStart, int sourceEnd, const QModelIndex & destinationParent, int destinationRow )
    { beginMoveRows(mapFromSource(sourceParent), sourceStart, sourceEnd, mapFromSource(destinationParent), destinationRow); }
    void	do_rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );

    void	do_rowsInserted ();

    void	do_rowsMoved ()
    { endMoveRows(); }
    void	do_rowsRemoved ()
    { endRemoveRows(); }

public slots:
    
};

#endif // UNIONMODEL_H
