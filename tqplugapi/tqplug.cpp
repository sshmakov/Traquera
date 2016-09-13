#include "tqplug.h"

class TQBaseRecordTypeDefPrivate
{
    TQAbstractProject *prj;
    friend class TQBaseRecordTypeDef;
};

TQBaseRecordTypeDef::TQBaseRecordTypeDef(TQAbstractProject *prj) :
    d(new TQBaseRecordTypeDefPrivate())
{
    d->prj = prj;
}

TQBaseRecordTypeDef::~TQBaseRecordTypeDef()
{
}

TQAbstractFieldType TQBaseRecordTypeDef::getFieldType(int vid, bool *ok) const
{
    if(ok)
        *ok = containFieldVid(vid);
    return TQAbstractFieldType(this, vid);
}

TQAbstractFieldType TQBaseRecordTypeDef::getFieldType(const QString &name, bool *ok) const
{
    int vid = fieldVid(name);
    if(vid == TQ::TQ_NO_VID)
    {
        if(ok)
            ok = false;
        return TQAbstractFieldType(this, vid);
    }
    return getFieldType(vid, ok);
}

bool TQBaseRecordTypeDef::containFieldVid(int vid) const
{
    QList<int> vids = fieldVids();
    return vids.contains(vid);
}

QString TQBaseRecordTypeDef::fieldSystemName(int vid) const
{
    return fieldName(vid);
}

QString TQBaseRecordTypeDef::fieldRoleName(int vid) const
{
    int role = fieldRole(vid);
    switch(role)
    {
    case TQAbstractRecordTypeDef::IdField:
        return "id";
    case TQAbstractRecordTypeDef::TitleField:
        return "title";
    case TQAbstractRecordTypeDef::DescriptionField:
        return "description";
    case TQAbstractRecordTypeDef::StateField:
            return "state";
    case TQAbstractRecordTypeDef::SubmitDateTimeField:
            return "submitDateTime";
    case TQAbstractRecordTypeDef::SubmitterField:
            return "submitter";
    case TQAbstractRecordTypeDef::OwnerField:
            return "owner";
    }
    return QString();
}

bool TQBaseRecordTypeDef::canFieldSubmit(int vid) const
{
    Q_UNUSED(vid)
    return true;
}

bool TQBaseRecordTypeDef::canFieldUpdate(int vid) const
{
    Q_UNUSED(vid)
    return true;
}

bool TQBaseRecordTypeDef::isNullable(int vid) const
{
    Q_UNUSED(vid)
    return true;
}

bool TQBaseRecordTypeDef::hasChoiceList(int vid) const
{
    Q_UNUSED(vid)
    return false;
}

TQChoiceList TQBaseRecordTypeDef::choiceTable(const QString &tableName) const
{
    Q_UNUSED(tableName)
    return TQChoiceList();
}

QIODevice *TQBaseRecordTypeDef::defineSource() const
{
    return 0;
}

QString TQBaseRecordTypeDef::dateTimeFormat() const
{
    QLocale locale = QLocale::system();
    return locale.dateFormat(QLocale::ShortFormat)+" "+locale.timeFormat(QLocale::LongFormat);
//    return QLocale::system().dateTimeFormat(QLocale::ShortFormat);
}

QString TQBaseRecordTypeDef::valueToDisplay(int vid, const QVariant &value) const
{
    if(value.type() == QVariant::DateTime)
        return value.toDateTime().toString(dateTimeFormat());
    return value.toString();
}

QVariant TQBaseRecordTypeDef::displayToValue(int vid, const QString &text) const
{
    return text;
}

TQAbstractProject *TQBaseRecordTypeDef::project() const
{
    return d->prj;
}

bool TQBaseRecordTypeDef::hasFieldCustomEditor(int vid) const
{
    Q_UNUSED(vid)
    return false;
}

QWidget *TQBaseRecordTypeDef::createCustomEditor(int vid, QWidget *parent) const
{
    Q_UNUSED(vid)
    Q_UNUSED(parent)
    return 0;
}

TQFieldDelegate *TQBaseRecordTypeDef::fieldDelegate(int vid) const
{
    return 0;
}

QStringList TQBaseRecordTypeDef::noteTitleList() const
{
    return QStringList();
}

QVariant TQBaseRecordTypeDef::optionValue(const QString &optionName) const
{
    QString option = QString("RecordTypes/%1/%2").arg(recordType()).arg(optionName);
    return project()->optionValue(option);
}

void TQBaseRecordTypeDef::setOptionValue(const QString &optionName, const QVariant &value) const
{
    QString option = QString("RecordTypes/%1/%2").arg(recordType()).arg(optionName);
    project()->setOptionValue(option, value);
}

bool TQAbstractFieldType::hasCustomFieldEditor() const
{
    if(isValid())
        return def->hasFieldCustomEditor(vid);
    return false;
}

QWidget *TQAbstractFieldType::createFieldEditor(QWidget *parent) const
{
    if(isValid())
        return def->createCustomEditor(vid, parent);
    return 0;
}

