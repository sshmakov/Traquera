#ifndef TQPLANSWIDGET_H
#define TQPLANSWIDGET_H

#include "tqpluginwidget.h"
#include "plans.h"
#include "planproxy.h"
#include <QTreeView>

class TQPlansWidget : public TQPluginWidget
{
    Q_OBJECT
protected:
    PlanModel *loadedPlanModel;
    PlanProxyModel planViewModel;
    QTreeView *planTreeView;
    QObject *globalObject;
    QObject *parentObject;
    QTimer *detailsTimer;

public:
    explicit TQPlansWidget(QWidget *parent = 0);
    void initWidgets();
    void setPlanModel(PlanModel *newmodel);

    Q_INVOKABLE void setGlobalObject(QObject *obj);
    Q_INVOKABLE void setParentObject(QObject *obj);
signals:
    void openRecordsClicked(ScrSet set);
    void addScrTasksClicked();

protected:
    int getColNum(const QString &colname, const QAbstractItemModel *model);

public slots:
    void contextMenuRequested(const QPoint &pos);
    void selectingRecordsChanged();

protected slots:
    void addScrTask(PrjItemModel *prj);
    void addScrTasks();
    void copyScrFromTasks();
    void showScrFromTasks();
    void filterForRecords();

};

#endif // TQPLANSWIDGET_H
