#include "projecttree.h"

TQProjectTree::TQProjectTree(QObject *parent) :
    UnionModel(parent)
{
}

void TQProjectTree::setProject(TQAbstractProject *prj)
{
    m_prj = prj;
}

TQAbstractProject *TQProjectTree::project() const
{
    return m_prj;
}
