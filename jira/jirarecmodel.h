#ifndef JIRARECMODEL_H
#define JIRARECMODEL_H

#include <tqmodels.h>

class JiraRecModelPrivate;
class JiraRecord;
class JiraProject;

class JiraRecModel : public TQRecModel
{
    Q_OBJECT
    Q_PROPERTY(JiraProject jiraProject READ jiraProject)
private:
    JiraRecModelPrivate *d;
public:
    JiraRecModel(TQAbstractProject *project, int type, QObject *parent);
    bool canFetchMore(const QModelIndex &parent) const;
    void setJQuery(const QString &jql);
    void open();
    void fetchMore(const QModelIndex &parent);
    void storeFields(JiraRecord *rec, QVariantMap fieldMap);
    JiraRecord *recordByKey(const QString & key);
    JiraProject *jiraProject() const;
protected slots:
    void loadFields();
public slots:
    void updateIssue(const QString &key, QVariantMap fieldMap);
};

#endif // JIRARECMODEL_H
