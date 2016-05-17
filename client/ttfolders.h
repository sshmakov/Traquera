#ifndef TTFOLDERS_H
#define TTFOLDERS_H

#include <QAbstractItemModel>
#include <QtSql>
#include <QSortFilterProxyModel>
//class QSqlDatabase;

struct TTFolder
{
public:
    int id;
    int parentId;
    QString title;
    QString projectId;
    QList<int> childrens; //list of Id, ordered
    QList<int> folderContent() const;
    QString folderRecords() const;
    bool addRecordId(int recordId);
    bool deleteRecordId(int recordId);
    bool setRecords(const QString &records);
    static bool setRecords(int folderId, const QString &records);
};

class TTFolderModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    QSqlDatabase db;
    QString tableName;
    QString projectId;
    QHash<int, TTFolder> folders; // by Id
    //QList<int> rows; // sort ordered Id for top folders
    QIcon folderIcon;
    mutable QMutex mutex;
public:
    explicit TTFolderModel(QObject *parent = 0);
    void setDatabaseTable(const QSqlDatabase &database, const QString &table, const QString& projectName);
    void refreshAll();
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual bool hasChildren(const QModelIndex &parent) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    Qt::DropActions supportedDropActions () const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    bool dropMimeData(const QMimeData *data,
         Qt::DropAction action, int row, int column, const QModelIndex &parent);
    QList<int> folderContent(const QModelIndex &index);
    void setFolderContent(const QModelIndex &index, const QList<int> &newContent);
    void addRecordId(const QModelIndex &index, int recordId);
    void deleteRecordId(const QModelIndex &index, int recordId);
    TTFolder folder(const QModelIndex &index);
    int findRow(const TTFolder &f, const TTFolder &p, bool *found=0);
private:
    void removeChildrens(int parentId);
    bool moveIndexToNewParent(const QModelIndex &index, const QModelIndex &newParent);
    QModelIndex findIndexById(int id);
signals:
    
public slots:
    
};

#ifdef CLIENT_APP

class TQQryFilter: public QSortFilterProxyModel
{
    Q_OBJECT
protected:
    QIcon queryIcon;
public:
    enum Filter {All, UserOnly, PublicOnly} filter;
    TQQryFilter(QObject *parent=0);
    void setSourceQueryModel(QAbstractItemModel *sourceModel, Filter filter);
    void setSourceQueryModel(QAbstractItemModel *sourceModel, QString value, int column);
    virtual QVariant data(const QModelIndex &index, int role) const;
private:
    Filter curFilter;
};
#endif


#endif // TTFOLDERS_H
