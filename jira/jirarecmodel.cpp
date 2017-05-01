#include "jirarecmodel.h"
#include "jiradb.h"

class JiraRecModelPrivate
{
public:
    int nextIndex;
    int totalCount;
    QString jql;
    JiraDB *db;
    JiraProject *project;
    JiraRecTypeDef *rdef;
    int recType;

    JiraRecModelPrivate()
    {
        nextIndex = 0;
        totalCount = 0;
    }
};

JiraRecModel::JiraRecModel(TQAbstractProject *project, int type, QObject *parent)
    : TQRecModel(project, type, parent), d(new JiraRecModelPrivate())
{
    d->project = qobject_cast<JiraProject *>(project);
    d->db = d->project->jiraDb();
    d->recType = type;
    d->rdef = (JiraRecTypeDef *)d->project->recordTypeDef(type);
    setHeaders(d->rdef->fieldNames());
}


bool JiraRecModel::canFetchMore(const QModelIndex &parent) const
{
    if(parent.isValid())
        return false;
    return d->totalCount > d->nextIndex;
}

void JiraRecModel::setJQuery(const QString &jql)
{
    d->jql = jql;
}

void JiraRecModel::open()
{
    d->nextIndex = -1;
    d->totalCount = 0;
    //fetchMore(QModelIndex());
}

void JiraRecModel::fetchMore(const QModelIndex &parent)
{
    if(parent.isValid())
        return;
    QVariantMap body;
    body["jql"] = d->jql;
    body["startAt"] = d->nextIndex < 0? 0 : d->nextIndex;
    QVariantList fList;
    const JiraRecTypeDef *rdef = (JiraRecTypeDef *)d->project->recordTypeDef(d->recType);
    QList<int> baseFields = d->project->baseRecordFields(d->recType).keys();
    foreach(int vid, baseFields)
    {
        QString field = rdef->fieldSystemName(vid);
        fList << field;
    }
    body["fields"] = fList;
    QVariantMap map = d->project->serverPost("rest/api/2/search", body).toMap();
    QVariantList issueList = map.value("issues").toList();
    QList<TQRecord*> records;

    foreach(QVariant i, issueList)
    {
        QVariantMap issue = i.toMap();
        QString key = issue.value("key").toString();
        int pos = key.indexOf('-')+1;
        int id = key.mid(pos).toInt();
        QString projectKey = key.left(pos-1);
//        if(projectKey.compare(d->project->jiraProjectKey(), Qt::CaseInsensitive))
//            continue;
        JiraRecord *rec = new JiraRecord(d->project, d->recType, id);
        rec->key = key;
        rec->internalId = issue.value("id").toInt();
        QVariantMap fieldsMap = issue.value("fields").toMap();
        for(QVariantMap::iterator i = fieldsMap.begin(); i!=fieldsMap.end(); i++)
            rec->storeReadedField(i.key(), i.value());
//        d->project->readRecordFields(rec);  // !!! надо убрать !!!
        records.append(rec);
    }
    d->totalCount = map.value("total").toInt();
    d->nextIndex = map.value("startAt").toInt() + issueList.size();
    qDebug() << d->nextIndex << d->totalCount;
    append(records);
}

