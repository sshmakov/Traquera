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
//    emit layoutAboutToBeChanged();
    foreach(rec, records)
    {
        append(rec);
        addedIds.insert(rec->recordId());
        deletedIds.remove(rec->recordId());
        rec->addLink();
    }
//    emit layoutChanged();
    endResetModel();

}

void TQRecModel::removeRecordIds(const QList<int> &ids)
{
    beginResetModel();
//    emit layoutAboutToBeChanged();
    foreach(int id, ids)
    {
        int r = rowOfRecordId(id);
        if(r==-1)
            continue;
        beginRemoveRows(QModelIndex(),r,r);
        TQRecord *rec = records.takeAt(r);
        if(rec)
            rec->removeLink(this);
        addedIds.remove(id);
        deletedIds.insert(id);
        endRemoveRows();
    }
//    emit layoutChanged();
    endResetModel();
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
    TQRecord * rec = at(row);
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
    return index(row,idCol).data().toUInt();
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
    return at(row);
}

Qt::ItemFlags TQRecModel::flags ( const QModelIndex & index ) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;
    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    const TQRecord *rec = at(index.row());
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
    connect(prec, SIGNAL(recordChanged(int)), SLOT(recordChanged(int)));
    connect(prec,SIGNAL(destroyed(QObject*)),SLOT(recordDestroyed(QObject*)));
}

void TQRecModel::append(const QList<PTQRecord> &list)
{
    BaseRecModel<PTQRecord>::append(list);
    foreach(TQRecord *prec, list)
    {
        connect(prec, SIGNAL(recordChanged(int)), SLOT(recordChanged(int)));
        connect(prec,SIGNAL(destroyed(QObject*)),SLOT(recordDestroyed(QObject*)));
    }
}


