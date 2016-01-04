#include "tqbase.h"
#include "tqjson.h"

QHash<QString, TQAbstractProject *> TQAbstractDB::projectList;

TQAbstractDB::TQAbstractDB(QObject *parent)
    :QObject(parent)
{
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
    if(value)
        selected[recType].insert(id);
    else
        selected[recType].remove(id);
}

void TQBaseProject::clearSelected(int recType)
{
    selected.remove(recType);
}

TQRecModel *TQBaseProject::selectedModel(int recType)
{
    return selectedModels[recType];
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

QList<TQToolFile> TQBaseProject::attachedFiles(TQRecord *record)
{
    Q_UNUSED(record)
    return QList<TQToolFile>();
}

bool TQBaseProject::saveFileFromRecord(TQRecord *record, int fileIndex, const QString &dest)
{
    Q_UNUSED(record)
    Q_UNUSED(fileIndex)
    Q_UNUSED(dest)
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
TQRecord::TQRecord()
    :QObject(0), m_prj(0), recType(0), recMode(TQRecord::View), recId(0), links(0)
{
}


TQRecord::TQRecord(TQAbstractProject *prj, int rtype, int id)
    :QObject(prj), m_prj(prj), recType(rtype), recMode(TQRecord::View), recId(id), links(0)
{
}

TQRecord::TQRecord(const TQRecord &src)
    :QObject(), m_prj(src.m_prj), recType(src.recType), recMode(src.recMode), recId(src.recId), links(0)
{
}

/*
TQRecord &TQRecord::operator =(const TQRecord &src)
{
    recType = src.recType;
    recMode = View;
    return *this;
}
*/

TQRecord::~TQRecord()
{
}

bool TQRecord::isValid() const
{
    return this && m_prj!=0 && (recId > 0 || mode() == Insert);
}

int TQRecord::recordType() const
{
    return recType;
}

int TQRecord::recordId() const
{
    return recId;
}

TQAbstractRecordTypeDef *TQRecord::recordDef() const
{
    return project()->recordTypeDef(recordType());
}

int TQRecord::mode() const
{
    return recMode;
}

void TQRecord::setMode(int newMode)
{
    if(recMode != newMode)
    {
        recMode = newMode;
        emit changedState(recMode);
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
    return project()->commitRecord(this);
}

bool TQRecord::cancel()
{
    if(!isValid())
        return false;
    return project()->cancelRecord(this);
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

QString TQRecord::description()
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
    return 0;
}

QList<TQToolFile> TQRecord::fileList()
{
    TQAbstractProject *p = project();
    if(!p)
        return QList<TQToolFile>();
    return p->attachedFiles(this);
}

bool TQRecord::saveFile(int fileIndex, const QString &dest)
{
    Q_UNUSED(fileIndex)
    Q_UNUSED(dest)
    return false;
}

bool TQRecord::hasFiles()
{
    return fileList().count() != 0;
}

TQAbstractProject *TQRecord::project() const
{
    return m_prj;
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

QVariant TQRecord::value(int vid, int role) const
{
    Q_UNUSED(vid)
    Q_UNUSED(role)
    return QVariant();
}

QVariant TQRecord::value(const QString &fieldName, int role) const
{
    Q_UNUSED(role)
    const TQAbstractRecordTypeDef *def = typeDef();
    if(def)
    {
        int vid = def->fieldVid(fieldName);
        if(vid)
            return value(vid);
    }
    return QVariant();
}

bool TQRecord::setValue(int vid, const QVariant &newValue, int role)
{
    Q_UNUSED(vid)
    Q_UNUSED(newValue)
    Q_UNUSED(role)
    return false;
}

bool TQRecord::setValues(const QHash<QString, QVariant> &values)
{
    const TQAbstractRecordTypeDef *def = typeDef();
    if(!def)
        return false;
    QHash<QString, QVariant>::const_iterator i;
    for (i = values.constBegin(); i != values.constEnd(); ++i)
    {
        int vid = def->fieldVid(i.key());
        if(!vid)
            return false;
        if(!setValue(vid, i.value()))
            return false;
    }
    return true;
}

QDomDocument TQRecord::toXML()
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
        QString fname = def->fieldName(vid);
        QVariant ftext = value(vid,Qt::DisplayRole);
        QVariant fvalue = value(vid,Qt::EditRole);

        QDomElement f = xml.createElement("field");
        f.setAttribute("name", fname);
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
        note.setAttribute("author", project()->userFullName(tn.author));
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

/*
QString TQRecord::toHTML(const QString &xqCodeFile)
{
    QDomDocument xml=toXML();
    QFile xq(xqCodeFile);
    xq.open(QIODevice::ReadOnly);
    QFile trackerXML("data/tracker.xml");
    trackerXML.open(QIODevice::ReadOnly);
    QXmlQuery query;
//#ifdef CLIENT_APP
//    query.setMessageHandler(sysMessager);
//#endif


    QString page;
    //QString src=xml.toString();
    QByteArray ba=xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    query.bindVariable("scrdoc",&buf);
    query.bindVariable("def",&trackerXML);
    query.setQuery(&xq);
    //query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
    query.evaluateTo(&page);
//#ifdef QT_DEBUG
//    QFile testXml("!testEdit.xml");
//    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
//    QTextStream textOut(&testXml);
//    xml.save(textOut,4);

//    QFile testRes("!testResult.html");
//    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
//    //QTextStream textOutHTML(&testRes);
//    testRes.write(page.toLocal8Bit());
//#endif
    return page;
}
*/

void TQRecord::addLink()
{
    links++;
}

void TQRecord::removeLink(const QObject *receiver)
{
    if(!this)
        return;
    if(receiver)
        disconnect(receiver);
    if(--links <= 0)
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
    if(recMode == Insert)
        return;
    m_prj->readRecordWhole(this);
    //valuesReloaded();
}

