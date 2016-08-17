#ifndef TQBASE_H
#define TQBASE_H

#include <QtCore>
#include <QtXml>
//#include <QtGui>
#include <tqplugin_global.h>
#include <tqplug.h>
//#include <tqcond.h>


#define DBPARAM_CLASS "DBClass"
#define DBPARAM_TYPE "DBType"
#define DBPARAM_SERVER "DBServer"
#define DBPARAM_OSUSER "DBOSUser"
#define DBPARAM_USER "DBUser"
#define DBPARAM_PASSWORD "DBPassword"
#define PRJPARAM_USER "User"
#define PRJPARAM_PASSWORD "Password"
#define PRJPARAM_NAME "Project"
#define PRJPARAM_RECORDTYPE "RecordType"
#define PRJPARAM_AUTOLOGIN "AutoLogin"



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
    QString authorFullName;
    QString text;
    QDateTime crdate;
    QDateTime mddate;
    int perm;
    bool isAdded;
    bool isDeleted;
    bool isChanged;
    int internalId;
    inline TQNote() : isAdded(false), isDeleted(false), isChanged(false) {}
//    TrkNote(const TrkNote &src);
};

typedef QList<TQNote> TQNotesCol;


struct TQAttachedFile
{
    QString fileName;
    QDateTime createDateTime;
    bool isValid()
    {
        return fileName.isEmpty();
    }
    bool isAdded;
    bool isDeleted;
    bool isChanged;
    int id;
    QVariant data;
    int size;
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
class QDialog;

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
    virtual void setConnectString(const QString &connectString);
    virtual QWidget *createConnectWidget() const;
    virtual QDialog *createConnectDialog() const;
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

class TQScopeSettings
{
protected:
    QSettings *set;
public:
    TQScopeSettings(QSettings *set = 0)
    {
        this->set = set;
    }
    TQScopeSettings(TQScopeSettings *src)
    {
        this->set = src->take();
    }
    ~TQScopeSettings()
    {
        if(set)
            delete set;
    }
    const TQScopeSettings &operator =(QSettings *srcSet)
    {
        if(set)
        {
            delete set;
            set = 0;
        }
        set = srcSet;
    }

    QSettings *take()
    {
        QSettings *res = set;
        set = 0;
        return res;
    }

    QSettings * operator ->()
    {
        return set;
    }
};


class TQQueryDef;
class TQAbstractQWController;

class TQAbstractProjectPrivate;


#define TQOPTION_VIEW_TEMPLATE "ViewTemplate"
#define TQOPTION_EDIT_TEMPLATE "EditTemplate"
#define TQOPTION_PRINT_TEMPLATE "PrintTemplate"
#define TQOPTION_EMAIL_TEMPLATE "EmailTemplate"
#define TQOPTION_EMAIL_SCRIPT "EmailScript"
#define TQOPTION_GROUP_FILE "GroupFile"
#define TQOPTION_GROUP_XML "GroupXML"

class TQAbstractRecordTypeDef;

class TQPLUGIN_SHARED TQAbstractProject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *db READ db)
    Q_PROPERTY(bool isOpened READ isOpened)
    Q_PROPERTY(QString currentUser READ currentUser)
    Q_PROPERTY(QString projectName READ projectName)
protected:
    TQAbstractProjectPrivate *d;
public:
    TQAbstractProject(TQAbstractDB *db);
    ~TQAbstractProject();
    // Standard manipulations
    TQAbstractDB *db() const;
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
    Q_INVOKABLE virtual TQRecord *createRecordById(int id, int rectype) = 0;
    Q_INVOKABLE virtual TQRecord *newRecord(int rectype) = 0;
    Q_INVOKABLE virtual TQAbstractRecordTypeDef *recordTypeDef(int rectype) = 0;
    virtual QDomDocument recordTypeDefDoc(int rectype) = 0;

    virtual TQRecModel *openQueryModel(const QString &queryName, int recType, bool emitEvent = true) = 0;
    virtual bool renameQuery(const QString &oldName, const QString &newName, int recordType);
    virtual bool deleteQuery(const QString &queryName, int recordType);
    Q_INVOKABLE virtual TQRecModel *openIdsModel(const IntList &ids, int recType, bool emitEvent = true) = 0;
    Q_INVOKABLE virtual TQRecModel *openRecords(const QString &queryText, int recType, bool emitEvent = true) = 0;
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
    //virtual bool insertRecordBegin(TQRecord *record) = 0;
    Q_INVOKABLE virtual bool updateRecordBegin(TQRecord *record) = 0;
    Q_INVOKABLE virtual bool commitRecord(TQRecord *record) = 0;
    Q_INVOKABLE virtual bool cancelRecord(TQRecord *record) = 0;
    Q_INVOKABLE virtual QList<TQAttachedFile> attachedFiles(TQRecord *record) = 0;
    Q_INVOKABLE virtual bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest) = 0;
    Q_INVOKABLE virtual int attachFileToRecord(TQRecord *record, const QString &filePath) = 0;
    Q_INVOKABLE virtual bool removeFileFromRecord(TQRecord *record, int fileIndex) = 0;
    Q_INVOKABLE virtual QStringList historyList(TQRecord *record) = 0;
    Q_INVOKABLE virtual QHash<int,QString> baseRecordFields(int rectype) = 0;
    virtual bool isSystemModel(QAbstractItemModel *model) const = 0;
    virtual TQQueryDef *queryDefinition(const QString &queryName, int rectype) = 0;
    virtual TQQueryDef *createQueryDefinition(int rectype) = 0;
    virtual TQAbstractQWController *queryWidgetController(int rectype) = 0;
    virtual bool saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype) = 0;
    Q_INVOKABLE virtual QStringList userNames() = 0;
    virtual QMap<QString, TQUser> userList() = 0;
    Q_INVOKABLE virtual QString userFullName(const QString &login) = 0;
    Q_INVOKABLE virtual QString userFullName(int userId) = 0;
    Q_INVOKABLE virtual QString userLogin(int userId) = 0;
    virtual int userId(const QString &login) = 0;
    virtual TQGroupList userGroups() = 0;
    virtual QSettings *projectSettings() const;
    Q_INVOKABLE virtual QVariant optionValue(const QString &option) const;
    Q_INVOKABLE virtual void setOptionValue(const QString &option, const QVariant &value);
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
    Q_PROPERTY(QString projectName READ projectName)
protected:
    QString name;
    bool opened;
    QMap<int, QString> recordTypes;
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
    virtual QList<TQAttachedFile> attachedFiles(TQRecord *record);
    virtual bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest);
    virtual int attachFileToRecord(TQRecord *record, const QString &filePath);
    virtual bool removeFileFromRecord(TQRecord *record, int fileIndex);
    virtual TQQueryDef *queryDefinition(const QString &queryName, int rectype);
    virtual TQQueryDef *createQueryDefinition(int rectype);
    virtual TQAbstractQWController *queryWidgetController(int rectype);
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

typedef QHash<int, QVariant> TQFieldValues;
Q_DECLARE_METATYPE(TQFieldValues)

class TQRecordPrivate;

class TQPLUGIN_SHARED TQRecord: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY changedState)
    Q_PROPERTY(bool isEditing READ isEditing)
    Q_PROPERTY(bool isModified READ isModified WRITE setModified)
    Q_PROPERTY(bool isValid READ isValid)
    Q_PROPERTY(bool isSelected READ isSelected WRITE setSelected)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QObject *project READ project)
    Q_PROPERTY(const TQAbstractRecordTypeDef *typeDef READ typeDef)
    Q_PROPERTY(const TQAbstractRecordTypeDef *typeEditDef READ typeEditDef)
    Q_PROPERTY(int recordType READ recordType)
    Q_PROPERTY(int recordId READ recordId)
    Q_PROPERTY(QVariantHash values READ values WRITE setValues)
    Q_PROPERTY(int noteCount READ noteCount)
public:
    enum TQRecMode {View, Edit, Insert};
private:
    TQRecordPrivate *d;
public:
    TQRecord();
    TQRecord(TQAbstractProject *prj, int rtype, int id);
    TQRecord(const TQRecord &src);
//    TQRecord & operator = (const TQRecord &src);
    virtual ~TQRecord();
    virtual bool isValid() const;
    virtual int recordType() const;
    virtual int recordId() const;

    virtual int mode() const;
    virtual void setMode(int newMode);
    virtual bool isEditing() const;

public slots:
    virtual bool updateBegin();
    virtual bool commit();
    virtual bool cancel();
    virtual void setModified(bool value);

public:
    virtual bool isModified() const;

public:
    virtual QString title() const;
    virtual bool setTitle(const QString &newTitle);

    virtual QString description() const ;
    virtual bool setDescription(const QString &newDesc);

    virtual TQNotesCol notes() const;
    int noteCount() const;
    Q_INVOKABLE virtual QString noteTitle(int index) const;
    Q_INVOKABLE virtual QString noteText(int index) const;
    Q_INVOKABLE virtual bool setNoteTitle(int index, const QString &newTitle);
    Q_INVOKABLE virtual bool setNoteText(int index, const QString &newText);
    Q_INVOKABLE virtual bool setNote(int index, const QString &newTitle, const QString &newText);
    Q_INVOKABLE virtual int addNote(const QString &noteTitle, const QString &noteText); // return note index base 0 or -1
    Q_INVOKABLE virtual bool removeNote(int index);

    virtual QList<TQAttachedFile> fileList();
    Q_INVOKABLE virtual bool saveFile(int fileIndex, const QString &dest);
    Q_INVOKABLE virtual bool hasFiles();
    Q_INVOKABLE virtual int appendFile(const QString &filePath); // return new file index or -1
    Q_INVOKABLE virtual bool removeFile(int fileIndex);


    virtual TQAbstractProject *project() const;
    virtual const TQAbstractRecordTypeDef *typeDef() const;
    virtual const TQAbstractRecordTypeDef *typeEditDef() const;

    Q_INVOKABLE virtual QVariant value(int vid, int role = Qt::DisplayRole) const ;
    Q_INVOKABLE virtual QVariant value(const QString& fieldName, int role = Qt::DisplayRole) const;
    Q_INVOKABLE virtual bool setValue(int vid, const QVariant& newValue);
    Q_INVOKABLE virtual bool setValues(const QVariantHash &values);
    virtual QVariantHash values() const;
    Q_INVOKABLE virtual bool seVidValues(const TQFieldValues &values);
    Q_INVOKABLE virtual TQFieldValues vidChanges() const;
    Q_INVOKABLE virtual QVariantHash fieldChanges() const;

//    Q_INVOKABLE  int recordId() const { return values[VID_Id].toInt(); } // TRK_UINT
//    Q_INVOKABLE void setValue(const QString& fieldName, const QVariant& value, int role = Qt::EditRole);

    Q_INVOKABLE virtual QDomDocument toXML() const;
    Q_INVOKABLE virtual QString toJSON();
//    Q_INVOKABLE virtual QString toHTML(const QString &xqCodeFile);
    Q_INVOKABLE virtual QStringList historyList() const;
    Q_INVOKABLE bool isFieldReadOnly(const QString &field) const;

    Q_INVOKABLE virtual void addLink();
    Q_INVOKABLE virtual void removeLink(const QObject *receiver=0);
    virtual bool isSelected() const;
    virtual void setSelected(bool value);
    Q_INVOKABLE virtual void refresh();
protected:
    virtual void doSetMode(int newMode) const;
    virtual void doSetRecordId(int id) const;
signals:
    void changedState(int newMode) const;
    void changed(int recId) const;

};

Q_DECLARE_METATYPE(TQAbstractDB*)
Q_DECLARE_METATYPE(TQAbstractProject*)
Q_DECLARE_METATYPE(TQAbstractRecordTypeDef*)
Q_DECLARE_METATYPE(TQRecord)
Q_DECLARE_METATYPE(TQRecord *)


#endif // TQBASE_H
