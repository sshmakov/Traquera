#include "tqbase.h"
#include "tqjson.h"
#include "ttglobal.h"
#include <tqmodels.h>

QHash<QString, TQAbstractProject *> TQAbstractDB::projectList;

TQAbstractDB::TQAbstractDB(QObject *parent)
    :QObject(parent)
{
    qRegisterMetaType<TQAbstractRecordTypeDef*>("TQAbstractRecordTypeDef*");
    qRegisterMetaType<TQRecord>("TQRecord");
    qRegisterMetaType<TQAbstractProject*>("TQAbstractProject*");
}

void TQAbstractDB::setDbmsType(const QString &dbType)
{
    m_dbType = dbType;
}

QString TQAbstractDB::dbmsType() const
{
    return m_dbType;
}

void TQAbstractDB::setDbmsServer(const QString &server)
{
    m_dbServer = server;
}

QString TQAbstractDB::dbmsServer() const
{
    return m_dbServer;
}

void TQAbstractDB::setDbmsUser(const QString &dbmsUser, const QString &dbmsPass)
{
    m_dbUser = dbmsUser;
    m_dbPass = dbmsPass;
}

QString TQAbstractDB::dbmsUser() const
{
    return m_dbUser;
}

QString TQAbstractDB::dbmsPass() const
{
    return m_dbPass;
}

void TQAbstractDB::setConnectString(const QString &connectString)
{
    QVariantMap params = TQJson().toVariant(connectString).toMap();
    setDbmsType(params.value(DBPARAM_TYPE).toString());
    setDbmsServer(params.value(DBPARAM_SERVER).toString());
    setDbmsUser(params.value(DBPARAM_USER).toString(), params.value(DBPARAM_PASSWORD).toString());
}

QWidget *TQAbstractDB::createConnectWidget() const
{
    return 0;
}

QDialog *TQAbstractDB::createConnectDialog() const
{
    return 0;
}

TQAbstractProject *TQAbstractDB::getProject(const QString &projectName)
{
    return projectList.value(projectName, 0);
}

void TQAbstractDB::registerProject(TQAbstractProject *prj)
{
    projectList.insert(prj->projectName(), prj);
}

void TQAbstractDB::unregisterProject(TQAbstractProject *prj)
{
    projectList.remove(prj->projectName());
}

static QMap<QString, TQAbstractDB::createDbFunc> dbClasses;

bool TQAbstractDB::registerDbClass(const QString &dbClass, createDbFunc func)
{
    if(dbClasses.contains(dbClass))
    {
        if(dbClasses.value(dbClass) == func)
            return true;
        return false;
    }
    dbClasses.insert(dbClass, func);
    return true;
}

bool TQAbstractDB::unregisterDbClass(const QString &dbClass)
{
    if(dbClasses.contains(dbClass))
    {
        dbClasses.remove(dbClass);
        return true;
    }
    return false;
}

QStringList TQAbstractDB::registeredDbClasses()
{
    return dbClasses.keys();
}

TQAbstractDB *TQAbstractDB::createDbClass(const QString &dbClass, QObject *parent)
{
    if(dbClasses.contains(dbClass))
    {
        TQAbstractDB::createDbFunc func = dbClasses[dbClass];
        if(func)
            return func(parent);
    }
    return 0;
}


class TQAbstractProjectPrivate
{
public:
    TQAbstractDB *db;
    TQAbstractProjectPrivate()
        : db(0)
    {}
    ~TQAbstractProjectPrivate()
    {
        db = 0;
    }
};


/* TQAbstractProject */
TQAbstractProject::TQAbstractProject(TQAbstractDB *db)
    :QObject(db), d(new TQAbstractProjectPrivate())
{
    d->db = db;
}

TQAbstractProject::~TQAbstractProject()
{
    delete d;
}

TQAbstractDB *TQAbstractProject::db() const
{
    return d->db;
}

bool TQAbstractProject::renameQuery(const QString &oldName, const QString &newName, int recordType)
{
    Q_UNUSED(oldName)
    Q_UNUSED(newName)
    Q_UNUSED(recordType)
    return false;
}

bool TQAbstractProject::deleteQuery(const QString &queryName, int recordType)
{
    Q_UNUSED(queryName)
    Q_UNUSED(recordType)
    return false;
}

QSettings *TQAbstractProject::projectSettings() const
{
    QSettings *set = new QSettings();
    set->beginGroup("Projects");
    set->beginGroup(QString("%1").arg(projectName()));
    return set;
}

QVariant TQAbstractProject::optionValue(const QString &option) const
{
    QScopedPointer<QSettings>sets(projectSettings());
    if(sets->contains(option))
        return sets->value(option);
    return ttglobal()->optionDefaultValue(option);
}

void TQAbstractProject::setOptionValue(const QString &option, const QVariant &value)
{
    QScopedPointer<QSettings>sets(projectSettings());
    if(sets.isNull())
        return;
    sets->setValue(option, value);
}

/*  TQBaseProject  */
TQBaseProject::TQBaseProject(TQAbstractDB *db)
    :TQAbstractProject(db)
{
    opened = false;
}

bool TQBaseProject::isOpened() const
{
    return opened;
}

QString TQBaseProject::currentUser() const
{
    return user;
}

QString TQBaseProject::projectName() const
{
    return name;
}

QString TQBaseProject::fieldVID2Name(int rectype, int vid)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(rectype);
    if(def)
        return def->fieldName(vid);
    return QString();
}

int TQBaseProject::fieldName2VID(int rectype, const QString &fname)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(rectype);
    if(def)
        return def->fieldVid(fname);
    return 0;
}

int TQBaseProject::fieldNativeType(const QString &name, int recType)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(recType);
    if(def)
        return def->fieldNativeType(def->fieldVid(name));
    return 0;
}


int TQBaseProject::defaultRecType() const
{
    if(recordTypes.isEmpty())
        return -1;
    return recordTypes.keys().first();
}

bool TQBaseProject::isSelectedId(int id, int recType) const
{
    if(!selected.contains(recType))
        return false;
    return selected[recType].contains(id);
}

void TQBaseProject::setSelectedId(int id, bool value, int recType)
{
    if(!selected.contains(recType))
        selected.insert(recType,TQSelectedSet());
    TQRecModel *model = selectedModels.value(recType, 0);
    if(value)
    {
        selected[recType].insert(id);
        if(model)
            model->doAppendRecordIds(QList<int>() << id);
    }
    else
    {
        selected[recType].remove(id);
        if(model)
            model->doRemoveRecordIds(QList<int>() << id);
    }
}

void TQBaseProject::clearSelected(int recType)
{
    selected.remove(recType);
    TQRecModel *model = selectedModels.value(recType, 0);
    if(model)
        model->clearRecords();
}

TQRecModel *TQBaseProject::selectedModel(int recType)
{
    TQRecModel *model = selectedModels.value(recType, 0);
    if(!model)
    {
        TQSelectedSet set = selected.value(recType);
        model = openIdsModel(set.toList(), recType);
        selectedModels.insert(recType, model);
    }
    return model;
}

bool TQBaseProject::canFieldSubmit(int vid, int recType)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(recType);
    if(def)
        def->canFieldSubmit(vid);
    return false;
}

bool TQBaseProject::canFieldUpdate(int vid, int recType)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(recType);
    if(def)
        def->canFieldUpdate(vid);
    return false;
}

QString TQBaseProject::userFullName(const QString &login)
{
    if(!m_userList.contains(login))
        return login;
    return m_userList.value(login).fullName;
}

TQRecord *TQBaseProject::createRecordById(int id, int rectype)
{
    Q_UNUSED(id)
    Q_UNUSED(rectype)
    return 0;
}

TQRecord *TQBaseProject::newRecord(int rectype)
{
    Q_UNUSED(rectype)
    return 0;
}

/*
TQAbstractRecordTypeDef *TQBaseProject::recordTypeDef(int rectype)
{
    return recordDef[rectype];
}
*/

QDomDocument TQBaseProject::recordTypeDefDoc(int recType)
{
    TQAbstractRecordTypeDef* def = recordTypeDef(recType);
    if(!def)
        return QDomDocument();
    QDomDocument doc;
    QDomElement root = doc.createElement("RecordDef");
    root.setAttribute("recordType", recType);
    QDomElement fields = doc.createElement("Fields");
    foreach(int vid, def->fieldVids())
    {
        QDomElement f = doc.createElement("Field");
        f.setAttribute("name",def->fieldName(vid));
        f.setAttribute("type",def->fieldNativeType(vid));
        f.setAttribute("simpleType",def->fieldSimpleType(vid));
        f.setAttribute("minValue",def->fieldMinValue(vid).toString());
        f.setAttribute("maxValue",def->fieldMaxValue(vid).toString());
        if(def->hasChoiceList(vid))
        {
            f.setAttribute("choices",def->fieldChoiceTable(vid));
        }
        fields.appendChild(f);
    }
    root.appendChild(root);
    return doc;
}

void TQBaseProject::initQueryModel(int recType)
{
    Q_UNUSED(recType)
}

QAbstractItemModel *TQBaseProject::queryModel(int type)
{
    Q_UNUSED(type)
    return 0;
}

QList<TQAttachedFile> TQBaseProject::attachedFiles(TQRecord *record)
{
    Q_UNUSED(record)
    return QList<TQAttachedFile>();
}

bool TQBaseProject::saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest)
{
    Q_UNUSED(record)
    Q_UNUSED(fileIndex)
    Q_UNUSED(dest)
    return false;
}

int TQBaseProject::attachFileToRecord(TQRecord *record, const QString &filePath)
{
    Q_UNUSED(record)
    Q_UNUSED(filePath)
    return -1;
}

bool TQBaseProject::removeFileFromRecord(TQRecord *record, int fileIndex)
{
    Q_UNUSED(record)
    Q_UNUSED(fileIndex)
    return false;
}

TQQueryDef *TQBaseProject::queryDefinition(const QString &queryName, int rectype)
{
    Q_UNUSED(queryName)
    Q_UNUSED(rectype)
    return 0;
}

TQQueryDef *TQBaseProject::createQueryDefinition(int rectype)
{
    Q_UNUSED(rectype)
    return 0;
}

TQAbstractQWController *TQBaseProject::queryWidgetController(int rectype)
{
    Q_UNUSED(rectype)
    return 0;
}

bool TQBaseProject::saveQueryDefinition(TQQueryDef *queryDefinition, const QString &queryName, int rectype)
{
    Q_UNUSED(queryDefinition)
    Q_UNUSED(queryName)
    Q_UNUSED(rectype)
    return false;
}

QStringList TQBaseProject::userNames()
{
    QStringList names;
    foreach(const TQUser &user, m_userList)
        names.append(user.fullName);
    return names;
}

QMap<QString, TQUser> TQBaseProject::userList()
{
    return m_userList;
}

QString TQBaseProject::userFullName(int userId)
{
    QString login = userLogin(userId);
    return userFullName(login);
}

QString TQBaseProject::userLogin(int userId)
{
    foreach(const TQUser &user, userList())
        if(user.id == userId)
            return user.login;
    return QString();
}

int TQBaseProject::userId(const QString &login)
{
    QMap<QString, TQUser> list = userList();
    if(list.contains(login))
        return list[login].id;
    return -1;
}

TQGroupList TQBaseProject::userGroups()
{
    return TQGroupList();
}

//========================= TQRecord ==================================
class TQRecordPrivate
{
public:
    TQAbstractProject *m_prj;
    int recType;
    int recMode;
    int recId;
    int links;
    bool modified;
    QHash<int, QVariant> vidChanges;

    TQRecordPrivate() : m_prj(0), recType(0), recMode(TQRecord::View), recId(0), links(0), modified(false) {}
};

TQRecord::TQRecord()
    : QObject(0), d(new TQRecordPrivate())
{
}


TQRecord::TQRecord(TQAbstractProject *prj, int rtype, int id)
    :QObject(prj), d(new TQRecordPrivate())
{
    d->m_prj = prj;
    d->recType = rtype;
    d->recId = id;
}

TQRecord::TQRecord(const TQRecord &src)
    :QObject(), d(new TQRecordPrivate())
{
    d->m_prj = src.d->m_prj;
    d->recType = src.d->recType;
    d->recId = src.d->recId;
}

TQRecord::~TQRecord()
{
    delete d;
}

bool TQRecord::isValid() const
{
    return this && d->m_prj!=0 && (d->recId > 0 || mode() == Insert);
}

int TQRecord::recordType() const
{
    return d->recType;
}

int TQRecord::recordId() const
{
    return d->recId;
}

int TQRecord::mode() const
{
    return d->recMode;
}

void TQRecord::setMode(int newMode)
{
    if(d->recMode != newMode)
    {
        d->recMode = newMode;
        emit changedState(d->recMode);
    }
}

bool TQRecord::isEditing() const
{
    return mode() != TQRecord::View;
}

bool TQRecord::updateBegin()
{
    if(!isValid())
        return false;
    return project()->updateRecordBegin(this);
}

bool TQRecord::commit()
{
    if(!isValid())
        return false;
    bool res = project()->commitRecord(this);
    if(res)
    {
        d->vidChanges.clear();
    }
    return res;
}

bool TQRecord::cancel()
{
    if(!isValid())
        return false;
    return project()->cancelRecord(this);
}

void TQRecord::setModified(bool value)
{
    d->modified = value;
}

bool TQRecord::isModified() const
{
    return d->modified;
}

QString TQRecord::title() const
{
    if(!isValid())
        return QString();
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->roleVid(TQAbstractRecordTypeDef::TitleField);
        if(vid)
            return value(vid).toString();
    }
    return QString();
}

bool TQRecord::setTitle(const QString &newTitle)
{
    if(!isValid())
        return false;
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->roleVid(TQAbstractRecordTypeDef::TitleField);
        if(vid)
            return setValue(vid,newTitle);
    }
    return false;
}

QString TQRecord::description() const
{
    if(!isValid())
        return QString();
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->roleVid(TQAbstractRecordTypeDef::DescriptionField);
        if(vid)
            return value(vid).toString();
    }
    return QString();
}

bool TQRecord::setDescription(const QString &newDesc)
{
    if(!isValid())
        return false;
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->roleVid(TQAbstractRecordTypeDef::DescriptionField);
        if(vid)
            return setValue(vid,newDesc);
    }
    return false;
}

TQNotesCol TQRecord::notes() const
{
    return TQNotesCol();
}

QString TQRecord::noteTitle(int index) const
{
    if(!isValid())
        return QString();
    TQNotesCol list = notes();
    if(index >= 0 && index < list.count())
        return list.value(index).title;
    return QString();
}

QString TQRecord::noteText(int index) const
{
    if(!isValid())
        return QString();
    TQNotesCol list = notes();
    if(index >= 0 && index < list.count())
        return list.value(index).text;
    return QString();
}

bool TQRecord::setNoteTitle(int index, const QString &newTitle)
{
    Q_UNUSED(index)
    Q_UNUSED(newTitle)
    return false;
}

bool TQRecord::setNoteText(int index, const QString &newText)
{
    Q_UNUSED(index)
    Q_UNUSED(newText)
    return false;
}

bool TQRecord::setNote(int index, const QString &newTitle, const QString &newText)
{
    Q_UNUSED(index)
    Q_UNUSED(newTitle)
    Q_UNUSED(newText)
    return false;
}

int TQRecord::addNote(const QString &noteTitle, const QString &noteText)
{
    Q_UNUSED(noteTitle)
    Q_UNUSED(noteText)
    return -1;
}

bool TQRecord::removeNote(int index)
{
    Q_UNUSED(index)
    return false;
}

QList<TQAttachedFile> TQRecord::fileList()
{
    TQAbstractProject *p = project();
    if(!p)
        return QList<TQAttachedFile>();
    return p->attachedFiles(this);
}

bool TQRecord::saveFile(int fileIndex, const QString &dest)
{
    TQAbstractProject *p = project();
    if(!p)
        return false;
    return p->saveFileFromRecord(this, fileIndex, dest);
//    Q_UNUSED(fileIndex)
//    Q_UNUSED(dest)
    return false;
}

bool TQRecord::hasFiles()
{
    return fileList().count() != 0;
}

int TQRecord::appendFile(const QString &filePath)
{
    TQAbstractProject *p = project();
    if(!p)
        return -1;
    return p->attachFileToRecord(this, filePath);
}

bool TQRecord::removeFile(int fileIndex)
{
    TQAbstractProject *p = project();
    if(!p)
        return false;
    return p->removeFileFromRecord(this, fileIndex);
}

TQAbstractProject *TQRecord::project() const
{
    return d->m_prj;
}

const TQAbstractRecordTypeDef *TQRecord::typeDef() const
{
    if(!isValid())
        return 0;
    TQAbstractProject *p = project();
    if(!p)
        return 0;
    return p->recordTypeDef(recordType());
}

const TQAbstractRecordTypeDef *TQRecord::typeEditDef() const
{
    return typeDef();
}

QVariant TQRecord::value(int vid, int role) const
{
    Q_UNUSED(vid)
    Q_UNUSED(role)
    return QVariant();
}

QVariant TQRecord::value(const QString &fieldName, int role) const
{
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->fieldVid(fieldName);
        if(vid == TQ::TQ_NO_VID)
            return QVariant();
        return value(vid, role);
//        switch(role)
//        {
//        case Qt::EditRole:
//            return value(vid);
//        case Qt::DisplayRole:
//            return def->valueToDisplay(vid, value(vid));
//        }
    }
    return QVariant();
}

bool TQRecord::setValue(int vid, const QVariant &newValue)
{
    if(!isEditing())
        return false;
    d->vidChanges[vid] = newValue;
    return true;
}

bool TQRecord::setValues(const QVariantHash &values)
{
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return false;
    TQFieldValues vidValues;
    QVariantHash::const_iterator i;
    for (i = values.constBegin(); i != values.constEnd(); ++i)
    {
        int vid = def->fieldVid(i.key());
        if(vid == TQ::TQ_NO_VID)
            return false;
        vidValues.insert(vid, i.value());
    }
    if(!seVidValues(vidValues))
        return false;
    return true;
}

QVariantHash TQRecord::values() const
{
    QVariantHash hash;
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return hash;
    foreach(const QString &fName, def->fieldNames())
    {
        QVariant v = value(fName);
        hash.insert(fName, v);
    }
    return hash;
}

bool TQRecord::seVidValues(const TQFieldValues &values)
{
    bool res = true;
    TQFieldValues::const_iterator i;
    for (i = values.constBegin(); i != values.constEnd(); ++i)
    {
        res = res && setValue(i.key(), i.value());
    }
    return res;
}

TQFieldValues TQRecord::vidChanges() const
{
    return d->vidChanges;
}

QVariantHash TQRecord::fieldChanges() const
{
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return QVariantHash();
    QVariantHash res;
    TQFieldValues::const_iterator i;
    for(i = d->vidChanges.begin(); i != d->vidChanges.end(); ++i)
    {
        int vid = i.key();
        QVariant value = i.value();
        QString field = def->fieldName(vid);
        res.insert(field, value);
    }
    return res;
}

QDomDocument TQRecord::toXML() const
{
    QDomDocument xml("scr");
    QDomElement root=xml.createElement("scr");
    xml.appendChild(root);
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return xml;
    QDomElement flds =xml.createElement("fields");
    QList<int> vids = def->fieldVids();
    foreach(int vid, vids)
    {
        bool ok = false;

        QString fname = def->fieldName(vid);
        QString sysName = def->fieldSystemName(vid);
        QString role = def->fieldRoleName(vid);
        QVariant ftext = value(vid,Qt::DisplayRole);
        QVariant fvalue = value(vid,Qt::EditRole);

        QDomElement f = xml.createElement("field");
        f.setAttribute("name", fname);
        f.setAttribute("id", sysName);
        if(!role.isEmpty())
            f.setAttribute("role", role);
        f.setAttribute("value",fvalue.toString());
        QDomText v = xml.createTextNode(ftext.toString());
        f.appendChild(v);
        flds.appendChild(f);
    }
    root.appendChild(flds);
    int descVid = def->roleVid(TQAbstractRecordTypeDef::DescriptionField);
    if(descVid)
    {
        QDomElement desc = xml.createElement("Description");
        desc.setAttribute("name", def->fieldName(descVid));
        QDomText v = xml.createTextNode(description());
        desc.appendChild(v);
        root.appendChild(desc);
    }
    // fill <notes>
    QDomElement notes = xml.createElement("notes");

    TQNotesCol notesCol = this->notes();
    int index=0;
    foreach(const TQNote &tn, notesCol)
    {
        QDomElement note = xml.createElement("note");
        //const TrkNote &tn = notesCol.at(i);
        note.setAttribute("title", tn.title);
        QString fullName = tn.authorFullName;
        if(fullName.isEmpty())
            fullName = project()->userFullName(tn.author);
        note.setAttribute("author", fullName);
        note.setAttribute("cdatetime", tn.crdate.toString(Qt::ISODate));
        note.setAttribute("createdate", tn.crdate.toString(def->dateTimeFormat()));
        note.setAttribute("mdatetime", tn.mddate.toString(Qt::ISODate));
        note.setAttribute("modifydate", tn.mddate.toString(def->dateTimeFormat()));
        note.setAttribute("editable", QString(tn.perm?"true":"false"));
        if(tn.isAdded)
            note.setAttribute("isAdded","true");
        if(tn.isChanged)
            note.setAttribute("isChanged","true");
        if(tn.isDeleted)
            note.setAttribute("isDeleted","true");
        note.setAttribute("index",index++);
        QDomText v = xml.createTextNode(tn.text);
        note.appendChild(v);
        notes.appendChild(note);
    }
    //delete col;
    root.appendChild(notes);

    // fill <history>
    QDomElement history = xml.createElement("history");
    index=0;
    foreach(const QString &item, historyList())
    {
        QDomElement change = xml.createElement("change");
        QStringList sections = item.split(QChar(' '));
        QString changeDate = sections[0];
        QString changeTime = sections[1];//item.left(10+1+5);
        QDateTime changeDateTime = QDateTime::fromString(changeDate + " " + changeTime, def->dateTimeFormat());
        QString changeAuthor = sections[2].mid(1,sections[2].length()-3);

        QString changeDesc = QStringList(sections.mid(5)).join(" ");
        change.setAttribute("author", changeAuthor);
        change.setAttribute("datetime", changeDateTime.toString(Qt::ISODate));
        change.setAttribute("createdate", changeDate + " " + changeTime);
        change.setAttribute("action", changeDesc);
        change.setAttribute("index",index++);
        QDomText v = xml.createTextNode(item);
        change.appendChild(v);
        history.appendChild(change);
    }
    root.appendChild(history);
    return xml;
}

static QString v2s(const QVariant &value)
{
    switch(value.type())
    {
    case QVariant::Bool:
        return value.toBool() ? "true" : "false";
    case QVariant::Int:
        return value.toString();
    }
    QString s = value.toString();
    s.replace(QChar('"'),"\\\"");
    return "\"" + s + "\"";
}

QString TQRecord::toJSON()
{
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return QString();
    QString res = "{";
    res += "fields:[";
    QList<int> vids = def->fieldVids();
    int index = 0;
    foreach(int vid, vids)
    {
        QString fname = def->fieldName(vid);
//        QVariant ftext = value(vid,Qt::DisplayRole);
        QVariant fvalue = value(vid,Qt::EditRole);
        if(index++)
            res += ",";
        res += QString("{name:%1, value:%2}").arg(v2s(fname), v2s(fvalue));
    }
    res += "],";
    int descVid = def->roleVid(TQAbstractRecordTypeDef::DescriptionField);
    if(descVid)
    {
        res += QString("Description: %1").arg(v2s(description()));
    }
    // fill <notes>
    res += ", notes:[";
    TQNotesCol notesCol = this->notes();
    index=0;
    foreach(const TQNote &tn, notesCol)
    {
        if(index)
            res += ",";
        QString s = QString("title:%1, author:%2, cdatetime:%3, createdate:%4, mdatetime:%5, modifydate:%6, editable:%7")
                .arg(
                    v2s(tn.title),
                    v2s(project()->userFullName(tn.author)),
                    v2s(tn.crdate.toString(Qt::ISODate)),
                    v2s(tn.crdate.toString(def->dateTimeFormat())),
                    v2s(tn.mddate.toString(Qt::ISODate)),
                    v2s(tn.mddate.toString(def->dateTimeFormat())),
                    QString(tn.perm?"true":"false"));
        s += QString(", isAdded:%1, isChanged:%2, isDeleted:%3, index:%4").arg(
                    v2s(tn.isAdded),
                    v2s(tn.isChanged),
                    v2s(tn.isDeleted),
                    v2s(index++)
                    );
        res += "{" + s + "}";
    }
    res += "]";

    /* // fill <history>
    QDomElement history = xml.createElement("history");
    index=0;
    foreach(const QString &item, historyList())
    {
        QDomElement change = xml.createElement("change");
        QStringList sections = item.split(QChar(' '));
        QString changeDate = sections[0];
        QString changeTime = sections[1];//item.left(10+1+5);
        QDateTime changeDateTime = QDateTime::fromString(changeDate + " " + changeTime, def->dateTimeFormat());
        QString changeAuthor = sections[2].mid(1,sections[2].length()-3);

        QString changeDesc = QStringList(sections.mid(5)).join(" ");
        change.setAttribute("author", changeAuthor);
        change.setAttribute("datetime", changeDateTime.toString(Qt::ISODate));
        change.setAttribute("createdate", changeDate + " " + changeTime);
        change.setAttribute("action", changeDesc);
        change.setAttribute("index",index++);
        QDomText v = xml.createTextNode(item);
        change.appendChild(v);
        history.appendChild(change);
    }
    root.appendChild(history);
    return xml;
    */
    res += "}";
    return res;
}

QStringList TQRecord::historyList() const
{
    return QStringList();
}

bool TQRecord::isFieldReadOnly(const QString &field) const
{
    if(mode() == TQRecord::View)
        return true;
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return true;
    int vid = def->fieldVid(field);
    if(mode() == TQRecord::Edit)
        return !def->canFieldUpdate(vid);
    if(mode() == TQRecord::Insert)
        return !def->canFieldSubmit(vid);
    return true;
}

void TQRecord::addLink()
{
    d->links++;
}

void TQRecord::removeLink(const QObject *receiver)
{
    if(!this)
        return;
    if(receiver)
        disconnect(receiver);
    if(--d->links <= 0)
        deleteLater();
}

bool TQRecord::isSelected() const
{
    if(!isValid())
        return false;
    return project()->isSelectedId(recordId(),recordType());
}

void TQRecord::setSelected(bool value)
{
    if(!isValid())
        return;
    project()->setSelectedId(recordId(), value, recordType());
    emit changed(recordId());
}

void TQRecord::refresh()
{
    if(mode() == Insert)
        return;
    project()->readRecordWhole(this);
}

void TQRecord::doSetMode(int newMode) const
{
    d->recMode = newMode;
}

void TQRecord::doSetRecordId(int id) const
{
    d->recId = id;
}
