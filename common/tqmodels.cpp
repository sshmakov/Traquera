#include "tqmodels.h"
//#ifdef CLIENT_APP
    #include <QFont>
//#endif

// ============= TrkToolModel ===============

TQRecModel::TQRecModel(TQAbstractProject *project, int type, QObject *parent)
    :
    prj(project),
    rectype(type),
    BaseRecModel<PTQRecord>(parent),
    prevTransId(0)
{
    TQAbstractRecordTypeDef *recDef = prj->recordTypeDef(rectype);
    if(recDef)
    {
        int idVid = recDef->roleVid(TQAbstractRecordTypeDef::IdField);
        idFieldName = prj->fieldVID2Name(rectype,idVid); //.toLocal8Bit().constData();
        //QHash<int, TrkFieldDef>::const_iterator fi;
        headers = recDef->fieldNames();
        vids = recDef->fieldVids();
        idCol = vids.indexOf(idVid);
    }
    connect(project,SIGNAL(recordChanged(int)),this,SLOT(recordChanged(int)));
}

TQRecModel::~TQRecModel()
{
    clear();
}

/*
bool TrkToolModel::openQuery(const QString &queryName, qint64 afterTransId)
{
    beginResetModel();
    clearRecords();
    this->queryName = queryName;
    isQuery = true;
    bool res = prj->fillModel(this, queryName, this->rectype, afterTransId);
    endResetModel();
    return res;
}

bool TrkToolModel::openIds(const QList<int> &ids)
{
    beginResetModel();
    QList <int> unique = uniqueIntList(ids);
    prevTransId=0;
    foreach(int id, unique)
    {
        TrkToolRecord *rec = prj->createRecordById(id,rectype);
        if(rec)
            append(rec);
    }
    queryName = intListToString(unique);
    isQuery = false;
    endResetModel();
    return true;

}
*/

void TQRecModel::appendRecordIds(const QList<int> &ids)
{
    QList<int> res = doAppendRecordIds(ids);
    /*
    QList<TQRecord *> records;
    TQRecord *rec;
    foreach(int id, ids)
    {
        if(rowOfRecordId(id)!=-1)
            continue;
        rec = prj->createRecordById(id, rectype);
        if(rec)
            records << rec;
    }
    if(!records.size())
        return;
    beginResetModel();
    */
    foreach(int id, res)
    {
        addedIds.insert(id);
        deletedIds.remove(id);
    }
//    endResetModel();

}

void TQRecModel::removeRecordIds(const QList<int> &ids)
{
    QList<int> res = doRemoveRecordIds(ids);

//    beginResetModel();
    foreach(int id, res)
    {
        addedIds.remove(id);
        deletedIds.insert(id);
    }
//    endResetModel();
}

void TQRecModel::appendRecordId(int id)
{
    appendRecordIds(QList<int>() << id);
}

void TQRecModel::removeRecordId(int id)
{
    removeRecordIds(QList<int>() << id);
}

QVariant TQRecModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(index.row()<0 || index.row()>=records.size())
        return QVariant();
    if(role == Qt::CheckStateRole && index.column()==idCol)
    {
        bool isSel = records[index.row()]->isSelected();
        Qt::CheckState state = isSel ? Qt::Checked : Qt::Unchecked;
        return QVariant::fromValue<int>(state);
    }
//#ifdef CLIENT_APP
    if(role == Qt::FontRole)
    {
        QFont font;
        if(addedIds.contains(records[index.row()]->recordId()))
           font.setItalic(true);
        return font;
    }
//#endif
    return BaseRecModel::data(index, role);
}

bool TQRecModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if(index.row()<0 || index.row()>=records.size())
        return false;
    if(role == Qt::CheckStateRole && index.isValid() /*&& index.column()==idCol*/)
    {
        Qt::CheckState state = (Qt::CheckState)value.toInt();
        bool sel = (state == Qt::Checked);
        int row = index.row();
        if(!records[row]->isSelected() == sel)
        {
            records[row]->setSelected(sel);
            emit dataChanged(index, index);
        }
        return true;
    }
    return BaseRecModel::setData(index, value, role);
}

const TQAbstractRecordTypeDef *TQRecModel::typeDef()
{
    if(!prj)
        return 0;
    return prj->recordTypeDef(rectype);
}

void TQRecModel::clearRecords()
{
    TQRecord *rec;
    foreach(rec, records)
        if(rec)
            rec->removeLink(this);
    addedIds.clear();
    BaseRecModel::clearRecords();
}

QVariant TQRecModel::displayColData(const PTQRecord &rec, int col) const
{
    int vid = vids[col];
    return rec->value(vid,Qt::DisplayRole);
}

QVariant TQRecModel::editColData(const PTQRecord &rec, int col) const
{
    int vid = vids[col];
    return rec->value(vid,Qt::EditRole);
}

bool TQRecModel::setEditColData(const PTQRecord &rec, int col, const QVariant & value)
{
    int vid = vids[col];
    if(vid)
    {
        rec->setValue(vid, value);
        return true;
    }
    return false;
}

QList<int> TQRecModel::doAppendRecordIds(const QList<int> &ids)
{
    QSet<int> set = getIdList().toSet();
    QList<int> add = ids.toSet().subtract(set).toList();
    beginInsertRows(QModelIndex(), records.size(), records.size() + add.size() - 1);
    QList<int> res;
    foreach(int id, add)
    {
        TQRecord *rec = prj->createRecordById(id, rectype);
        if(!rec)
            continue;
        records << rec;
        rec->addLink();
        res << id;
    }
    endInsertRows();
    return res;
}

QList<int> TQRecModel::doRemoveRecordIds(const QList<int> &ids)
{
    QSet<int> set = getIdList().toSet();
    QList<int> del = ids.toSet().intersect(set).toList();
    QList<int> res;
    foreach(int id, del)
    {
        int row = rowOfRecordId(id);
        if(row == -1)
            continue;
        beginRemoveRows(QModelIndex(), row, row);
        TQRecord *rec = records.takeAt(row);
        if(rec)
        {
            rec->removeLink(this);
            res << id;
        }
        endRemoveRows();
    }
    return res;
}

void TQRecModel::recordChanged(int id)
{
    int row = rowOfRecordId(id);
    if(row<0)
        return;
    emit dataChanged(index(row,0),index(row,columnCount()-1));
}

void TQRecModel::recordDestroyed(QObject *rec)
{
    TQRecord *prec = qobject_cast<TQRecord*>(rec);
}

QDomDocument TQRecModel::recordXml(int row) const
{
    TQRecord * rec = recordInRow(row);
    if(!rec)
        return QDomDocument();
    rec->refresh();
    return rec->toXML();
}

void TQRecModel::refreshQuery()
{
    beginResetModel();
    prj->refreshModel(this);
    /*
    QSet<int> a = addedIds;
    if(isQuery)
    {
        QString q = queryName;
        openQuery(q,prevTransId);
    }
    else
    {
        QList<int> ids = stringToIntList(queryName);
        openIds(ids);
    }
    foreach(int id, a)
        appendRecordId(id);
        */
    endResetModel();
}

int TQRecModel::rowId(int row) const
{
    //int col=vids.indexOf(VID_Id);
    if(idCol == -1)
        return 0;
    return index(row,idCol).data(Qt::EditRole).toUInt();
}

int TQRecModel::rowOfRecordId(int id) const
{
    for(int r=0; r<rowCount(); ++r)
        if(rowId(r) == id)
            return r;
    return -1;
}

TQRecord *TQRecModel::recordInRow(int row) const
{
    if(row<0 || row>=records.size())
        return 0;
    return at(row);
}

Qt::ItemFlags TQRecModel::flags ( const QModelIndex & index ) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    const TQRecord *rec = recordInRow(index.row());
    if(!rec)
        return Qt::NoItemFlags;
    if(!index.column())
        res |= Qt::ItemIsUserCheckable;
    if(rec->mode() == TQRecord::View)
        return res;
    return res | Qt::ItemIsEditable;
}

QString TQRecModel::queryName() const
{
    return query;
}

void TQRecModel::setQueryName(const QString &queryName)
{
    query = queryName;
}

QList<int> TQRecModel::getIdList() const
{
    QList<int> res;
    foreach(PTQRecord rec, records)
    {
        uint id = rec->recordId(); // value(idFieldName).toUInt();
        res << id;
    }
    return res;
}

QList<int> TQRecModel::addedIdList() const
{
    return addedIds.toList();
}

QList<int> TQRecModel::deletedIdList() const
{
    return deletedIds.toList();
}

bool TQRecModel::isSystemModel()
{
    return prj->isSystemModel(this);
}

void TQRecModel::append(const PTQRecord &rec)
{
    BaseRecModel<PTQRecord>::append(rec);
    TQRecord *prec = rec;
//    connect(prec, SIGNAL(changed(int)), SLOT(recordChanged(int)));
    connect(prec,SIGNAL(destroyed(QObject*)),SLOT(recordDestroyed(QObject*)));
}

void TQRecModel::append(const QList<PTQRecord> &list)
{
    BaseRecModel<PTQRecord>::append(list);
    foreach(TQRecord *prec, list)
    {
//        connect(prec, SIGNAL(changed(int)), SLOT(recordChanged(int)));
        connect(prec,SIGNAL(destroyed(QObject*)),SLOT(recordDestroyed(QObject*)));
    }
}


