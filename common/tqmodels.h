#ifndef TQMODELS_H
#define TQMODELS_H

#include <QtCore>
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


struct TrkHistoryItem
{
    QString projectName;
    QString queryName;
    bool isQuery;
    QString foundIds;
    int rectype;
    QDateTime createDateTime;
};

class TrkHistory: public BaseRecModel<TrkHistoryItem>
{
    Q_OBJECT
    //Q_PROPERTY(bool unique READ unique WRITE setUnique)
protected:
    TQAbstractProject *prj;
    bool unique;
public:
    TrkHistory(QObject *parent = 0);
    virtual ~TrkHistory();
    void setProject(TQAbstractProject *project);
    void setUnique(bool value);
    //virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    //virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    void removeLast();
protected:
    virtual QVariant displayColData(const TrkHistoryItem & rec, int col) const;
public slots:
    void openedModel(const TQRecModel *model);
};

typedef TQRecord * PTQRecord;

class TQRecModel: public BaseRecModel<PTQRecord> //QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString queryName READ getQueryName)
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
    QString queryName;
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
    QString getQueryName() const;
    QList<int> getIdList() const;
    int idColumn() const { return idCol; }
    bool isSystemModel();
    int lastTransactionId() { return lastTransId; }
    /*
public:
    bool openScrs(const QList<int> &ids);
    bool openScrs(const QStringList &lines);
    bool openScrs(const QString &line);
    void close();*/
    void appendRecordId(int id);
    void removeRecordId(int id);
    void refreshQuery();
protected:
//    bool appendRecordByHandle(TRK_RECORD_HANDLE recHandle);
    virtual QVariant displayColData(const PTQRecord & rec, int col) const;
    virtual QVariant editColData(const PTQRecord & rec, int col) const;
    virtual bool setEditColData(const PTQRecord & rec, int col, const QVariant & value);
protected slots:
    void recordChanged(int id);

    friend class TrkHistory;
    friend class TrkToolProject;
};

#endif // TQMODELS_H
