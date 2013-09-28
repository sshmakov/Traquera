#ifndef _TRACKER_H_
#define _TRACKER_H_

class QSqlDatabase;

#ifndef CONSOLE_APP
    #include <QMessageBox>
#include <QSortFilterProxyModel>
#endif
#include <QDateTime>
#include <QtCore>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QSqlQueryModel>


#ifdef _DEBUG
#define DEBUGINFO(title, text) \
	        (QMessageBox::information(0, title,	text, QMessageBox::Cancel))
#define DEBUGERROR(title, text) \
	        (QMessageBox::critical(0, title,	text, QMessageBox::Cancel))
#else
#define DEBUGINFO(title, text) (0)
#define DEBUGERROR(title, text) (0)
#endif


struct TrkField {
		int fldid;
		int fldClass;
		int fldFlags;
		int fldVid;
		int fldLen;
		QString fldLabel;
		int fldSlot;
		QString fldtable;
		QString fldfield;
};

typedef QHash<int, TrkField> FieldCol; // by FieldId

struct TrkUser {
	int usrId; // Internal unique ID for this user. [PK]
	int usrClass; // The Admin user has a usrClass of 1. All other users have a usrClass of 0. The Admin user should not be deleted.
	int usrFlags; // If usrFlags =1, then the user is not active in the system. SCRs or time records might still reference this user ID. However, this user will not appear as a valid choice in user fields.
	QString usrLoginId; // Tracker login ID. User should use this string to login to Tracker or Tracker Admin. Login ID is unique within the project. Login ID is case sensitive.
	QString usrFullNm; // char(64) User's full name. This string is optional.
	QString usrTel; // char(64) User's telephone number. This string is optional.
	QString usrEmail; // char(128) User's electronic mail address. This is used by Tracker and Notify to send mail to the users.
	QString usrPwd; // char(72) Scrambled version of user's password.
	QString usrComment; // varchar(254) Reserved for future use.
	int usrLstLid; // int Reserved for future use.
	int usrNfyTid; // int Last InTray notify transaction ID. The user is notified of all changes before this transaction ID.
	int usrMailTid; // int Last mail notify transaction ID. The user has received mail notification of all changes before this ID.
	int usrViewTid; // int Last view transaction ID. User has read all changes in their InTray or query window before this transaction ID.
	QString usrMailOpt; // text Comma-separated list of mail configuration options. See Notes below for details.
	int usrSrvMsg; // int Reserved for future use.
	int usrProjMsg; // int Reserved for future use.
	int usrParm1; // int If this value is 0, then there is no automatic notification code running on behalf of this user. Before the automatic InTray or E-mail notification is started, the procId from the trkproc table for the running process is written in usrParm1 to ensure that no other notification loop will start at the exact same time. When the notification code is finished for this user, it clears usrParm1.
	int usrParm2;
	int usrParm3;
	int usrParm4;
	int usrParm5;
	int usrParm6;
	int usrParm7;
	int usrParm8; // int Reserved for future use.
};

typedef QHash<int, TrkUser> UserCol;

struct TrkGroup {
	int ugId;
	int ugClass;
	int ugFlags;
	QString ugName;
	QList<int> users;
};

typedef QHash<int, TrkGroup> GroupCol;

struct TrkChoice {
	int id;
	QString label;
	int deleted;
	int order;
	int value;
};

typedef QHash<int, TrkChoice> ChoiceCol; // by choice id

struct TrkQuery {
	int qryId; // int Internal unique ID for this query. [PK]
	int qryClass; // int A value of 0 signifies a query that was created in version v6.5 or earlier of Tracker. Values of 1 and greater signify queries created in later versions.
	int qryFlags; // int Reserved for future use.
	int qryTypeId; // int The record type ID from the trktype table that this query corresponds to. Foreign key: typeVid in the trktype table.
	QString qryName; // char(64) The name of the query. If it is empty, then it is an embedded query from a report.
	int qryAuthId; // int The user ID that originally authored this query. Foreign key: usrId in the trkusr table.
	int qryOwnerId; // int The user ID that currently owns this query. Stored separately from author ID for permission reasons and in case the original author is deleted. The initial owner is the author. Foreign key: usrId in the trkusr table.
	int qryCrDate; // int The date and time this report was created. Tracker does not display this value anywhere.
	int qryMdDate; // int The date and time this report was last modified. Tracker does not display this value anywhere.
	QString qryTitle; // varchar(128) The name of the query; a duplicate of the qryname field. If it is empty, then it is an embedded query from a report.
	QString qryCont; // text A null terminated, comma-separated string representing the contents of the query. See Notes below.
	QString qryComment; // varchar(254) A comment about the query. Tracker displays it in the Query Run dialog.
	QString qryDescr; // varchar(254) Reserved for future use.
	// qryParm1 - qryParm8 // int Reserved for future use.
    bool isPublic;
};

typedef QHash<int, TrkQuery> QueryCol; // by query id

class TrkDb;

class TrkQryModel: public QAbstractItemModel
{
	Q_OBJECT
protected:
	int rows, cols;
	QueryCol queries;
	QList<int> queryIds;
public:
	TrkQryModel(QObject *parent = 0)
		:QAbstractItemModel(parent)
	{
		rows=0;
		cols=0;
	}
	virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex parent(const QModelIndex &child) const;
	bool read(TrkDb *trkdb);

	enum QryCols {
		QryId = 0,
		QryName = 1,
		QryAuhId = 2,
		QryOwnerId = 3,
		QryCrDate = 4,
		QryMdDate = 5,
		QryCont = 6,
		QryComment = 7,
		QryDescr = 8
	};
	friend TrkDb;
};


class TrkDb : public QObject {
	Q_OBJECT
private:
	static QList<TrkDb *>dblist;
public:
	TrkDb(QObject *parent=0);
	~TrkDb();
	//bool open(const QString &dsn, const QString &projectName);
	bool openProject(const QString &dsn, const QString &projectName, const QString &login);
	void close();
	bool readFields();
	bool readUsers();
	bool readChoices();
	bool readQueries();
	bool readGroups();
public:
	QList<int> fldIds; // list of field id -- main order fields
	FieldCol flds; // field of id
	QHash<QString, int> fieldByLabel; // id of label
	UserCol users;
	QHash<int, ChoiceCol> choices; // by field id 
	GroupCol groups;
	int curUserId;
	//QString queryQueries;
	QSqlDatabase db;
	TrkQryModel queryModel;
	QString dbname;
	QList<int> selectedScr;
public:
	QString queryStr();
	QString getQueryById(int queryid);
	QString getQueryBySCRs(const QString& numbers);
	int getFieldId(const QString &fldname);
	bool check(bool res);

	bool findScrs(const QList<int> &ids, QList<int> &found);
public:
	static TrkDb *defaultDb();
//private:
};

//extern TrkDb trkdb;

class TCond {
public:
	int rel; // 1 = AND, 2 = OR
	TrkDb *db;
	TCond(TrkDb *trkdb)
	{
		rel = 1;
		db = trkdb;
	}
	QString doMakeQuery()  const 
	{
		return makeQuery(); 
	}
	virtual QString makeQuery()  const 
	{
		return "1=1"; 
	}
};

class TFieldConds {
public:
	TrkDb *db;
	TFieldConds(TrkDb *trkdb) : db(trkdb)
	{
	}
	QList<TCond*> conds;
	//virtual QString getTStr();
	void parseCondStr(QStringList::const_iterator i);
	virtual QString makeQuery() const;
};

class TCondField : public TCond {
public:
	int fid;
	int par; // 1 = NOT, 2 = open parenthesis, 3 = closed parenthesis
	TCondField(TrkDb *trkdb) : TCond(trkdb)
	{
		par = 0;
	}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i) = 0;
	virtual QString makeQuery() const;
	virtual QString makeQueryField(TrkField &f) const = 0;
};

class TCondNum : public TCondField {
public:
	int op;
	QString value1, value2;
	TCondNum(TrkDb *trkdb) :TCondField(trkdb) {}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i);
	virtual QString makeQueryField(TrkField &f) const;
};

class TCondChoice : public TCondField {
public:
	int op;
	QList<int> values;
	TCondChoice(TrkDb *trkdb) :TCondField(trkdb) {}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i);
	virtual QString makeQueryField(TrkField &f) const;
};

class TCondUser : public TCondField {
public:
	bool byGroup;
	bool onlyActive;
	bool onlyDeleted;
	QList<int> ids;
	TCondUser(TrkDb *trkdb) :TCondField(trkdb) {}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i);
	virtual QString makeQueryField(TrkField &f) const;
};

class TCondDate : public TCondField {
public:
	int op;
	int flags1, flags2;
	int value1, value2;
	TCondDate(TrkDb *trkdb) :TCondField(trkdb) {}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i);
	virtual QString makeQueryField(TrkField &f) const;
};

class TCondString : public TCondField {
public:
	int caseSens; // 0 - case insensetive
	QString value;
	TCondString(TrkDb *trkdb) :TCondField(trkdb) {}
	virtual void parseOp(TrkField &f, QStringList::const_iterator i);
	virtual QString makeQueryField(TrkField &f) const;
};

class TrkModel;
class TrkRecord;
class TrkItemModel;

class TrkRecordSet: public QObject
{
	Q_OBJECT
protected:
	QStringList fields;
	QList<TrkRecord *> records;
	//TrkDb *trkdb;
public:
	TrkRecordSet(/*TrkDb *db,*/ QObject *parent = 0);
	int fieldNum(const QString &field) const;
	QString fieldName(int index) const;
	int recCount() const;
	int fieldCount() const;
	QVariant data(int record, int fieldNum) const;
	void clear();
	void readQuery(QSqlQuery * query);

	friend TrkRecord;
	friend TrkItemModel;
};

class TrkRecord: public QObject
{
	Q_OBJECT
protected:
	//QHash<QString, QVariant> values;
	TrkRecordSet *recordset;
	QHash<int, QVariant> values;
public:
	TrkRecord(TrkRecordSet *parent);
	QVariant operator[](const QString& fieldName) const;
	QVariant operator[](int fieldNum) const;
	QVariant data(const QString& fieldName) const;
	QVariant data(int fieldNum) const;
	void setValue(const QString& fieldName, const QVariant& value);
	friend TrkModel;
	friend TrkItemModel;
};

struct TrkNote
{
	QString title;
	QString author;
	QString text;
	QDateTime crdate;
	QDateTime mddate;
    int perm;
//    TrkNote();
//    TrkNote(const TrkNote &src);
};

typedef QList<TrkNote> NotesCol;

#ifndef CONSOLE_APP

class TrkModel : public QObject 
{
	Q_OBJECT
public:
	QSqlQuery *query;
	QString origStr;
	QSqlQueryModel queryModel;
	QSortFilterProxyModel userFilter;
	QSortFilterProxyModel dependFilter;
	QAbstractItemModel *model;
	TrkDb *trkdb;
	TFieldConds *conds;

	TrkModel(QObject *parent = 0);
	~TrkModel();
	void setQuery(int queryId, TrkDb *db);
	void setQueryById(const QString& numbers, TrkDb *db);
	void reread();
	
	QVariant fieldByName(const QString& field, int row);
	int getColNum(const QString &fieldName);
	QSqlRecord record(const QModelIndex& index);
	//TrkRecord record(int id);
	NotesCol *notes(int row);
};


class TrkItemModel : public QAbstractItemModel
{
	Q_OBJECT
protected:
	TrkDb *trkdb;
	//TFieldConds *conds;
	QList<int> scrs;
	TrkRecordSet rset;

	//QSqlQuery *recQuery;
	int rows;
public:
	TrkItemModel(TrkDb *db, QObject *parent = 0);
	~TrkItemModel();
	virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
	QModelIndex parent(const QModelIndex &) const;
public:
	bool openScrs(const QList<int> &ids);
	bool openScrs(const QStringList &lines);
	bool openScrs(const QString &line);
	void close();
};

class TrkModel2 : public QObject 
{
	Q_OBJECT
public:
	QSqlQuery *query;
	QString origStr;
	QSqlQueryModel queryModel;
	QSortFilterProxyModel userFilter;
	QSortFilterProxyModel dependFilter;
	QAbstractItemModel *model;
	TrkDb *trkdb;
	TFieldConds *conds;

	TrkModel2(QObject *parent = 0);
	~TrkModel2();
	void setQuery(int queryId, TrkDb *db);
	void reread();
	
	QVariant fieldByName(const QString& field, int row);
	int getColNum(const QString &fieldName);
	QSqlRecord record(const QModelIndex& index);
	//TrkRecord record(int id);
	NotesCol *notes(int row);
};

#endif

#endif //_TRACKER_H_
