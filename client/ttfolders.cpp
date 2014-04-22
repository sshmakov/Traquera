#include "ttfolders.h"
#include "ttglobal.h"
#include "ttutils.h"
#include <QtCore>
#include <QtSql>

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>

TTFolderModel::TTFolderModel(QObject *parent) :
    QAbstractItemModel(parent), db(QSqlDatabase())
{
}

void TTFolderModel::setDatabaseTable(const QSqlDatabase &database, const QString &table, const QString &filterValue)
{
    db = database;
    tableName = table;
    this->filterValue = filterValue;
    refreshAll();
}

void TTFolderModel::refreshAll()
{
    emit layoutAboutToBeChanged();
    folders.clear();
    //rows.clear();
    TTFolder pf;
    pf.id = 0;
    pf.parentId = 0;
    pf.childrens.clear();
    folders.insert(0,pf);
    QSqlQuery query(db);
    if(query.exec(QString("select id, parentId, title from folders order by title") /* .arg(tableName).arg(filterValue) */))
    {
        QList<int> keys;
        while(query.next())
        {
            TTFolder f;
            f.id = query.value(0).toInt();
            f.parentId = query.value(1).toInt();
            f.title = query.value(2).toString();
            folders.insert(f.id, f);
            keys.append(f.id);
            //rows.append(f.id);
        }
        foreach(const int fid, keys)
        {
            TTFolder &f = folders[fid];
            if(f.id && f.parentId>=0)
            {
                if(!folders.contains(f.parentId))
                    f.parentId = 0;
                folders[f.parentId].childrens.append(f.id);
            }
        }
    }
    else
        SQLError(query.lastError());
    emit layoutChanged();
}

QVariant TTFolderModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || role != Qt::DisplayRole)
        return QVariant();
    int id = index.internalId();
    if(!folders.contains(id))
        return QVariant();
    switch(index.column())
    {
    case 0:
        return folders[id].title;
    case 1:
        return folders[id].id;
    case 2:
        return folders[id].parentId;
    case 3:
        return folders[id].folderRecords();
    }
    return QVariant();
}

bool TTFolderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(role)
    if(!index.isValid())
        return false;
    int fid = index.internalId();
    QSqlQuery q(db);
    bool res;
    switch(index.column())
    {
    case 0:
        q.prepare("update folders set title=? where id = ?");
        q.bindValue(0,value);
        q.bindValue(1,fid);
        res = q.exec();
        if(res)
        {
            folders[fid].title = value.toString();
            emit dataChanged(index,index);
        }
        else
            SQLError(q.lastError());
        return res;
    case 2:
        q.prepare("update folders set parentId=? where id = ?");
        q.bindValue(0,value);
        q.bindValue(1,fid);
        res = q.exec();
        if(res)
        {
            folders[fid].parentId = value.toInt();
            emit dataChanged(index,index);
        }
        else
            SQLError(q.lastError());
        return res;
    case 3:
        q.prepare("update folders set records=? where id = ?");
        QString recs = intListToString(uniqueIntList(stringToIntList(value.toString())));
        q.bindValue(0,recs);
        q.bindValue(1,fid);
        res = q.exec();
        if(res)
        {
            folders[fid].setRecords(recs);
            emit dataChanged(index,index);
        }
        else
            SQLError(q.lastError());
        return res;
    }
    return false;
}

bool TTFolderModel::hasChildren(const QModelIndex &parent) const
{
    if(!parent.isValid())
        return folders.count()>1;
    int id=parent.internalId();
    return folders.contains(id) && folders[id].childrens.count()>0;
}

QModelIndex TTFolderModel::index(int row, int column, const QModelIndex &parent) const
{
    int parentId;
    if(parent.isValid())
        parentId = parent.internalId();
    else
        parentId = 0;
    int chCount = rowCount(parent);
    if(row<0 || row>=chCount || column < 0 || column > 3)
        return QModelIndex();
    return createIndex(row,column,folders[parentId].childrens[row]);
}

QModelIndex TTFolderModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();
    int cid = child.internalId();
    if(cid==0)
        return QModelIndex();
    int pid = folders[cid].parentId;
    if(pid==0)
        return createIndex(0,0,0);
    int ppid = folders[pid].parentId;
    int prow = folders[ppid].childrens.indexOf(pid);
    if(prow<0)
        return QModelIndex();
    return createIndex(prow,0,pid);
}


int TTFolderModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return folders[parent.internalId()].childrens.count();
    else
        return folders[0].childrens.count();
}

int TTFolderModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 4;
}

QVariant TTFolderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal || role != Qt::DisplayRole || section)
        return QVariant();
    return QVariant(tr("Папка"));
}

bool TTFolderModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent,row,row+count-1);
    int pid;
    if(!parent.isValid())
        pid=0;
    else
        pid = parent.internalId();
    for(int i=0; i<count; i++)
    {
        QString title = tr("Папка %1(%2)")
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"))
                .arg(i);
        QSqlQuery q(db);
        if(q.exec(QString("insert into folders(parentId, title) values(%1, '%2');")
               .arg(pid)
               .arg(title)
               ))
        {
            int fid = q.lastInsertId().toInt();
            TTFolder f;
            f.id = fid;
            f.parentId = pid;
            f.title = title;
            folders.insert(fid,f);
            folders[pid].childrens.insert(row, fid);
        }
        else
            SQLError(q.lastError());
    }
    endInsertRows();
    emit dataChanged(parent,parent);

    //emit dataChanged(index(0,0,parent),index(folders[pid].childrens.count(),1,parent));
    //emit dataChanged(parent,parent);
    return true;
}

bool TTFolderModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row+count-1);
    int pid;
    if(!parent.isValid())
        pid=0;
    else
        pid = parent.internalId();
    TTFolder &parentFolder = folders[pid];
    for(int i=0; i<count; i++)
    {
        int fid = parentFolder.childrens[row];
        //TTFolder &f =  folders[fid];
        removeChildrens(fid);
        QSqlQuery q(db);
        if(!q.exec(QString("delete from folders where id = %1").arg(fid)))
        {
            SQLError(q.lastError());
            break;
        }
        folders.remove(fid);
        parentFolder.childrens.removeAt(row);
    }
    /*
    for(int i=0; i<count; i++)
    {
        if(f.childrens.count()<=row)
            break;
        QSqlQuery q(db);
        int fid = f.childrens[row];
        if(!q.exec(QString("delete from folders where id = %1").arg(fid)))
        {
            SQLError(q.lastError());
            break;
        }
        f.childrens.removeAt(row);
        folders.remove(fid);
    }
    */
      endRemoveRows();
    return true;
}


QList<int> TTFolderModel::folderContent(const QModelIndex &index)
{
    if(index.isValid())
    {
        int fid = index.internalId();
        return folders[fid].folderContent();
    }
    return QList<int>();
    /*
    QList<int> ids;
    QSqlQuery q(db);
    if(q.exec(QString("select recordId from recordInFolder where folderId = %1").arg(index.internalId())))
        while(q.next())
            ids << q.value(0).toInt();
    return ids;
    */
}

void TTFolderModel::setFolderContent(const QModelIndex &index, const QList<int> &newContent)
{
    if(!index.isValid())
        return;
    QString recs = intListToString(newContent);
    setData(index.sibling(index.row(),3),recs);
}

void TTFolderModel::addRecordId(const QModelIndex &index, int recordId)
{
    if(index.isValid())
    {
        int fid = index.internalId();
        folders[fid].addRecordId(recordId);
    }
    /*
    QSqlQuery q(db);
    q.exec(QString("insert into recordInFolder(recordId, folderId) values (%1, %2)").arg(recordId).arg(index.internalId()));
    */
}

void TTFolderModel::deleteRecordId(const QModelIndex &index, int recordId)
{
    if(index.isValid())
    {
        int fid = index.internalId();
        folders[fid].deleteRecordId(recordId);
    }
    /*
    QSqlQuery q(db);
    q.exec(QString("delete from recordInFolder where recordId = %1 and folderId = %2").arg(recordId).arg(index.internalId()));
    */
}

TTFolder TTFolderModel::folder(const QModelIndex &index)
{
    static TTFolder dummy;
    if(index.isValid())
    {
        int fid = index.internalId();
        return folders[fid];
    }
    return dummy;
}

void TTFolderModel::removeChildrens(int parentId)
{
    TTFolder &f = folders[parentId];
    for(int i=0; i<f.childrens.count(); i++)
    {
        int fid = f.childrens[i];
        removeChildrens(fid);
        QSqlQuery q(db);
        if(!q.exec(QString("delete from folders where id = %1").arg(fid)))
        {
            SQLError(q.lastError());
            break;
        }
        folders.remove(fid);
    }
    f.childrens.clear();
}

// ================= TTFolder ================
QList<int> TTFolder::folderContent() const
{
    return uniqueIntList(stringToIntList(folderRecords()));
}

QString TTFolder::folderRecords() const
{
    QSqlQuery q(TTGlobal::global()->userDatabase());
    q.prepare("select records from folders where id = ?");
    q.bindValue(0,id);
    if(q.exec() && q.next())
        return q.value(0).toString();
    SQLError(q.lastError());
    return QString();
}


bool TTFolder::addRecordId(int recordId)
{
    QSet<int> r =  folderContent().toSet();
    r.insert(recordId);
    QString nRecs = intListToString(r.toList());
    return setRecords(nRecs);
}

bool TTFolder::setRecords(int folderId, const QString &records)
{
    QSqlQuery q(TTGlobal::global()->userDatabase());
    q.prepare("update folders set records = ? where id = ?");
    q.bindValue(0,records);
    q.bindValue(1, folderId);
    bool res = q.exec();
    if(!res)
        SQLError(q.lastError());
    return res;
}

bool TTFolder::deleteRecordId(int recordId)
{
    QSet<int> r = folderContent().toSet();
    r.remove(recordId);
    QString nRecs = intListToString(r.toList());
    return setRecords(nRecs);
}

bool TTFolder::setRecords(const QString &records)
{
    return TTFolder::setRecords(id, records);
}

