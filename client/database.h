#ifndef DATABASE_H
#define DATABASE_H


#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSettings>

extern QSettings *settings;
//bool createConnection();
//QSqlDatabase openProjectDB(const QString &prjfile);

/*
class TrackModel : public QSqlQueryModel {
	Q_OBJECT
public:
	TrackModel(QObject *parent = 0): QSqlQueryModel(parent)
	{
	}

     //QVariant data(const QModelIndex &item, int role) const;

};
*/
#endif


