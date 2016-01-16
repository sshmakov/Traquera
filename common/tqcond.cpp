#include "tqbase.h"
#include "tqcond.h"

#include <QtGui>
#include "tqcondwidgets.h"

TQCond::TQCond(TQQueryDef *parent) :
    QObject(parent), queryDef(parent), m_flags(0), m_mask(0)
{
}


TQCond::TQCond(const TQCond &src)
    : QObject(src.parent()), //nestedCond(src.nestedCond), nestedAnd(src.nestedAnd)
      m_flags(src.m_flags), m_mask(src.m_mask),
      queryDef(src.queryDef)
{

}

TQCond &TQCond::operator =(const TQCond &src)
{
    m_flags = src.flags();
    m_mask = src.mask();
    m_vid = src.vid();
//    m_isAnd = src.m_isAnd;
//    m_isOpenBracket = src.m_isOpenBracket;
//    m_isCloseBracket = src.m_isCloseBracket;
//    m_isNot = src.m_isNot;
    queryDef = src.queryDef;
    return *this;
}


QString TQCond::makeQueryString(int pos) const
{
    QString orS = pos != TQCond::First ? (isOr() ? tr("ИЛИ ") : tr("И ")) : "";
    QString openB = isOpenBracket() ? "(" : "";
    QString closeB = isCloseBracket() ? ")" : "";
    QString notS = isNot() ? tr("НЕ ") : "";
    return tr("%1%2%3%4%5").arg(orS, openB, notS, condSubString(), closeB);
}

QString TQCond::fieldName() const
{
    if(!queryDef)
        return QString();
    TQAbstractRecordTypeDef *recDef = queryDef->recordDef();
    if(!recDef)
        return QString();
    QString fname = recDef->fieldName(m_vid);
    return fname;
}

QString TQCond::condSubString() const
{
    QString fname = fieldName();
    return tr("[%1]").arg(fname);
}

int TQCond::vid() const
{
    return m_vid;
}

void TQCond::setVid(int value)
{
    m_vid = value;
}

bool TQCond::editProperties()
{
    QMessageBox::information(0,tr("Condition"),condSubString());
    return false;
}

TQCond::CondFlags TQCond::flags() const
{
    return m_flags;
}

TQCond::CondFlags TQCond::mask() const
{
    return m_mask;
}

bool TQCond::isOr() const
{
    return m_flags & TQCond::OrFlag;
}

bool TQCond::isOpenBracket() const
{
    return m_flags & TQCond::OpenFlag;
}

bool TQCond::isCloseBracket() const
{
    return m_flags & TQCond::CloseFlag;
}

bool TQCond::isNot() const
{
    return m_flags & TQCond::NotFlag;
}

void TQCond::setIsOr(bool value)
{
    if(m_mask & TQCond::OrFlag)
        return;
    if(value)
        m_flags |= TQCond::OrFlag;
    else
        m_flags &= ~TQCond::OrFlag;
}

void TQCond::setIsOpenBracket(bool value)
{
    if(m_mask & TQCond::OpenFlag)
        return;
    if(m_flags & TQCond::CloseFlag)
        return;
    if(value)
        m_flags |= TQCond::OpenFlag;
    else
        m_flags &= ~TQCond::OpenFlag;
}

void TQCond::setIsNot(bool value)
{
    if(m_mask & TQCond::NotFlag)
        return;
    if(value)
        m_flags |= TQCond::NotFlag;
    else
        m_flags &= ~TQCond::NotFlag;
}

void TQCond::setIsCloseBracket(bool value)
{
    if(m_mask & TQCond::CloseFlag)
        return;
    if(m_flags & TQCond::OpenFlag)
        return;
    if(value)
        m_flags |= TQCond::CloseFlag;
    else
        m_flags &= ~TQCond::CloseFlag;
}

void TQCond::setFlags(TQCond::CondFlags value)
{
    m_flags = value;
}

void TQCond::setMask(TQCond::CondFlags value)
{
    m_mask = value;
}

TQAbstractRecordTypeDef *TQCond::recordType() const
{
    if(!queryDef)
        return 0;
    return queryDef->recordDef();
}

TQChoiceCond::TQChoiceCond(TQQueryDef *parent)
    : TQCond(parent), op(TQChoiceCond::Null)
{

}

TQChoiceCond::TQChoiceCond(const TQChoiceCond &src)
    : TQCond(src), op(src.op), values(src.values)
{
}

TQCond &TQChoiceCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQChoiceCond *csrc = qobject_cast<const TQChoiceCond *>(&src);
    if(csrc)
    {
        op = csrc->op;
        values = csrc->values;
    }
    return *this;
}

QString TQChoiceCond::condSubString() const
{
    switch(op)
    {
    case TQChoiceCond::Null:
        return tr("%1 не заполнено").arg(fieldName());
    case TQChoiceCond::Any:
        return tr("%1 заполнено").arg(fieldName());
    case TQChoiceCond::Selected:
        QString s = values.count()>0 ? tr("%1 равно (%2)") : tr("%1 равно %2");
        bool first = true;
        QString list;
        foreach(const TQChoiceItem &value, values)
        {
            if(!first)
                list += ", ";
            first = false;
            list += value.displayText;
        }
        return s.arg(fieldName()).arg(list);
    }
    return tr("(%1 ?)").arg(fieldName());
}

bool TQChoiceCond::editProperties()
{
    TQChoiceCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
}

TQNumberCond::TQNumberCond(TQQueryDef *parent)
    : TQCond(parent)
{
    op = Equals;
    value1 = 0;
    value2 = 1;
}

TQNumberCond::TQNumberCond(const TQNumberCond &src)
    : TQCond(src), op(src.op), value1(src.value1), value2(src.value2)
{
}

TQCond &TQNumberCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQNumberCond * nsrc = qobject_cast<const TQNumberCond *>(&src);
    if(nsrc)
    {
        op = nsrc->op;
        value1 = nsrc->value1;
        value2 = nsrc->value2;
    }
    return *this;
}

QString TQNumberCond::condSubString() const
{
    switch(op)
    {
    case TQNumberCond::Equals:
        return tr("%1 = %2").arg(fieldName()).arg(value1.toInt());
    case TQNumberCond::Between:
        return tr("%1 между %2 и %3").arg(fieldName()).arg(value1.toInt()).arg(value2.toInt());
    case TQNumberCond::GreaterThan:
        return tr("%1 >= %2").arg(fieldName()).arg(value1.toInt());
    case TQNumberCond::LessThan:
        return tr("%1 <= %2").arg(fieldName()).arg(value1.toInt());
    }
    return TQCond::condSubString();
}

bool TQNumberCond::editProperties()
{
    TQNumberCondDialog dlg;
    dlg.setCondition(*this);

    int res = dlg.exec();
    if(!res)
        return false;
    *this = dlg.condition();
    return true;
}

TQUserCond::TQUserCond(TQQueryDef *parent)
    : TQCond(parent)
{
    isGroups = false;
    isActiveIncluded = false;
    isDeletedIncluded = false;
    isNullIncluded = false;
    isCurrentIncluded = false;
}

TQCond &TQUserCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQUserCond *usrc = qobject_cast<const TQUserCond *>(&src);
    if(usrc)
    {
        isGroups = usrc->isGroups;
        isActiveIncluded = usrc->isActiveIncluded;
        isDeletedIncluded = usrc->isDeletedIncluded;
        isNullIncluded = usrc->isNullIncluded;
        isCurrentIncluded = usrc->isCurrentIncluded;
        ids = usrc->ids;
    }
    return *this;
}

QString TQUserCond::condSubString() const
{
    QString pre;
    bool first = true;
    if(isNullIncluded)
    {
        pre += "<Пустое>";
        first = false;
    }
    if(isCurrentIncluded)
    {
        if(!first)
            pre += ", ";
        else
            first = false;
        pre += "<Текущий>";
        first = false;
    }
    if(isGroups)
    {
        TQGroupList groups =  queryDef->recordDef()->project()->userGroups();
        QHash<int, QString> hash;
        foreach(const TQGroup &gr, groups)
        {
            hash.insert(gr.id, gr.name);
        }

        foreach(uint id, ids)
        {
            if(!first)
                pre += ", ";
            else
                first = false;
            QString name = hash.value(id);
            pre += name;
        }
        pre = tr("%1 в группах (%2)").arg(fieldName()).arg(pre);
    }
    else
    {
        foreach(uint id, ids)
        {
            if(!first)
                pre += ", ";
            else
                first = false;
            QString name = queryDef->recordDef()->project()->userFullName(id);
            pre += name;
        }
        pre = tr("%1 в (%2)").arg(fieldName()).arg(pre);
    }
    if(isActiveIncluded && isDeletedIncluded)
        pre += tr("");
    else if(isActiveIncluded)
        pre += tr(", если активный");
    else if(isDeletedIncluded)
        pre += tr(", если удаленный");
    return pre;
}

bool TQUserCond::editProperties()
{
    TQUserCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
}

TQDateCond::TQDateCond(TQQueryDef *parent)
    : TQCond(parent)
{
    op = Equals;
    isDaysValue = false;
    value1 = QDateTime::currentDateTime();
    value2 = QDateTime::currentDateTime();
    days1 = 0;
    days2 = 0;
    flags1 = 0;
    flags2 = 0;
    isCurrentDate1 = true;
    isCurrentDate2 = true;
}

TQCond &TQDateCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQDateCond *dsrc =qobject_cast<const TQDateCond *>(&src);
    if(dsrc)
    {
        op = dsrc->op;
        isDaysValue = dsrc->isDaysValue;
        value1 = dsrc->value1;
        value2 = dsrc->value2;
        days1 = dsrc->days1;
        days2 = dsrc->days2;
        flags1 = dsrc->flags1;
        flags2 = dsrc->flags2;
        isCurrentDate1 = dsrc->isCurrentDate1;
        isCurrentDate2 = dsrc->isCurrentDate2;
    }
    return *this;
}

QString TQDateCond::condSubString() const
{
    if(isDaysValue)
    {
        switch(op)
        {
        case TQDateCond::Equals:
            return tr("%1 = %2 дней назад от текущей даты").arg(fieldName()).arg(days1);
        case TQDateCond::Between:
            return tr("%1 между %2 и %3 днями назад от текущей даты").arg(fieldName()).arg(days1).arg(days2);
        case TQDateCond::GreaterThan:
            return tr("%1 позже %2 дней назад от текущей даты").arg(fieldName()).arg(days1);
        case TQDateCond::LessThan:
            return tr("%1 ранее %2 дней назад от текущей даты").arg(fieldName()).arg(days1);
        }
    }
    else
    {
        QString s1, s2;
        QString cur(tr("<Текущая дата>")), format(tr("dd.MM.yy hh:mm")), emp(tr("<Пусто>"));
        if(isCurrentDate1)
            s1 = cur;
        else if(value1.isNull())
            s1 = emp;
        else
            s1 = value1.toString(format);
        if(isCurrentDate2)
            s2 = cur;
        else if(value2.isNull())
            s2 = emp;
        else
            s2 = value2.toString(format);
        switch(op)
        {
        case TQDateCond::Equals:
            return tr("%1 = %2").arg(fieldName()).arg(s1);
        case TQDateCond::Between:
            return tr("%1 между %2 и %3").arg(fieldName(),s1,s2);
        case TQDateCond::GreaterThan:
            return tr("%1 позже %2").arg(fieldName(),s1);
        case TQDateCond::LessThan:
            return tr("%1 ранее %2").arg(fieldName(),s2);
        }
    }
    return tr("(%1)").arg(fieldName());
}

TQStringCond::TQStringCond(TQQueryDef *parent)
    : TQCond(parent)
{
}

TQCond &TQStringCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQStringCond *ssrc = qobject_cast<const TQStringCond *>(&src);
    if(ssrc)
    {
        value = ssrc->value;
        isCaseSensitive = ssrc->isCaseSensitive;
    }
    return *this;
}

QString TQStringCond::condSubString() const
{
    return tr("%1 равно \"%2\"").arg(fieldName()).arg(isCaseSensitive ? value : value.toLower());
}

bool TQStringCond::editProperties()
{
    TQStringCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
}

/*
TQQueryCond::TQQueryCond(TQQueryDef *parent)
    : QObject(parent)
{

}

TQQueryCond::TQQueryCond(const TQQueryCond &src)
    : QObject(src.parent()), nestedCond(src.nestedCond)//, nestedAnd(src.nestedAnd)
{
}

bool TQQueryCond::parseSavedString(QString &s)
{
    return true;
}

QString TQQueryCond::makeSavedString() const
{
    return QString();
}
*/

TQQueryDef::TQQueryDef(TQAbstractProject *prj, int rectype)
    : QObject(prj), m_project(prj), recordType(rectype)
{
//    condition = new TQQueryCond(this);
}

TQAbstractRecordTypeDef *TQQueryDef::recordDef()
{
    if(!m_project)
        return 0;
    return m_project->recordTypeDef(recordType);
}

TQAbstractProject *TQQueryDef::project() const
{
    return m_project;
}

QStringList TQQueryDef::queryDefLines()
{
    QStringList lines;
    for(int i=0; i<nestedCond.count(); i++)
        lines.append(condLine(i));
    return lines;
}

QString TQQueryDef::condLine(int index)
{
    if(index < 0 || index >= nestedCond.count())
        return QString();
    TQCond::StringPos pos = index == 0 ? TQCond::First :
                                         index == nestedCond.count()-1 ? TQCond::Last :
                                                                         TQCond::Middle;
    return nestedCond[index]->makeQueryString(pos);
}

TQCond *TQQueryDef::condition(int index)
{
    if(index < 0 || index >= nestedCond.count())
        return 0;
    return nestedCond[index];
}

int TQQueryDef::indexOf(TQCond *cond)
{
    return nestedCond.indexOf(cond);
}

bool TQQueryDef::editProperties(int index)
{
    if(index < 0 || index >= nestedCond.count())
        return false;
    TQCond *cond = nestedCond[index];
    return cond->editProperties();
}

TQCond *TQQueryDef::newCondition(int fieldVid)
{
    Q_UNUSED(fieldVid)
    return 0;
}

void TQQueryDef::insertCondition(int before, TQCond *cond)
{
    nestedCond.insert(before, cond);
    cond->setParent(this);
    emit changed();
}

void TQQueryDef::appendCondition(TQCond *cond)
{
    insertCondition(nestedCond.size(), cond);
}

void TQQueryDef::removeCondition(int index)
{
    TQCond *cond = nestedCond.takeAt(index);
    if(cond)
        delete cond;
}

TQCond *TQQueryDef::takeCondition(int index)
{
    TQCond *cond = nestedCond.takeAt(index);
    return cond;
}

int TQQueryDef::count()
{
    return nestedCond.size();
}

QStringList TQQueryDef::miscActions()
{
    return QStringList();
}

QString TQQueryDef::name()
{
    return m_name;
}

void TQQueryDef::setName(const QString &name)
{
    m_name = name;
}

void TQQueryDef::miscActionTriggered(const QString &actionText)
{
    Q_UNUSED(actionText)
}

/*
QList<QAction *> TQQueryDef::actionsAddCond()
{
    return QList<QAction *>();
}
*/

/*
TQFieldCond::TQFieldCond(TQQueryDef *parent)
    : TQCond(parent)
{
}

TQCond &TQFieldCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TQFieldCond *fsrc = qobject_cast<const TQFieldCond *>(&src);
    if(fsrc)
        m_vid = fsrc->vid();
    return *this;
}
QString TQFieldCond::fieldName() const
{
    if(!queryDef)
        return QString();
    TQAbstractRecordTypeDef *recDef = queryDef->recordDef();
    if(!recDef)
        return QString();
    QString fname = recDef->fieldName(m_vid);
    return fname;
}

QString TQFieldCond::condSubString() const
{
    QString fname = fieldName();
    return tr("[%1]").arg(fname);
}

int TQFieldCond::vid() const
{
    return m_vid;
}

void TQFieldCond::setVid(int value)
{
    m_vid = value;
}

*/
