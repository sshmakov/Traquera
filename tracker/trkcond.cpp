#include "trkcond.h"
#include "trkview.h"

#ifdef CLIENT_APP
#include "trkcondwidgets.h"
#endif

TrkCond::TrkCond(TrkQueryDef *parent)
    : TQCond(parent), q(parent)
{
}

TrkCond::TrkCond(TrkCond &src)
    : TQCond(src.q)
{
}

TQCond &TrkCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TrkCond *tsrc = qobject_cast<const TrkCond *>(&src);
    if(tsrc)
    {
        q = tsrc->q;
    }
    return *this;
}

/*
TrkQueryCond::TrkQueryCond(TrkQueryDef *parent)
    : TQQueryCond(parent)
{
}
*/

TrkQueryDef::TrkQueryDef(TrkToolProject *project, TrkRecordTypeDef *def)
    : TQQueryDef(project, def->recordType()), prj(project), recDef(def)
{
    listActions.append(tr("Добавить текстовый поиск..."));
    listActions.append(tr("Добавить поиск по изменениям..."));
    listActions.append(tr("Добавить поиск в модулях..."));
}

TQCond *TrkQueryDef::newCondition(int fieldVid)
{
    if(!recDef)
        return 0;
    int ftype = recordDef()->fieldNativeType(fieldVid);
    switch(ftype)
    {
    case TRK_FIELD_TYPE_CHOICE:
    {
        QString table = recDef->fieldChoiceTable(fieldVid);
        TQChoiceList list = recDef->choiceTable(table);

        TQChoiceCond *ccond = new TQChoiceCond(this);
        ccond->setVid(fieldVid);
        ccond->op = TQChoiceCond::Selected;
        return ccond;
    }
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_NUMBER:
    {
        TQNumberCond *ncond = new TQNumberCond(this);
        ncond->setVid(fieldVid);
        ncond->op = TQNumberCond::Equals;
        ncond->value1 = 0;
        ncond->value2 = 0;
        return ncond;
    }
    case TRK_FIELD_TYPE_OWNER:
    case TRK_FIELD_TYPE_SUBMITTER:
    case TRK_FIELD_TYPE_USER:
    {
        TQUserCond *ucond = new TQUserCond(this);
        ucond->setVid(fieldVid);
        ucond->isGroups = false;
        ucond->isActiveIncluded = true;
        ucond->isDeletedIncluded = true;
        return ucond;
    }
    case TRK_FIELD_TYPE_DATE:
    {
        TQDateCond *dcond = new TQDateCond(this);
        dcond->isDaysValue = false;
        dcond->op = TQDateCond::Equals;
        dcond->value1 = QDateTime::currentDateTime();
        return dcond;
    }
    case TRK_FIELD_TYPE_STRING:
    {
        TQStringCond *scond = new TQStringCond(this);
        scond->isCaseSensitive = false;
        scond->value = "";
        return scond;
    }
    }
    return 0;
}

/*
 *
        1,  -- 0 or 1 (1 if there are any module search strings, otherwise 0)
        1,  -- 0 or 1 (1 if there are any keyword search strings, otherwise 0)
        0,
        0,  -- 0 or 1 (0 = use sort option stored with query, 1 = use sorting options from the query window)
        1,  -- 0 or 1 (0 = use style sheet stored with query, 1 = use style sheet from query window)
        1,  -- 0 or 1 (0 = use print options stored with query, 1 = use print options from query window)
        0,  -- 0 or 1 (0 = output to screen, 1 = output to printer)

        1,  -- number of sort fields,
            -- for each sort field:
        25, -- field ID
        1,  -- 0 or 1 (1 = print section breaks)
        1,  -- 0 or 1 (1 = page break after each sort section)
        0,  -- 0 or 1 (1 = descending)
        2,  -- 0, 1, or 2 (sequence)
            -- For choice field: 0 = sort by choice label; 1 = sort by weight; 2 = sort by choice order.
            -- For user field: 0 = sort by full name; 1 = sort by login name.
            -- For date field: 0 = sort by date and time; 1 = sort by date only.
            -- For time field: 0 = sort by time; 1 = sort by hour only.
            -- For other fields, 0 = default (no options).

        2,         -- 0, 1, 2 (style sheet locations, 0 = personal style sheet, 1 = project style sheet, 2 = system style sheet)
        Standard,  -- style sheet name

        0, -- 0, 1 or 2 (print orientation: 0 = use printer setup, 1 = portrait, 2 = landscape).
        2, -- 0, 1, or 2 (page break: 0 = page break between records; 1 = do not page break; 2 = fit whole records on page).
        0.500000,0.500000,0.500000,0.500000, -- left, top, right, bottom margins (these 4 numbers are in double)
        0, -- 0 or 1 (print summary, detail and grand total)
        0,1,
        0.250000,0.250000, -- header, footer offset (these 2 numbers are in double)
        ,"$project,- $page -,$date", -- header, footer strings (can use macros such as $project, $login, etc. $1, $2, $3 and $4 are sort fields)

        1,    -- 0, 1, 2 (module search logic. 0 = records that have any modules, 1 = OR, 2 = AND)
        1,    -- number of modules (if module search logic = 1 or 2)
        C:\Users\ВС\Documents\Log.txt.zip,  -- for each module, the module names.
        1,    -- number of keywords.
        abs,  -- for each keyword, the keyword to search for.
        ,     -- note title to search in.
        0,    -- 0 or 1 (keyword search logic: 0 = OR, 1 = AND)
        0,    -- 0 or 1 (0 = keyword search is not case sensitive, 1 = case sensitive)
        0,    -- 0 or 1 (0 = do not search in note titles)
        1,    -- 0 or 1 (0 = do not search in record descriptions)
        1,    -- 0 or 1 (0 = do not search in record titles)
        1,    -- 0 or 1 (0 = do not search in note descriptions)
        0,    -- 0 or 1 (1 = search only in the note title specified in "note title to search in" string)

        2,  -- ? record type ?
        4,
        7,"0,*test*",1,3,
        7,"0,""* """"tit*""",1,4,
        6,"4,87654",1,0,
        10,"7,0,7,0,0",1,0,
 */
bool TrkQueryDef::parseSavedString(QString str)
{
    if(!str.isEmpty())
    {
        bool moduleSearch = nextCItem(str).toInt() == 1;
        bool changeSearch = nextCItem(str).toInt() == 1;
        int res = nextCItem(str).toInt();
        bool useQuerySort  = nextCItem(str).toInt() == 0;
        bool useQueryStyle = nextCItem(str).toInt() == 0;
        bool useQueryPrint = nextCItem(str).toInt() == 0;
        bool outToPrinter = nextCItem(str).toInt() == 1;
        int sortFCount = nextCItem(str).toInt();
        while(!str.isEmpty() && sortFCount--)
        {
            int vid = nextCItem(str).toInt();
            bool isPrintSectionBreaks = nextCItem(str).toInt() == 1;
            bool isPageBreaks = nextCItem(str).toInt() == 1;
            bool isDescending = nextCItem(str).toInt() == 1;
            int sequence = nextCItem(str).toInt() == 1;
        }
        int styleLoc = nextCItem(str).toInt();
        QString styleName = nextCItem(str);
        int printerOrient = nextCItem(str).toInt();
        int pageBreak = nextCItem(str).toInt();
        double leftMargin = nextCItem(str).toDouble();
        double topMargin = nextCItem(str).toDouble();
        double rightMargin = nextCItem(str).toDouble();
        double bottomMargin = nextCItem(str).toDouble();
        bool isPrintSummary = nextCItem(str).toInt() == 1;
        bool isPrintDetail = nextCItem(str).toInt() == 1;
        bool isPrintTotal = nextCItem(str).toInt() == 1;
        double headerOffset = nextCItem(str).toDouble();
        double footerOffset = nextCItem(str).toDouble();
        QString header = nextCItem(str);
        QString footer = nextCItem(str);
        int moduleLogic = nextCItem(str).toInt();
        int moduleCount = nextCItem(str).toInt();
        QStringList moduleNames;
        while(!str.isEmpty() && moduleCount--)
        {
            moduleNames.append(nextCItem(str));
        }
        TrkKeywordCond *keyCond = new TrkKeywordCond(this);
        int keyCount = nextCItem(str).toInt();
        while(!str.isEmpty() && keyCount--)
        {
            QString key = nextCItem(str);
            keyCond->keys.append(key);
        }
        keyCond->noteTitleSearch = nextCItem(str);
        keyCond->isKeyAnd = nextCItem(str).toInt() == 1;
        keyCond->isKeyCase = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteTitles = nextCItem(str).toInt() == 1;
        keyCond->isKeyInDesc = nextCItem(str).toInt() == 1;
        keyCond->isKeyInRecTitles = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteText = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteOnly = nextCItem(str).toInt() == 1;

        int reserv = nextCItem(str).toInt();

        int fcount = nextCItem(str).toInt();
        while(!str.isEmpty() && fcount--)
        {
            int vid;
            int internalId = nextCItem(str).toInt();
            QString condStr = nextCItem(str);
            int relOp = nextCItem(str).toInt();
            bool isOr = relOp != 1;
            int flags = nextCItem(str).toInt();
            bool isNot = (flags & 1);
            bool openBracket = (flags & 2);
            bool closeBracket = (flags & 4);
            TQCond *cond = 0;
            if(internalId == 0)
            {
                TrkChangeCond *chcond = new TrkChangeCond(this);
                cond = chcond;
                chcond->parseString(condStr);
                vid = chcond->vid();
            }
            else
            {
                vid = recDef->fieldVidByInternalId(internalId);
                int ftype = recDef->fieldNativeType(vid);
                switch(ftype)
                {
                case TRK_FIELD_TYPE_CHOICE:
                {
                    QString table = recDef->fieldChoiceTable(vid);
                    TQChoiceList list = recDef->choiceTable(table);

                    TQChoiceCond *ccond = new TQChoiceCond(this);
                    cond = ccond;
                    int op = nextCItem(condStr).toInt();
                    switch(op)  // 1 - null, 2 - any, 3 - selected values
                    {
                    case 1:
                        ccond->op = TQChoiceCond::Null;
                        break;
                    case 2:
                        ccond->op = TQChoiceCond::Any;
                        break;
                    case 3:
                        ccond->op = TQChoiceCond::Selected;
                        for(int vcount = nextCItem(condStr).toInt();vcount>0;vcount--)
                        {
                            QString chValue;
                            int chId = nextCItem(condStr).toInt();
                            foreach(const TQChoiceItem &c, list)
                            {
                                if(c.id == chId)
                                    chValue = c.displayText;
                            }
                            if(!chValue.isEmpty())
                                ccond->values.append(chValue);
                            else
                                ccond->values.append(chValue);
                        }
                        break;
                    default:
                        ccond->op = TQChoiceCond::Any;
                    }
                    break;
                }
                case TRK_FIELD_TYPE_ELAPSED_TIME:
                case TRK_FIELD_TYPE_NUMBER:
                {
                    TQNumberCond *ncond = new TQNumberCond(this);
                    cond = ncond;
                    int op = nextCItem(condStr).toInt(); // 1 = equals, 2 = between, 3 = greater than, 4 = less than
                    ncond->op = op == 1 ? TQNumberCond::Equals :
                                          op == 2 ? TQNumberCond::Between :
                                                    op == 3 ? TQNumberCond::GreaterThan :
                                                              op == 4 ? TQNumberCond::LessThan :
                                                                        0;
                    ncond->value1 = nextCItem(condStr).toInt();
                    if(ncond->op == 2)
                    {
                        ncond->value2 = nextCItem(condStr);
                    }
                    break;
                }
                case TRK_FIELD_TYPE_OWNER:
                case TRK_FIELD_TYPE_SUBMITTER:
                case TRK_FIELD_TYPE_USER:
                {
                    TQUserCond *ucond = new TQUserCond(this);
                    cond = ucond;
                    int usersCount = nextCItem(condStr).toInt();
                    bool waitFlags = true;
                    bool specialUser = false;
                    for(int i=0;i<usersCount; i++)
                    {
                        uint id = nextCItem(condStr).toUInt();
                        if(!i && waitFlags)
                        {
                            waitFlags = false;
                            if(id & 0x80000000)
                            {
                                i--;
                                ucond->isGroups = id & 0x1;
                                ucond->isActiveIncluded = id & 0x2;
                                ucond->isDeletedIncluded = id & 0x4;
                                continue;
                            }
                            else
                            {
                                ucond->isGroups = false;
                                ucond->isActiveIncluded = true;
                                ucond->isDeletedIncluded = false;
                            }
                        }
                        if(specialUser)
                        {
                            ucond->isNullIncluded = id & 0x1;
                            ucond->isCurrentIncluded = id & 0x2;
                            specialUser = false;
                            continue;
                        }
                        if(!id)
                        {
                            specialUser = true;
                            i--;
                        }
                        ucond->ids.append(id);
                    }
                    break;
                }
                case TRK_FIELD_TYPE_DATE:
                {
                    TQDateCond *dcond = new TQDateCond(this);
                    cond = dcond;
                    dcond->isDaysValue = false;
                    int op = nextCItem(condStr).toInt();
                    if(op > 4)
                    {
                        dcond->isDaysValue = true;
                        if(op == 5)
                            dcond->op = 4;
                        else if(op == 6)
                            dcond->op = 3;
                        else
                            dcond->op = 2;
                    }
                    else
                        dcond->op = op;
                    dcond->flags1 = nextCItem(condStr).toInt();
                    QString v = nextCItem(condStr);
                    if(dcond->isDaysValue)
                        dcond->days1 = v.toInt();
                    else
                        dcond->value1 = QDateTime::fromTime_t(v.toUInt());
                    if(dcond->op == 2) // between
                    {
                        dcond->flags2 = nextCItem(condStr).toInt();
                        if(dcond->isDaysValue)
                            dcond->days2 = v.toInt();
                        else
                            dcond->value2 = QDateTime::fromTime_t(v.toUInt());
                    }
                    break;
                }
                case TRK_FIELD_TYPE_STRING:
                {
                    TQStringCond *scond = new TQStringCond(this);
                    cond = scond;
                    scond->isCaseSensitive = nextCItem(condStr).toInt() != 0;
                    scond->value = nextCItem(condStr);
                    break;
                }
                default:
                    return false;
                }
            }
            if(cond)
            {
                cond->setVid(vid);
                cond->setIsOr(isOr);
                cond->setIsNot(isNot);
                cond->setIsOpenBracket(openBracket);
                cond->setIsCloseBracket(closeBracket);
                nestedCond.append(cond);
            }
            else
                return false;
        }

        if(keyCond->keys.count())
            nestedCond.append(keyCond);
        else
            delete keyCond;
        if(moduleSearch)
        {
            TrkModuleCond *moduleCond = new TrkModuleCond(this);
            moduleCond->moduleOp = moduleLogic == 0 ?
                        TrkModuleCond::Any : moduleLogic == 1 ?
                            TrkModuleCond::Or : TrkModuleCond::And;
            moduleCond->moduleNames = moduleNames;
            nestedCond.append(moduleCond);
        }
    }
    return true;
}

QString TrkQueryDef::nextCItem(QString &str)
{
    if(str.isEmpty())
        return QString();
    if(str[0] == QChar('"'))
    {
        int i;
        QString res;
        bool wasQuote = false;
        for(i=1; i<str.size(); i++)
        {
            QCharRef c = str[i];
            if(c == QChar('"'))
            {
                if(wasQuote)
                    res += '"';
                wasQuote = !wasQuote;
                continue;
            }
            if(c == QChar(',') && wasQuote)
            {
                str = str.mid(i + 1);
                return res;
            }
            wasQuote = false;
            res += c;
        }
        str = str.mid(i + 1);
        return res;
    }
    else
    {
        int i = str.indexOf(',');
        QString res;
        if(i<0)
        {
            res = str;
            str = QString();
        }
        else
        {
            res =str.left(i);
            str = str.mid(i + 1);
        }
        return res;
    }
}

/*
QList<QAction *> TrkQueryDef::actionsAddCond()
{
#ifdef CLIENT_APP
    QList<QAction *> list;
    QAction *a = new QAction();
    connect(a, SIGNAL(triggered()), SLOT());
    list.append(a);

#else
    return TQQueryDef::actionsAddCond();
#endif
}
*/

QStringList TrkQueryDef::miscActions()
{

    return listActions;
}

void TrkQueryDef::miscActionTriggered(const QString &actionText)
{
    int index = listActions.indexOf(actionText);
    switch(index)
    {
    case 0:
        TrkKeywordCond *kcond = new TrkKeywordCond(this);
        if(kcond->editProperties())
            appendCondition(kcond);
        else
            delete kcond;
        return;
    }
}

void TrkQueryDef::addKeywordSearch()
{
    TrkKeywordCond *cond = new TrkKeywordCond(this);
    if(cond->editProperties())
        appendCondition(cond);
    else
        delete cond;
}

// ================== TrkKeywordCond ==========================

TrkKeywordCond::TrkKeywordCond(TrkQueryDef *parent)
    : TrkCond(parent)
{
    isKeyAnd = false;
    isKeyCase = false;
    isKeyInNoteTitles = true;
    isKeyInDesc = true;
    isKeyInRecTitles = true;
    isKeyInNoteText = true;
    isKeyInNoteOnly = false;
}

TQCond &TrkKeywordCond::operator =(const TQCond &src)
{
    TrkCond::operator =(src);
    const TrkKeywordCond *ksrc = qobject_cast<const TrkKeywordCond *>(&src);
    if(ksrc)
    {
        keys = ksrc->keys;
        isKeyAnd          = ksrc->isKeyAnd         ;
        isKeyCase         = ksrc->isKeyCase        ;
        isKeyInNoteTitles = ksrc->isKeyInNoteTitles;
        isKeyInDesc       = ksrc->isKeyInDesc      ;
        isKeyInRecTitles  = ksrc->isKeyInRecTitles ;
        isKeyInNoteText   = ksrc->isKeyInNoteText  ;
        isKeyInNoteOnly   = ksrc->isKeyInNoteOnly  ;
        noteTitleSearch   = ksrc->noteTitleSearch;
    }
    return *this;
}

QString TrkKeywordCond::condSubString() const
{
    return tr("<Keywords>");
}

bool TrkKeywordCond::editProperties()
{
#ifdef CLIENT_APP
    TrkKeywordCondDialog dlg;
    dlg.setCondition(*this);
    if(dlg.exec())
    {
        *this = dlg.condition();
        return true;
    }
    return false;
#else
    return false;
#endif
}

//======================= TrkModuleCond ==========================
TrkModuleCond::TrkModuleCond(TrkQueryDef *parent)
{
    setMask(TQCond::OrFlag | TQCond::NotFlag | TQCond::OpenFlag | TQCond::CloseFlag);
}

TQCond &TrkModuleCond::operator =(const TQCond &src)
{
    const TrkModuleCond *msrc = qobject_cast<const TrkModuleCond *>(&src);
    if(msrc)
    {
        moduleOp = msrc->moduleOp;
        moduleNames = msrc->moduleNames;
    }
    return *this;
}

QString TrkModuleCond::condSubString() const
{
    return tr("<Модули>");
}

bool TrkModuleCond::editProperties()
{
    return false;
}

/*
void TrkModuleCond::setIsAnd(bool value)
{
    Q_UNUSED(value)
}

void TrkModuleCond::setIsOpenBracket(bool value)
{
    Q_UNUSED(value)
}

void TrkModuleCond::setIsNot(bool value)
{
    Q_UNUSED(value)
}

void TrkModuleCond::setIsCloseBracket(bool value)
{
    Q_UNUSED(value)
}
*/


TrkChangeCond::TrkChangeCond(TrkQueryDef *parent)
    :TQCond(parent), qDef(parent)
{
}

TQCond &TrkChangeCond::operator =(const TQCond &src)
{
    TQCond::operator =(src);
    const TrkChangeCond *csrc = qobject_cast<const TrkChangeCond *>(&src);
    if(csrc)
    {
        changeObject = csrc->changeObject;
        changeDate = csrc->changeDate;
        dateMode = csrc->dateMode;
        changeType = csrc->changeType;
        fileName = csrc->fileName;
        authorId = csrc->authorId;
        choiceIds1 = csrc->choiceIds1;
        choiceIds2 = csrc->choiceIds2;
        date1 = csrc->date1;
        date2 = csrc->date2;
        days1 = csrc->days1;
        days2 = csrc->days2;
        oldValues = csrc->oldValues;
        newValues = csrc->newValues;
        noteTitles = csrc->noteTitles;
    }
    return *this;
}

QString TrkChangeCond::condSubString() const
{
    QString s, ch;
    QString fname;
    if(vid() != 0)
    {
        fname = fieldName();
        if(fname.isEmpty())
            fname = "???";
    }
    switch(changeType)
    {
    case AnyChange:
        ch = tr("Любое изменение");
        break;
    case AddChange:
        ch = tr("Добавление");
        break;
    case ModifyChange:
        ch = tr("Редактирование");
        break;
    case DeleteChange:
        ch = tr("Удаление");
        break;
    case CheckedIn:
        ch = tr("Check-in");
        break;
    case CheckedOut:
        ch = tr("Check-out");
        break;
    case AssignLabelChange:
        ch = tr("Присвоение метки");
        break;
    }
    QStringList oldText, newText;
    switch(changeObject)
    {
    case FieldChange:
    {
        if(!fname.isEmpty())
            s = tr("Изменение поля %1").arg(fname);
        else
            s = tr("Изменение любого поля");
        QString chTableName = queryDef->recordDef()->fieldChoiceTable(vid());
        TQChoiceList chTable = queryDef->recordDef()->choiceTable(chTableName);
        QStringList oList, nList;
        foreach(const TQChoiceItem &ch, chTable)
        {
            foreach(const QString &vs, oldValues)
            {
                bool ok;
                int id = vs.toInt(&ok);
                if(ok && ch.id == id)
                    oList.append(ch.displayText);
            }
            foreach(const QString &vs, newValues)
            {
                bool ok;
                int id = vs.toInt(&ok);
                if(ok && ch.id == id)
                    nList.append(ch.displayText);
            }
        }
        if(!oList.isEmpty())
            s += tr(" со значения (%1)").arg(oList.join(", "));
        if(!nList.isEmpty())
            s += tr(" на значение (%1)").arg(nList.join(", "));
        break;
    }
    case RecordChange:
        s = tr("%1 записи");
        break;
    case FileChange:
        if(!fileName.isEmpty())
            s = tr("%1 файла \"%2\"").arg(ch,fileName);
        else
            s = tr("%1 любого файла").arg(ch);
        break;
    case NoteChange:
        if(!noteTitles.isEmpty())
            s += tr("%1 ноты (%2)").arg(ch,noteTitles.join(", "));
        else
            s = tr("%1 любой ноты").arg(ch);
        break;
    case ModuleChange:
        s = tr("%1 модуля").arg(ch);
        if(!fileName.isEmpty())
            s += tr(" \"%1\"").arg(fileName);
        break;
    }
    QString sday = dateMode == Days ? " дней назад" : "";
    QString dateFormat = tr("dd.MM.yyyy");
    QString dateTimeFormat = tr("dd.MM.yyyy");
    QString sdate1 = dateMode == Days ? QString::number(days1) :
                                        dateMode == Date ? date1.toString(dateFormat) :
                                                           date1.toString(dateTimeFormat);
    QString sdate2 = dateMode == Days ? QString::number(days2) :
                                        dateMode == Date ? date2.toString(dateFormat) :
                                                           date2.toString(dateTimeFormat);
    switch(changeDate)
    {
    case AnyDate:
        break;
    case BetweenDates:
        s += tr(" между %1 и %2%3").arg(sdate1, sdate2, sday);
        break;
    case NotBetweenDates:
        s += tr(" до %1 и после %2%3").arg(sdate1, sdate2, sday);
        break;
    case AfterDate:
        s += tr(" после %1%2").arg(sdate1, sday);
        break;
    case BeforeDate:
        s += tr(" до %1%2").arg(sdate1, sday);
        break;
    }

    if(authorId)
    {
        QString user = queryDef->project()->userFullName(authorId);
        if(user.isEmpty())
            user = tr("???");
        s += tr(" пользователем %1").arg(user);
    }

    return s;
}

bool TrkChangeCond::editProperties()
{
#ifdef CLIENT_APP
    TrkChangeCondDialog dlg;
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
0,AgingStartVersion2
1        -- 1-field или record, 2-note, 3-файл, 4-модуль
0
0        -- 0-days, 1-date, 2-datetime
         -- id старое значение
         -- id новое значение
1        -- 1-field, 2-record, 3-файл, 4-note, 5-модуль
1
0        -- 1-not
1        -- ор 0-between, 1-after, 2-before
1428580800 -- дата1 (без времени)
1428580800 -- дата2
12     -- days1 before
0      -- вид изменений 0-any , 1-добавление, 2-изменение, 3-удаление / 4-checked in, 5-checked out, 6-assigned version label, 2-modified, 1-added, 0-any, 3-removed
       -- заголовок ноты или маска файла
26     -- id поля или -1 для record и note
5      -- days2 after  если days1=days2=0, то op == any
4      -- id автора изменений или 0-any
0
End

 */
bool TrkChangeCond::parseString(QString s)
{
    QStringList lines = s.split(QRegExp("\n"));

    QString key = lines.takeFirst();
    int tableClass = lines.takeFirst().toInt();
    QString res1 = lines.takeFirst();
    dateMode = (DateModeEnum)lines.takeFirst().toInt();
    oldValues = lines.takeFirst().split("\t");
    newValues = lines.takeFirst().split("\t");
    changeObject = (ChangeObjectEnum)lines.takeFirst().toInt();
    QString res2 = lines.takeFirst();
    bool isNot = lines.takeFirst().toInt();
    int op = lines.takeFirst().toInt();
    switch(op)
    {
    case 0:
        changeDate = isNot ? NotBetweenDates : BetweenDates;
        break;
    case 1:
        changeDate = AfterDate;
        break;
    case 2:
        changeDate = BeforeDate;
        break;
    default:
        changeDate = AnyDate;
    }

//    const int base = 25569; // 01.01.1970
    QString l = lines.takeFirst();
    qint64 secs;
    secs = l.toLongLong();
    date1.setTime_t(secs);
    l = lines.takeFirst();
    secs = l.toLongLong();
    date2.setTime_t(secs);
    days1 = lines.takeFirst().toInt();
    changeType = (ChangeTypeEnum)lines.takeFirst().toInt();
    QString fileLine = lines.takeFirst();
    switch(changeObject)
    {
    case FileChange:
    case ModuleChange:
        fileName = fileLine;
        break;
    case NoteChange:
        if(fileLine.isEmpty())
            noteTitles.clear();
        else
            noteTitles = fileLine.split("\t");
    }
    int fieldId = lines.takeFirst().toInt();
//    m_vid = fieldId;

    TrkRecordTypeDef *recDef = (TrkRecordTypeDef *)(queryDef->recordDef());
    if(recDef)
        m_vid = recDef->fieldVidByInternalId(fieldId);
    else
        m_vid = -1;

    days2 = lines.takeFirst().toInt();
    authorId = lines.takeFirst().toInt();
    QString res3 = lines.takeFirst();

    if(dateMode == Days)
    {
        if(days1 == 0 && days2)
            changeDate = AnyDate;
    }
    return true;
}

