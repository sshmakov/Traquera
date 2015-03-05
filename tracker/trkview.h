#ifndef _TRKVIEW_H_
#define _TRKVIEW_H_ 1

#ifndef CONSOLE_APP
    #include <QTableView>
#include <QAction>
#endif

#include <QtCore>
#include <QtXml>
#include "trktool.h"
#include "trktool2.h"
#include "tracker.h"
#include "tqplug.h"
#include "tqbase.h"
#include "tqmodels.h"

#define TT_DATETIME_FORMAT "dd.MM.yyyy H:mm:ss"

class TrkDb;
//class TrkModel;

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


struct TQUser {
    QString userName;
    QString fullName;
    QString email;
};

typedef QList<TQUser> UserList;

class TQRecordTypeDef;

class TrkFieldDef //: public AbstractFieldDef
{
protected:
    mutable ChoiceList *cashedChoiceList;
//    QString choiceName;
    TQRecordTypeDef *recDef;
    int vid;
    //static ChoiceList emptyChoices;

	QString name;
    TRK_FIELD_TYPE nativeType;
    bool nullable;
    int minValue;
    int maxValue;
    QVariant defaultValue;
    //const ChoiceList *userList();

    TrkFieldDef(TQRecordTypeDef *recordDef, int fieldVID);
    TrkFieldDef(const TrkFieldDef& src);
public:
    ~TrkFieldDef()
    {
//		if(p_choiceList)
//		{
//            delete p_choiceList;
//			p_choiceList = 0;
//		}
    }

    /*
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
    */

    ChoiceList choiceList() const;
//    QString choiceListName() const;
    QStringList choiceStringList(bool isDisplayText = true) const;


    bool isChoice() const
    {
        int fType = nativeType;
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
        int fType = nativeType;
        return (fType == TRK_FIELD_TYPE_SUBMITTER)
                || (fType == TRK_FIELD_TYPE_OWNER)
                || (fType == TRK_FIELD_TYPE_USER)
                ;
    }
    /*
    inline bool isValid() const
    {
        return recDef && !name.isEmpty();
    }
    */
    bool isNullable() const
    {
        return nullable;
    }
    bool canSubmit();
    bool canUpdate();
    QString valueToDisplay(const QVariant &value) const;
    QVariant displayToValue(const QString &text) const;
    TQRecordTypeDef *recordDef() const;

    friend class TrkToolProject;
    //friend class TrkFieldType;
    friend class TQRecordTypeDef;
};

/*class TrkFieldType: public AbstractFieldType
{
protected:
    TrkFieldDef *def;
    TrkFieldType(TrkFieldDef *srcDef):def(srcDef) {}
    TrkFieldType(TrkFieldDef &srcDef):def(&srcDef) {}
public:
    TrkFieldType():def(0) {}
    TrkFieldType(const TrkFieldType &src)
        :def(src.def)
    {}
    inline bool isValid() const { return def!=0; }
    TRK_FIELD_TYPE fType() const
    {
        if(isValid())
            return def->fType;
        return TRK_FIELD_TYPE_NONE;
    }
    ChoiceList choiceList() const
    {
        if(isValid())
            return def->choiceList();
        return QList<TrkToolChoice>();//&TrkFieldDef::emptyChoices;
    }
    QStringList choiceStringList(bool isDisplayText = true) const
    {
        if(isValid())
            return def->choiceStringList(isDisplayText);
        return QStringList();
    }
    inline bool isChoice() const
    {
        if(isValid())
            return def->isChoice();
        return false;
    }
    inline bool isUser() const
    {
        if(isValid())
            return def->isUser();
        return false;
    }
    inline bool isNullable() const
    {
        if(isValid())
            return def->isNullable();
        return false;
    }
    inline bool canSubmit() const
    {
        if(isValid())
            return def->canSubmit();
        return false;
    }
    inline bool canUpdate() const
    {
        if(isValid())
            return def->canUpdate();
        return false;
    }

    inline QString valueToDisplay(const QVariant &value) const
    {
        if(isValid())
            return def->valueToDisplay(value);
        return "";
    }
    inline QVariant displayToValue(const QString &text) const
    {
        if(isValid())
            return def->displayToValue(text);
        return QVariant();
    }
    inline QVariant defaultValue() const
    {
        if(isValid())
            return def->defaultValue;
        return QVariant();
    }
    inline int minValue() const
    {
        if(isValid())
            return def->minValue;
        return 0;
    }
    inline int maxValue() const
    {
        if(isValid())
            return def->maxValue;
        return 0;
    }

    friend class RecordTypeDef;
};
*/

typedef QHash<TRK_VID, TrkFieldDef *> TrkVidDefs;
typedef QHash<QString, int> NameVid;


class TQRecordTypeDef: public QObject, public TQAbstractRecordTypeDef
{
    Q_OBJECT
    Q_INTERFACES(TQAbstractRecordTypeDef)
protected:
    TrkToolProject *prj;
    TRK_RECORD_TYPE recType;
    bool isReadOnly;
    TrkVidDefs fieldDefs;
    QHash<int, QString> baseFields;
    QHash<QString, int> nameVids;
    mutable QHash<QString, ChoiceList *> choices; // by tableName
    void clearFieldDefs();
    ChoiceList userChoices;
private:
    ChoiceList emptyChoices;
public:
    TQRecordTypeDef(TrkToolProject *project = 0);
    ~TQRecordTypeDef();
    int roleVid(int role) const;
    QStringList fieldNames() const;
    TQAbstractFieldType getFieldType(int vid, bool *ok = 0) const;
    TQAbstractFieldType getFieldType(const QString &name, bool *ok = 0) const;
    int fieldNativeType(const QString &name) const;
    int fieldNativeType(int vid) const;
    int fieldSimpleType(int vid) const;
    bool canFieldSubmit(int vid) const;
    bool canFieldUpdate(int vid) const;
    bool isNullable(int vid) const;
    bool hasChoiceList(int vid) const;
    //ChoiceList choiceList(int vid) const;
//    ChoiceList choiceList(const QString &fieldName) const;
    ChoiceList choiceTable(const QString &tableName) const;


    bool containFieldVid(int vid) const;
//    QList<TRK_VID> fieldVids() const;
    int fieldVid(const QString &name) const;
    QList<int> fieldVids() const;
    int recordType() const;
    QString fieldName(int vid) const;
    QIODevice *defineSource() const;
    QString valueToDisplay(int vid, const QVariant &value) const;
    QVariant displayToValue(int vid, const QString &text) const;
    virtual QVariant fieldDefaultValue(int vid) const;
    virtual QVariant fieldMinValue(int vid) const;
    virtual QVariant fieldMaxValue(int vid) const;
    virtual QString fieldChoiceTable(int vid) const;
    virtual QString dateTimeFormat() const;
    QStringList noteTitleList() const;

    TrkToolProject *project() const;


    friend class TrkToolProject;
};


/* not used
 *
class TrkStringModel : public BaseRecModel<QString>
{
	Q_OBJECT
public:
	TrkStringModel(QObject *parent = 0)
		:BaseRecModel<QString>(parent)
	{
    }
protected:
    virtual QVariant displayColData(const QString &rec, int col) const
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

*/

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


class TrkToolProject;

class TrkToolDB: public TQAbstractDB
{
    Q_OBJECT
public:
	TRK_HANDLE handle;
//    QString dbmsType, serverName;
//	QString dbmsName, dbmsUser, dbmsPassword;
	TrkToolDB(QObject *parent = 0);
	~TrkToolDB();
    virtual QStringList dbmsTypes();
    virtual QString dbmsServer() const;
    virtual QStringList projects(const QString &dbmsType);
    virtual TQAbstractProject *openProject(
		const QString &projectName,
        const QString &user = QString(),
        const QString &pass = QString());
    TQAbstractProject *getProject(const QString &projectName);
protected:
	QStringList dbmsTypeList;
	QHash<QString, QStringList> projectList; // by DBMStype
	static QHash<QString, TrkToolProject *> openedProjects;
    friend class TrkToolProject;
};


class TrkToolRecord;
class TrkToolModel;

class TrkRecHandler {
public:
    TRK_RECORD_HANDLE handle;
    TRK_RECORD_TYPE recType;
    int id;
    bool isInsert;
    bool isModify;
    int links;
};



class TrkToolProject: public TQAbstractProject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ projectName)
private:
    TRK_HANDLE handle;
protected:
    QString name;
    TrkToolDB *db;
    bool opened;
	QHash<TRK_RECORD_TYPE, QString> recordTypes;
	QHash<TRK_RECORD_TYPE, QStringList*> qList; //QueryList
    QHash<TRK_RECORD_TYPE, TQRecordTypeDef*> recordDef;
    //QHash<TRK_RECORD_TYPE, TrkIntDef> fields; // field by vid by record_type
    //QHash<TRK_RECORD_TYPE, NameVid> nameVids;
    QHash<int, TrkToolQryModel *> theQueryModel;
    QHash<TRK_RECORD_TYPE, TQSelectedSet> selected;
    QHash<TRK_RECORD_TYPE, TrkToolModel*> selectedModels;
    QMap<QString, QString> userList;
    QString user;
    QStringList noteTitles;
protected: // Work with record handlers
    mutable QMutex handlerMutex;
    QList<TrkRecHandler> handlers;
    //QHash<TrkToolRecord *, int> locks;

    TrkRecHandler *allocRecHandler(int recID = 0, TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
    void freeRecHandler(TrkRecHandler *recHandler);
    void clearAllHandlers();
    bool resetRecHandler(TrkRecHandler *rec);
    //TrkRecHandler *pointTrkRecHandler(int recID, TRK_RECORD_TYPE recType = TRK_SCR_TYPE);
public:
    TrkToolProject(TrkToolDB *db = 0);
    ~TrkToolProject();
    virtual bool isOpened() const;
    virtual QString currentUser() const;
    virtual QString projectName() const;

    // Tracker specific
    const QStringList &queryList(TRK_RECORD_TYPE type = TRK_SCR_TYPE) const;
	void close();
    QAbstractItemModel *queryModel(int type);
#ifdef CLIENT_APP
    //QAbstractItemModel *createProxyQueryModel(int filter, QObject *parent=0, int type);
#endif
    TrkToolModel *openQueryModel(const QString &name, int type, bool emitEvent = true);
    QList<int> getQueryIds(const QString &name, int type,
                           qint64 afterTransId = 0);
    //TrkToolModel *openRecentModel(int afterTransId, const QString &name, TRK_RECORD_TYPE type = TRK_SCR_TYPE);
    QAbstractItemModel *openIdsModel(const QList<int> &ids, int type, bool emitEvent = true);
    void refreshModel(QAbstractItemModel *model);
    //NotesCol getNotes(int recId, TRK_RECORD_TYPE type) const;
    QStringList noteTitleList();
public:
    // Standard manipulations
    virtual QString fieldVID2Name(int rectype, int vid);
    virtual int fieldName2VID(int rectype, const QString &fname);
    virtual int defaultRecType() const;
    virtual bool isSelectedId(int id, int recType) const;
    virtual void setSelectedId(int id, bool value, int recType);
    virtual void clearSelected(int recType);
    void initQueryModel(int type = TRK_SCR_TYPE);
    virtual int fieldNativeType(const QString &name, int recType);
    virtual TrkToolModel *selectedModel(int recType);
    virtual bool canFieldSubmit(int vid, int recType);
    virtual bool canFieldUpdate(int vid, int recType);
    virtual QString userFullName(const QString &login);
    virtual TQRecord *createRecordById(int id, int rectype);
    virtual TQRecord *newRecord(int rectype);
    virtual TQAbstractRecordTypeDef *recordTypeDef(int rectype);
    virtual QDomDocument recordTypeDefDoc(int rectype);
    virtual TQRecord *recordOfIndex(const QModelIndex &index);
    virtual bool isSystemModel(QAbstractItemModel *model) const;
protected:
    /* Model manipulations */
    bool fillModel(TrkToolModel *model, const QString &queryName, int type,
                   qint64 afterTransId = 0);
    /* Record manipulations */
    bool readRecordWhole(TQRecord *record);
    bool readRecordFields(TQRecord *record);
    bool readRecordTexts(TQRecord *record);
    bool readRecordBase(TQRecord *record);
//    virtual TrkToolRecord *createRecordByHandle(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    QVariant getFieldValue(const TQRecord *record, const QString &fname, bool *ok = 0);
    QVariant getFieldValue(const TQRecord *record, int vid, bool *ok = 0);
    bool setFieldValue(TQRecord *record, const QString &fname, const QVariant &value);
    //virtual bool insertRecordBegin(TrkToolRecord *record);
    bool updateRecordBegin(TQRecord *record);
    bool commitRecord(TQRecord *record);
    bool cancelRecord(TQRecord *record);
    QList<TQToolFile> attachedFiles(TQRecord *record);
    QStringList historyList(TQRecord *record);
    QHash<int,QString> baseRecordFields(int rectype);
    bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest);
    //    bool saveFileFromRecord(TrkToolRecord *record, int fileIndex, const QString &dest);
private:
    // Tracker specific
    bool doCommitInsert(TrkToolRecord *record);
    bool doCommitUpdate(TrkToolRecord *record);
    bool doCancelInsert(TrkToolRecord *record);
    bool doCancelUpdate(TrkToolRecord *record);
    QString doGetDesc(TRK_RECORD_HANDLE recHandle) const;
    TQNotesCol doGetNotes(TRK_RECORD_HANDLE recHandle) const;
    QVariant doGetValue(TRK_RECORD_HANDLE recHandle, const QString &fname, TRK_FIELD_TYPE fType, bool *ok=0);
    bool doReadBaseFields(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle);
    bool doSaveFields(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle);
    bool doSetValue(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle, TRK_VID vid, const QVariant& value);
    bool doSaveNotes(TrkToolRecord *record, TRK_RECORD_HANDLE recHandle);
    bool doAppendNote(TRK_RECORD_HANDLE recHandle, const QString &noteTitle, const QString &noteText);
    bool doDeleteNote(TRK_RECORD_HANDLE recHandle, int noteId);
    bool doChangeNote(TRK_RECORD_HANDLE recHandle, int noteId, const QString &noteTitle, const QString &noteText);
    int doGetRecordId(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    QStringList doGetHistoryList(TRK_RECORD_HANDLE recHandle);
    //QString doFieldVID2Name(TRK_RECORD_TYPE rectype, TRK_VID vid);
protected:
    ChoiceList *fieldChoiceList(const QString &name, int recType);
    bool login(
        const QString &user,
        const QString &pass,
        const QString &project);
    bool readQueryList();
	bool readDefs();
    void readUserList();
    void readNoteTitles();
    bool readNoteRec(TRK_NOTE_HANDLE noteHandle, TQNote *note) const;

    friend class TrkToolDB;
    friend class TrkToolModel;
    friend class TrkToolRecord;
    friend class TQRecordTypeDef;
    friend class TrkScopeRecHandle;
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

    friend class TrkToolModel;
    friend class TrkToolRecord;

};

typedef TrkToolRecord *PTrkToolRecord;
class QDomDocument;

class QDomDocument;

class TrkScopeRecHandle
{
protected:
    bool isTemp;
    bool isOk;
    bool fromPrj;
    TrkToolProject *project;
    TRK_RECORD_HANDLE handle;
    TrkRecHandler *pRecHandler;
public:
    //TrkScopeRecHandle(TRK_HANDLE prjHandle, TRK_RECORD_HANDLE recHandle = 0, TRK_UINT recId = 0, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    TrkScopeRecHandle(TrkToolProject *prj, TRK_RECORD_HANDLE recHandle = 0, TRK_UINT recId = 0, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE);
    TrkScopeRecHandle(TrkToolProject *prj, const TrkToolRecord *record);
    ~TrkScopeRecHandle();
    inline bool isValid() { return (handle != 0) && isOk; }
    TRK_RECORD_HANDLE &nativeHandle();
    inline TRK_RECORD_HANDLE & operator*() { return nativeHandle(); }
    inline TRK_RECORD_HANDLE * operator&() { return &nativeHandle(); }
    TrkRecHandler *recHandler() { return pRecHandler; }
    //int getRecord(TRK_UINT id, TRK_RECORD_TYPE rectype);
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

class TrkToolRecord: public TQRecord
{
    Q_OBJECT
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected)
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QString description READ description WRITE setDescription)
public:
//	enum RecMode {View, Edit, Insert};
protected:
	//QHash<QString, QVariant> values;
	TrkToolProject *prj;
//	TRK_RECORD_TYPE rectype;
    mutable QHash<int, QVariant> values; // by VID
    QHash<int, bool> changedValue; // by VID
//    int trecMode;
    qint64 lastTransaction;
	QStringList fieldList;

    //void readHandle(TRK_RECORD_HANDLE handle, bool force=false);
    //void readFieldValue(TRK_RECORD_HANDLE handle, TRK_VID vid);
//    int links;
    //NotesCol addedNotes;
//    QList<int> deletedNotes;
//    QHash<int, TrkNote> addedNotes;
    TQNotesCol notesList;
    bool textsReaded;
    QString desc;
    bool descChanged; //, descLoaded;
    // void needRecHandle();
    // void freeRecHandle();
    bool readed;
    bool historyReaded;
    QStringList historyListMem;
    void readFullRecord();
    virtual void init();
public: //protected: // Project specific data
    //TRK_RECORD_HANDLE lockHandle;
    void setRecordId(TRK_UINT id) { values[VID_Id] = QVariant::fromValue(id); }
public:
    TrkToolRecord(TrkToolProject *parent=0, TRK_RECORD_TYPE rtype=TRK_SCR_TYPE);
    TrkToolRecord(const TrkToolRecord &src);
//    TrkToolRecord & operator =(const TrkToolRecord &src);
    virtual ~TrkToolRecord();
//    Q_INVOKABLE bool isValid()
//    {
//        return prj!=0;
//    }

    Q_INVOKABLE QVariant value(const QString& fieldName, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant value(int vid, int role = Qt::DisplayRole) const ;
    Q_INVOKABLE  int recordId() const { return values[VID_Id].toInt(); } // TRK_UINT
    Q_INVOKABLE QString title() const;
    Q_INVOKABLE bool setTitle(const QString &newTitle);
    Q_INVOKABLE bool setValue(const QString& fieldName, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE bool setValue(int vid, const QVariant& value, int role = Qt::EditRole);
    //Q_INVOKABLE bool insertBegin();
//    Q_INVOKABLE bool updateBegin();
//    Q_INVOKABLE bool commit();
//    Q_INVOKABLE bool cancel();
//    Q_INVOKABLE int mode() const { return trecMode; }
    Q_INVOKABLE bool isInsertMode() const;
    Q_INVOKABLE bool isEditMode() const;
    QDomDocument toXML();
//    Q_INVOKABLE QString toHTML(const QString &xqCodeFile);
    Q_INVOKABLE QStringList historyList();
    QString description();
    Q_INVOKABLE bool setDescription(const QString &newDesc);
    Q_INVOKABLE const QStringList & fields() const;
//    Q_INVOKABLE bool isSelected() const;
//    Q_INVOKABLE void setSelected(bool value);
    Q_INVOKABLE bool isFieldReadOnly(const QString &field) const;
    const TQAbstractRecordTypeDef *typeDef() const
    {
        return prj->recordTypeDef(recType);
    }
    TQAbstractFieldType fieldDef(TRK_VID vid) const
    {
        return typeDef()->getFieldType(vid);
    }
    TQAbstractFieldType fieldDef(const QString &name) const
    {
        return typeDef()->getFieldType(name);
    }
//    Q_INVOKABLE bool isEditing() const
//    {
//        return mode() != View;
//    }
    bool setValues(const FieldValues &values)
    {
        if(!isEditing())
            return false;
        foreach(const QString &fieldName, values.keys())
            if(!setValue(fieldName, values.value(fieldName), Qt::EditRole))
                return false;
        return true;
    }

    Q_INVOKABLE bool setNote(int index, const QString &title, const QString &text);
    Q_INVOKABLE bool setNoteText(int index, const QString &text);
    Q_INVOKABLE bool deleteNote(int index);
    Q_INVOKABLE int addNote(const QString &noteTitle, const QString &noteText);
//    Q_INVOKABLE bool appendNote(const QString &noteTitle, const QString &note);
    Q_INVOKABLE QString noteTitle(int index);
    Q_INVOKABLE QString noteText(int index);
//    void addLink();
//    void removeLink(const QObject *receiver=0);
    TQNotesCol notes();
    QList<TQToolFile> fileList();
    bool saveFile(int fileIndex, const QString &dest);
    TrkToolProject *project() const
    {
        return prj;
    }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void releaseBuffer();
//    TRK_RECORD_TYPE recordType() const { return rectype; }
protected slots:
    void somethingChanged();
    void valuesReloaded();
/*
signals:
    void changedState(TQRecord::TQRecMode newmode);
    void changed(int recId);
*/
    friend class TrkToolRecordSet;
    //friend class TrkToolModel;
    friend class TrkToolProject;
};

//TrkToolRecord & operator =(const TrkToolRecord &src);

Q_DECLARE_METATYPE(TrkToolRecord)

/*class TrkToolModel: public QAbstractItemModel
{
	//Q_OBJECT
protected:
	//QList<int> recIds;
	TrkToolProject *prj;
	TRK_RECORD_TYPE rectype;
	QStringList queries;
	QString idFieldName;
    qint64 transId;

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
