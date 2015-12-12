#ifndef PROJECTTREE_H
#define PROJECTTREE_H
#include "unionmodel.h"

class TQAbstractDB;
class TQAbstractProject;
class TQOneProjectTree;
class TTFolderModel;
class TQQryFilter;

class TQProjectTreeItem {
public:
    struct {
        int id;
        TQOneProjectTree *tree;
        QString dbClass;
        QString connectString;
        QString projectName;
        QHash<QString, QString> params;
        int recType;
        TQAbstractProject *prj;
        TQAbstractDB *db;
    } data;

    TQProjectTreeItem(TQOneProjectTree *projectTree);
    TQProjectTreeItem & operator = (const TQProjectTreeItem &src);
    virtual bool isOpened() const;
    virtual bool open(const QString &connString = QString());
    virtual void setConnectString(const QString &string);
    void updateParams(const QString &string);
};

class QAuthenticator;

class TQOneProjectTree : public UnionModel
{
    Q_OBJECT
    Q_PROPERTY(QString connectString READ connectString() WRITE setConnectString)
protected:
    TQProjectTreeItem info;
public:
    TTFolderModel *folders;
    TQQryFilter *userModel;
    TQQryFilter *publicModel;

    explicit TQOneProjectTree(QObject *parent = 0);
//    void setProject(TQAbstractProject *prj, int recordType);
    void setConnectString(const QString &connectString);
    QString connectString() const;

//    void setProjectName(const QString &projectName);
//    void setProject(const QString &dbClass, const QString &projectName, const QString &connectString);
    void setItem(const TQProjectTreeItem &item);
    const TQProjectTreeItem &item() const;
    TQAbstractProject *project() const;
    QString projectTitle() const;
    int recordType() const;
    
    virtual bool isOpened() const;
    virtual bool isAutoOpen() const;
    virtual bool open(const QString &connectString = QString());
    virtual void close();
signals:
    void projectOpened();
    void projectClosed();
public slots:
    void onProjectAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator);
    
};

class TQProjectsTree : public UnionModel
{
    Q_OBJECT
private:
    QAbstractItemModel *selModel;
public:
    explicit TQProjectsTree(QObject *parent = 0);
    int appendProject(TQOneProjectTree *prjTree);
    void removeProject(TQOneProjectTree *prjTree);
    void setSelectedModel(QAbstractItemModel *model);
    QVariant data(const QModelIndex &proxyIndex, int role) const;
protected slots:
//    void slotProjectOpened();
//    void slotProjectClosed();
};

#endif // PROJECTTREE_H
