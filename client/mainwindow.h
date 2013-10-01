#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDomDocument>
#include <QMainWindow>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QHash>
#include <QTimer>
#include "ui_tracksmain.h"
#include "tracker.h"
#include "plans.h"
#include "trkview.h"
#include "modifypanel.h"
#include "unionmodel.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QFile;
class QGroupBox;
class QLabel;
class QListWidget;
class QSqlQuery;
class QSqlQueryModel;
class QSqlRelationalTableModel;
class QSqlDatabase;
class QTableView;
QT_END_NAMESPACE

class PlanFilesModel;
class QueryPage;
class ProjectPage;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private slots:
    void about();
private:
    //QList<QSqlQueryModel *> trkmodels;
    //QList<QSqlQueryModel *> prjmodels;
	//QList<QSqlDatabase> prjdbs;
	//TrkDb *trkdb;
	TrkToolDB *trkdb;
	TrkToolProject *trkproject;
    TrkHistory *journal;
    PlanFilesModel *projects;
	QAbstractItemModel *excels;
	PlanModel planModel;
    friend class PlanFilesModel;
    QComboBox *openIdEdit;
    UnionModel *treeModel;
    QLabel *statusLine;
    QNetworkAccessManager *am;
//    QString solrUrl;
public slots:
	void readQueries();
    void openQuery(const QModelIndex &index, bool reusePage=true);
    void showCurrentPlan();
//    void showPlan(bool linked = false);
    void showPlan(const QModelIndex &index);
    void showPlanTask(const QString &fileName, int taskNum);
//    void openLinkedPlan();
	void readFilters();
    void readDbms();
	void readProjects();
    void openQueryByIdClip();
    void openQueryById(const QString &numbers, bool reusePage=true);
    void openQueryById(const QList<int> &idList, const QString &title = QString(), bool reusePage=true);
    void connectTracker();
    void setDbmsType();
    void setProject();
    void closeTab(int index);
    void closeCurTab();
	void sendEmail();
    void loadSettings();
    void saveSettings();
public:
	void initProjectModel();
    //QMenu MainWindow::planMenu(bool forLink=false);
    void readModifications();
    TrkToolProject *currentProject()
    {
        return trkproject;
    }

    void openCurItem(bool reuse);
    void calcCountRecords();
    ProjectPage *openPlanPage(const QString &fileName);
    void findTrkRecords(const QString &line);
protected:
    QMenu *menuQueryList;
    QMenu *menuId;
	QString getTrkConnectString();
    void makeMenus();
    void setupToolbar();
    QueryPage *createNewPage(const QString &title);
    ModifyPanel *modifyPanel;
    QTimer *selectionTimer;
    QueryPage *curQueryPage();
    FieldValues lastChanges;
    virtual void closeEvent(QCloseEvent *event);
    void saveIdsToList(const QString &list);
protected slots:
    void deleteTheObject();
    void curSelectionChanged();
    void refreshSelection();
    void finishedSearch(QNetworkReply *reply);
public slots:
    void addPlanFile();
    void delCurrentPlan();
    void loadCurrentPlan();
	void loadAllProjects();
	void checkAutoProjects();
    void addMSProjectFile();
	void trustChanged(int State);
    void idEntered();
    void idEnteredNewPage();
    void slotAppendRecordsId();
    void slotRemoveRecordsId();
    void applyChanges();
    void repeatLastChanges();
    void slotOpenRecordsClicked(ScrSet res);

private slots:
    void on_changedQuery(const QString & projectName, const QString & queryName);
    void on_actionPrevious_triggered();
    void on_actionOpen_Query_triggered();
    void on_queriesView_customContextMenuRequested(const QPoint &pos);
    void on_actionOpen_QueryInNew_triggered();
    void on_actionForward_triggered();
    void on_actionQueryById_triggered();
    void on_actionPrint_Preview_triggered();
    void on_btnDBMS_clicked();
    void on_journalView_doubleClicked(const QModelIndex &index);
    void on_tabWidget_currentChanged(QWidget *arg1);
    void on_actionPasteNumbers_triggered();
    void on_actionNewQueryTab_triggered();
    void on_actionCloseTab_triggered();
    void on_actionOpenSelected_triggered();
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_actionAddToFolder_triggered();
    void on_actionNewFolder_triggered();
    void on_actionDeleteFolder_triggered();
    void on_actionRenameFolder_triggered();
    void on_actionOpenIds_triggered();
    void on_dockProps_visibilityChanged(bool visible);
    void on_dockQueries_visibilityChanged(bool visible);
    void on_dockWidget_visibilityChanged(bool visible);
    void on_actionViewQueriesFolders_triggered(bool checked);
    void on_actionViewHistory_triggered(bool checked);
    void on_actionViewModify_triggered(bool checked);
    void on_actionRefresh_Query_triggered();
    void on_actionEditContents_triggered();
    void on_actionPlansDialog_triggered();
};

#endif
