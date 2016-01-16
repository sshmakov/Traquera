#include "tqhistory.h"
#include "ttutils.h"

//================== TQHistory =======================

TQHistory::TQHistory(QObject *parent)
    : BaseRecModel(parent),
      prj(0)
{
    headers << tr("Выборка");
    headers << tr("Тип");
    headers << tr("Записи");
    headers << tr("Тип записей");
    headers << tr("Время");
}

TQHistory::~TQHistory()
{
}

void TQHistory::setProject(TQAbstractProject *project)
{
    if(prj!=project)
    {
        clearRecords();
//        if(prj)
//            prj->disconnect(this);
        prj = project;
        if(prj)
            connect(prj,SIGNAL(openedModel(const QAbstractItemModel*)),this,SLOT(openedModel(const TQRecModel*)));
    }
}

void TQHistory::setUnique(bool value)
{
    unique = value;
}

void TQHistory::removeLast()
{
    records.removeLast();
}

QVariant TQHistory::displayColData(const TQHistoryItem &rec, int col) const
{
    switch(col)
    {
    case 0:
        return rec.queryName;
    case 1:
        return rec.isQuery;
    case 2:
        return rec.foundIds;
    case 3:
        return QVariant((uint)rec.rectype);
    case 4:
        return rec.createDateTime;
    }
    return QVariant();
}

void TQHistory::openedModel(const TQRecModel *model)
{
    TQHistoryItem item;
    bool isNew=true;
    QString qName = model->queryName();
    int i;
    if(unique)
        for(i = 0; i < records.count(); i++)
        {
            const TQHistoryItem &fi = records[i];
            if(fi.projectName == model->project()->projectName()
                    && fi.queryName == qName)
            {
                isNew = false;
                break;
            }
        }
    if(!isNew)
        item = records[i];
    else
    {
        item.projectName = model->project()->projectName();
        item.queryName = model->queryName();
    }
    item.foundIds = intListToString(model->getIdList());
    item.isQuery = model->isQuery();
    item.rectype = model->recordType();
    item.createDateTime = QDateTime::currentDateTime();
    if(isNew)
        append(item);
    else
        records[i]=item;
}

void TQHistory::append(const TQHistoryItem &rec)
{
    BaseRecModel::append(rec);
}


