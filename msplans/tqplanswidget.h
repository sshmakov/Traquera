#ifndef TQPLANSWIDGET_H
#define TQPLANSWIDGET_H

#include "tqpluginwidget.h"
#include "plans.h"
#include "planproxy.h"
#include <QTreeView>

class TQViewController;

class TQPlansWidget : public TQPluginWidget
{
    Q_OBJECT
protected:
    PlanModel *loadedPlanModel;
    PlanProxyModel planViewModel;
    QTreeView *planTreeView;
    QObject *parentObject;
    QTimer *detailsTimer;
    TQViewController *controller;

public:
    explicit TQPlansWidget(QWidget *parent = 0);
    void initWidgets();
    void setPlanModel(PlanModel *newmodel);

    Q_INVOKABLE void setParentObject(QObject *obj);
    QStringList selectedSCRs();
    QList<int> selectedSCRIds();
    QObjectList curRecords;
signals:
    void openRecordsClicked(ScrSet set);
    void addScrTasksClicked();

protected:
    int getColNum(const QString &colname, const QAbstractItemModel *model);

public slots:
    void contextMenuRequested(const QPoint &pos);
    void selectingRecordsChanged();
    void recordsChanged(QObjectList records);

private:
    bool isSetsLoaded;
protected slots:
    void addScrTask(PrjItemModel *prj);
    void addScrTasks();
    void copyScrFromTasks();
    void showScrFromTasks();
    void filterForRecords();
    void showCurrentTaskInPlan();
    void showCurrentPlan();
    void updateDetailWindows();
    void slotCheckPlannedIds();
    void slotCheckNoPlannedIds();
    void headerChanged();

    friend class PlansPlugin;
};

#endif // TQPLANSWIDGET_H
