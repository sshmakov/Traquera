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
    : TQQueryDef(project, def->recordType()), prj(project), recDef(def),
      styleLoc(2), styleName("Standard")
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
        TrkDateCond *dcond = new TrkDateCond(this);
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
    oldString = str;
    if(!str.isEmpty())
    {
        moduleSearch = nextCItem(str).toInt() == 1;
        changeSearch = nextCItem(str).toInt() == 1;
//        int res = nextCItem(str).toInt();
        parseFields(str);
        useQuerySort  = nextCItem(str).toInt() == 0;
        useQueryStyle = nextCItem(str).toInt() == 0;
        useQueryPrint = nextCItem(str).toInt() == 0;
        outToPrinter = nextCItem(str).toInt() == 1;
        int sortFCount = nextCItem(str).toInt();
        while(!str.isEmpty() && sortFCount--)
        {
            SortF cond;
            cond.vid = nextCItem(str).toInt();
            cond.isPrintSectionBreaks = nextCItem(str).toInt() == 1;
            cond.isPageBreaks = nextCItem(str).toInt() == 1;
            cond.isDescending = nextCItem(str).toInt() == 1;
            cond.sequence = nextCItem(str).toInt() == 1;
            sortCond.append(cond);
        }
        styleLoc = nextCItem(str).toInt();
        styleName = nextCItem(str);
        printerOrient = nextCItem(str).toInt();
        pageBreak = nextCItem(str).toInt();
        leftMargin = nextCItem(str).toDouble();
        topMargin = nextCItem(str).toDouble();
        rightMargin = nextCItem(str).toDouble();
        bottomMargin = nextCItem(str).toDouble();
        isPrintSummary = nextCItem(str).toInt() == 1;
        isPrintDetail = nextCItem(str).toInt() == 1;
        isPrintTotal = nextCItem(str).toInt() == 1;
        headerOffset = nextCItem(str).toDouble();
        footerOffset = nextCItem(str).toDouble();
        header = nextCItem(str);
        footer = nextCItem(str);
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
        keyCond->isKeyInRecTitles = nextCItem(str).toInt() == 1;
        keyCond->isKeyInDesc = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteTitles = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteText = nextCItem(str).toInt() == 1;
        keyCond->isKeyInNoteOnly = nextCItem(str).toInt() == 1;

        QString flag = nextCItem(str);

        parseFields(str);

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

QString TrkQueryDef::makeSaveString()
{
/*
1,  -- 0 or 1 (1 if there are any module search strings, otherwise 0)
1,  -- 0 or 1 (1 if there are any keyword search strings, otherwise 0)
(fields conditions),
0,  -- 0 or 1 (0 = use sort option stored with query, 1 = use sorting options from the query window)
1,  -- 0 or 1 (0 = use style sheet stored with query, 1 = use style sheet from query window)
1,  -- 0 or 1 (0 = use print options stored with query, 1 = use print options from query window)
0,  -- 0 or 1 (0 = output to screen, 1 = output to printer)
(sort)
(style)
(print)
(module search)
(keyword search)
2,  -- ? record type ?
(fields conditions),

 */

    QString res;
    TrkModuleCond * mcond = 0;
    TrkKeywordCond * kcond = 0;
    TrkChangeCond *ccond = 0;
    foreach(TQCond *cond, nestedCond)
    {
        if(!mcond)
            mcond =qobject_cast<TrkModuleCond *>(cond);
        if(!kcond)
            kcond =qobject_cast<TrkKeywordCond *>(cond);
        if(!ccond)
            ccond =qobject_cast<TrkChangeCond *>(cond);
    }
    addCItem(res, QString(mcond ? "1": "0"));
    addCItem(res, QString(kcond ? "1": "0"));
    if(ccond)
        res += ",0";
    else
        res += "," + makeFieldsString();
    addCItem(res, useQuerySort ? "0" : "1");
    addCItem(res, useQueryStyle ? "0" : "1");
    addCItem(res, useQueryPrint ? "0" : "1");
    addCItem(res, outToPrinter ? "1" : "0");
//    if(useQuerySort)
        res += "," + makeSortString();
//    if(useQueryStyle)
        res += "," + makeStyleString();
//    if(useQueryPrint)
        res += "," + makePrintString();
    res += "," + makeModuleString();
    res += "," + makeKeywordString();
    if(ccond)
        res += ",2," + makeFieldsString();
    else
        res +=",";
//    addCItem(res, QString::number(recordType));
    return res;
}


QString TrkQueryDef::makeFieldsString()
{
    QString res;
    int fcount = 0;
    TrkModuleCond * mcond = 0;
    TrkKeywordCond * kcond = 0;
    foreach(TQCond *cond, nestedCond)
    {
        mcond =qobject_cast<TrkModuleCond *>(cond);
        if(mcond)
            continue;
        kcond =qobject_cast<TrkKeywordCond *>(cond);
        if(kcond)
            continue;
        fcount++;
    }
    addCItem(res, QString::number(fcount));
    foreach(TQCond *cond, nestedCond)
    {
        mcond =qobject_cast<TrkModuleCond *>(cond);
        if(mcond)
            continue;
        kcond =qobject_cast<TrkKeywordCond *>(cond);
        if(kcond)
            continue;
        QString fc;
        int internalId;
        if(cond->vid())
            internalId = recDef->fieldInternalIdByVid(cond->vid());
        TQNumberCond *ncond = qobject_cast<TQNumberCond *>(cond);
        if(ncond)
        {
            addCItem(res,QString::number(internalId));
            addCItem(fc,QString::number(ncond->op));
            addCItem(fc,ncond->value1.toString());
            if(ncond->op == TQNumberCond::Between)
                addCItem(fc,ncond->value2.toString());
        }
        TQDateCond *dcond = qobject_cast<TQDateCond *>(cond);
        if(dcond)
        {
            addCItem(res,QString::number(internalId));
            // op:
            // date: 1 = equals, 2 = between, 3 = greater than, 4 = less than.
            // days ago: 7 = between, 6 = greater than, 5 = less than
            // datetime: 11 = before, 10 = after, 9 = between, 8 equals
            switch(dcond->op)
            {
            case TQDateCond::Equals:
                addCItem(fc, "8");
                break;
            case TQDateCond::Between:
                addCItem(fc, dcond->isDaysValue ? "7" : "9");
                break;
            case TQDateCond::GreaterThan:
                addCItem(fc, dcond->isDaysValue ? "6" : "10");
                break;
            case TQDateCond::LessThan:
                addCItem(fc, dcond->isDaysValue ? "5" : "11");
                break;
            default:
                addCItem(fc, "1");
            }
            // 0 = use the next number as the date value; 1 = use current date/time as the date value; 2 = <<Unassigned>>.
            int flags;
            if(!dcond->isDaysValue && dcond->isCurrentDate1)
                flags = 1;
            else if(!dcond->isDaysValue && dcond->value1.isNull())
                flags = 2;
            else
                flags = 0;
            addCItem(fc, QString::number(flags));
            if(flags == 0)
                if(dcond->isDaysValue)
                    addCItem(fc, QString::number(dcond->days1));
                else
                    addCItem(fc, QString::number(dcond->value1.toTime_t()));
//            else
//                addCItem(fc, "0");

            if(dcond->op == TQDateCond::Between)
            {
                int flags;
                if(!dcond->isDaysValue && dcond->isCurrentDate2)
                    flags = 1;
                else if(!dcond->isDaysValue && dcond->value2.isNull())
                    flags = 2;
                else
                    flags = 0;
                addCItem(fc, QString::number(flags));
                if(flags == 0)
                    if(dcond->isDaysValue)
                        addCItem(fc, QString::number(dcond->days2));
                    else
                        addCItem(fc, QString::number(dcond->value2.toTime_t()));
//                else
//                    addCItem(fc, "0");
            }
        }
        TQUserCond *ucond = qobject_cast<TQUserCond *>(cond);
        if(ucond)
        {
            addCItem(res,QString::number(internalId));
            /*
            count, [flag] [id, id ...]
            count is the number of user ID that follows.
            The optional flag is distinguished by having its high bit (0x80000000) set.
            If the low bit (0x1) is set, then any following IDs represent groups rather than users.
            If the next bit (0x2) is set, then active users will be included in the target set.
            If the following bit (0x4) is set, then deleted users will be included in the target set.
            If no flag is given, then any IDs are assumed to be user IDs
                and only active users will be included in the target set.
            Membership in the target set is determined for user IDs
                when the query is composed and for group IDs when the query is run.

            If a user ID = 0, then the next number will be either 1, 2, or 3.
                1 means <<Unassigned>>, 2 means <<Current Login User>>, 3 means both.
            For instance, if the condition is ...
                2, 14, 0, 1 it means “select where that user field = 14 or 0.”
                2, 14, 0, 2 and the current login user has a user ID of 3, it means “select where that user field = 14 or 3.”
            */
            int idcount = ucond->ids.count();
            if(ucond->isNullIncluded || ucond->isCurrentIncluded)
                idcount++;
            addCItem(fc, QString::number(idcount));
            if(ucond->isGroups || ucond->isDeletedIncluded)
                addCItem(fc, QString::number(
                             0x80000000
                             | (ucond->isGroups ? 0x1 : 0)
                             | (ucond->isActiveIncluded ? 0x2 : 0)
                             | (ucond->isDeletedIncluded ? 0x4 : 0)
                             )
                         );
            foreach(uint id, ucond->ids)
            {
                addCItem(fc, QString::number(id));
            }
            if(ucond->isNullIncluded && ucond->isCurrentIncluded)
            {
                addCItem(fc, "0");
                addCItem(fc, "3");
            }
            else if(ucond->isNullIncluded)
            {
                addCItem(fc, "0");
                addCItem(fc, "1");
            }
            else if(ucond->isCurrentIncluded)
            {
                addCItem(fc, "0");
                addCItem(fc, "2");
            }
        }
        TQChoiceCond *ccond = qobject_cast<TQChoiceCond *>(cond);
        if(ccond)
        {
            addCItem(res,QString::number(internalId));
            /*
            Condition String for a choice field:
            op, [count, [ choice ID, ... ] ]
            op: 1 = None, 2 = All, 3 = selected choice ID.
            If op = 3, then count is the number of choices, and then choice ids will follow that.
            */
            switch(ccond->op)
            {
            case TQChoiceCond::Null:
                addCItem(fc, "1");
                break;
            case TQChoiceCond::Any:
                addCItem(fc, "2");
                break;
            case TQChoiceCond::Selected:
                addCItem(fc, "3");
                addCItem(fc, QString::number(ccond->values.count()));
                foreach(const TQChoiceItem &item, ccond->values)
                    addCItem(fc, QString::number(item.id));
                break;
            default:
                addCItem(fc, "2");
            }
        }
        TQStringCond *scond = qobject_cast<TQStringCond *>(cond);
        if(scond)
        {
            addCItem(res,QString::number(internalId));
            /*
            Condition String for a String field:
            case, string
            case: 1 = case sensitive, 0 = case insensitive.
            string: can contain wild cards:
            -    ? is match any character,
            -    * is match any sub-strings.
            -    A back slash (\) preceding any character is the character itself.
            */
            addCItem(fc, scond->isCaseSensitive ? "1" : "0");
            addCItem(fc, scond->value);
        }
        TrkChangeCond *chcond = qobject_cast<TrkChangeCond *>(cond);
        if(chcond)
        {
            addCItem(res, "0");  // special field vid
            fc = chcond->makeString();
        }
        addCItem(res,fc);
        addCItem(res,cond->isOr() ? "2" : "1");
        addCItem(res,QString::number(cond->flags()));
    }
    return res;
}

QString TrkQueryDef::makeSortString()
{
    /*
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
     */
    QString res;
    addCItem(res, QString::number(sortCond.count()));
    foreach(const SortF &c, sortCond)
    {
        addCItem(res, QString::number(c.vid));
        addCItem(res, c.isPrintSectionBreaks ? "1" : "0");
        addCItem(res, c.isPageBreaks ? "1" : "0");
        addCItem(res, c.isDescending ? "1" : "0");
        addCItem(res, QString::number(c.sequence));
    }

    return res;
}

QString TrkQueryDef::makeStyleString()
{
/*
2,         -- 0, 1, 2 (style sheet locations, 0 = personal style sheet, 1 = project style sheet, 2 = system style sheet)
Standard,  -- style sheet name
 */
    QString res;
    addCItem(res, QString::number(styleLoc));
    addCItem(res, styleName);
    return res;
}

QString TrkQueryDef::makePrintString()
{
/*
0, -- 0, 1 or 2 (print orientation: 0 = use printer setup, 1 = portrait, 2 = landscape).
2, -- 0, 1, or 2 (page break: 0 = page break between records; 1 = do not page break; 2 = fit whole records on page).
0.500000,0.500000,0.500000,0.500000, -- left, top, right, bottom margins (these 4 numbers are in double)
0, -- 0 or 1 (print summary, detail and grand total)
0,1,
0.250000,0.250000, -- header, footer offset (these 2 numbers are in double)
,"$project,- $page -,$date", -- header, footer strings (can use macros such as $project, $login, etc. $1, $2, $3 and $4 are sort fields)
 */
    QString res;
    addCItem(res, QString::number(printerOrient));
    addCItem(res, QString::number(pageBreak));
    addCItem(res, QString::number(leftMargin));
    addCItem(res, QString::number(topMargin));
    addCItem(res, QString::number(rightMargin));
    addCItem(res, QString::number(bottomMargin));
    addCItem(res, isPrintSummary ? "1" : "0");
    addCItem(res, isPrintDetail ? "1" : "0");
    addCItem(res, isPrintTotal ? "1" : "0");
    addCItem(res, QString::number(headerOffset));
    addCItem(res, QString::number(footerOffset));
    addCItem(res, header);
    addCItem(res, footer);
    return res; //"0,2,0.500000,0.500000,0.500000,0.500000,0,0,1,0.250000,0.250000,,\"$project,- $page -,$date\"";
}

QString TrkQueryDef::makeModuleString()
{
/*
1,    -- 0, 1, 2 (module search logic. 0 = records that have any modules, 1 = OR, 2 = AND)
1,    -- number of modules (if module search logic = 1 or 2)
C:\Users\ВС\Documents\Log.txt.zip,  -- for each module, the module names.
*/

    QString res;
    TrkModuleCond * mcond = 0;
    foreach(TQCond *cond, nestedCond)
    {
        mcond =qobject_cast<TrkModuleCond *>(cond);
        if(mcond)
            break;
    }
    if(!mcond)
        return "1,0";
    addCItem(res, QString::number((int)mcond->moduleOp));
    if(mcond->moduleOp == TrkModuleCond::Or || mcond->moduleOp == TrkModuleCond::And)
        foreach(QString module, mcond->moduleNames)
            addCItem(res, module);
    return res;
}

QString TrkQueryDef::makeKeywordString()
{
/*
1,    -- number of keywords.
abs,  -- for each keyword, the keyword to search for.
,     -- note title to search in.
0,    -- 0 or 1 (keyword search logic: 0 = OR, 1 = AND)
0,    -- 0 or 1 (0 = keyword search is not case sensitive, 1 = case sensitive)
0,    -- 0 or 1 (0 = do not search in record titles)
1,    -- 0 or 1 (0 = do not search in record descriptions)
1,    -- 0 or 1 (0 = do not search in note titles)
1,    -- 0 or 1 (0 = do not search in note descriptions)
0,    -- 0 or 1 (1 = search only in the note title specified in "note title to search in" string)
*/
    QString res;
    TrkKeywordCond * kcond = 0;
    foreach(TQCond *cond, nestedCond)
    {
        kcond =qobject_cast<TrkKeywordCond *>(cond);
        if(kcond)
            break;
    }
    if(!kcond)
        return "0,,0,0,1,1,1,1,0";
    addCItem(res, QString::number(kcond->keys.count()));
    foreach(QString key, kcond->keys)
        addCItem(res, key);
    addCItem(res, kcond->noteTitleSearch);
    addCItem(res, kcond->isKeyAnd ? "1" : "0");
    addCItem(res, kcond->isKeyCase ? "1" : "0");
    addCItem(res, kcond->isKeyInRecTitles ? "1" : "0");
    addCItem(res, kcond->isKeyInDesc ? "1" : "0");
    addCItem(res, kcond->isKeyInNoteTitles ? "1" : "0");
    addCItem(res, kcond->isKeyInNoteText ? "1" : "0");
    addCItem(res, kcond->isKeyInNoteOnly ? "1" : "0");
    return res;
}

bool TrkQueryDef::parseFields(QString &str)
{
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
//                        QString chValue;
                        int chId = nextCItem(condStr).toInt();
                        foreach(const TQChoiceItem &c, list)
                        {
                            if(c.id == chId)
                            {
                                ccond->values.append(c);
                                break;
                            }
//                                chValue = c.displayText;
                        }
//                        if(!chValue.isEmpty())
//                            ccond->values.append(chValue);
//                        else
//                            ccond->values.append(chValue);
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
                    else
                        ucond->ids.append(id);
                }
                break;
            }
            case TRK_FIELD_TYPE_DATE:
            {
                TrkDateCond *dcond = new TrkDateCond(this);
                cond = dcond;
                dcond->isDaysValue = false;
                int op = nextCItem(condStr).toInt();
                // op = 0 -
                if(op > 7)
                {
                    dcond->isDaysValue = false;
                    op = op - 7;
                }
                else if(op > 4)
                {
                    dcond->isDaysValue = true;
                    op = 9 - op;
                }
                // op:
                // date: 1 = equals, 2 = between, 3 = greater than, 4 = less than.
                // days ago: 7 = between, 6 = greater than, 5 = less than
                // datetime: 11 = before, 10 = after, 9 = between, 8 equals

                switch(op)
                {
                case 1: ////  1 = equals, 2 = between, 3 = greater than, 4 = less than.
                    //Equals, Between, GreaterThan, LessThan
                    dcond->op = TQDateCond::Equals;
                    break;
                case 2:
                    dcond->op = TQDateCond::Between;
                    break;
                case 3:
                    dcond->op = TQDateCond::GreaterThan;
                    break;
                case 4:
                    dcond->op = TQDateCond::LessThan;
                    break;
                default:
                    dcond->op = TQDateCond::Equals;
                }
                QString v;
                int flags;
                flags = nextCItem(condStr).toInt(); // 0 = use the next number as the date value; 1 = use current date/time as the date value; 2 = <<Unassigned>>.
                dcond->isCurrentDate1 = false;
                switch(flags)
                {
                case 0:
                    v = nextCItem(condStr);
                    if(dcond->isDaysValue)
                        dcond->days1 = v.toInt();
                    else
                        dcond->value1 = QDateTime::fromTime_t(v.toUInt());
                    break;
                case 1:
                    dcond->isCurrentDate1 = true;
                case 2:
                    dcond->days1 = 0;
                    dcond->value1 = QDateTime();
                    break;
                }
                if(dcond->op == TQDateCond::Between) // between
                {
                    flags = nextCItem(condStr).toInt(); // 0 = use the next number as the date value; 1 = use current date/time as the date value; 2 = <<Unassigned>>.
                    dcond->isCurrentDate2 = false;
                    switch(flags)
                    {
                    case 0:
                        v = nextCItem(condStr);
                        if(dcond->isDaysValue)
                            dcond->days2 = v.toInt();
                        else
                            dcond->value2 = QDateTime::fromTime_t(v.toUInt());
                        break;
                    case 1:
                        dcond->isCurrentDate2 = true;
                    case 2:
                        dcond->days2 = 0;
                        dcond->value2 = QDateTime();
                        break;
                    }
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
    return true;
}

QString TrkQueryDef::title() const
{
    return qryTitle;
}

void TrkQueryDef::setTitle(const QString &title)
{
    qryTitle = title;
}

QString TrkQueryDef::comment() const
{
    return qryComment;
}

void TrkQueryDef::setComment(const QString &comment)
{
    qryComment = comment;
}

TrkRecordTypeDef *TrkQueryDef::trkRecordDef()
{
    if(!this || !recDef)
        return 0;
    return recDef;
}

TQAbstractRecordTypeDef *TrkQueryDef::recordDef()
{
    return trkRecordDef();
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

QString & TrkQueryDef::addCItem(QString &list, const QString &item, const QString &separator)
{
    QString res = item;
    if(res.contains(QRegExp("['\",]")))
    {
        res.replace(QChar('"'), "\"\"").prepend(QChar('"')).append(QChar('"'));
    }
    if(list.isEmpty())
        list = res;
    else
        list += separator + res;
    return list;
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
    {
        TrkKeywordCond *kcond = new TrkKeywordCond(this);
        if(kcond->editProperties())
            appendCondition(kcond);
        else
            delete kcond;
        break;
    }
    case 1:
    {
        TrkChangeCond *chcond = new TrkChangeCond(this);
        if(chcond->editProperties())
            appendCondition(chcond);
        else
            delete chcond;
        break;
    }
    case 2:
    {
        TrkModuleCond *mcond = new TrkModuleCond(this);
        if(mcond->editProperties())
            appendCondition(mcond);
        else
            delete mcond;
        break;
    }
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
    setMask(TQCond::OrFlag
            | TQCond::NotFlag
            | TQCond::OpenFlag
            | TQCond::CloseFlag);
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
    QString templ = tr("Поиск текста \"%1\" %2%3%4%5%6%7");
    QString sop = isKeyAnd ? tr(" И ") : tr(" ИЛИ ");
    QString scase = isKeyCase ? tr("с учетом регистра") : tr("без учета регистра"); //2
    QString sinrt = isKeyInRecTitles ? tr(", в заголовке") : ""; //3
    QString sind = isKeyInDesc ? tr(", в описании") : ""; //4
    QString sinnt = isKeyInNoteTitles ? tr(", в заголовках нот") : ""; //3
    QString sinn = isKeyInNoteText ? tr(", во всех нотах") : ""; //6
    QString sinn1 = isKeyInNoteOnly ? tr(", только в ноте '%1'").arg(noteTitleSearch) : ""; //7
    QString res = templ.arg(keys.join(sop),scase,sinrt,sind,sinnt,sinn,sinn1);

    return res;
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
    changeObject = FieldChange;
    changeDate = AnyDate;
    dateMode = Date;
    changeType = AnyChange;
    fileName = "";
    authorId = 0;
//    choiceIds1;
//    choiceIds2;
    date1 = QDateTime(QDate::currentDate().addDays(-1));
    date2 = QDateTime(QDate::currentDate());
    days1 = 1;
    days2 = 0;
//    oldValues;
//    newValues;
//    noteTitles;
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
//        oldValues = csrc->oldValues;
//        newValues = csrc->newValues;
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
//    QStringList oldText, newText;
    switch(changeObject)
    {
    case FieldChange:
    {
        if(!fname.isEmpty())
            s = tr("Изменение поля %1").arg(fname);
        else
            s = tr("Изменение любого поля");
        TQChoiceList chTable = choiceTable();
        QStringList oList, nList;
        QString emp(tr("<пусто>"));
        if(choiceIds1.contains(0))
            oList.append(emp);
        if(choiceIds2.contains(0))
            nList.append(emp);
        foreach(const TQChoiceItem &ch, chTable)
        {
            foreach(int id, choiceIds1)
            {
                if(ch.id == id)
                    oList.append(ch.displayText);
            }
            foreach(int id, choiceIds2)
            {
                if(ch.id == id)
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

    if(authorId > 0)
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
0      -- anydate (?)
End

 */
bool TrkChangeCond::parseString(QString s)
{
    QStringList lines = s.split(QRegExp("\n"));

    QString key = lines.takeFirst();
    int tableClass = lines.takeFirst().toInt();
    QString res1 = lines.takeFirst();
    int dm = (DateModeEnum)lines.takeFirst().toInt();
    switch(dm)
    {
    case 0:
        dateMode = Days;
        break;
    case 1:
        dateMode = Date;
        break;
    case 2:
        dateMode = DateTime;
        break;
    default:
        dateMode = Days;
    }

    choiceIds1.clear();
    foreach(QString s, lines.takeFirst().split("\t"))
    {
        bool ok;
        int id = s.toInt(&ok);
        if(ok)
            choiceIds1.append(id);
    }
    choiceIds2.clear();
    foreach(QString s, lines.takeFirst().split("\t"))
    {
        bool ok;
        int id = s.toInt(&ok);
        if(ok)
            choiceIds2.append(id);
    }
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
    qint64 secs, oldSecs;
    secs = l.toLongLong();
    oldSecs = date1.toTime_t();
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
    int any = lines.takeFirst().toInt();

    if(any) //(dateMode == Days && changeDate == AfterDate && !days1 && !days2)
        changeDate = AnyDate;
    return true;
}

QString TrkChangeCond::makeString()
{
    QString res;
    addLine(res, "AgingStartVersion2");
    switch(changeObject)
    {
    case FieldChange:
    case RecordChange:
        addLine(res, "1");
        break;
    case NoteChange:
        addLine(res, "2");
        break;
    case FileChange:
        addLine(res, "3");
        break;
    case ModuleChange:
        addLine(res, "4");
        break;
    }
    addLine(res, "0");
    addLine(res, dateMode == Days ?
                "0" :
                dateMode == Date ?
                    "1" :
                    "2");
    if(changeObject == FieldChange)
    {
        QString sc;
        foreach(int id, choiceIds1)
            TrkQueryDef::addCItem(sc, QString::number(id), "\t");
        addLine(res, sc);
        sc.clear();
        foreach(int id, choiceIds2)
            TrkQueryDef::addCItem(sc, QString::number(id), "\t");
        addLine(res, sc);
    }
    else
    {
        addLine(res, "");
        addLine(res, "");
    }
    switch(changeObject)
    {
    case FieldChange:
        addLine(res, "1");
        break;
    case RecordChange:
        addLine(res, "2");
        break;
    case FileChange:
        addLine(res, "3");
        break;
    case NoteChange:
        addLine(res, "4");
        break;
    case ModuleChange:
        addLine(res, "5");
        break;
    }
    addLine(res, "1");
    // 0        -- 1-not
    addLine(res, changeDate == NotBetweenDates ? "1" : "0");
    // 1        -- ор 0-between, 1-after, 2-before
    switch(changeDate)
    {
    case NotBetweenDates:
    case BetweenDates:
        addLine(res, "0");
        break;
    case AnyDate:
    case AfterDate:
        addLine(res, "1");
        break;
    case BeforeDate:
        addLine(res, "2");
        break;
    default:
        addLine(res, "0");
    }
    addLine(res, QString::number(date1.toTime_t()));
    addLine(res, QString::number(date2.toTime_t()));
    //     12     -- days1 before
    if(dateMode == Days && changeDate != AnyDate)
        addLine(res,QString::number(days1));
    else
        addLine(res, "0");

    //  0      -- вид изменений 0-any , 1-добавление, 2-изменение, 3-удаление / 4-checked in, 5-checked out, 6-assigned version label, 2-modified, 1-added, 0-any, 3-removed
    switch(changeType)
    {
    case AnyChange:
        addLine(res, "0");
        break;
    case AddChange:
        addLine(res, "1");
        break;
    case ModifyChange:
        addLine(res, "2");
        break;
    case DeleteChange:
        addLine(res, "3");
        break;
    case CheckedIn:
        addLine(res, "4");
        break;
    case CheckedOut:
        addLine(res, "5");
        break;
    case AssignLabelChange:
        addLine(res, "6");
        break;
    default:
        addLine(res, "0");
    }

    //       -- заголовок ноты или маска файла
    if(changeObject == FileChange || changeObject == ModuleChange)
        addLine(res, fileName);
    else if(changeObject == NoteChange)
        addLine(res, noteTitles.join("\t"));
    else
        addLine(res, "");
    //    26     -- id поля или -1 для record и note
    if(changeObject == FieldChange)
    {
        if(qDef && qDef->trkRecordDef())
        {
            int id = qDef->trkRecordDef()->fieldInternalIdByVid(vid());
            addLine(res, QString::number(id));
        }
        else
            addLine(res, "0");
    }
    else
        addLine(res, "-1");

    //  5      -- days2 after  если days1=days2=0, то op == any
    if(dateMode == Days && changeDate != AnyDate)
        addLine(res,QString::number(days2));
    else
        addLine(res, "0");
    //  4      -- id автора изменений или 0-any
    addLine(res, QString::number(authorId));
    // 1 - any date
    if(changeDate == AnyDate)
        addLine(res, "1");
    else
        addLine(res, "0");
    addLine(res, "End");
    return res;
}

QString &TrkChangeCond::addLine(QString &result, const QString &line)
{
    result += line + "\n";
    return result;
}

TQChoiceList TrkChangeCond::choiceTable() const
{
    int id = vid();
    if(id <= 0 || !queryDef)
        return TQChoiceList();
    TQAbstractRecordTypeDef *rDef = queryDef->recordDef();
    if(!rDef)
        return TQChoiceList();
    QString chTableName = rDef->fieldChoiceTable(id);
    if(chTableName.isEmpty())
        return TQChoiceList();
    return rDef->choiceTable(chTableName);
}

void TrkChangeCond::setVid(int value)
{
    if(vid() == value)
        return;
    TQCond::setVid(value);
    choiceIds1.clear();
    choiceIds2.clear();
}


TrkDateCond::TrkDateCond(TrkQueryDef *parent)
    : TQDateCond(parent)
{

}

TQCond &TrkDateCond::operator =(const TQCond &src)
{
    return TQDateCond::operator =(src);
}

bool TrkDateCond::editProperties()
{
#ifdef CLIENT_APP
    TrkDateCondDialog dlg;
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
