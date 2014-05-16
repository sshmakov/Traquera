#ifndef _PROJECTPAGE_H_
#define _PROJECTPAGE_H_

#include <QtCore>
#include <QWidget>
#include <QModelIndex>
#include "ui_projectpage.h"
#include "project.h"
//#include "trkview.h"
//#include "trkdecorator.h"

struct PEditDef {
	QWidget *edit;
	int fieldcol;
	int fieldid;
	QString title;
};


class ProjectPage : public QWidget, private Ui::projectForm
{
    Q_OBJECT
public:
	PrjModel prjmodel;
    //TrkToolModel *trkmodel;
	ProjectPage(PrjItemModel *projectModel, QWidget *parent = 0);
	void loadDefinition();
	QList<PEditDef> fieldEdits;
	void openProject(const QString& file);
	void setParentColumn(QHeaderView *hv, const QString& srcFieldTitle, const QString& dstFieldTitle);
	QString projectName();
protected:
	int colSCR;
	bool isDefLoaded;
	bool isInteractive;
	QList<QAction*> headerActions;
	void initPopupMenu();
    QTimer *selectionTimer;
public slots:
    void changedParentView(const QModelIndex &index, const QModelIndex & /*prev*/)
	{
		prjmodel.changedParent(index);
	}
	void headerChanged();
	void headerToggled(bool checked);
	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void goToTask(int taskNum);
private slots:
    void updateDetails();

};

#endif //_PROJECTPAGE_H_
