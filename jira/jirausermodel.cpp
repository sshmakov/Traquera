#include "jirausermodel.h"
#include <tqbase.h>
#include <jiradb.h>

class JiraUserModelPrivate
{
public:
    JiraProject *prj;
    QStringList users;
    QStringList keys;
};

JiraUserModel::JiraUserModel(JiraProject *project) :
    QAbstractListModel(project),
    d(new JiraUserModelPrivate())
{
    d->prj = project;
}

JiraUserModel::~JiraUserModel()
{
    delete d;
}

QVariant JiraUserModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.column()!=0)
        return QVariant();
    if(role == Qt::DisplayRole)
        return d->users.value(index.row());
    if(role == Qt::EditRole)
        return d->keys.value(index.row());
    return QVariant();
}

int JiraUserModel::rowCount(const QModelIndex &parent) const
{
    return d->users.size();
}


void JiraUserModel::refresh(const QString &firstChars)
{
    beginResetModel();
    d->users.clear();
    d->keys.clear();
    QVariant reply = d->prj->jiraDb()->sendRequest("GET",
                                                   QString("rest/api/2/user/search?username=%1")
                                                   .arg(firstChars));
    if(reply.type() == QVariant::List)
    {
        QVariantList list = reply.toList();
        foreach(QVariant v, list)
        {
            QString key = v.toMap().value("key").toString();
            QString displayName = v.toMap().value("displayName").toString();
            d->keys.append(key);
            d->users.append(displayName);
            d->keys.append(displayName);
            d->users.append(displayName);
        }
    }
    endResetModel();
}
