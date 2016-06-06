#ifndef TQMODELS_H
#define TQMODELS_H

#include <QtCore>
#include <tqplugin_global.h>
#include "tqbase.h"

class TQAbstractProject;
class TQRecModel;

template <class R>
class BaseRecModel: public QAbstractItemModel
{
    //Q_OBJECT
protected:
    QList<R> records;

    virtual QVariant displayColData(const R & /*rec*/, int /*col*/) const = 0;
    virtual QVariant editColData(const R & rec, int col) const
    {
        return displayColData(rec, col);
    }
    /*
    {
        return QVariant();
    }
    */
    virtual bool setEditColData(const R & /*rec*/, int /*col*/, const QVariant & /*value*/)
    {
        return false;
    }
public:
    QStringList headers;

    BaseRecModel(QObject *parent = 0)
        :QAbstractItemModel(parent)
    {
    }
    virtual ~BaseRecModel()
    {
    }
    virtual void setHeaders(const QStringList &newHeaders)
    {
        headers.clear();
        headers.append(newHeaders);
    }
    virtual int	columnCount(const QModelIndex & /*parent*/ = QModelIndex() ) const
    {
        return headers.count();
    }
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const
    {
        if(!index.isValid())
            return QVariant();
        int row=index.row();
        int col=index.column();
        if(row<0 || row>=rowCount())
            return QVariant();
        if(col<0 || col >=columnCount())
            return QVariant();
        if(row>=records.count())
            return QVariant();
        switch(role)
        {
        case Qt::DisplayRole:
            return displayColData(records[row], col);
        case Qt::EditRole:
            return editColData(records[row], col);
        }
        return QVariant();
    }
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole)
    {
        if(!index.isValid())
            return false;
        int row=index.row();
        int col=index.column();
        if(role != Qt::EditRole)
            return false;
        bool res = setEditColData(records[row], col, value);
        if(res)
            emit dataChanged(index, index);
        return res;
    }
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
    {
        if(role !=Qt::DisplayRole)
            return QVariant();
        if(orientation != Qt::Horizontal)
            return QVariant();
        if(section <0 || section >= headers.count())
            return QVariant();
        return headers[section];
    }
    virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const
    {
        if(parent.isValid())
            return 0;
        return records.count();
    }

    virtual QModelIndex	index ( int row, int column, const QModelIndex & /*parent*/ = QModelIndex() ) const
    {
        return createIndex(row,column);
    }

    virtual QModelIndex parent(const QModelIndex &/*child*/) const
    {
        return QModelIndex();
    }

    virtual bool insertRow(int row, const QModelIndex &parent)
    {
        if(parent.isValid())
            return false;
        if(row<0 || row >records.count())
            return false;
        beginInsertRows(parent, row, row);
        R value;
        records.insert(row, value);
        endInsertRows();
        return true;
    }

    virtual void append(const R &rec)
    {
        beginInsertRows(QModelIndex(), records.count(), records.count());
        records.append(rec);
        endInsertRows();
    }

    virtual void append(const QList<R> &list)
    {
        if(list.count())
        {
            beginInsertRows(QModelIndex(), records.count(), records.count()+list.count());
            records.append(list);
            endInsertRows();
        }
    }

    virtual void clearRecords()
    {
        if(records.count())
        {
            beginRemoveRows(QModelIndex(), 0, records.count()-1);
            records.clear();
            endRemoveRows();
        }
    }
    virtual void clear()
    {
        beginResetModel();
        clearRecords();
        headers.clear();
        endResetModel();
    }
    const R & at(int row) const
    {
        return records.at(row);
    }
    virtual const R &operator [](int i) const
    {
        return at(i);
    }
};

typedef TQRecord * PTQRecord;

class TQPLUGIN_SHARED TQRecModel: public BaseRecModel<PTQRecord> //QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString query READ queryName WRITE setQueryName)
protected:
    //QList<int> recIds;
    //TrkToolProject *prj;
    TQAbstractProject *prj;
    int rectype;
    QString idFieldName;
    int idCol;
    qint64 prevTransId, lastTransId;

    //TrkToolRecordSet rset;
    QList<int> vids;
    QSet<int> addedIds;
    QSet<int> deletedIds;
    QString query;
    bool isQuery;
public:
    TQRecModel(TQAbstractProject *project, int type, QObject *parent = 0);
    virtual ~TQRecModel();

    TQAbstractProject *project() const
    {
        return prj;
    }
    // Tracker specific
//    bool openQuery(const QString &queryName, qint64 afterTransId=0);
//    bool openIds(const QList<int> &ids);
    QDomDocument recordXml(int row) const;
    int rowId(int row) const;
    int rowOfRecordId(int id) const;
    TQRecord *recordInRow(int row) const;
//    bool saveRecord(TQRecord *record);
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    const TQAbstractRecordTypeDef *typeDef();
    virtual void clearRecords();
    QString queryName() const;
    void setQueryName(const QString &queryName);
    QList<int> getIdList() const;
    QList<int> addedIdList() const;
    QList<int> deletedIdList() const;
    int idColumn() const { return idCol; }
    bool isSystemModel();
    int lastTransactionId() { return lastTransId; }
    bool isQueryType() const { return isQuery; }
    void setQueryType(bool value) { isQuery = value; }
    int recordType() const { return rectype; }
    /*
public:
    bool openScrs(const QList<int> &ids);
    bool openScrs(const QStringList &lines);
    bool openScrs(const QString &line);
    void close();*/
    void append(const PTQRecord &rec);
    void append(const QList<PTQRecord> &list);
    void appendRecordIds(const QList<int> &ids);
    void removeRecordIds(const QList<int> &ids);
    void appendRecordId(int id);
    void removeRecordId(int id);
    void refreshQuery();
protected:
//    bool appendRecordByHandle(TRK_RECORD_HANDLE recHandle);
    virtual QVariant displayColData(const PTQRecord & rec, int col) const;
    virtual QVariant editColData(const PTQRecord & rec, int col) const;
    virtual bool setEditColData(const PTQRecord & rec, int col, const QVariant & value);
    QList<int> doAppendRecordIds(const QList<int> &ids);
    QList<int> doRemoveRecordIds(const QList<int> &ids);
protected slots:
    void recordChanged(int id);
    void recordDestroyed(QObject *rec);

    friend class TrkHistory;
//    friend class TrkToolProject;
    friend class TQBaseProject;
};

#endif // TQMODELS_H
