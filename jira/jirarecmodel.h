#ifndef JIRARECMODEL_H
#define JIRARECMODEL_H

#include <tqmodels.h>

class JiraRecModelPrivate;

class JiraRecModel : public TQRecModel
{
    Q_OBJECT
private:
    JiraRecModelPrivate *d;
public:
    JiraRecModel(TQAbstractProject *project, int type, QObject *parent);
    bool canFetchMore(const QModelIndex &parent) const;
    void setJQuery(const QString &jql);
    void open();
    void fetchMore(const QModelIndex &parent);
};

#endif // JIRARECMODEL_H
