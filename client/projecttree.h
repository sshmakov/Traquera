#ifndef PROJECTTREE_H
#define PROJECTTREE_H
#include "unionmodel.h"

class TQAbstractProject;

class TQProjectTree : public UnionModel
{
    Q_OBJECT
protected:
    TQAbstractProject *m_prj;
    int m_recType;
public:
    explicit TQProjectTree(QObject *parent = 0);
    void setProject(TQAbstractProject *prj, int recordType);
    TQAbstractProject *project() const;
    int recordType() const;
    
signals:
    
public slots:
    
};

#endif // PROJECTTREE_H
