#ifndef _TRKLOGIN_H_
#define _TRKLOGIN_H_

#include "ui_trklogin.h"
#include "tracker.h"

class TrkConnect : public QDialog, public Ui::TrkConnect
{
	Q_OBJECT
protected:
    QDialogButtonBox::StandardButton m_btn;
public:
	TrkConnect(QWidget *parent=0);
//	static TrkDb *connectTracker();

    QString dbClass();
    QString dbType();
    QString dbServer();
    QString user();
    QString project();
    QString password();
    bool dbOsUser();
    QString dbmsUser();
    QString dbmsPass();
    bool autoLogin();
    ConnectParams params();
    void setParams(const ConnectParams &p);
    QDialogButtonBox::StandardButton lastButton();
protected:
    void showDBTypesMenu();
    void showProjectsMenu();
public slots:
//	void reloadProjects();
    void setDBType(const QString &type);
    void setProject(const QString &project);
private slots:
    void on_trustedUserBox_clicked();

    void on_btnProjects_clicked();

    void on_dbClassComboBox_currentIndexChanged(const QString &arg1);

    void on_btnDBMS_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
	QSqlDatabase db;
};


#endif // _TRKLOGIN_H_
