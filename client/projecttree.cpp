#include "projecttree.h"

TQProjectTree::TQProjectTree(QObject *parent) :
    UnionModel(parent)
{
}

void TQProjectTree::setProject(TQAbstractProject *prj, int recordType)
{
    m_prj = prj;
    m_recType = recordType;
}

TQAbstractProject *TQProjectTree::project() const
{
    return m_prj;
}

int TQProjectTree::recordType() const
{
    return m_recType;
}
