#ifndef TQPROXYRECMODEL_H
#define TQPROXYRECMODEL_H

#include <QAbstractProxyModel>
#include <tqmodels.h>

class TQProxyRecModel : public QAbstractProxyModel
{
    Q_OBJECT
private:
    int parentRows, parentCols;
    bool isActual;
    TQRecModel *recModel;
public:
    explicit TQProxyRecModel(QObject *parent = 0);
    void actualize();
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data ( const QModelIndex & proxyIndex, int role = Qt::DisplayRole ) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual void setSourceModel(QAbstractItemModel *sourceModel);
private slots:
    void _q_sourceDataChanged(QModelIndex &topLeft, QModelIndex &bottomRight);
    void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);
    void _q_sourceRowsAboutToBeInserted(QModelIndex &parent, int first, int last);
    void _q_sourceRowsInserted(QModelIndex &parent, int first, int last);
    void _q_sourceColumnsAboutToBeInserted(QModelIndex &parent, int first, int last);
    void _q_sourceColumnsInserted(QModelIndex &parent, int first, int last);
    void _q_sourceRowsAboutToBeRemoved(QModelIndex &parent, int first, int last);
    void _q_sourceRowsRemoved(QModelIndex &parent, int first, int last);
    void _q_sourceColumnsAboutToBeRemoved(QModelIndex &parent, int first, int last);
    void _q_sourceColumnsRemoved(QModelIndex &parent, int first, int last);
    void _q_sourceLayoutAboutToBeChanged();
    void _q_sourceLayoutChanged();
    void _q_sourceAboutToBeReset();
    void _q_sourceReset();

signals:
    
public slots:
    
};

#endif // TQPROXYRECMODEL_H
