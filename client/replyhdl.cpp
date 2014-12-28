#include "replyhdl.h"
#include "tqbase.h"

const char *SoapNS = "http://schemas.xmlsoap.org/soap/envelope/";
const char *AppNS = "http://shmakov.ru/TQService/";
const char *XsiNS="http://www.w3.org/2001/XMLSchema-instance";
const char *XsdNS="http://www.w3.org/2001/XMLSchema";


TQReplyHandler::TQReplyHandler()
    : QXmlDefaultHandler(), faultCode(), faultString(), faultDetail(), mode(Unk), isFault(false)
{
}

bool TQReplyHandler::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(namespaceURI == SoapNS)
    {
        if(!localName.compare("Fault",Qt::CaseInsensitive))
        {
            mode = InFault;
            isFault = true;
            return true;
        }
        if(mode = InFault)
        {
            if(!localName.compare("faultcode",Qt::CaseInsensitive))
            {
                mode = InFaultCode;
                return true;
            }
            if(!localName.compare("faultstring",Qt::CaseInsensitive))
            {
                mode = InFaultString;
                return true;
            }
            if(!localName.compare("detail",Qt::CaseInsensitive))
            {
                mode = InFaultDetail;
                return true;
            }
        }
        return true;
    }
    return startTQElement(namespaceURI, localName, qName, atts);
}

bool TQReplyHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    return true;
}

bool TQReplyHandler::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    if(namespaceURI == SoapNS)
    {
        if(mode == InFault && !localName.compare("Fault",Qt::CaseInsensitive))
        {
            mode = Unk;
            return true;
        }
        if(mode == InFaultCode && !localName.compare("faultcode",Qt::CaseInsensitive))
        {
            mode = InFault;
            return true;
        }
        if(mode == InFaultString && !localName.compare("faultstring",Qt::CaseInsensitive))
        {
            mode = InFault;
            return true;
        }
        if(mode == InFaultDetail && !localName.compare("detail",Qt::CaseInsensitive))
        {
            mode = InFault;
            return true;
        }
        return true;
    }
    return endTQElement(namespaceURI, localName, qName);
}

bool TQReplyHandler::endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    return true;
}

bool TQReplyHandler::characters(const QString &ch)
{
    switch(mode)
    {
    case InFaultCode:
        faultCode += ch;
    case InFaultString:
        faultString += ch;
    case InFaultDetail:
        faultDetail += ch;
    }
    return true;
}

TQProjectListReplyHandler::TQProjectListReplyHandler()
{
}

bool TQProjectListReplyHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName == "project")
    {
        QString name = atts.value("name");
        if(!name.isEmpty())
            projects.append(name);
        return true;
    }
    return false;
}


TQLoginReplyHandler::TQLoginReplyHandler()
{
}

bool TQLoginReplyHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName.compare("LoginResult",Qt::CaseInsensitive)==0)
    {
        sessionID = atts.value("sessionID");
        projectID = atts.value("projectID");
        return true;
    }
    return false;
}


TQRecordDefReplyHandler::TQRecordDefReplyHandler()
{
    state = Idle;
    def = new TQServiceRecordDef();
}

TQRecordDefReplyHandler::~TQRecordDefReplyHandler()
{
    if(def && !def->parent())
        delete def;
}

/*
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
*/

static inline int aInt(const QXmlAttributes &atts, const QString &name, int defValue = 0)
{
    QString val = atts.value(name);
    if(val.isEmpty())
        return defValue;
    bool ok;
    int res = val.toInt(&ok);
    if(!ok)
        return defValue;
    return res;
}

static inline bool aBool(const QXmlAttributes &atts, const QString &name, bool defValue = false)
{
    QString val = atts.value(name).trimmed();
    if(val.isEmpty())
        return defValue;
    if(val.compare("yes",Qt::CaseInsensitive) == 0
            || val.compare("true",Qt::CaseInsensitive) == 0)
        return true;
    if(val.compare("no",Qt::CaseInsensitive) == 0
            || val.compare("false",Qt::CaseInsensitive) == 0)
        return false;
    return defValue;
}

static QVariant aVar(const QString &value, int simpleType)
{
    switch(simpleType)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
    case TQ::TQ_FIELD_TYPE_STRING:
        return value;
    case TQ::TQ_FIELD_TYPE_PERCENT:
    case TQ::TQ_FIELD_TYPE_NUMBER:
        return value.toInt();
    case TQ::TQ_FIELD_TYPE_ELAPSED_TIME:
    case TQ::TQ_FIELD_TYPE_DATE:
        return QVariant(QDateTime::fromString(value));
    case TQ::TQ_FIELD_TYPE_NONE:
        return QVariant();
    }
    return QVariant(value);
}

bool TQRecordDefReplyHandler::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    switch(state)
    {
    case Idle:
        if(localName.compare("RecordDef",Qt::CaseInsensitive)==0)
        {
            state = Rec;
            def->setRecType(aInt(atts,"recordType"));
        }
        return true;
    case Rec:
        if(localName.compare("Fields",Qt::CaseInsensitive)==0)
            state = Fields;
        else if(localName.compare("Choices",Qt::CaseInsensitive)==0)
            state = Choices;
        return true;
    case Fields:
        if(localName.compare("Field",Qt::CaseInsensitive)==0)
        {
            TQServiceRecordDef::FieldProps prop;
            QString fname = atts.value("name");
            int vid = aInt(atts,"vid",0);
            prop.simpleType = aInt(atts,"simpleType",TQ::TQ_FIELD_TYPE_NONE);
            prop.nativeType = aInt(atts,"nativeType",prop.simpleType);
            prop.canSubmit = aBool(atts,"canSubmit", false);
            prop.canUpdate = aBool(atts,"canUpdate", false);
            prop.nullable = aBool(atts,"nullable", false);
            prop.hasChoice = aBool(atts,"hasChoice", false);
            prop.chTable = atts.value("choiceTable");
            prop.defValue = atts.value("defaultValue");
            prop.minValue = atts.value("minValue");
            prop.maxValue = atts.value("maxValue");
            def->addField(fname, vid, prop);
        }
        return true;
    case Choices:
        ;
    }

    return true;
}

bool TQRecordDefReplyHandler::endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    if(localName.compare("Fields",Qt::CaseInsensitive)==0)
        state = Rec;
    else if(localName.compare("Choices",Qt::CaseInsensitive)==0)
        state = Rec;
    else if(localName.compare("Rec",Qt::CaseInsensitive)==0)
        state = Idle;
    return true;
}

bool TQRecordDefReplyHandler::characters(const QString &ch)
{
    return true;
}


TQRecordsetReply::TQRecordsetReply()
{
}

bool TQRecordsetReply::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName.compare("Record",Qt::CaseInsensitive)==0)
        idList.append(atts.value("id"));
    return true;
}


TQRecordReply::TQRecordReply()
{
}

bool TQRecordReply::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName.compare("field",Qt::CaseInsensitive)==0)
    {
        lastField = atts.value("name");
        val.clear();
        return true;
    }
    lastField.clear();
    return true;
}

bool TQRecordReply::endTQElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
    if(localName.compare("field",Qt::CaseInsensitive)==0)
    {
        if(!lastField.isEmpty())
            values[lastField] = val;
        lastField.clear();
        val.clear();
    }
    return true;
}

bool TQRecordReply::characters(const QString &ch)
{
    if(!lastField.isEmpty())
        val += ch;
    return true;
}


TQQueryListReply::TQQueryListReply()
{
}

bool TQQueryListReply::startTQElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    if(localName.compare("Query",Qt::CaseInsensitive)==0)
    {
        QueryItem item;
        item.name = atts.value("name");
        item.pub = atts.value("public").compare("true",Qt::CaseInsensitive) == 0;
        item.rectype = atts.value("rectype").toInt();
        queries.append(item);
    }
    return true;
}

//========================= TQServiceRecordDef ==============================
TQServiceRecordDef::TQServiceRecordDef(QObject *parent)
    :QObject(parent)
{
    m_dateTimeFormat = "dd.MM.yyyy hh:mm:ss";
}

void TQServiceRecordDef::setRecType(int rectype)
{
    m_recType = rectype;
}

void TQServiceRecordDef::addField(const QString &fname, int vid, const TQServiceRecordDef::FieldProps &prop)
{
    m_fields.insert(vid, fname);
    m_Vids.insert(fname, vid);
    m_props.insert(vid, prop);
}

int TQServiceRecordDef::roleVid(int role) const
{
    return m_roleVids.value(role, 0);
}

QStringList TQServiceRecordDef::fieldNames() const
{
    return m_fields.values();
}

TQAbstractFieldType TQServiceRecordDef::getFieldType(int vid, bool *ok) const
{
    if(m_fields.contains(vid))
    {
        if(ok)
            *ok = true;
        return TQAbstractFieldType(this,vid);
    }
    if(ok)
        *ok =false;
    return TQAbstractFieldType();
}

TQAbstractFieldType TQServiceRecordDef::getFieldType(const QString &name, bool *ok) const
{
    int vid = fieldVid(name);
    return getFieldType(vid, ok);
}

int TQServiceRecordDef::fieldNativeType(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->nativeType;
    return TQ::TQ_FIELD_TYPE_NONE;
}

int TQServiceRecordDef::fieldSimpleType(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->simpleType;
    return TQ::TQ_FIELD_TYPE_NONE;
}

bool TQServiceRecordDef::canFieldSubmit(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->canSubmit;
    return false;
}

bool TQServiceRecordDef::canFieldUpdate(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->canUpdate;
    return false;
}

bool TQServiceRecordDef::isNullable(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->nullable;
    return true;
}

bool TQServiceRecordDef::hasChoiceList(int vid) const
{
    const FieldProps *prop = props(vid);
    if(prop)
        return prop->hasChoice;
    return false;
}

ChoiceList TQServiceRecordDef::choiceTable(const QString &tableName) const
{
    return choices.value(tableName);
}

bool TQServiceRecordDef::containFieldVid(int vid) const
{
    return m_props.contains(vid);
}


int TQServiceRecordDef::fieldVid(const QString &name) const
{
    return m_Vids.value(name,0);
}

QList<int> TQServiceRecordDef::fieldVids() const
{
    return m_fields.keys();
}

QString TQServiceRecordDef::fieldName(int vid) const
{
    return m_fields.value(vid, QString());
}

QIODevice *TQServiceRecordDef::defineSource() const
{
    QBuffer *buffer = new QBuffer();
    buffer->setData(m_defSrc);
    buffer->open(QIODevice::ReadOnly);
    return buffer;
}

int TQServiceRecordDef::recordType() const
{
    return m_recType;
}

QString TQServiceRecordDef::valueToDisplay(int vid, const QVariant &value) const
{
    int simple = fieldSimpleType(vid);
    switch(simple)
    {
    case TQ::TQ_FIELD_TYPE_NONE:
        return QString();
    case TQ::TQ_FIELD_TYPE_CHOICE:
    {
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return "";
        ChoiceList list = choiceTable(table);
        foreach(const TrkToolChoice &c, list)
        {
            if(c.fieldValue == value)
                return c.displayText;
        }
        return "";
    }
    case TQ::TQ_FIELD_TYPE_STRING:
    case TQ::TQ_FIELD_TYPE_NUMBER:
        return value.toString();
    case TQ::TQ_FIELD_TYPE_DATE:
        return value.toDateTime().toString(dateTimeFormat());
    case TQ::TQ_FIELD_TYPE_USER:
        return m_prj->userFullName(value.toString());

    }
    return TQ::TQ_FIELD_TYPE_NONE;
}

QVariant TQServiceRecordDef::displayToValue(int vid, const QString &text) const
{
    QDateTime dt;
    int simple = fieldSimpleType(vid);
    switch(simple)
    {
    case TQ::TQ_FIELD_TYPE_DATE:
        dt = QDateTime::fromString(dateTimeFormat());
        return QVariant(dt);
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
    {
        QString table = fieldChoiceTable(vid);
        if(table.isEmpty())
            return "";
        ChoiceList list = choiceTable(table);
        foreach(const TrkToolChoice &c, list)
        {
            if(c.displayText.compare(text,Qt::CaseInsensitive) == 0)
                return c.fieldValue;
        }
        return QVariant(QVariant::String);
    }
    case TQ::TQ_FIELD_TYPE_STRING:
        return text;
    case TQ::TQ_FIELD_TYPE_NUMBER:
        bool ok;
        int v = text.toInt(&ok);
        if(!ok)
            return QVariant(QVariant::Int);
        return v;
    }
    return text;
}

QVariant TQServiceRecordDef::fieldDefaultValue(int vid) const
{
    const FieldProps *prop = props(vid);
    if(!prop)
        return QVariant();
    return stringToVar(prop->defValue,prop->simpleType);
}

QVariant TQServiceRecordDef::fieldMinValue(int vid) const
{
    const FieldProps *prop = props(vid);
    if(!prop)
        return QVariant();
    return stringToVar(prop->minValue,prop->simpleType);
}

QVariant TQServiceRecordDef::fieldMaxValue(int vid) const
{
    const FieldProps *prop = props(vid);
    if(!prop)
        return QVariant();
    return stringToVar(prop->maxValue,prop->simpleType);
}

QString TQServiceRecordDef::fieldChoiceTable(int vid) const
{
    const FieldProps *prop = props(vid);
    if(!prop)
        return QString();
    return prop->chTable;
}

QString TQServiceRecordDef::dateTimeFormat() const
{
    return m_dateTimeFormat;
}

QStringList TQServiceRecordDef::noteTitleList() const
{
    return m_noteTitles;
}

const TQServiceRecordDef::FieldProps *TQServiceRecordDef::props(int vid) const
{
    if(!m_props.contains(vid))
        return 0;
    return &m_props[vid];
}

QVariant TQServiceRecordDef::stringToVar(const QString &value, int simpleType) const
{
    switch(simpleType)
    {
    case TQ::TQ_FIELD_TYPE_USER:
    case TQ::TQ_FIELD_TYPE_CHOICE:
    case TQ::TQ_FIELD_TYPE_STRING:
        return value;
    case TQ::TQ_FIELD_TYPE_PERCENT:
    case TQ::TQ_FIELD_TYPE_NUMBER:
        return value.toInt();
    case TQ::TQ_FIELD_TYPE_ELAPSED_TIME:
    case TQ::TQ_FIELD_TYPE_DATE:
        return QVariant(QDateTime::fromString(value,dateTimeFormat()));
    case TQ::TQ_FIELD_TYPE_NONE:
        return QVariant();
    }
    return QVariant(value);
}
