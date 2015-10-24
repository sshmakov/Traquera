#ifndef JIRADB_H
#define JIRADB_H

#include <QtCore>

#include <tqbase.h>
#include <tqplug.h>

#include "jira_global.h"

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

    JiraPlugin(QObject *parent = 0);
    Q_INVOKABLE void initPlugin(QObject *obj, const QString &modulePath);
//    Q_INVOKABLE QWidget * getPropWidget(QWidget *parentWidget = 0);
    //Q_INVOKABLE void setSettings(QSettings *settings);
    Q_INVOKABLE bool saveSettings();
    Q_INVOKABLE bool loadSettings();
    static JiraPlugin *plugin();
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

class /*JIRASHARED_EXPORT*/ JiraDB: public TQAbstractDB
{
    Q_OBJECT
protected:
//    QNetworkAccessManager *man;
//    QList<QObject *> readyReplies;
    QMap<QString, JiraPrjInfoList> projectInfo; // by dbmsType
    WebForm *webForm;
    TQOAuth *oa;
    TQJson *parser;
public:
    JiraDB(QObject *parent = 0);
    virtual QStringList dbmsTypes();
    virtual QStringList projects(const QString &dbmsType,
                                 const QString &user = QString(),
                                 const QString &pass = QString());
    virtual TQAbstractProject *openProject(
            const QString &projectName,
            const QString &user = QString(),
            const QString &pass = QString()
            );
    virtual TQAbstractProject *openConnection(const QString &connectString);
    QVariant sendRequest(const QString &dbmsType, const QString &method, const QString &query, const QString &body = QString());
    QVariant parseValue(const QVariant &source, const QString &path);
    static TQAbstractDB *createJiraDB(QObject *parent);
protected:
    JiraPrjInfoList getProjectList(const QString& dbmsType);
protected slots:
//    void	replyFinished(QNetworkReply * reply);
    void callbackClicked();
private:

};

class JiraRecTypeDef;

class JiraProject: public TQBaseProject
{
    Q_OBJECT
protected:
    JiraDB *db;
    QString dbmsType;
    QHash<int,JiraRecTypeDef *> recordDefs;
    QString projectKey;
    int projectId;
    QMap<QString, QString> favSearch;
    QString token;
public:
    JiraProject(TQAbstractDB *db);
    void loadDefinition();
    //Redefine
    TQAbstractRecordTypeDef *recordTypeDef(int recordType);
    TQRecModel *openQueryModel(const QString &queryName, int recType, bool emitEvent = true);
    QAbstractItemModel *openIdsModel(const IntList &ids, int recType, bool emitEvent = true);
    void refreshModel(QAbstractItemModel *model);
    TQRecord *recordOfIndex(const QModelIndex &index);
    QList<int> getQueryIds(const QString &name, int type, qint64 afterTransId = 0);
    bool readRecordWhole(TQRecord *record);
    bool readRecordFields(TQRecord *record);
    bool readRecordTexts(TQRecord *record);
    bool readRecordBase(TQRecord *record);
    QVariant getFieldValue(const TQRecord *record, const QString &fname, bool *ok = 0);
    QVariant getFieldValue(const TQRecord *record, int vid, bool *ok = 0);
    bool setFieldValue(TQRecord *record, const QString &fname, const QVariant &value);
    bool updateRecordBegin(TQRecord *record);
    bool commitRecord(TQRecord *record);
    bool cancelRecord(TQRecord *record);
    QStringList historyList(TQRecord *record);
    QHash<int,QString> baseRecordFields(int rectype);
    bool isSystemModel(QAbstractItemModel *model) const;
protected:
//    TQAbstractRecordTypeDef *loadRecordTypeDef(int recordType);
    void loadRecordTypes();
    void loadQueries();

    friend class JiraDB;
};

struct JiraFieldDesc
{
    int vid;
    QString id; //: "watches",
    QString name; //: "Watchers",
    bool custom; //: false,
    bool orderable; //: false,
    bool navigable; //: true,
    bool searchable; //: false,
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

    QVariant defaultValue, minValue, maxValue;

    bool isValid() const
    {
        return !id.isEmpty();
    }
    bool isChoice() const
    {
        return simpleType == TQ::TQ_FIELD_TYPE_CHOICE
                || simpleType == TQ::TQ_FIELD_TYPE_USER;
    }
    bool isUser() const
    {
        return simpleType == TQ::TQ_FIELD_TYPE_USER;
    }
};

class JiraRecTypeDef : public TQAbstractRecordTypeDef
{
protected:
    JiraProject *prj;
    int recType;
    QString recTypeName;
    QMap<int, JiraFieldDesc> fields;
    QHash<QString, int> systemNames;
    QMap<QString, int> vids;
    QHash<int, QString> roleFields;
//    QHash<int,int> fIndexByVid;
    QStringList schemaTypes;
    QMap<QString, int> nativeTypes; // schema type to native type
    QMap<QString, int> schemaToSimple;
public:
    JiraRecTypeDef(JiraProject *project);
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
    virtual QList<int> fieldVids() const;
    virtual QString fieldName(int vid) const;
    virtual QIODevice *defineSource() const;
    virtual int recordType() const;
    virtual QString valueToDisplay(int vid, const QVariant &value) const;
    virtual QVariant displayToValue(int vid, const QString &text) const;
    virtual QVariant fieldDefaultValue(int vid) const;
    virtual QVariant fieldMinValue(int vid) const;
    virtual QVariant fieldMaxValue(int vid) const;
    virtual QString fieldChoiceTable(int vid) const;
    virtual int roleVid(int role) const;
    virtual QString dateTimeFormat() const;
    virtual QStringList noteTitleList() const;
    virtual TQAbstractProject *project() const;

    friend class JiraProject;
protected:
    int schemaToSimpleType(const QString &schemaType);
};

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

class JiraRecord: public TQRecord
{
    Q_OBJECT
protected:
    QMap<int, QVariant> values;
    QMap<int, QVariant> displayValues;
    TQNotesCol notesCol;
    QString desc;
    QString key;
public:
    JiraRecord();
    JiraRecord(TQAbstractProject *prj, int rtype, int id);
    JiraRecord(const TQRecord &src);
    Q_INVOKABLE QString jiraKey();
    QVariant value(int vid, int role = Qt::DisplayRole) const ;
    TQNotesCol notes() const;


    friend class JiraProject;
};

#endif // JIRADB_H
