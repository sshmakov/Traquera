#include "jirausermodel.h"
#include <tqbase.h>
#include <jiradb.h>
#include <tqdebug.h>

class JiraUserModelPrivate
{
public:
    JiraProject *prj;
    QStringList users;
    QStringList keys;
    QString autoCompleteUrl;
};

JiraUserModel::JiraUserModel(JiraProject *project) :
    QAbstractListModel(project),
    d(new JiraUserModelPrivate())
{
    d->prj = project;
    d->autoCompleteUrl = d->prj->jiraDb()->queryUrl("rest/api/2/user/search?username=").toString();
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
    if(role == Qt::EditRole || role == Qt::UserRole)
        return d->keys.value(index.row());
    return QVariant();
}

int JiraUserModel::rowCount(const QModelIndex &parent) const
{
    return d->users.size();
}

void JiraUserModel::setCompleteLink(const QString &url)
{
    d->autoCompleteUrl = url;
}

void JiraUserModel::refresh(const QString &firstChars)
{
    beginResetModel();
    d->users.clear();
    d->keys.clear();
    QVariant reply = d->prj->serverGet(QString(d->autoCompleteUrl +"%1").arg(firstChars));
    if(reply.type() == QVariant::List)
    {
        QVariantList list = reply.toList();
        foreach(QVariant v, list)
        {
            QVariantMap userMap = v.toMap();
            QString key = userMap.value("key").toString();
            QString displayName = userMap.value("displayName").toString();
            tqDebug() << "key:" << key << "displayName:" << displayName;
            d->keys.append(key);
            d->users.append(displayName);
//            d->keys.append(displayName);
//            d->users.append(displayName);
//            d->prj->appendUserToKnown(userMap);
        }
    }
    endResetModel();
}
