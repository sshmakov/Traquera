#ifndef PROJECTTREE_H
#define PROJECTTREE_H
#include "unionmodel.h"

class TQAbstractDB;
class TQAbstractProject;
class TQProjectTree;
class TTFolderModel;
class TrkQryFilter;

class TQProjectTreeItem {
public:
    struct {
        int id;
        TQProjectTree *tree;
        QString dbClass;
        QString connectString;
        QString projectName;
        QHash<QString, QString> params;
        int recType;
        TQAbstractProject *prj;
        TQAbstractDB *db;
    } data;

    TQProjectTreeItem(TQProjectTree *projectTree);
    TQProjectTreeItem & operator = (const TQProjectTreeItem &src);
    virtual bool isOpened() const;
    virtual bool open();
};

class QAuthenticator;

class TQProjectTree : public UnionModel
{
    Q_OBJECT
protected:
    TQProjectTreeItem info;
public:
    TTFolderModel *folders;
    TrkQryFilter *userModel;
    TrkQryFilter *publicModel;

    explicit TQProjectTree(QObject *parent = 0);
    void setProject(TQAbstractProject *prj, int recordType);
    void setProject(const QString &dbClass, const QString &projectName, const QString &connectString);
    void setItem(const TQProjectTreeItem &item);
    TQAbstractProject *project() const;
    int recordType() const;
    
    virtual bool isOpened() const;
    virtual bool open();
    virtual void close();
signals:
    
public slots:
    void onProjectAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator);
    
};

#endif // PROJECTTREE_H
