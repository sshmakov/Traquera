#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDomDocument>
#include <QMainWindow>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QHash>
#include <QTimer>
#include "ui_tracksmain.h"
#include "mainproc.h"
//#include "tracker.h"
//#include "plans.h"
//#include "trkview.h"
#include "modifypanel.h"
#include "unionmodel.h"
#include <ttglobal.h>
#include "projecttree.h"
#include <QNetworkProxy>

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

//class PlanFilesModel;
class QueryPage;
//class ProjectPage;
//class TTGlobal;
class TQOneProjectTree;
class TQConnectWidget;

struct ProjectRec {
        int id;
        QString projectName;
        QString dbClass;
        QString dbType;
        QString dbServer;
        QString user;
        QString password;
        bool dbOsUser;
        QString dbmsUser;
        QString dbmsPass;
        bool autoLogin;
};

class MainWindow;
class MainProc;
class TQRecordViewController;
class LogForm;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT
    Q_PROPERTY(TQAbstractProject *currentProject READ currentProject)
public:
	MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
protected:
    struct ProjectModel {
        TQOneProjectTree *prjTree;
        TTFolderModel *folders;
        TQQryFilter *userModel;
        TQQryFilter *publicModel;
    };
    QList<ProjectModel> projectModels;
    QList<ProjectRec> projectRecords;
    MainProc proc;

private slots:
    void about();
private:
    //QList<QSqlQueryModel *> trkmodels;
    //QList<QSqlQueryModel *> prjmodels;
	//QList<QSqlDatabase> prjdbs;
    //TrkDb *trkdb;

//    TQAbstractDB *trkdb;
//    TQAbstractProject * trkproject;
    TQAbstractProject * currentPrj;
    TQConnectWidget *connectWidget;

//    typedef QPair<QString,QString> src_type; //class, source
//    QList<src_type> dbSources;
    QMap<QString, TQAbstractDB *> dbList; // db by connectingString
//    QList<TQAbstractProject *>projects;
    QMap<QString, TQAbstractProject *>projectByName;

    TQHistory *journal;
    LogForm *logForm;
    //PlansPlugin *planPlugin;
    //PlanFilesModel *projects;
//	QAbstractItemModel *excels;
    //PlanModel planModel;
//    friend class PlanFilesModel;
    QComboBox *openIdEdit;
    TQProjectsTree *treeModel;
    QLabel *statusLine;
    QProgressBar *progressBar;
    int progressLevel;
//    QNetworkAccessManager *am;
//    QString solrUrl;
    struct {
        QModelIndex curIndex;
        TQOneProjectTree *prjModel;
        TQAbstractProject *prj;
        TTFolderModel *folderModel;
        TQQryFilter *qryModel;
        bool isProjectSelected;
        bool isFoldersGroupSelected;
        bool isFolderSelected;
        QModelIndex folderIndex;
        bool isQryGroupSelected;
        bool isQuerySelected;
        QModelIndex queryIndex;
        QString queryName;
        int recordType;
        QIcon icon;
    } selectedTreeItem;
public slots:
    void readProjectTree();
    void saveProjectTree();
    void autoConnect();
//    void readQueries(TQAbstractProject *prj);
    QueryPage *openQuery(TQAbstractProject *project, const QString &queryName, int recordType, bool reusePage);
//    void showCurrentPlan();
//    void showPlan(bool linked = false);
    void showPlan(const QModelIndex &index);
//    void showPlanTask(const QString &fileName, int taskNum);
//    void openLinkedPlan();
	void readFilters();
    void readDbms();
	void readProjects();
    void openQueryByIdClip();
    void openQueryById(const QString &numbers, int recordType=0, bool reusePage=true);
    void openQueryById(const QList<int> &idList, const QString &title = QString(), int recordType = 0, bool reusePage=true);
    void connectTracker();
//    void connectTrackerParams(const ConnectParams &params);
    void setDbmsType();
    void setProject();
    void closeTab(int index);
    void closeCurTab();
	void sendEmail();
    void loadSettings();
    void saveSettings();
    void showProgressBar();
    void hideProgressBar();
public:
	void initProjectModel();
    void loadPlugins();
    //QMenu MainWindow::planMenu(bool forLink=false);
    void readModifications();
    Q_INVOKABLE TQAbstractProject *currentProject();
    void setCurrentProject(TQAbstractProject *prj);
    int projectTreeRow(TQAbstractProject *prj);
    void openCurItem(bool reuse);
    void calcCountRecords();
    //ProjectPage *openPlanPage(const QString &fileName);
    void findTrkRecords(const QString &line, bool reuse=true);
    TQOneProjectTree *selectedProjectTree();
    QModelIndex selectedFolder(TTFolderModel **folderModel);
    void readSelectedTreeItem();
    QModelIndex mapFolderIndexToTree(const QModelIndex &index);
    int addProjectItem(const QString &connectString);
protected:
    QMenu *menuQueryList;
    QMenu *menuId;
//	QString getTrkConnectString();
    void makeMenus();
    void setupToolbar();
    QueryPage *createNewPage(const QString &title);
    ModifyPanel *modifyPanel;
    QTimer *selectionTimer;
    QueryPage *curQueryPage();
    FieldValues lastChanges;
    virtual void closeEvent(QCloseEvent *event);
    void saveIdsToList(const QString &list);
    TQAbstractDB *newDb(const QString &dbClass, const QString &dbType, const QString &dbServer);
    TQAbstractDB *getDb(const QString &dbClass, const QString &dbType, const QString &dbServer);
    struct TQDockInfo {
        QDockWidget *dw;
        QWidget *widget;
        QAction *action;
    };
    QHash<QDockWidget *, TQDockInfo> dockInfo;
    void registerDBClasses();
protected slots:
    void deleteTheObject();
    void curSelectionChanged();
    void refreshSelection();
    void finishedSearch(QNetworkReply *reply);
public slots:
    /*
     * for plan plugin
     *
    void addPlanFile();
    void delCurrentPlan();
    void loadCurrentPlan();
	void loadAllProjects();
	void checkAutoProjects();
    void addMSProjectFile();
    */

    /*
     * Public invokable function for using from plugins
     */
    bool addPropWidget(QWidget *widget);
    int addTab(const QString &title, QWidget *widget, const QIcon &icon = QIcon());
    void focusTab(QWidget *widget);
    QToolBar *addToolBar(const QString &title);
    QDockWidget *addWidgetToDock(const QString &title, QWidget *widget, Qt::DockWidgetArea area = Qt::RightDockWidgetArea);
    void updateModifyPanel(const TQAbstractRecordTypeDef *typeDef, const QObjectList &records);
    void proxyAuthentication(QNetworkProxy proxy ,QAuthenticator* auth);

signals:
    void updatingDetails();

public slots:
    void trustChanged(int State);
    void idEntered();
    void idEntered(int flag);
    void slotAppendRecordsId();
    void slotRemoveRecordsId();
    void applyChanges();
    void repeatLastChanges();
    void slotOpenRecordsClicked(QSet<int> res);
    void showRecordInList(TQRecord *record);

private slots:
//    void slotRecordWindowOpened(TQRecordViewController *controller);
    void slotViewOpened(QWidget *widget, TQViewController *controller);
    void slotDockVisibilityChanged(bool visible);
    void slotDockAction();

private:
    bool openProjectTree(TQOneProjectTree *pm, const QString &connString = QString());
    bool closeProjectTree(TQOneProjectTree *pm);
    bool deleteProjectTree(TQOneProjectTree *pm);
    bool setCurrentProjectTree(TQOneProjectTree *pm);


private slots:
    void on_changedQuery(const QString & projectName, const QString & queryName);
    void on_tabChanged(int tab);
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
    void on_actionViewQueriesFolders_triggered(bool checked);
    void on_actionViewModify_triggered(bool checked);
    void on_actionRefresh_Query_triggered();
    void on_actionEditContents_triggered();
    //void on_actionPlansDialog_triggered();
    void on_actionNewRequest_triggered();
    void on_btnService_clicked();
    void on_actionClose_Project_triggered();
    void on_actionEditQuery_triggered();
    void on_actionNewQuery_triggered();
    void on_actionSettings_triggered();
    void on_actionOpen_Project_triggered();
    void slotNewDBConnect(const QString &dbClass);
    void on_actionDelete_Project_triggered();
    void on_actionRename_Item_triggered();
    void on_actionRefreshQueryList_triggered();
    void on_actionDeleteQuery_triggered();
    void on_actionEditProject_triggered();
    void on_actionMakeActive_triggered();
    void on_cbCurrentProjectName_currentIndexChanged(int index);
    void on_actionProjectOptions_triggered();
};



#endif
