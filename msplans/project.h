#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <QWidget>
#include <QAxObject>
//#include <QSqlQuery>
//#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QList>
#include <QHash>
#include <QStandardItemModel>
#include <QStandardItem>
#include "tqplug.h"

struct PrjField {
	QString internalField;
	QString field;
	QString title;
    int fieldId;
	QString type;
    int stdType;
    QString display;
    QString format;
    int colSize;
};

typedef QHash<QString, QVariant> PrjRecord;

class PrjItemModel: public QAbstractItemModel
{
	Q_OBJECT
protected:
	QList<PrjField> fields;
	QHash<QString, int> colByTitle;
	QHash<QString, int> colByField;
	QStandardItemModel *cash;
	int rows, cols;
    //QSqlDatabase prjdb;
	QString cashTbl;
	bool isCashed;
	QAxObject app;
	QAxObject *prj;
	QAxObject *tasks;
private:
    int outlineCol;
public:
	QString prjName;
	QString fileName;
	bool isReadOnly;
public:
    PrjItemModel(QObject *parent = 0);
	virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
    QVariant fieldValue(int row, const QString &fieldName, int role = Qt::DisplayRole) const;
public:
	bool open(const QString &file, bool readOnly = true);
	void loadDefinition(QAxObject &app, const QString &file);
	void saveToMemory(QAxObject *tasks);
	QString projectName();
	friend class PrjModel;
    bool addTask(const PrjRecord &record/*, bool byTitle=true*/);
	void readTask(int row, QAxObject *task);
    int colTaskNum();
protected slots:
    void slotException( int code, const QString & source, const QString & desc, const QString & help );
    friend class TaskTypeDef;
};

class PrjModel//: public QObject
{
	//Q_OBJECT
public:
	QAbstractItemModel *model;
	PrjItemModel *prjmodel;

	//QSqlDatabase prjdb;
	//QAxObject prjapp;
	//QSqlQuery *query;
	//QSqlQueryModel queryModel;
	//QSortFilterProxyModel userFilter;
	QSortFilterProxyModel dependFilter;
	int parentColumn;
	PrjModel(PrjItemModel *projectModel = 0, QObject *parent=0);
	bool openProject(const QString &prjfile);
	void setParentColumn(int srcfield, const QString &dstfield);
	void changedParent(const QModelIndex &index);
    int rowOfTask(int taskNum);
	//void readCash();
};

class TaskTypeDef: /*public QObject,*/ public TQAbstractRecordTypeDef
{
//    Q_OBJECT
//    Q_INTERFACES(AbstractRecordTypeDef)
protected:
    PrjItemModel *model;

public:
    TaskTypeDef(PrjItemModel *srcModel);
    virtual QStringList fieldNames() const;
    virtual int fieldNativeType(const QString &name) const;
    virtual bool canFieldSubmit(const QString &name) const;
    virtual bool canFieldUpdate(const QString &name) const;
//    virtual ChoiceList choiceList(const QString &fieldName);
    virtual QList<int> fieldVids() const;
    virtual QString fieldName(int vid) const;
};



#endif // _PROJECT_H_
