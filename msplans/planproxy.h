#ifndef _PLANPROXY_H_
#define _PLANPROXY_H_

#include <QAbstractItemModel>
#include <QAbstractProxyModel>
#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QSet>
//#include "trdefs.h"
#include "plans.h"

class PlanProxyModel: public QSortFilterProxyModel
{
	Q_OBJECT
protected:
    ScrSet scrSet;
public:
	PlanProxyModel(QObject * parent = 0);
	void setFilterSCR(const QString &scrString);
	void clearFilter();
    ScrSet taskScrSet(const QModelIndex &index);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
protected:
	virtual bool filterAcceptsRow (int source_row, const QModelIndex & source_parent) const;
};


#endif
