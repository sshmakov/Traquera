#ifndef REPLYHDL_H
#define REPLYHDL_H

#include <QtCore>
#include <QtXml>
#include <tqplug.h>

class TQReplyHandler: public QXmlDefaultHandler
{
protected:
    enum Mode {Unk, InFault, InFaultCode, InFaultString, InFaultDetail};
    Mode mode;
public:
    QString faultCode, faultString, faultDetail;
    bool isFault;
    explicit TQReplyHandler();
    virtual bool	startElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    virtual bool endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters ( const QString & ch );

    friend class TQServiceProject;
};


class TQProjectListReplyHandler: public TQReplyHandler
{
public:
    QStringList projects;
    TQProjectListReplyHandler();
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

class TQLoginReplyHandler: public TQReplyHandler
{
public:
    QString sessionID, projectID;
    TQLoginReplyHandler();
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

class TQAbstractProject;

class TQServiceRecordDef: public QObject, public TQAbstractRecordTypeDef
{
    Q_OBJECT
    Q_INTERFACES(TQAbstractRecordTypeDef)
public:
    struct FieldProps {
        int nativeType;
        int simpleType;
        bool canSubmit, canUpdate, nullable;
        bool hasChoice;
        QString chTable;
        QString defValue, minValue, maxValue;
    };
protected:
    TQAbstractProject *m_prj;

    int m_recType;
    QHash<int, QString> m_fields; // vid => name
    QHash<QString, int> m_Vids; // name => vid
    QHash<int, FieldProps> m_props; // vid =>prop
    QHash<QString, TQChoiceList> choices; // tableName =>list
    QByteArray m_defSrc;
    QString m_dateTimeFormat;
    QHash<int, int> m_roleVids; // role =>vid
    QStringList m_noteTitles;


//    QHash<int, TQAbstractFieldType *> m_fieldTypes;
    /*
    bool isReadOnly;
    TrkVidDefs fieldDefs;
    QHash<int, QString> baseFields;
    QHash<QString, int> nameVids;
    mutable QHash<TRK_VID, ChoiceList *> choices;
    void clearFieldDefs();
    ChoiceList userChoices;
private:
    ChoiceList emptyChoices;
    */
public:
    TQServiceRecordDef(QObject *parent = 0);
    void setRecType(int rectype);
    void addField(const QString &fname, int vid, const FieldProps &prop);

    int roleVid(int role) const;
    virtual QStringList fieldNames() const;
    virtual TQAbstractFieldType getFieldType(int vid, bool *ok = 0) const;
    virtual TQAbstractFieldType getFieldType(const QString &name, bool *ok = 0) const;
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
    virtual QString dateTimeFormat() const;
    virtual QStringList noteTitleList() const;
protected:
    const FieldProps *props(int vid) const;
    QVariant stringToVar(const QString &value, int simpleType) const;
};

class TQRecordDefReplyHandler: public TQReplyHandler
{
protected:
    enum State {
        Idle, Rec, Fields, Choices
    };

    State state;
public:
    QString sessionID, projectID;
    TQServiceRecordDef *def;

    TQRecordDefReplyHandler();
    ~TQRecordDefReplyHandler();
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    virtual bool endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    virtual bool characters(const QString &ch);
};

class TQRecordsetReply: public TQReplyHandler
{
public:
    QStringList idList;
    TQRecordsetReply();
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

class TQRecordReply: public TQReplyHandler
{
public:
    QString lastField, val;
    QHash<QString, QString> values;
    TQRecordReply();
    virtual bool	startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
    virtual bool endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    virtual bool characters(const QString &ch);
};

class TQQueryListReply: public TQReplyHandler
{
public:
    class QueryItem {
    public:
        QString name;
        bool pub;
        int rectype;
    };
    QList<QueryItem> queries;
    TQQueryListReply();
    virtual bool startTQElement ( const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts );
};

#endif // REPLYHDL_H
