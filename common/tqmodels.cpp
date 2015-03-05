#include "tqmodels.h"
#ifdef CLIENT_APP
    #include <QtGui>
#endif

// ============= TrkToolModel ===============

TrkToolModel::TrkToolModel(TQAbstractProject *project, int type, QObject *parent)
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

TrkToolModel::~TrkToolModel()
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

void TrkToolModel::appendRecordId(int id)
{
    if(rowOfRecordId(id)!=-1)
        return;
    TQRecord *rec = qobject_cast<TQRecord *>(prj->createRecordById(id, rectype));
    if(rec)
    {
        emit layoutAboutToBeChanged();
        append(rec);
        addedIds.insert(id);
        rec->addLink();
        emit layoutChanged();
    }
}

void TrkToolModel::removeRecordId(int id)
{
    int r = rowOfRecordId(id);
    if(r==-1)
        return;
    beginRemoveRows(QModelIndex(),r,r);
    TQRecord *rec = records.takeAt(r);
    if(rec)
        rec->removeLink(this);
    endRemoveRows();
    addedIds.remove(id);
}

QVariant TrkToolModel::data(const QModelIndex & index, int role) const
{
    if(!index.isValid())
        return QVariant();
    if(role == Qt::CheckStateRole && index.column()==idCol)
    {
        bool isSel = records[index.row()]->isSelected();
        Qt::CheckState state = isSel ? Qt::Checked : Qt::Unchecked;
        return QVariant::fromValue<int>(state);
    }
#ifdef CLIENT_APP
    if(role == Qt::FontRole)
    {
        QFont font;
        if(addedIds.contains(records[index.row()]->recordId()))
           font.setItalic(true);
        return font;
    }
#endif
    return BaseRecModel::data(index, role);
}

bool TrkToolModel::setData(const QModelIndex & index, const QVariant & value, int role)
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

const TQAbstractRecordTypeDef *TrkToolModel::typeDef()
{
    if(!prj)
        return 0;
    return prj->recordTypeDef(rectype);
}

void TrkToolModel::clearRecords()
{
    TQRecord *rec;
    foreach(rec, records)
        if(rec)
            rec->removeLink(this);
    addedIds.clear();
    BaseRecModel::clearRecords();
}

QVariant TrkToolModel::displayColData(const PTQRecord &rec, int col) const
{
    int vid = vids[col];
    return rec->value(vid,Qt::DisplayRole);
}

QVariant TrkToolModel::editColData(const PTQRecord &rec, int col) const
{
    int vid = vids[col];
    return rec->value(vid,Qt::EditRole);
}

bool TrkToolModel::setEditColData(const PTQRecord &rec, int col, const QVariant & value)
{
    int vid = vids[col];
    if(vid)
    {
        rec->setValue(vid, value);
        return true;
    }
    return false;
}

void TrkToolModel::recordChanged(int id)
{
    int row = rowOfRecordId(id);
    if(row<0)
        return;
    emit dataChanged(index(row,0),index(row,columnCount()-1));
}

QDomDocument TrkToolModel::recordXml(int row) const
{
    TQRecord * rec = at(row);
    if(!rec)
        return QDomDocument();
    rec->refresh();
    return rec->toXML();
}

void TrkToolModel::refreshQuery()
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

int TrkToolModel::rowId(int row) const
{
    //int col=vids.indexOf(VID_Id);
    if(idCol == -1)
        return 0;
    return index(row,idCol).data().toUInt();
}

int TrkToolModel::rowOfRecordId(int id) const
{
    for(int r=0; r<rowCount(); ++r)
        if(rowId(r) == id)
            return r;
    return -1;
}

Qt::ItemFlags TrkToolModel::flags ( const QModelIndex & index ) const
{
    Qt::ItemFlags res = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    const TQRecord *rec = at(index.row());
    if(!index.column())
        res |= Qt::ItemIsUserCheckable;
    if(rec->mode() == TQRecord::View)
        return res;
    return res | Qt::ItemIsEditable;
}

QString TrkToolModel::getQueryName() const
{
    return queryName;
}

QList<int> TrkToolModel::getIdList() const
{
    QList<int> res;
    foreach(PTQRecord rec, records)
    {
        uint id = rec->recordId(); // value(idFieldName).toUInt();
        res << id;
    }
    return res;
}

bool TrkToolModel::isSystemModel()
{
    return prj->isSystemModel(this);
}


