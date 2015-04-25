#include "tqbase.h"
#include "tqcond.h"

#ifdef CLIENT_APP
#include <QtGui>
#include "tqcondwidgets.h"
#endif

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
    m_vid = src.vid();
    m_flags = src.flags();
    m_mask = src.mask();
//    m_isAnd = src.m_isAnd;
//    m_isOpenBracket = src.m_isOpenBracket;
//    m_isCloseBracket = src.m_isCloseBracket;
//    m_isNot = src.m_isNot;
    queryDef = src.queryDef;
    return *this;
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

QString TQCond::makeQueryString(int pos) const
{
    QString orS = pos != TQCond::First ? (isOr() ? tr("ÈËÈ ") : tr("È ")) : "";
    QString openB = isOpenBracket() ? "(" : "";
    QString closeB = isCloseBracket() ? ")" : "";
    QString notS = isNot() ? tr("ÍÅ ") : "";
    return tr("%1%2%3%4%5").arg(orS, openB, notS, condSubString(), closeB);
}

QString TQCond::condSubString() const
{
    QString fname = fieldName();
    return tr("[%4]").arg(fname);
}

bool TQCond::editProperties()
{
#ifdef CLIENT_APP
    QMessageBox::information(0,tr("Condition"),condSubString());
#endif
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

int TQCond::vid() const
{
    return m_vid;
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

void TQCond::setVid(int value)
{
    m_vid = value;
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
        return tr("%1 is null").arg(fieldName());
    case TQChoiceCond::Any:
        return tr("%1 is not null").arg(fieldName());
    case TQChoiceCond::Selected:
        QString s = tr("%1 in (%2)");
        bool first = true;
        QString list;
        foreach(const QVariant &value, values)
        {
            if(!first)
                list += ", ";
            first = false;
            list += value.toString();
        }
        return s.arg(fieldName()).arg(list);
    }
    return tr("(%1 ?)").arg(fieldName());
}

bool TQChoiceCond::editProperties()
{
#ifdef CLIENT_APP
    TQChoiceCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
    /*
    QVBoxLayout *lay = new QVBoxLayout(&dlg);
    QListWidget *vList = new QListWidget();
    lay->addWidget(vList);
    QString chTable = queryDef->recordDef()->fieldChoiceTable(vid);
    TQChoiceList choices = queryDef->recordDef()->choiceTable(chTable);
    foreach(const TQChoiceItem &item, choices)
    {
        vList->addItem(item.displayText);
    }
    int res = dlg.exec();
    if(!res)
        return false;
    return true;
    */
#else
    return false;
#endif
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
        return tr("%1 between %2 and %3").arg(fieldName()).arg(value1.toInt()).arg(value2.toInt());
    case TQNumberCond::GreaterThan:
        return tr("%1 >= %2").arg(fieldName()).arg(value1.toInt());
    case TQNumberCond::LessThan:
        return tr("%1 <= %2").arg(fieldName()).arg(value1.toInt());
    }
    return TQCond::condSubString();
}

bool TQNumberCond::editProperties()
{
#ifdef CLIENT_APP
    TQNumberCondDialog dlg;
    dlg.setCondition(*this);

    int res = dlg.exec();
    if(!res)
        return false;
    *this = dlg.condition();
    return true;
#else
    return false;
#endif
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
        pre += "<Null>";
        first = false;
    }
    if(isCurrentIncluded)
    {
        if(!first)
            pre += ", ";
        else
            first = false;
        pre += "<CurrentUser>";
        first = false;
    }
    foreach(uint id, ids)
    {
        if(!first)
            pre += ", ";
        else
            first = false;
        pre += QString::number(id);
    }
    if(isGroups)
        pre = tr("%1 in groups (%2)").arg(fieldName()).arg(pre);
    else
        pre = tr("%1 in (%2)").arg(fieldName()).arg(pre);
    if(isActiveIncluded && isDeletedIncluded)
        pre += tr(" in any state");
    else if(isActiveIncluded)
        pre += tr(" only active");
    else if(isDeletedIncluded)
        pre += tr(" only deleted");
    return pre;
}

bool TQUserCond::editProperties()
{
#ifdef CLIENT_APP
    TQUserCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
#else
    return false;
#endif
}

TQDateCond::TQDateCond(TQQueryDef *parent)
    : TQCond(parent)
{
}

QString TQDateCond::condSubString() const
{
    if(isDaysValue)
    {
        switch(op)
        {
        case TQDateCond::Equals:
            return tr("%1 = %2 days ago").arg(fieldName()).arg(days1);
        case TQDateCond::Between:
            return tr("%1 between %2 and %3 days ago").arg(fieldName()).arg(days1).arg(days2);
        case TQDateCond::GreaterThan:
            return tr("%1 >= %2").arg(fieldName()).arg(days1);
        case TQDateCond::LessThan:
            return tr("%1 <= %2").arg(fieldName()).arg(days1);
        }
    }
    else
    {
        switch(op)
        {
        case TQDateCond::Equals:
            return tr("%1 = %2").arg(fieldName()).arg(value1.toString());
        case TQDateCond::Between:
            return tr("%1 between %2 and %3").arg(fieldName()).arg(value1.toString()).arg(value2.toString());
        case TQDateCond::GreaterThan:
            return tr("%1 >= %2").arg(fieldName()).arg(value1.toString());
        case TQDateCond::LessThan:
            return tr("%1 <= %2").arg(fieldName()).arg(value1.toString());
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
    return tr("%1 like \"%2\"").arg(fieldName()).arg(isCaseSensitive ? value : value.toLower());
}

bool TQStringCond::editProperties()
{
#ifdef CLIENT_APP
    TQStringCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        this->operator =(dlg.condition());
        return true;
    }
    return false;
#else
    return false;
#endif
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
#ifdef CLIENT_APP
    if(index < 0 || index >= nestedCond.count())
        return false;
    TQCond *cond = nestedCond[index];
    return cond->editProperties();
    /*
    TQChoiceCond *ccond = qobject_cast<TQChoiceCond *>(cond);
    TQNumberCond *ncond = qobject_cast<TQNumberCond *>(cond);
    TQUserCond *ucond = qobject_cast<TQUserCond *>(cond);
    if(0 != ccond)
    {
        return ccond->editProperties();
    }
    else if(ncond)
    {
        TQNumberCondDialog dlg;
        dlg.setCondition(*ncond);

        int res = dlg.exec();
        if(!res)
            return false;
        *ncond = dlg.condition();
        return true;
    }
    else if(ucond)
    {
        return ucond->editProperties();
    }
    return false;
    */
#else
    Q_UNUSED(index)
    return false;
#endif
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
