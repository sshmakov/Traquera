#ifndef _TRKLOGIN_H_
#define _TRKLOGIN_H_

#include "ui_trklogin.h"
#include "tracker.h"

class TrkConnect : public QDialog, public Ui_TrkConnect
{
	Q_OBJECT
public:
	TrkConnect(QWidget *parent=0);
	static TrkDb *connectTracker();
public slots:
	void reloadProjects();
private:
	QSqlDatabase db;
};

#endif // _TRKLOGIN_H_