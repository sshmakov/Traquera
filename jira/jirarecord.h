#ifndef JIRARECORD_H
#define JIRARECORD_H

#include <tqbase.h>

class JiraProject;
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
    QVariantList historyArray, issueLinks;
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
    QVariantList historyList() const;
    QVariantList recordLinks() const;
    int appendFile(const QString &filePath);
    bool removeFile(int fileIndex);
    bool commit();

public slots:
    void markNeedFields() const;
    void clearNeedFields() const;
protected:
    void storeReadedField(const QString &fid, const QVariant &value);
    void clearReadedFields();
    bool isNeedFields() const;


    friend class JiraProject;
    friend class JiraRecModel;
};

Q_DECLARE_METATYPE(JiraRecord *)

#endif // JIRARECORD_H
