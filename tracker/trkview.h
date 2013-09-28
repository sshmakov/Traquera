#ifndef _TRKVIEW_H_
#define _TRKVIEW_H_ 1

#ifndef CONSOLE_APP
    #include <QTableView>
#include <QAction>
#endif

#include <QAbstractItemModel>
#include <QList>
#include <QHash>
#include <QSet>
#include "trktool.h"
#include "trktool2.h"
#include "tracker.h"

#define TT_DATETIME_FORMAT "dd.MM.yyyy H:mm:ss"

class TrkDb;
class TrkModel;

#ifndef CONSOLE_APP
class TrkView: public QTableView 
{
	Q_OBJECT
protected:
	bool isInteractive;
	bool isDefLoaded;
	QList<QAction*> headerActions;
	TrkModel trkmodel;
public:
	TrkView(QWidget *parent = 0);
	void setQuery(int id, TrkDb *trkdb);
	void setQueryById(const QString& numbers, TrkDb *trkdb);
	void loadDefinition();
protected:
	void initPopupMenu();
	int findColumn(const QString &label) const;
public slots:
//	void changedView(const QModelIndex &index, const QModelIndex &prev);
	void headerChanged();
	void headerToggled(bool checked);
//	void drawNotes(const QModelIndex &qryIndex);
//	void closeTab(int index);
//	void resetPlanFilter();
//	void addScrTask(PrjItemModel *prj);
//	void addScrTasks();
//protected slots:
//	void selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};
#endif

class TrkToolProject;
class TrkToolModel;

struct TrkToolChoice {
    QString displayText;
    QVariant fieldValue;
    //TRK_UINT weight;
    //TRK_UINT order;
    //TRK_UINT res;
};

struct TrkToolUser {
    QString userName;
    QString fullName;
    QString email;
};

typedef QHash<QString, QVariant> FieldValues;

typedef QList<TrkToolChoice> ChoiceList;
typedef QList<TrkToolUser> UserList;

class RecordTypeDef;

class TrkFieldDef {
protected:
    mutable ChoiceList *p_choiceList;
    RecordTypeDef *recDef;
public:
	QString name;
    TRK_FIELD_TYPE fType;
    bool nullable;
    int minValue;
    int maxValue;
    QVariant defaultValue;
    const ChoiceList &choiceList() const;
    QStringList choiceStringList(bool isDisplayText = true) const;
    //const ChoiceList *userList();

    TrkFieldDef(RecordTypeDef *recordDef=0)
        : recDef(recordDef), name(), fType(TRK_FIELD_TYPE_NONE), p_choiceList(new ChoiceList()),
          nullable(false), minValue(0), maxValue(INT_MAX)
    {

    }
    TrkFieldDef(const TrkFieldDef& src)
        : recDef(src.recDef), name(src.name), fType(src.fType), p_choiceList(new ChoiceList()),
          nullable(src.nullable), minValue(src.minValue), maxValue(src.maxValue), defaultValue(src.defaultValue)
    { // leak 4b

    }
    ~TrkFieldDef()
    {
		if(p_choiceList)
		{
			//delete p_choiceList;  //leak
			p_choiceList = 0;
		}
    }

    TrkFieldDef& operator =(const TrkFieldDef& src)
    {
        recDef = src.recDef;
        name = src.name;
        fType = src.fType;
        nullable = src.nullable;
        p_choiceList = src.p_choiceList;
        minValue = src.minValue;
        maxValue = src.maxValue;
        defaultValue = src.defaultValue;
        return *this;
    }

    bool isChoice() const
    {
        return (fType == TRK_FIELD_TYPE_CHOICE)
                || (fType == TRK_FIELD_TYPE_SUBMITTER)
                || (fType == TRK_FIELD_TYPE_OWNER)
                || (fType == TRK_FIELD_TYPE_USER)
                //|| (type == TRK_FIELD_TYPE_ELAPSED_TIME)
                || (fType == TRK_FIELD_TYPE_STATE)
                ;
    }
    bool isUser() const
    {
        return (fType == TRK_FIELD_TYPE_SUBMITTER)
                || (fType == TRK_FIELD_TYPE_OWNER)
                || (fType == TRK_FIELD_TYPE_USER)
                ;
    }
    inline bool isValid() const
    {
        return recDef && !name.isEmpty();
    }
    bool isNullable() const
    {
        return nullable;
    }
    bool canSubmit();
    bool canUpdate();
    QString valueToDisplay(const QVariant &value) const;
    QVariant displayToValue(const QString &text) const;

    friend class TrkToolProject;
};

typedef QHash<TRK_VID, TrkFieldDef> TrkIntDef;
typedef QHash<QString, TRK_VID> NameVid;

class RecordTypeDef {
public:
    TrkToolProject *prj;
    TRK_RECORD_TYPE recType;
    bool isReadOnly;
    TrkIntDef fieldDefs;
    NameVid nameVids;
    RecordTypeDef(TrkToolProject *project = 0)
        : prj(project), fieldDefs(), nameVids(), recType(-1), isReadOnly(true) {}
    RecordTypeDef(const RecordTypeDef & src): prj(src.prj), fieldDefs(src.fieldDefs), nameVids(src.nameVids) {}
    ~RecordTypeDef()
    {
        fieldDefs.clear();
        nameVids.clear();
    }

    QStringList getFieldList() const
    {
        return nameVids.keys();
    }
    TrkFieldDef getFieldDef(TRK_VID vid, bool *ok = 0) const
    {
        if(ok)
            *ok = fieldDefs.contains(vid);
        return fieldDefs[vid];
    }
    TrkFieldDef getFieldDef(const QString &name, bool *ok = 0) const
    {
        TRK_VID vid = nameVids[name];
        return getFieldDef(vid,ok);
    }

    /*
    const TrkFieldDef * findFieldDef(const QString &name) const
    {
        if(!nameVids.contains(name))
            return 0;
        TRK_VID vid = nameVids[name];
        return fieldDefs.  [vid];
    }
    */
    TRK_FIELD_TYPE fieldType(const QString &name) const
    {
        return getFieldDef(name).fType;
    }
    bool canFieldSubmit(const QString &name) const;
    bool canFieldUpdate(const QString &name) const;
};


template <class R>
class BaseRecModel: public QAbstractItemModel
{
	//Q_OBJECT
protected:
	QList<R> records;

	virtual QVariant displayColData(const R & /*rec*/, int /*col*/) const = 0;
    virtual QVariant editColData(const R & rec, int col) const
    {
        return displayColData(rec, col);
    }
    /*
	{
		return QVariant();
	}
	*/
	virtual bool setEditColData(const R & /*rec*/, int /*col*/, const QVariant & /*value*/)
	{
		return false;
	}
public:
	QStringList headers;

	BaseRecModel(QObject *parent = 0)
        :QAbstractItemModel(parent)
	{
	}
    virtual ~BaseRecModel()
    {
    }
	virtual void setHeaders(const QStringList &newHeaders)
	{
		headers.clear();
		headers.append(newHeaders);
	}
	virtual int	columnCount(const QModelIndex & /*parent*/ = QModelIndex() ) const
	{
		return headers.count();
	}
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const
	{
		if(!index.isValid())
			return QVariant();
		int row=index.row();
		int col=index.column();
        if(row<0 || row>=rowCount())
            return QVariant();
        if(col<0 || col >=columnCount())
            return QVariant();
        if(row>=records.count())
            return QVariant();
        switch(role)
        {
        case Qt::DisplayRole:
            return displayColData(records[row], col);
        case Qt::EditRole:
            return editColData(records[row], col);
        }
        return QVariant();
	}
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) 
	{
		if(!index.isValid())
			return false;
		int row=index.row();
		int col=index.column();
		if(role != Qt::EditRole)
			return false;
		bool res = setEditColData(records[row], col, value);
		if(res)
			emit dataChanged(index, index);
		return res;
	}
	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const
	{
		if(role !=Qt::DisplayRole)
			return QVariant();
		if(orientation != Qt::Horizontal)
			return QVariant();
		if(section <0 || section >= headers.count())
			return QVariant();
		return headers[section];
	}
    virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const
	{
        if(parent.isValid())
            return 0;
		return records.count();
	}

	virtual QModelIndex	index ( int row, int column, const QModelIndex & /*parent*/ = QModelIndex() ) const
	{
		return createIndex(row,column);
	}

	virtual QModelIndex parent(const QModelIndex &/*child*/) const
	{
		return QModelIndex();
	}

    virtual bool insertRow(int row, const QModelIndex &parent)
    {
        if(parent.isValid())
            return false;
        if(row<0 || row >records.count())
            return false;
        beginInsertRows(parent, row, row);
        R value;
        records.insert(row, value);
        endInsertRows();
        return true;
    }

	virtual void append(const R &rec)
	{
        beginInsertRows(QModelIndex(), records.count(), records.count());
        records.append(rec);
        endInsertRows();
	}

	virtual void append(const QList<R> &list)
	{
        if(list.count())
        {
            beginInsertRows(QModelIndex(), records.count(), records.count()+list.count());
            records.append(list);
            endInsertRows();
        }
    }

    virtual void clearRecords()
    {
        if(records.count())
        {
            beginRemoveRows(QModelIndex(), 0, records.count()-1);
            records.clear();
            endRemoveRows();
        }
    }
    virtual void clear()
	{
        beginResetModel();
        clearRecords();
		headers.clear();
        endResetModel();
	}
	const R & at(int row) const
	{
		return records.at(row);
	}
    virtual const R &operator [](int i) const
    {
        return at(i);
    }
};


class TrkStringModel : public BaseRecModel<QString>
{
	Q_OBJECT
public:
	TrkStringModel(QObject *parent = 0)
		:BaseRecModel<QString>(parent)
	{
    }
protected:
	virtual QVariant displayColData(const QString &rec, int /*col*/) const
	{
		return rec;
	}
};

class TrkStringListModel : public BaseRecModel<QStringList>
{
	Q_OBJECT
public:
	TrkStringListModel(QObject *parent = 0)
		:BaseRecModel<QStringList>(parent)
	{
	}

protected:
	virtual QVariant displayColData(const QStringList &rec, int col) const
	{
        if(col<0 || col >rec.count())
            return QVariant();
		return rec[col];
	}
};

//struct TrkQuery
class TrkToolQryModel: public BaseRecModel<TrkQuery>
{
    Q_OBJECT
public:
    TrkToolQryModel(QObject *parent = 0);
    void appendQry(const QString &queryName, bool isPublic=false, TRK_UINT rectype = TRK_SCR_TYPE);
protected:
    virtual QVariant displayColData(const TrkQuery &rec, int col) const;
};

#ifndef CONSOLE_APP

class TrkQryFilter: public QSortFilterProxyModel
{
    Q_OBJECT
public:
    enum Filter {All, UserOnly, PublicOnly} filter;
    TrkQryFilter(QObject *parent=0);
    virtual void setSourceModel(QAbstractItemModel *sourceModel);
};
#endif

class TrkToolProject;

class TrkToolDB: public QObject
{
    Q_OBJECT
public:
	TRK_HANDLE handle;
	QString dbmsName, dbmsUser, dbmsPassword;
	TrkToolDB(QObject *parent = 0);
	~TrkToolDB();
    QStringList dbmsTypes();
    QStringList projects(const QString &dbmsType);
	TrkToolProject *openProject(
		const QString &dbmsType,
		const QString &projectName,
		const QString &user, 
		const QString &pass);
	static TrkToolProject *getProject(const QString &projectName);
protected:
	QStringList dbmsTypeList;
	QHash<QString, QStringList> projectList; // by DBMStype
	static QHash<QString, TrkToolProject *> openedProjects;
    friend TrkToolProject;
};


class TrkToolRecord;
class TrkToolModel;

typedef QSet<int> SelectedSet;

class TrkToolProject: public QObject
{
    Q_OBJECT
public:
	TRK_HANDLE handle;
	QString name;
    TrkToolDB *db;
protected:
	bool opened;
	QHash<TRK_RECORD_TYPE, QString> recordTypes;
	QHash<TRK_RECORD_TYPE, QStringList*> qList; //QueryList
    QHash<TRK_RECORD_TYPE, RecordTypeDef*> recordDef;
    //QHash<TRK_RECORD_TYPE, TrkIntDef> fields; // field by vid by record_type
    //QHash<TRK_RECORD_TYPE, NameVid> nameVids;
    TrkToolQryModel theQueryModel;
    QHash<TRK_RECORD_TYPE, SelectedSet> selected;
    QHash<TRK_RECORD_TYPE, TrkToolModel*> selectedModels;
    QMap<QString, QString> userList;
    QString user;
public:
    TrkToolProject(TrkToolDB *db = 0);
    ~TrkToolProject();
    bool isOpened() const { return opened; }
    QString currentUser() const { return user; }
    QStringList noteTitles;

    // Tracker specific
	const QStringList &queryList(TRK_RECORD_TYPE type = TRK_SCR_TYPE) const;
	void close();
    QAbstractItemModel *queryModel(TRK_RECORD_TYPE type = TRK_SCR_TYPE);
#ifndef CONSOLE_APP
    QAbstractItemModel *createProxyQueryModel(TrkQryFilter::Filter filter, QObject *parent=0, TRK_RECORD_TYPE type = TRK_SCR_TYPE);
#endif
    TrkToolModel *openQueryModel(const QString &name, TRK_RECORD_TYPE type = TRK_SCR_TYPE, bool emitEvent = true);
    TrkToolModel *openRecentModel(int afterTransId, const QString &name, TRK_RECORD_TYPE type = TRK_SCR_TYPE);
    TrkToolModel *openIdsModel(const QList<int> &ids, TRK_RECORD_TYPE type = TRK_SCR_TYPE, bool emitEvent = true);
    NotesCol getNotes(int recId, TRK_RECORD_TYPE type) const;
    NotesCol getNotes(TRK_RECORD_HANDLE recHandle) const;
	QString fieldVID2Name(TRK_RECORD_TYPE rectype, TRK_VID vid);
	TRK_VID fieldName2VID(TRK_RECORD_TYPE rectype, const QString &fname);
    bool isSelectedId(TRK_UINT id, TRK_RECORD_TYPE recType = TRK_SCR_TYPE) const;
    void setSelectedId(TRK_UINT id, bool value, TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    void clearSelected(TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    void initQueryModel(TRK_RECORD_TYPE type = TRK_SCR_TYPE);
    ChoiceList *fieldChoiceList(const QString &name, TRK_RECORD_TYPE recType  = TRK_SCR_TYPE);
    TRK_FIELD_TYPE fieldType(const QString &name, TRK_RECORD_TYPE recType  = TRK_SCR_TYPE);
    TrkToolModel *selectedModel(TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    bool canFieldSubmit(const QString &name, TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    bool canFieldUpdate(const QString &name, TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    QString userFullName(const QString &login);
    TrkToolRecord *getRecordId(TRK_UINT id, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
protected:
    bool login(
        const QString &user,
        const QString &pass,
        const QString &project,
        const QString &dbmsType,
        const QString &dbmsName = QString(),
        const QString &dbmsUser = QString(),
        const QString &dbmsPassword = QString());
    bool readQueryList();
	bool readDefs();
    void readUserList();
    void readNoteTitles();
    bool readNoteRec(TRK_NOTE_HANDLE noteHandle, TrkNote *note) const;
signals:
    void openedModel(const TrkToolModel *model);
    void recordChanged(int id);

    friend class TrkToolDB;
    friend class TrkToolModel;
    friend class TrkToolRecord;
};

struct TrkHistoryItem
{
    QString projectName;
    QString queryName;
    bool isQuery;
    QString foundIds;
    TRK_RECORD_TYPE rectype;
    QDateTime createDateTime;
};

class TrkHistory: public BaseRecModel<TrkHistoryItem>
{
    Q_OBJECT
    //Q_PROPERTY(bool unique READ unique WRITE setUnique)
protected:
    TrkToolProject *prj;
    bool unique;
public:
    TrkHistory(QObject *parent = 0);
    virtual ~TrkHistory();
    void setProject(TrkToolProject *project);
    void setUnique(bool value);
    //virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    //virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
    void removeLast();
protected:
    virtual QVariant displayColData(const TrkHistoryItem & rec, int col) const;
public slots:
    void openedModel(const TrkToolModel *model);
};

class TrkToolRecordSet: public QObject
{
	Q_OBJECT
protected:
	//QStringList fields;
	QHash<TRK_UINT, TrkToolRecord *> records; // by Id
public:
	TrkToolRecordSet(QObject *parent = 0);
	int fieldNum(const QString &field) const;
	QString fieldName(int index) const;
	int recCount() const;
	int fieldCount() const;
	QVariant data(int record, int fieldNum) const;
	void clear();
	void readQuery(QSqlQuery * query);

	friend TrkToolModel;
	friend TrkToolRecord;
};

typedef TrkToolRecord *PTrkToolRecord;
class QDomDocument;

class TrkToolModel: public BaseRecModel<PTrkToolRecord> //QAbstractItemModel
{
	Q_OBJECT
    Q_PROPERTY(QString queryName READ getQueryName)
protected:
	//QList<int> recIds;
	TrkToolProject *prj;
	TRK_RECORD_TYPE rectype;
	QString idFieldName;
    int idCol;
    TRK_TRANSACTION_ID prevTransId, lastTransId;

    //TrkToolRecordSet rset;
	QList<int> vids;
    QSet<int> addedIds;
    QString queryName;
    bool isQuery;
public:
	TrkToolModel(TrkToolProject *project, TRK_RECORD_TYPE type, QObject *parent = 0);
    virtual ~TrkToolModel();

    TrkToolProject *project() const
    {
        return prj;
    }
	// Tracker specific
    bool openQuery(const QString &queryName, TRK_TRANSACTION_ID afterTransId=0);
    bool openIds(const QList<int> &ids);
	QDomDocument recordXml(int row) const;
	TRK_UINT rowId(int row) const;
    int rowOfRecordId(int id) const;
	bool saveRecord(TrkToolRecord *record);
	virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    const RecordTypeDef *typeDef();
    virtual void clearRecords();
    QString getQueryName() const;
    QList<int> getIdList() const;
    int idColumn() const { return idCol; }
    bool isSystemModel();
    int lastTransactionId() { return lastTransId; }
	/*
public:
	bool openScrs(const QList<int> &ids);
	bool openScrs(const QStringList &lines);
	bool openScrs(const QString &line);
    void close();*/
    void appendRecordId(TRK_UINT id);
    void removeRecordId(TRK_UINT id);
    void refreshQuery();
protected:
    bool appendRecord(TRK_RECORD_HANDLE recHandle);
    virtual QVariant displayColData(const PTrkToolRecord & rec, int col) const;
    virtual QVariant editColData(const PTrkToolRecord & rec, int col) const;
    virtual bool setEditColData(const PTrkToolRecord & rec, int col, const QVariant & value);
protected slots:
    void recordChanged(int id);

    friend class TrkHistory;
};

class QDomDocument;

struct TrkToolFile
{
    QString fileName;
    QDateTime createDateTime;
};

struct TrkScopeRecHandle
{
    TRK_RECORD_HANDLE handle;
    TrkScopeRecHandle(TRK_HANDLE prjHandle);
    ~TrkScopeRecHandle();
    inline bool isValid() { return handle != 0; }
    inline TRK_RECORD_HANDLE & operator*() { return handle; }
    inline TRK_RECORD_HANDLE * operator&() { return &handle; }
    int getRecord(TRK_UINT id, TRK_RECORD_TYPE rectype);
};

struct TrkScopeNoteHandle
{
    TRK_NOTE_HANDLE handle;
    TrkScopeNoteHandle(TRK_RECORD_HANDLE recHandle);
    ~TrkScopeNoteHandle();
    inline bool isValid() { return handle != 0; }
    inline TRK_NOTE_HANDLE & operator*() { return handle; }
    inline TRK_NOTE_HANDLE * operator&() { return &handle; }
};

class TrkToolRecord: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString description READ description WRITE setDescription)
public:
	enum RecMode {View, Edit, Insert};
protected:
	//QHash<QString, QVariant> values;
	TrkToolProject *prj;
	TRK_RECORD_TYPE rectype;
	QHash<int, QVariant> values; // by VID
	RecMode recMode;
	TRK_RECORD_HANDLE lockHandle;
	TRK_TRANSACTION_ID lastTransaction;
	QStringList fieldList;

	void readHandle(TRK_RECORD_HANDLE handle);
    void readFieldValue(TRK_RECORD_HANDLE handle, TRK_VID vid);
    int links;
    //NotesCol addedNotes;
    QList<int> deletedNotes;
public:
    TrkToolRecord(TrkToolProject *parent=0, TRK_RECORD_TYPE rtype=TRK_SCR_TYPE);
    TrkToolRecord(const TrkToolRecord& src);
    TrkToolRecord & operator =(const TrkToolRecord &src);
    virtual ~TrkToolRecord();
    Q_INVOKABLE bool isValid()
    {
        return prj!=0;
    }

    Q_INVOKABLE QVariant value(const QString& fieldName, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant value(TRK_VID vid, int role = Qt::DisplayRole) const;
	TrkToolRecord::RecMode mode() const { return recMode; }
    Q_INVOKABLE TRK_UINT recordId() const { return values[VID_Id].toUInt(); }
    Q_INVOKABLE void setValue(const QString& fieldName, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool insertBegin();
    Q_INVOKABLE bool updateBegin();
    Q_INVOKABLE bool commit();
    Q_INVOKABLE bool cancel();
    QDomDocument toXML() const;
    Q_INVOKABLE QString toHTML(const QString &xqCodeFile);
    Q_INVOKABLE QStringList historyList() const;
    QString description() const;
    Q_INVOKABLE bool setDescription(const QString &desc);
    Q_INVOKABLE const QStringList & fields() const;
    Q_INVOKABLE bool isSelected() const;
    Q_INVOKABLE void setSelected(bool value);
    Q_INVOKABLE bool isFieldReadOnly(const QString &field) const;
    const RecordTypeDef *typeDef() const
    {
        return prj->recordDef[rectype];
    }
    const TrkFieldDef fieldDef(TRK_VID vid) const
    {
        return typeDef()->getFieldDef(vid);
    }
    const TrkFieldDef fieldDef(const QString &name) const
    {
        return typeDef()->getFieldDef(name);
    }
    Q_INVOKABLE bool isEditing() const
    {
        return mode() != View;
    }
    void setValues(const FieldValues &values)
    {
        if(!isEditing())
            return;
        foreach(const QString &fieldName, values.keys())
            setValue(fieldName, values.value(fieldName), Qt::EditRole);

    }

    Q_INVOKABLE bool setNote(int index, const QString &title, const QString &text);
    Q_INVOKABLE bool setNoteText(int index, const QString &text);
    Q_INVOKABLE bool deleteNote(int index);
    Q_INVOKABLE bool appendNote(const QString &noteTitle, const QString &note);
    void addLink();
    void removeLink(const QObject *receiver=0);
    NotesCol getNotes() const;
    QList<TrkToolFile> fileList();
    bool saveFile(int fileIndex, const QString &dest);
    TrkToolProject *project() const
    {
        return prj;
    }

    Q_INVOKABLE void refresh();
    TRK_RECORD_TYPE recordType() const { return rectype; }
signals:
	void changedState(TrkToolRecord::RecMode newmode);
    void changed(int recId);

    friend class TrkToolRecordSet;
    friend class TrkToolModel;
    friend class TrkToolProject;
};

//TrkToolRecord & operator =(const TrkToolRecord &src);

Q_DECLARE_METATYPE(TrkToolRecord)

/*
class TrkToolModel: public QAbstractItemModel
{
	//Q_OBJECT
protected:
	//QList<int> recIds;
	TrkToolProject *prj;
	TRK_RECORD_TYPE rectype;
	QStringList queries;
	QString idFieldName;
	TRK_TRANSACTION_ID transId;

	TrkToolRecordSet rset;
public:
	TrkToolModel(TrkToolProject *project, TRK_RECORD_TYPE type, QObject *parent = 0);
	~TrkToolModel();
public:
	// Tracker specific
	bool openQuery(const QString &queryName);
public:
	virtual int	rowCount(const QModelIndex & parent = QModelIndex() ) const;
	
//	virtual int	columnCount(const QModelIndex & parent = QModelIndex() ) const;
//	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole ) const;
//	virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
//	virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
//	QModelIndex parent(const QModelIndex &) const;
//public:
//	bool openScrs(const QList<int> &ids);
//	bool openScrs(const QStringList &lines);
//	bool openScrs(const QString &line);
//	void close();
protected:
	void appendRecord(TRK_RECORD_HANDLE recHandle);
};

*/

#endif // _TRKVIEW_H_
