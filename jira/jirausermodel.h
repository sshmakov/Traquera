#ifndef JIRAUSERMODEL_H
#define JIRAUSERMODEL_H

#include <QAbstractListModel>

class JiraProject;
class JiraUserModelPrivate;

class JiraUserModel : public QAbstractListModel
{
    Q_OBJECT
private:
    JiraUserModelPrivate *d;
public:
    explicit JiraUserModel(JiraProject *project);
    ~JiraUserModel();
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    void setCompleteLink(const QString &url);
public slots:
    void refresh(const QString &firstChars);
signals:
    
public slots:
    
};

#endif // JIRAUSERMODEL_H
