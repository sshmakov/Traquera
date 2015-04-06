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

TrkQueryDef::TrkQueryDef(TrkToolProject *project, TQRecordTypeDef *def)
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
        ccond->vid = fieldVid;
        ccond->op = TQChoiceCond::Selected;
        return ccond;
    }
    case TRK_FIELD_TYPE_ELAPSED_TIME:
    case TRK_FIELD_TYPE_NUMBER:
    {
        TQNumberCond *ncond = new TQNumberCond(this);
        ncond->vid = fieldVid;
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
        ucond->vid =  fieldVid;
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
        while(!str.isEmpty() && moduleCount--)
        {
            QString moduleName = nextCItem(str);
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
            int internalId = nextCItem(str).toInt();
            int vid = recDef->fieldVidByInternalId(internalId);
            QString condStr = nextCItem(str);
            int relOp = nextCItem(str).toInt();
            bool isAnd = relOp == 1;
            int flags = nextCItem(str).toInt();
            bool isNot = (flags & 1);
            bool openBracket = (flags & 2);
            bool closeBracket = (flags & 4);
            int ftype = recDef->fieldNativeType(vid);
            TQCond *cond = 0;
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
            if(cond)
            {
                cond->vid = vid;
                cond->isAnd = isAnd;
                cond->isNot = isNot;
                cond->isOpenBracket = openBracket;
                cond->isCloseBracket = closeBracket;
                nestedCond.append(cond);
            }
            else
                return false;
        }
        if(keyCond->keys.count())
            nestedCond.append(keyCond);
        else
            delete keyCond;
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

