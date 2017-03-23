#include "jiradb.h"
#include "jirarecord.h"
#include <tqdebug.h>


// ====================== Jira Record ===================
class JiraRecordPrivate
{
public:
    JiraRecTypeDef *def, *editDef;
    QHash<int, bool> readedFields;

    JiraRecordPrivate()
    {
        def = editDef = 0;
    }
    ~JiraRecordPrivate()
    {
        if(editDef)
        {
            delete editDef;
            editDef = 0;
        }
    }
};

JiraRecord::JiraRecord()
    : TQRecord(), d(new JiraRecordPrivate())
{
    isFieldsReaded = false;
    isTextsReaded = false;
}

JiraRecord::JiraRecord(TQAbstractProject *prj, int rtype, int id)
    : TQRecord(prj, rtype, id), d(new JiraRecordPrivate())
{
    isFieldsReaded = false;
    isTextsReaded = false;
    if(prj)
        d->def = dynamic_cast<JiraRecTypeDef*>(prj->recordTypeDef(rtype));
}

JiraRecord::JiraRecord(const TQRecord &src)
    : TQRecord(src), d(new JiraRecordPrivate())
{
    isFieldsReaded = false;
    isTextsReaded = false;
    if(project())
        d->def = dynamic_cast<JiraRecTypeDef*>(project()->recordTypeDef(recordType()));
}

JiraRecord::~JiraRecord()
{
    delete d;
}

QString JiraRecord::jiraKey() const
{
    return key;
}

int JiraRecord::recordId() const
{
    return TQRecord::recordId();
}

int JiraRecord::recordInternalId() const
{
    return internalId;
}

QVariant JiraRecord::value(int vid, int role) const
{
    if(d->def && vid == d->def->idVid())
    {
        switch(role)
        {
        case Qt::DisplayRole:
            //return QString("%1-%2").arg(def->prj->projectKey).arg(recordId());
            return jiraKey();
        case Qt::EditRole:
            return recordId();
        }
        return QVariant();
    }
#ifdef QT_DEBUG
    QString fname = d->def->fieldName(vid);
#endif
    if(!d->readedFields.value(vid, false) && !isFieldsReaded)
    {
        TQRecord *rec = const_cast<JiraRecord *>(this);
        project()->readRecordFields(rec);
    }
    if(role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
    if(d->def && vid == d->def->descVid())
    {
        if(!isTextsReaded)
        {
            TQRecord *rec = const_cast<JiraRecord *>(this);
            project()->readRecordTexts(rec);
        }
        return desc;
    }
    QVariant value;
    switch(role)
    {
    case Qt::DisplayRole:
        value = displayValues.value(vid, QVariant());
        break;
    case Qt::EditRole:
        value = values.value(vid, QVariant());
    }
#ifdef QT_DEBUG
    QString s = value.toString();
#endif

    return value;
}

bool JiraRecord::setValue(int vid, const QVariant &newValue)
{
    if(!d->def)
        return false;
    if(vid == d->def->idVid())
        return false;
    if(vid == d->def->descVid())
    {
        desc = newValue.toString();
        TQRecord::setValue(vid, desc);
        setModified(true);
        return true;
    }
    const JiraFieldDesc *fdesc  = d->def->fieldDesc(vid);
    if(!fdesc)
        return false;
    if(fdesc->schemaSystem == "timetracking")
    {

    }
    bool res = TQRecord::setValue(vid, newValue);
    if(res)
    {
        values.insert(vid, newValue);
        displayValues.insert(vid, d->def->valueToDisplay(vid, newValue));
        setModified(true);
    }
    return true;
}

TQNotesCol JiraRecord::notes() const
{
    if(!isTextsReaded)
    {
        JiraRecord *rec = const_cast<JiraRecord *>(this);
        project()->readRecordTexts(rec);
    }
    return notesCol;
}

bool JiraRecord::setNoteTitle(int index, const QString &newTitle)
{
    if(!isEditing())
        return false;
    if(index<0 || index >=notesCol.size())
        return false;
    notesCol[index].title = newTitle;
    notesCol[index].isChanged = true;
    setModified(true);
    return true;
}

bool JiraRecord::setNoteText(int index, const QString &newText)
{
    if(!isEditing())
        return false;
    if(index<0 || index >=notesCol.size())
        return false;
    notesCol[index].text = newText;
    notesCol[index].isChanged = true;
    setModified(true);
    return true;
}

bool JiraRecord::setNote(int index, const QString &newTitle, const QString &newText)
{
    if(!isEditing())
        return false;
    if(index<0 || index >=notesCol.size())
        return false;
    notesCol[index].title = newTitle;
    notesCol[index].text = newText;
    notesCol[index].isChanged = true;
    setModified(true);
    return true;
}

const TQAbstractRecordTypeDef *JiraRecord::typeDef() const
{
    return d->def;
}

const TQAbstractRecordTypeDef *JiraRecord::typeEditDef() const
{
    if(!d->editDef)
    {
        d->editDef = jiraProject()->loadEditRecordDef(this);
    }
    return d->editDef;
}

JiraProject *JiraRecord::jiraProject() const
{
    return qobject_cast<JiraProject *>(project());
}

QVariantList JiraRecord::historyList() const
{
    return historyArray;
}

QVariantList JiraRecord::recordLinks() const
{
    return issueLinks;
}

int JiraRecord::appendFile(const QString &filePath)
{
    QString tempFile = QDir(jiraProject()->jiraDb()->tempFolder()).absoluteFilePath(QUuid::createUuid().toString());
    TQAttachedFile f;
    f.fileName = filePath;
    f.data = tempFile;
    f.isAdded = true;
    if(!QFile::copy(filePath, tempFile))
        return -1;
    files.append(f);
    return files.size()-1;
}

bool JiraRecord::removeFile(int fileIndex)
{
    if(fileIndex<0 || fileIndex>=files.size())
        return false;
    files[fileIndex].isDeleted = true;
    return true;
}

bool JiraRecord::commit()
{
    for(QMutableListIterator<TQAttachedFile> i(files); i.hasNext();)
    {
        TQAttachedFile &f = i.next();
        if(f.isAdded)
        {
            QString tempPath = f.data.toString();
            QFile file(tempPath);
            if(file.open(QFile::ReadOnly))
            {
                QFileInfo info(f.fileName);
                QVariant res = jiraProject()->postFile(this, info.fileName(), &file);
                QVariantList list = res.toList();
                QVariantMap map = list.value(0).toMap();
                if(!list.isEmpty() && !map.isEmpty())
                {
                    f.fileName = map.value("filename").toString();
                    f.createDateTime = map.value("created").toDateTime();
                    f.isAdded = false;
                    f.data = map;
                }
            }
        }
        else if(f.isDeleted)
        {
            if(jiraProject()->removeFile(f.data.toMap().value("id").toInt()))
                i.remove();
        }
    }
    bool res = TQRecord::commit();
    return res;
}

int JiraRecord::addNote(const QString &noteTitle, const QString &noteText)
{
    if(mode() == TQRecord::View)
        return -1;
    TQNote note;
    note.isAdded = true;
    note.title = noteTitle;
    note.text = noteText;
    note.crdate = QDateTime::currentDateTime();
    note.mddate = QDateTime::currentDateTime();
    notesCol.append(note);
    setModified(true);
    return notesCol.size()-1;
}

bool JiraRecord::removeNote(int index)
{
    if(index<0 ||index >= notesCol.size())
        return false;
    notesCol[index].isDeleted = true;
    setModified(true);
    return true;
}

void JiraRecord::storeReadedField(const QString &fid, const QVariant &value)
{
    int fvid = d->def->fieldVidSystem(fid);
    if(fvid == TQ::TQ_NO_VID)
    {
        return;
    }
    if(fid == "description")
        desc = value.toString();
    else if(jiraProject()->systemChoices.contains(fid))
    {
        int id = value.toMap().value("id").toInt();
        QString display = value.toMap().value("name").toString();
        values.insert(fvid, id);
        displayValues.insert(fvid, display);
    }
    else if(fid == "timetracking")
    {
        QVariantMap map = value.toMap();
        QString orig = map.value("originalEstimate").toString();
        QString remain = map.value("remainingEstimate").toString();
        values.insert(fvid, value);
        if(!orig.isEmpty())
            displayValues.insert(fvid, orig + " / " + remain);
    }
    else
    {
        const JiraFieldDesc *fdef = d->def->fieldDesc(fvid);
        if(fdef->isUser())
        {
            QVariantMap uMap = value.toMap();
            QString login = uMap.value("name").toString();
            QString displayName = uMap.value("displayName").toString();
            values.insert(fvid, login);
            displayValues.insert(fvid, displayName);
            jiraProject()->appendUserToKnown(uMap);
        }
        else if(value.type() == QVariant::Map)
        {
            QVariantMap vmap = value.toMap();
            int id = vmap.value("id").toInt();
            QString display;
            if(vmap.contains("name"))
                display = vmap.value("name").toString();
            else
                display = vmap.value("value").toString();
            values.insert(fvid, id);
            displayValues.insert(fvid, display);
        }
        else if(fdef->schemaType == "array")
        {
            QVariantList list = value.toList();
            QVariantList idList;
            QStringList displayList;
            foreach(QVariant v, list)
            {
//                if(v.type() == QVariant::Map)
                QVariantMap map = v.toMap();
                int id = map.value("id").toInt();
                QString display = map.value("value").toString();
                idList.append(id);
                displayList.append(display);
            }
            values.insert(fvid, idList);
            displayValues.insert(fvid, displayList.join(", "));
        }
        else
        {
            values.insert(fvid, value);
            QString displayValue = d->def->valueToDisplay(fvid,value);
            displayValues.insert(fvid, displayValue);
        }
    }
    d->readedFields[fvid] = true;
}

void JiraRecord::clearReadedFields()
{
    values.clear();
    displayValues.clear();
    d->readedFields.clear();
}


