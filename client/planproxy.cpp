#include "planproxy.h"
#include "plans.h"

PlanProxyModel::PlanProxyModel(QObject * parent)
: QSortFilterProxyModel(parent)
{

}

bool PlanProxyModel::filterAcceptsRow ( int source_row, const QModelIndex & source_parent ) const
{
	if(!source_parent.isValid())
		return true; // всегда показывать узлы верхнего уровня
	if(scrSet.isEmpty())
		return true;
	PlanModel *plan = (PlanModel *)sourceModel();
	ScrSet &s = plan->listScr(source_row, source_parent);
	if(!(s & scrSet).isEmpty())
		return true;
	return false;
}

void PlanProxyModel::setFilterSCR(const QString &scrString)
{
	scrSet = scrStringToSet(scrString);
	reset();
}

void PlanProxyModel::clearFilter()
{
	scrSet.clear();
    reset();
}

ScrSet PlanProxyModel::taskScrSet(const QModelIndex &index)
{
    if(!index.isValid())
        return ScrSet();
    QModelIndex s = mapToSource(index);
    if(!s.isValid())
        return ScrSet();
    PlanModel *plan = (PlanModel *)sourceModel();
    if(!plan)
        return ScrSet();
    return plan->listScr(s.row(), s.parent());
}

QVariant PlanProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    PlanModel *plan = (PlanModel *)sourceModel();
    if(!plan)
        return QVariant();
    return plan->headerData(section,orientation,role);
}
