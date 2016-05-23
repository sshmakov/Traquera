#ifndef _PLANS_H_
#define _PLANS_H_

#include <QAbstractItemModel>
#include <QtGui>
#include <QtPlugin>
#include "project.h"
//#include "trdefs.h"
#include "planfiles.h"
#include "tqplug.h"
#include <ttglobal.h>

#define Settings_ScrPlanView "ScrPlanView"

typedef QSet<int> ScrSet;
typedef QMap<int, ScrSet> ScrSetList;

inline ScrSet scrStringToSet(const QString & scrString)
{
	QString s = scrString;
	s.replace(QRegExp("[^0-9 ,]")," ");
	s.replace(' ',',');
	QStringList list = s.split(',',QString::SkipEmptyParts);
	ScrSet scrSet;
	for(int i=0; i<list.count(); i++)
	{
		bool ok;
		int n=list[i].toInt(&ok);
		if(ok)
			scrSet << n;
	}
	return scrSet;
}


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
    PlanModel(QObject *parent);
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
    int columnOfField(const QString& fieldName) const;
    QVariant fieldData(int row, const QString &fieldName, int role = Qt::DisplayRole) const;
	ScrSet listScr(int row, const QModelIndex & parent);
    PrjItemModel *prjModel(const QModelIndex & index);
    void readScrSetNewRec(PrjItemModel *plan, int row);
    void setFieldHeaders(QHeaderView *header);
protected:
    bool loadDefinition(const QString &fileName);
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
    TTGlobal *globalObject;
    QMainWindow *mainWindow;
    QString pluginModule;
    QDir pluginDir;
    QDir dataDir;
    QString projectXml;

    PlansPlugin(QObject * parent = 0);
    ~PlansPlugin();
    Q_INVOKABLE void initPlugin(QObject *obj, const QString &path);
    Q_INVOKABLE QWidget * getPropWidget(QWidget *parentWidget = 0);
    //Q_INVOKABLE void setSettings(QSettings *settings);
    Q_INVOKABLE bool saveSettings();
    Q_INVOKABLE bool loadSettings();
    PrjItemModel *loadPrjFile(const QString &fileName, bool readOnly);
    void emitError(const QString &msg);
signals:
    void error(const QString &pluginName, const QString &msg);
protected:
    void initProjectModel();
protected slots:
    void addPlanFile();
    void delCurrentPlan();
    void loadCurrentPlan();
    void showCurrentPlan();
    void loadAllProjects();
    void checkAutoProjects();
public slots:
    void addMSProjectFile();
    void openPlanPage(PrjItemModel *model, int selectedTask = 0);

public slots:
    void appendContextMenu(QMenu *menu);
    void onViewOpened(QWidget *widget, TQViewController *controller);
//    void recordOpened(QWidget *widget, QObject *record, const QString &recType = "scr");
    void slotPlanContextMenuRequested(const QPoint &pos);
};

extern PlansPlugin *pluginObject;

#endif // _PLANS_H_
