#ifndef _PLANS_H_
#define _PLANS_H_

#include <QAbstractItemModel>
#include <QtGui>
#include "project.h"
#include "trdefs.h"
#include "planfiles.h"

#define Settings_ScrPlanView "ScrPlanView"

struct ScrPrj 
{
	ScrSet allScrs;
	ScrSetList scrByRow;
};

class QHeaderView;

class PlanModel: public QAbstractItemModel
{
	Q_OBJECT
protected:
	QList<PrjItemModel *> plans;
	QList<PrjField> fields;
	QList<ScrPrj> scrByProject;
public:
    int SCRfield, parentField;
public:
	PlanModel();
	~PlanModel();
	virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	bool PlanModel::hasChildren(const QModelIndex & parent = QModelIndex()) const;
	QSize span ( const QModelIndex & index ) const;
public:
	void addPrjModel(PrjItemModel *plaQModeln);
    PrjItemModel *addPrjFile(const QString &fileName, bool readOnly = false);
	bool closePrjModel(PrjItemModel *model);
	void readScrSet(PrjItemModel *plan);
	int columnOfField(const QString& fieldName);
	ScrSet listScr(int row, const QModelIndex & parent);
    PrjItemModel *prjModel(const QModelIndex & index);
    void readScrSetNewRec(PrjItemModel *plan, int row);
    void setFieldHeaders(QHeaderView *header);
protected:
	void loadDefinition(const QString &fileName);
protected slots:
    void planChanged();
    void planRowsInserted(const QModelIndex & parent, int start, int end );
};

class PlanFilesForm;

class PlansPlugin: public QObject
{
    Q_OBJECT
public:
    PlanModel *loadedPlans;
    PlanFilesModel *planFiles;
    //QTableView *filesTableView;
    PlanFilesForm *propWidget;
    QSettings *settings;
    QObject *globalObject;

    PlansPlugin(QObject * parent = 0);
    ~PlansPlugin();
    Q_INVOKABLE void initPlugin();
    Q_INVOKABLE void setGlobalObject(QObject *obj);
    Q_INVOKABLE QWidget * getPropWidget(QWidget *parentWidget = 0);
    //Q_INVOKABLE void setSettings(QSettings *settings);
    Q_INVOKABLE bool saveSettings();
    Q_INVOKABLE bool loadSettings();
    PrjItemModel *loadPrjFile(const QString &fileName, bool readOnly);
protected:
    void initProjectModel();
public slots:
    void addPlanFile();
    void delCurrentPlan();
    void loadCurrentPlan();
    void loadAllProjects();
    void checkAutoProjects();
    void addMSProjectFile();

public slots:
    void appendContextMenu(QMenu *menu);
    void queryViewOpened(QWidget *widget, QTableView *view, const QString &recType = "scr");
    void recordOpened(QWidget *widget, QObject *record, const QString &recType = "scr");
    void slotPlanContextMenuRequested(const QPoint &pos);
};

#endif // _PLANS_H_
