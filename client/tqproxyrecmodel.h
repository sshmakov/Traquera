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
signals:
    
public slots:
    
};

#endif // TQPROXYRECMODEL_H
