#ifndef _QUERYPAGE_H_
#define _QUERYPAGE_H_

#include <QWidget>
#include <QString>
#include <QTimer>
#include <QtGui>
#include "ui_querypage.h"
#include "trdefs.h"
#include "tracker.h"
#include "tqbase.h"
//#include "trkview.h"
//#include "plans.h"
//#include "planproxy.h"
//#include "trkdecorator.h"
#include "ttfolders.h"
#include "preview.h"
#include <tqmodels.h>

/*
struct TrkHistoryItem 
{
	bool isQuery;
	QString projectName;
	TrkToolProject *prj;
	TRK_RECORD_TYPE recType;
	QString queryName;
	QList<int> recordIds;
	QList<int> addIds;
};
*/

class QueryPage : public QWidget, public Ui::queryForm
{
    Q_OBJECT
    Q_PROPERTY(const TQAbstractRecordTypeDef *recordTypeDef READ recordTypeDef)
protected:
	bool isInteractive;
	bool isDefLoaded;
//	PlanModel *planModel;
//    PlanProxyModel planViewModel;
    QSortFilterProxyModel *qryFilterModel;
	QString linkField;
//    TrkHistory history;
    //QList<TrkHistoryItem> history;
	int curHistoryPos;
    QString filterString;
    TTFolder folder;
    bool itIsFolder;
    QTreeView *planTreeView;
    QTabBar *tabBar;
    QTableWidget *filesTable;
    QStackedLayout *subLay;
    Preview *previewWidget;
public:
	//TrkModel trkmodel;
    TQAbstractProject *modelProject;
    TrkToolModel *tmodel;
    //QAbstractItemModel *tmodel;
    QueryPage(QWidget *parent = 0);
    ~QueryPage();
#ifdef DECORATOR
    EditDefList fieldEdits;
#endif
	//void setQuery(int id, TrkDb *trkdb);
	//void setQueryById(const QString& numbers, TrkDb *trkdb);
    void openQuery(TQAbstractProject *prj, const QString &queryName, int recType);
    void openQuery(const QString &projectName, const QString &queryName, int recType);
    void openIds(TQAbstractProject *prj, const QString &ids, const QString &title, int recType);
    void openIds(TQAbstractProject *prj, const QList<int> &idlist, const QString &title, int recType);
    //void openModel(TQAbstractProject *prj, TrkToolModel *newModel);
    void openModel(TQAbstractProject *prj, QAbstractItemModel *newModel);
    void openFolder(TQAbstractProject *prj, const TTFolder &afolder, int recType);
    //void setPlanModel(PlanModel *newmodel);
	int getColNum(const QString &colname);
	QString makeRecordPage(const QModelIndex &qryIndex, const QString& xqCodeFile);
    //QString makeRecordsPage(const QModelIndexList &records, const QString& xqCodeFile);
    QString makeRecordsPage(const QObjectList &records, const QString& xqCodeFile);
    void sendEmail(const QObjectList &records);
    void printPreview();
    Q_INVOKABLE QModelIndexList selectedRows();
    Q_INVOKABLE QObjectList selectedRecords(); //Q_INVOKABLE QList<TrkToolRecord *> selectedRecords();
    Q_INVOKABLE QList<int> selectedIds();
    Q_INVOKABLE QObjectList allRecords();
    Q_INVOKABLE QObjectList markedRecords();
    Q_INVOKABLE TQAbstractProject *currentProject();
    Q_INVOKABLE TQRecord *currentRecord();
    Q_INVOKABLE TQRecord *recordOnIndex(const QModelIndex &index);
    Q_INVOKABLE const TQAbstractRecordTypeDef *recordTypeDef();
    Q_INVOKABLE void setRecordsChecked(const QString &ids, bool flag);
    void initWidgets();
    bool hasMarked();
protected:
	QList<QAction*> headerActions;
	void initPopupMenu();
	void loadDefinition();
	void openHistoryItem(int pos);
    //void setQueryModel(TQAbstractProject *prj, TrkToolModel *model);
    void setQueryModel(TQAbstractProject *prj, TrkToolModel *model);
    QModelIndex mapIndexToModel(const QModelIndex &index) const;
    QTimer *detailsTimer;
    QTimer *previewTimer;
public slots:
	void changedView(const QModelIndex &index, const QModelIndex &prev);
	void headerChanged();
	void headerToggled(bool checked);
	void drawNotes(const QModelIndex &qryIndex);
	void closeTab(int index);
	void resetPlanFilter();
//	void addScrTask(PrjItemModel *prj);
//	void addScrTasks();
//    void copyScrFromTasks();
//    void showScrFromTasks();
	void recordOpen( const QModelIndex & index);
	void goBack();
	void goForward();
    void newFilterString();
    void newFilterString(const QString &text);
    void appendId(int id);
    void removeId(int id);
    void setIdChecked(int id, bool checked);
    void refreshQuery();
    void openRecordId(int id);
    //void showCurrentTaskInPlan();
    void previewFile(const QString &filePath);
    void popupScrMenu(int id);
    void execColumnsEditor();
protected slots:
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void currentChanged ( const QModelIndex & topLeft, const QModelIndex &  bottomRight);
    void slotTabChanged(int index);
    //void slotPlanContextMenuRequested ( const QPoint & pos );
    void populateJavaScriptWindowObject();
    void slotUnsupportedContent(QNetworkReply *reply);
    //void recordChanged(int recordId);
signals:
    void changedQuery(const QString & projectName, const QString & queryName);
    void openingModel(const QAbstractItemModel *model);
    void modelChanged(QAbstractItemModel *model);
    void selectionRecordsChanged();
    void openRecordsClicked(ScrSet set);
    void showTaskInPlanClicked(const QString &prjName, int taskNum);
private slots:
    void updateDetails();
    void on_actionAdd_Note_triggered();
    void on_actionFilter_toggled(bool arg1);
    void on_filterFieldComboBox_currentIndexChanged(int index);
    void on_actionCopyId_triggered();
    void on_actionCopyTable_triggered();
    void on_actionCopyRecords_triggered();
    void on_actionSelectRecords_triggered();
    void on_actionDeselectRecords_triggered();
    void on_actionDeleteFromList_triggered();
    void on_queryView_customContextMenuRequested(const QPoint &pos);
    void on_actionDeleteFromFolder_triggered();
    void slotFilesTable_doubleClicked(const QModelIndex &index);
    void slotFilesTable_pressed(const QModelIndex &index);
    void slotFilePreview();
    void slotCurrentFileChanged();
    void on_actionTest_triggered();
    void on_actionSelectTrigger_triggered();
    //void slotCheckPlannedIds();
    //void slotCheckNoPlannedIds();
    void on_actionCopyMarkedId_triggered();
    void on_actionCopyMarkedTable_triggered();
    void on_actionDeleteMarked_triggered();
    void on_actionSelectMarked_triggered();
    void on_actionCopyMarkedRecords_triggered();
public slots:
    void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon = QIcon());
};

#endif //_QUERYPAGE_H_
