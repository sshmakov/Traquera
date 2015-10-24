#ifndef TQBASE_H
#define TQBASE_H

//#include <QtGui>
#include <tqplugin_global.h>
#include <tqplug.h>
//#include <tqcond.h>

#include <QtCore>
#include <QtXml>

class ConnectParams
{
public:
    int id;
    QString dbClass;
    QString dbType;
    QString dbServer;
    QString user;
    QString project;
    QString password;
    bool dbOsUser;
    QString dbmsUser;
    QString dbmsPass;
    bool autoLogin;
};


//need delete
class TQRecModel;
// --

class TQAbstractProject;
class TQRecord;

struct TQNote
{
    QString title;
    QString author;
    QString text;
    QDateTime crdate;
    QDateTime mddate;
    int perm;
    bool isAdded;
    bool isDeleted;
    bool isChanged;
//    TrkNote();
//    TrkNote(const TrkNote &src);
};

typedef QList<TQNote> TQNotesCol;


struct TQToolFile
{
    QString fileName;
    QDateTime createDateTime;
};

struct TQUser
{
    int id;
    QString login;
    QString displayName;
    QString fullName;
    bool isDeleted;
};

typedef QList<TQUser> TQUserList;

struct TQGroup
{
    int id;
    QString name;
};

typedef QList<TQGroup> TQGroupList;

/*
class TQQueryFilter: public QSortFilterProxyModel
{
    enum Filter {
        AllQuery = 0,
        UserQuery = 1,
        PublicQuery = 2
    };
};
*/

class QAuthenticator;
class QWidget;

class TQPLUGIN_SHARED TQAbstractDB: public QObject
{
    Q_OBJECT
private:
    QString m_dbType, m_dbServer, m_dbUser, m_dbPass;
    static QHash<QString, TQAbstractProject *> projectList;
public:
    typedef TQAbstractDB *(*createDbFunc)(QObject *);
    TQAbstractDB(QObject *parent = 0);
    virtual QStringList dbmsTypes() = 0;
    virtual QStringList projects(const QString &dbmsType,
                                 const QString &user = QString(),
                                 const QString &pass = QString()) = 0;
    virtual TQAbstractProject *openProject(
            const QString &projectName,
            const QString &user = QString(),
            const QString &pass = QString()
            ) = 0;
    virtual TQAbstractProject *openConnection(
            const QString &connectionString
            ) = 0;
    virtual void setDbmsType(const QString &dbType);
    virtual QString dbmsType() const;
    virtual void setDbmsServer(const QString &server);
    virtual QString dbmsServer() const;
    virtual void setDbmsUser(const QString &dbmsUser, const QString &dbmsPass = QString());
    virtual QString dbmsUser() const;
    virtual QString dbmsPass() const;
    virtual QWidget *createConnectWidget() const;
    static TQAbstractProject *getProject(const QString &projectName);
protected:
    static void registerProject(TQAbstractProject *prj);
    static void unregisterProject(TQAbstractProject *prj);

public:
    static bool registerDbClass(const QString &dbClass, TQAbstractDB::createDbFunc func);
    static bool unregisterDbClass(const QString &dbClass);
    static QStringList registeredDbClasses();
    static TQAbstractDB *createDbClass(const QString &dbClass, QObject *parent = 0);

signals:
    void dbAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator);
    void projectAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator);


    friend class TQAbstractProject;
};

typedef QList<int> IntList;

/*
struct TQConditionLine {
    bool isOpenBracket;
    bool isNot;
    int vid;
    int op;
    QVariant value1, value2;
    bool isCloseBracket;
    bool isOr;
};
*/

class TQQueryDef;

class TQPLUGIN_SHARED TQAbstractProject: public QObject
{
    Q_OBJECT
protected:
    TQAbstractDB *db;
public:
    TQAbstractProject(TQAbstractDB *db);
    ~TQAbstractProject();
    // Standard manipulations
    virtual bool isOpened() const = 0;
    virtual QString currentUser() const = 0;
    virtual QString projectName() const = 0;
    virtual QString fieldVID2Name(int rectype, int vid) = 0;
    virtual int fieldName2VID(int rectype, const QString &fname) = 0;
    virtual int defaultRecType() const = 0;
    virtual bool isSelectedId(int id, int recType) const = 0;
    virtual void setSelectedId(int id, bool value, int recType) = 0;
    virtual void clearSelected(int recType) = 0;
    virtual void initQueryModel(int recType) = 0;
    virtual int fieldNativeType(const QString &name, int recType) = 0;
    virtual TQRecModel *selectedModel(int recType) = 0;
    virtual bool canFieldSubmit(int vid, int recType) = 0;
    virtual bool canFieldUpdate(int vid, int recType) = 0;
    virtual TQRecord *createRecordById(int id, int rectype) = 0;
    virtual TQRecord *newRecord(int rectype) = 0;
    virtual TQAbstractRecordTypeDef *recordTypeDef(int rectype) = 0;
    virtual QDomDocument recordTypeDefDoc(int rectype) = 0;

    virtual TQRecModel *openQueryModel(const QString &queryName, int recType, bool emitEvent = true) = 0;
    virtual QAbstractItemModel *openIdsModel(const IntList &ids, int recType, bool emitEvent = true) = 0;
    virtual void refreshModel(QAbstractItemModel *model) = 0;
    virtual QAbstractItemModel *queryModel(int type) = 0;
    virtual TQRecord *recordOfIndex(const QModelIndex &index) = 0;
    //virtual QAbstractItemModel *createProxyQueryModel(TQQueryFilter filter, QObject *parent=0, int type) = 0;
    virtual QList<int> getQueryIds(const QString &name, int type, qint64 afterTransId = 0) = 0;
    virtual bool readRecordWhole(TQRecord *record) = 0;
    virtual bool readRecordFields(TQRecord *record) = 0;
    virtual bool readRecordTexts(TQRecord *record) = 0;
    virtual bool readRecordBase(TQRecord *record) = 0;
//    virtual TQRecord *createRecordByHandle(TRK_RECORD_HANDLE recHandle, TRK_RECORD_TYPE rectype = TRK_SCR_TYPE) = 0;
    virtual QVariant getFieldValue(const TQRecord *record, const QString &fname, bool *ok = 0) = 0;
    virtual QVariant getFieldValue(const TQRecord *record, int vid, bool *ok = 0) = 0;
    virtual bool setFieldValue(TQRecord *record, const QString &fname, const QVariant &value) = 0;
    //virtual bool insertRecordBegin(TQRecord *record) = 0;
    virtual bool updateRecordBegin(TQRecord *record) = 0;
    virtual bool commitRecord(TQRecord *record) = 0;
    virtual bool cancelRecord(TQRecord *record) = 0;
    virtual QList<TQToolFile> attachedFiles(TQRecord *record) = 0;
    virtual bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest) = 0;
    virtual QStringList historyList(TQRecord *record) = 0;
    virtual QHash<int,QString> baseRecordFields(int rectype) = 0;
    virtual bool isSystemModel(QAbstractItemModel *model) const = 0;
    virtual TQQueryDef *queryDefinition(const QString &queryName, int rectype) = 0;
    virtual TQQueryDef *createQueryDefinition(int rectype) = 0;
    virtual bool saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype) = 0;
    virtual QStringList userNames() = 0;
    virtual QMap<QString, TQUser> userList() = 0;
    virtual QString userFullName(const QString &login) = 0;
    virtual QString userFullName(int userId) = 0;
    virtual QString userLogin(int userId) = 0;
    virtual int userId(const QString &login) = 0;
    virtual TQGroupList userGroups() = 0;
signals:
    void openedModel(const QAbstractItemModel *model);
    void recordChanged(int id);
    void recordValuesLoaded(int recId) const;
    void recordStateChanged(int recId);
};

typedef QSet<int> TQSelectedSet;

class TQPLUGIN_SHARED TQBaseProject: public TQAbstractProject
{
    Q_OBJECT
protected:
    QString name;
    bool opened;
    QHash<int, QString> recordTypes;
    QHash<int, QStringList*> qList; //QueryList
//    QHash<int, TQAbstractRecordTypeDef*> recordDef;
    //QHash<TRK_RECORD_TYPE, TrkIntDef> fields; // field by vid by record_type
    //QHash<TRK_RECORD_TYPE, NameVid> nameVids;
    //TrkToolQryModel theQueryModel;
    QHash<int, TQSelectedSet> selected;
    QHash<int, TQRecModel*> selectedModels;
    QMap<QString, TQUser> m_userList;
    QString user;
    QStringList noteTitles;
public:
    TQBaseProject(TQAbstractDB *db);
    // Standard manipulations
    bool isOpened() const;
    QString currentUser() const;
    QString projectName() const;
    virtual QString fieldVID2Name(int rectype, int vid);
    virtual int fieldName2VID(int rectype, const QString &fname);
    virtual int fieldNativeType(const QString &name, int recType);
    virtual int defaultRecType() const;
    virtual bool isSelectedId(int id, int recType) const;
    virtual void setSelectedId(int id, bool value, int recType);
    virtual void clearSelected(int recType);
    virtual TQRecModel *selectedModel(int recType);
    virtual bool canFieldSubmit(int vid, int recType);
    virtual bool canFieldUpdate(int vid, int recType);
    virtual QString userFullName(const QString &login);
    virtual TQRecord *createRecordById(int id, int rectype);
    virtual TQRecord *newRecord(int rectype);
//    virtual TQAbstractRecordTypeDef *recordTypeDef(int rectype);
    virtual QDomDocument recordTypeDefDoc(int recType);
    virtual void initQueryModel(int recType);
    QAbstractItemModel *queryModel(int type);
    virtual QList<TQToolFile> attachedFiles(TQRecord *record);
    virtual bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest);
    virtual TQQueryDef *queryDefinition(const QString &queryName, int rectype);
    virtual TQQueryDef *createQueryDefinition(int rectype);
    virtual bool saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype);
    virtual QStringList userNames();
    virtual QMap<QString, TQUser> userList();
    virtual QString userFullName(int userId);
    virtual QString userLogin(int userId);
    virtual int userId(const QString &login);
    virtual TQGroupList userGroups();

    // need redefine
/*
TQAbstractRecordTypeDef *recordTypeDef(int);
TQRecModel *openQueryModel(const QString &,int,bool);
QAbstractItemModel *openIdsModel(const IntList &,int,bool);
void refreshModel(QAbstractItemModel *);
TQRecord *recordOfIndex(const QModelIndex &);
QList<int> getQueryIds(const QString &,int,qint64);
bool readRecordWhole(TQRecord *);
bool readRecordFields(TQRecord *);
bool readRecordTexts(TQRecord *);
bool readRecordBase(TQRecord *);
QVariant getFieldValue(const TQRecord *,int,bool *);
QVariant getFieldValue(const TQRecord *,const QString &,bool *);
bool setFieldValue(TQRecord *,const QString &,const QVariant &);
bool updateRecordBegin(TQRecord *);
bool commitRecord(TQRecord *);
bool cancelRecord(TQRecord *);
QStringList historyList(TQRecord *);
QHash<int,QString> baseRecordFields(int);
bool isSystemModel(QAbstractItemModel *) const;
*/
};


class TQPLUGIN_SHARED TQRecord: public QObject
{
    Q_OBJECT
public:
    enum TQRecMode {View, Edit, Insert};
protected:
    TQAbstractProject *m_prj;
    int recType;
    int recMode;
    int recId;
    int links;

    /*
    //QHash<QString, QVariant> values;
    mutable QHash<int, QVariant> values; // by VID
    QHash<int, bool> changedValue; // by VID
    qint64 lastTransaction;
    QStringList fieldList;

    //void readHandle(TRK_RECORD_HANDLE handle, bool force=false);
    //void readFieldValue(TRK_RECORD_HANDLE handle, TRK_VID vid);
    int links;
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
    */
public: //protected: // Project specific data
    //TRK_RECORD_HANDLE lockHandle;
    TQRecord();
    TQRecord(TQAbstractProject *prj, int rtype, int id);
    TQRecord(const TQRecord &src);
//    TQRecord & operator = (const TQRecord &src);
    virtual ~TQRecord();
    Q_INVOKABLE virtual bool isValid() const;
    Q_INVOKABLE virtual int recordType() const;
    Q_INVOKABLE virtual int recordId() const;
    TQAbstractRecordTypeDef *recordDef() const;

    Q_INVOKABLE virtual int mode() const;
    Q_INVOKABLE virtual void setMode(int newMode);
    Q_INVOKABLE virtual bool isEditing() const;

    Q_INVOKABLE virtual bool updateBegin();
    Q_INVOKABLE virtual bool commit();
    Q_INVOKABLE virtual bool cancel();

    Q_INVOKABLE virtual QString title() const;
    Q_INVOKABLE virtual bool setTitle(const QString &newTitle);

    Q_INVOKABLE virtual QString description() ;
    Q_INVOKABLE virtual bool setDescription(const QString &newDesc);

    virtual TQNotesCol notes() const;
    Q_INVOKABLE virtual QString noteTitle(int index) const;
    Q_INVOKABLE virtual QString noteText(int index) const;
    Q_INVOKABLE virtual bool setNoteTitle(int index, const QString &newTitle);
    Q_INVOKABLE virtual bool setNoteText(int index, const QString &newText);
    Q_INVOKABLE virtual bool setNote(int index, const QString &newTitle, const QString &newText);
    Q_INVOKABLE virtual int addNote(const QString &noteTitle, const QString &noteText);

    virtual QList<TQToolFile> fileList();
    Q_INVOKABLE virtual bool saveFile(int fileIndex, const QString &dest);
    Q_INVOKABLE virtual bool hasFiles();

    virtual TQAbstractProject *project() const;
    virtual const TQAbstractRecordTypeDef *typeDef() const;

    Q_INVOKABLE virtual QVariant value(int vid, int role = Qt::DisplayRole) const ;
    Q_INVOKABLE virtual QVariant value(const QString& fieldName, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual bool setValue(int vid, const QVariant& newValue, int role = Qt::EditRole);
    virtual bool setValues(const QHash<QString, QVariant> &values);
//    Q_INVOKABLE  int recordId() const { return values[VID_Id].toInt(); } // TRK_UINT
//    Q_INVOKABLE void setValue(const QString& fieldName, const QVariant& value, int role = Qt::EditRole);

    virtual QDomDocument toXML();
//    Q_INVOKABLE virtual QString toHTML(const QString &xqCodeFile);
    Q_INVOKABLE virtual QStringList historyList() const;
    Q_INVOKABLE bool isFieldReadOnly(const QString &field) const;

    Q_INVOKABLE virtual void addLink();
    Q_INVOKABLE virtual void removeLink(const QObject *receiver=0);
    Q_INVOKABLE virtual bool isSelected() const;
    Q_INVOKABLE virtual void setSelected(bool value);
    Q_INVOKABLE virtual void refresh();
signals:
    void changedState(int newMode) const;
    void changed(int recId) const;

//    virtual void setRecordId(int id) { values[VID_Id] = QVariant::fromValue(id); }
//    TQRecord(const TQRecord& src);
//    TQRecord & operator =(const TQRecord &src);
    /*
    Q_INVOKABLE bool isValid()
    {
        return prj!=0;
    }

    Q_INVOKABLE QVariant value(const QString& fieldName, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant value(TRK_VID vid, int role = Qt::DisplayRole) const ;
    TQRecord::RecMode mode() const { return recMode; }
    Q_INVOKABLE  int recordId() const { return values[VID_Id].toInt(); } // TRK_UINT
    Q_INVOKABLE QString title();
    Q_INVOKABLE void setTitle(const QString &newTitle);
    Q_INVOKABLE void setValue(const QString& fieldName, const QVariant& value, int role = Qt::EditRole);
    Q_INVOKABLE void setValue(TRK_VID vid, const QVariant& value, int role = Qt::EditRole);
    //Q_INVOKABLE bool insertBegin();
    Q_INVOKABLE bool updateBegin();
    Q_INVOKABLE bool commit();
    Q_INVOKABLE bool cancel();
    Q_INVOKABLE bool isInsertMode() const;
    Q_INVOKABLE bool isEditMode() const;
    QDomDocument toXML();
    Q_INVOKABLE QString toHTML(const QString &xqCodeFile);
    Q_INVOKABLE QStringList historyList();
    QString description();
    Q_INVOKABLE bool setDescription(const QString &newDesc);
    Q_INVOKABLE const QStringList & fields() const;
    Q_INVOKABLE bool isSelected() const;
    Q_INVOKABLE void setSelected(bool value);
    Q_INVOKABLE bool isFieldReadOnly(const QString &field);
    const TQAbstractRecordTypeDef *typeDef() const
    {
        return prj->recordTypeDef(rectype);
    }
    TQAbstractFieldType fieldDef(TRK_VID vid) const
    {
        return typeDef()->getFieldType(vid);
    }
    TQAbstractFieldType fieldDef(const QString &name) const
    {
        return typeDef()->getFieldType(name);
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
    Q_INVOKABLE int addNote(const QString &noteTitle, const QString &noteText);
    Q_INVOKABLE bool appendNote(const QString &noteTitle, const QString &note);
    Q_INVOKABLE QString noteTitle(int index);
    Q_INVOKABLE QString noteText(int index);
    void addLink();
    void removeLink(const QObject *receiver=0);
    TQNotesCol notes();
    QList<TQToolFile> fileList();
    bool saveFile(int fileIndex, const QString &dest);
    TrkToolProject *project() const
    {
        return prj;
    }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void releaseBuffer();
    TRK_RECORD_TYPE recordType() const { return rectype; }
protected slots:
    void somethingChanged();
    void valuesReloaded();


    friend class TrkToolRecordSet;
    //friend class TrkToolModel;
    friend class TrkToolProject;
    */
};

Q_DECLARE_METATYPE(TQRecord)

#endif // TQBASE_H
