#ifndef JIRADB_H
#define JIRADB_H

#include <QtCore>

#include <tqbase.h>
#include <tqplug.h>
#include <tqmodels.h>

#include "jira_global.h"
#include <QtNetwork>

class QNetworkReply;

class QMainWindow;

class JiraPlugin : public QObject
{
    Q_OBJECT
public:
    QSettings *settings;
    QObject *globalObject;
    QMainWindow *mainWindow;
    QString pluginModule;
    QDir pluginDir;
    QDir dataDir;
    QStringList servers;
    QString keyFile;
    QString keyPass;

    JiraPlugin(QObject *parent = 0);
    Q_INVOKABLE void initPlugin(QObject *obj, const QString &modulePath);
//    Q_INVOKABLE QWidget * getPropWidget(QWidget *parentWidget = 0);
    //Q_INVOKABLE void setSettings(QSettings *settings);
    Q_INVOKABLE bool saveSettings();
    Q_INVOKABLE bool loadSettings();
    static JiraPlugin *plugin();
signals:
    void error(const QString &pluginName,const QString &errorText);
};

class QNetworkAccessManager;

struct JiraProjectInfo
{
    QString self; //"http://www.example.com/jira/rest/api/2/project/EX",
    int id; //: "10000",
    QString key; // "key": "EX",
    QString name; //"name": "Example",
//    "avatarUrls": {
//        "16x16": "http://www.example.com/jira/secure/projectavatar?size=small&pid=10000",
//        "48x48": "http://www.example.com/jira/secure/projectavatar?size=large&pid=10000"
//    }

};

typedef QList<JiraProjectInfo> JiraPrjInfoList;

class WebForm;
class TQOAuth;
class TQJson;

class JiraDBPrivate;
class QNetworkRequest;

class /*JIRASHARED_EXPORT*/ JiraDB: public TQAbstractDB
{
    Q_OBJECT
public:
    enum JiraConnectMethod {
        BaseAuth = 0,
        CookieAuth,
        OAuth
    };
private:
    JiraDBPrivate *d;
protected:
    QNetworkAccessManager *man;
//    QList<QObject *> readyReplies;
    QMap<QString, JiraPrjInfoList> projectInfo; // by dbmsType
    WebForm *webForm;
    TQOAuth *oa;
    TQJson *parser;
    JiraConnectMethod connectMethod;
    int timeOutSecs;
public:
    JiraDB(QObject *parent = 0);
    ~JiraDB();
    virtual QStringList dbmsTypes();
    virtual QStringList projects(const QString &dbmsType,
                                 const QString &user = QString(),
                                 const QString &pass = QString());
    virtual TQAbstractProject *openProject(
            const QString &projectName,
            const QString &user = QString(),
            const QString &pass = QString()
            );
    bool isAuthorized();
    void setConnectMethod(JiraConnectMethod method);
    virtual TQAbstractProject *openConnection(const QString &connectString);
    void setConnectString(const QString &connectString);
    QVariant sendRequest(const QString &method, const QUrl &url, QVariantMap bodyMap = QVariantMap());
    Q_INVOKABLE QVariant get(const QString &query, QVariantMap bodyMap = QVariantMap());
    Q_INVOKABLE QVariant post(const QString &query, QVariantMap bodyMap = QVariantMap());
    Q_INVOKABLE QVariant put(const QString &query, QVariantMap bodyMap = QVariantMap());
    Q_INVOKABLE QVariant del(const QString &query, QVariantMap bodyMap = QVariantMap());
    QUrl queryUrl(const QString &query) const;
    QNetworkReply *sendRequestNative(const QUrl &url, const QString &method, const QByteArray &body = QByteArray());
    Q_INVOKABLE int lastHTTPCode() const;
    Q_INVOKABLE QString lastHTTPErrorString() const;
    QVariant parseValue(const QVariant &source, const QString &path);
    static TQAbstractDB *createJiraDB(QObject *parent);
    Q_INVOKABLE QVariant sendSimpleRequest(const QString &dbmsType, const QString &method, const QString &query, const QString &body = QString());
    QDialog *createConnectDialog() const;
    bool oauthLogin();
    QNetworkReply *sendWait(const QString &method, QNetworkRequest &request, const QByteArray &body = QByteArray());
    QNetworkReply *sendWaitOp(QNetworkAccessManager::Operation op, QNetworkRequest &request, const QByteArray &body  = QByteArray());
    TQJson *jsonParser();
protected:
    QList<QNetworkReply*> readyReplies;
    QList<QNetworkCookie> cookies;
    bool waitReply(QNetworkReply *reply);
    JiraPrjInfoList getProjectList();
    bool showLoginPage(const QString &server,
                     const QString &user = QString(),
                     const QString &pass = QString());
    bool loginCookie(const QString &user = QString(),
                     const QString &pass = QString());
    void dumpRequest(QNetworkRequest *req, const QString &method, const QByteArray &body);
    void dumpReply(QNetworkReply *reply, const QByteArray &buf);
protected slots:
    void	replyFinished(QNetworkReply * reply);
    void callbackClicked();
private:

};

class JiraRecTypeDef;
class JiraRecord;
struct JiraFilter {
    QString name;
    QString jql;
    bool isSystem;
    bool isServerStored;
};

class JiraFilterModel;

struct JiraUser
{
    QString key;
    QString name;
    QString displayName;
    QString email;
    bool isActive;
};

class JiraProject: public TQBaseProject
{
    Q_OBJECT
protected:
    JiraDB *db;
//    QString dbmsServer;
    QMap<int,JiraRecTypeDef *> recordDefs;
    QString projectKey;
    int projectId;
    QMap<QString, int> favSearch;
    JiraFilterModel *filters;
    QMap<QString,JiraUser> knownUsers; // by name
public:
    JiraProject(TQAbstractDB *db);
    void loadDefinition();
    Q_INVOKABLE JiraDB *jiraDb() const;
    //Redefine
    TQAbstractRecordTypeDef *recordTypeDef(int recordType);
    int defaultRecType() const;
    TQRecModel *openQueryModel(const QString &queryName, int recType, bool emitEvent = true);
    TQRecModel *openIdsModel(const IntList &ids, int recType, bool emitEvent = true);
    TQRecModel *openRecords(const QString &queryText, int recType, bool emitEvent = true);
    TQRecord *createRecordById(int id, int rectype);
    void refreshModel(QAbstractItemModel *model);
    TQRecord *recordOfIndex(const QModelIndex &index);
    QList<int> getQueryIds(const QString &name, int type, qint64 afterTransId = 0);
    bool readRecordWhole(TQRecord *record);
    bool readRecordFields(TQRecord *record);
    bool readRecordTexts(TQRecord *record);
    bool readRecordBase(TQRecord *record);
    Q_INVOKABLE bool updateRecordBegin(TQRecord *record);
    Q_INVOKABLE bool commitRecord(TQRecord *record);
    Q_INVOKABLE bool cancelRecord(TQRecord *record);
    Q_INVOKABLE TQRecord *newRecord(int rectype);
    QList<TQAttachedFile> attachedFiles(TQRecord *record);
    Q_INVOKABLE bool saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest);
    Q_INVOKABLE QStringList historyList(TQRecord *record);
    QHash<int,QString> baseRecordFields(int rectype);
    bool isSystemModel(QAbstractItemModel *model) const;
    QSettings *projectSettings() const;
    QAbstractItemModel *queryModel(int type);
    TQQueryDef *queryDefinition(const QString &queryName, int rectype);
    TQAbstractQWController *queryWidgetController(int rectype);

    Q_INVOKABLE QVariant optionValue(const QString &option) const;
    Q_INVOKABLE QString jiraProjectKey() const;
protected:
    QVariantList fieldList;
    QVariantList typesList;
    bool doCommitUpdateRecord(TQRecord *record);
    void doParseComments(JiraRecord *rec, const QVariantMap &issue);
//    TQAbstractRecordTypeDef *loadRecordTypeDef(int recordType);
    void readRecordDef(JiraRecTypeDef *rdef, int recordType, const QVariantMap &typeMap);
    void readRecordDef2(JiraRecTypeDef *rdef, const QVariantMap &fieldsMap);
    void loadRecordTypes();
    JiraRecTypeDef *loadEditRecordDef(const JiraRecord *record);
    TQChoiceList loadChoiceTables(JiraRecTypeDef *rdef, const QString &url);
    void loadQueries();
    void loadUsers();
    void storeReadedField(JiraRecord *rec, const JiraRecTypeDef *rdef, const QString &fid, const QVariant &value);
//    void appendUserToKnown(const TQUser &user);
public slots:
    void appendUserToKnown(const QVariantMap &userRec);
protected slots:
    void showSelectUser();

    friend class JiraDB;
    friend class JiraRecTypeDef;
    friend class JiraRecord;
};

Q_DECLARE_METATYPE(JiraProject *)

struct JiraFieldDesc
{
    int vid;
    QString id; //: "watches",
    QString name; //: "Watchers",
    bool custom; //: false,
    bool orderable; //: false,
    bool navigable; //: true,
    bool searchable; //: false,
    bool editable;
    QStringList clauseNames;//: ["watchers"],
    QString schemaType; // type: "array",
    QString schemaItems; //: "watches",
    QString schemaSystem; //: "watches"
    int nativeType;
    int simpleType;
    bool createRequired;
    bool createShow;
    QString choiceTable;
    TQChoiceList choices;
    QString autoCompleteUrl;

    QVariant defaultValue, minValue, maxValue;

    inline bool isValid() const
    {
        return !id.isEmpty();
    }
    inline bool isChoice() const
    {
        return simpleType == TQ::TQ_FIELD_TYPE_CHOICE
                || simpleType == TQ::TQ_FIELD_TYPE_USER;
    }
    inline bool isUser() const
    {
        return simpleType == TQ::TQ_FIELD_TYPE_USER;
    }
    inline bool isArray() const
    {
        return schemaType == "array";
    }
    inline bool isOption() const
    {
        return schemaType == "option";
    }
};

struct JiraRecTypeDefPrivate;

class JiraRecTypeDef : public TQBaseRecordTypeDef
{
private:
    JiraRecTypeDefPrivate *d;
    /*
protected:
    JiraProject *prj;
    int recType;
    QString recTypeName;
    QMap<int, JiraFieldDesc> fields;
    QMap<QString, int> systemNames;
    QMap<QString, int> vids;
    QMap<int, QString> roleFields;
    QStringList systemChoices;
//    QHash<int,int> fIndexByVid;
    QStringList schemaTypes;
    QMap<QString, int> nativeTypes; // schema type to native type
    QMap<QString, int> schemaToSimple;
    int idVid, descVid, summaryVid, assigneeVid, creatorVid, createdVid;
//    QMap<QString, JiraUser> knownUsers; // by name
    */

public:
    JiraRecTypeDef(JiraProject *project);
    JiraRecTypeDef(JiraRecTypeDef *src);
    virtual ~JiraRecTypeDef();
    virtual QStringList fieldNames() const;
    virtual TQAbstractFieldType getFieldType(int vid, bool *ok = 0) const;
    virtual TQAbstractFieldType getFieldType(const QString &name, bool *ok = 0) const;
    QString fieldSchemaType(int vid) const;
    virtual int fieldNativeType(int vid) const;
    virtual int fieldSimpleType(int vid) const;
    virtual bool canFieldSubmit(int vid) const;
    virtual bool canFieldUpdate(int vid) const;
    virtual bool isNullable(int vid) const;
    virtual bool hasChoiceList(int vid) const;
    //virtual ChoiceList choiceList(int vid) const = 0;
    //virtual ChoiceList choiceList(const QString &fieldName) const = 0;
    virtual TQChoiceList choiceTable(const QString &tableName) const;
    virtual bool containFieldVid(int vid) const;
    virtual int fieldVid(const QString &name) const;
    int fieldVidSystem(const QString &systemName) const;
    virtual QList<int> fieldVids() const;
    virtual QString fieldName(int vid) const;
    virtual QString fieldSystemName(int vid) const;
    virtual int fieldRole(int vid) const;
    virtual QIODevice *defineSource() const;
    virtual int recordType() const;
    virtual QString valueToDisplay(int vid, const QVariant &value) const;
    virtual QVariant displayToValue(int vid, const QString &text) const;
    virtual QVariant fieldDefaultValue(int vid) const;
    virtual QVariant fieldMinValue(int vid) const;
    virtual QVariant fieldMaxValue(int vid) const;
    virtual QString fieldChoiceTable(int vid) const;
    virtual int roleVid(int role) const;
//    virtual QString dateTimeFormat() const;
    virtual QStringList noteTitleList() const;
    virtual TQAbstractProject *project() const;
    bool hasFieldCustomEditor(int vid) const;
    QWidget *createCustomEditor(int vid, QWidget *parent) const;
    const JiraFieldDesc *fieldDesc(int vid) const;
    QString typeName() const;


    friend class JiraProject;
    friend class JiraRecord;
protected:
    int schemaToSimpleType(const QString &schemaType);
};

Q_DECLARE_METATYPE(JiraRecTypeDef*)

/*
class JiraFieldTypeDef: public TQAbstractFieldType
{
protected:
    JiraFieldTypeDef(JiraRecTypeDef *recordDef, int vid)
        : TQAbstractFieldType()
    {

    }
};
*/

class JiraRecordPrivate;

class JiraRecord: public TQRecord
{
    Q_OBJECT
    Q_PROPERTY(QString key READ jiraKey)
    Q_PROPERTY(int internalId READ recordInternalId)
protected:
    JiraRecordPrivate *d;
    QMap<int, QVariant> values;
    QMap<int, QVariant> displayValues;
    TQNotesCol notesCol;
    QString desc;
    QString key;
    int internalId;
    QList<TQAttachedFile> files;
    bool isFieldsReaded, isTextsReaded;
public:
    JiraRecord();
    JiraRecord(TQAbstractProject *prj, int rtype, int id);
    JiraRecord(const TQRecord &src);
    ~JiraRecord();
    Q_INVOKABLE QString jiraKey() const;
    Q_INVOKABLE int recordId() const;
    Q_INVOKABLE int recordInternalId() const;
    Q_INVOKABLE QVariant value(int vid, int role = Qt::DisplayRole) const ;
    Q_INVOKABLE bool setValue(int vid, const QVariant &newValue);
    TQNotesCol notes() const;
    Q_INVOKABLE bool setNoteTitle(int index, const QString &newTitle);
    Q_INVOKABLE bool setNoteText(int index, const QString &newText);
    Q_INVOKABLE bool setNote(int index, const QString &newTitle, const QString &newText);
    Q_INVOKABLE int addNote(const QString &noteTitle, const QString &noteText);
    Q_INVOKABLE bool removeNote(int index);
    Q_INVOKABLE const TQAbstractRecordTypeDef *typeDef() const;
    Q_INVOKABLE const TQAbstractRecordTypeDef *typeEditDef() const;
    Q_INVOKABLE JiraProject *jiraProject()const;

    friend class JiraProject;
};

Q_DECLARE_METATYPE(JiraRecord *)

class JiraFilterModel: public BaseRecModel<JiraFilter>
{
    Q_OBJECT
public:
    JiraFilterModel(QObject *parent = 0);
//    void appendFilter(const QString &filterName, bool isServerStored=false, int rectype = TRK_SCR_TYPE);
//    void appendFilter(const QStringList &queryList, bool isPublic=false, TRK_UINT rectype = TRK_SCR_TYPE);
//    void removeFilter(const QString &queryName, TRK_UINT rectype = TRK_SCR_TYPE);
protected:
    virtual QVariant displayColData(const JiraFilter &rec, int col) const;
};

#endif // JIRADB_H
