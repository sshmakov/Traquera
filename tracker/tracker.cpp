#include "tracker.h"
#include <QStringList>
#include <QDate>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlResult>
#include <QMessageBox>

// TrkDb ======================================================

TrkDb::TrkDb(QObject *parent)
: QObject(parent)
{
}

TrkDb::~TrkDb()
{
	close();
}

/*
bool TrkDb::open(const QString &dsn, const QString &projectName)
{
	db = QSqlDatabase::addDatabase("QODBC","tracker");
	db.setDatabaseName(dsn);
	if(!db.open())
	{
        QMessageBox::critical(0, "Cannot open database",
			db.lastError().text(), QMessageBox::Cancel);
		return false;
	}
	QSqlQuery query(db);
	query.prepare("select prjDbName from trkmaster.dbo.trkprj where prjDescr = :prj");
	query.bindValue(":prj",projectName);
	if(!query.exec() || !query.next())
	{
        QMessageBox::critical(0, "Cannot open project",
			query.lastError().text(), QMessageBox::Cancel);
		return false;
	}
	dbname = query.value(0).toString().trimmed();
	query.exec("use "+dbname);
	if(!dblist.contains(this))
		dblist.append(this);
	//openProject(projectName,"");
	return true;
}
*/

bool TrkDb::check(bool res)
{
	if(!res)
	{
        QMessageBox::critical(0, "Error on database",
			db.lastError().text(), QMessageBox::Cancel);
		return false;
	}
	return res;
}

bool TrkDb::openProject(const QString &dsn, const QString &projectName, const QString &login)
{
	db = QSqlDatabase::addDatabase("QODBC","tracker");
	db.setDatabaseName(dsn);
	if(!check(db.open()))
		return false;
	QSqlQuery query(db);
	query.prepare("select prjDbName from trkmaster.dbo.trkprj"
	  " where prjDescr = :prj"
	  );
	query.bindValue(":prj", projectName);
	if(!check(query.exec()))
		return false;
	//QMessageBox::information(0,"i", projectName);
	if(!query.next())
	{
        QMessageBox::critical(0, "Cannot open database",
			query.lastError().text(), QMessageBox::Cancel);
		return false;
	}
	dbname = query.value(0).toString().trimmed();
	if(!check(query.exec("use "+dbname)))
		return false;
	query.prepare("select usrId from trkusr where usrLoginId = :login");
	query.bindValue(":login",login);
	if(!check(query.exec()))
		return false;
	if(!query.next())
		return false;
	curUserId = query.value(0).toInt();
	//queryQueries = "select * from trkqry";
	readFields();
	readUsers();
	readChoices();
	readQueries();
	readGroups();
	if(!dblist.contains(this))
		dblist.append(this);
	return true;
}

void TrkDb::close()
{
	db.close();
	dblist.removeAll(this);
}


bool TrkDb::readFields()
{
	QSqlQuery query(db);
	if(!check(query.exec("select fldId, fldClass, fldFlags, fldVid, fldLen, fldLabel, fldSlot from "+dbname+".dbo.trkfld where fldTypeId=1 and fldActive<>0")))
		return false;
	while(query.next())
	{
		QSqlRecord rec=query.record();
		TrkField f;
		f.fldid = rec.value(0).toInt();
		f.fldClass = rec.value(1).toInt();
		f.fldFlags = rec.value(2).toInt();
		f.fldVid = rec.value(3).toInt();
		f.fldLen = rec.value(4).toInt();
		f.fldLabel = rec.value(5).toString().trimmed();
		f.fldSlot = rec.value(6).toInt();
		if(f.fldClass != 2)
		{
			f.fldtable = "trkscrsl";
			f.fldfield = "x"+QString::number(f.fldSlot);
		}
		else
		{
			f.fldtable = "trkscrst";
			f.fldfield = "z"+QString::number(f.fldSlot);
		}
		flds[f.fldid]=f;
		fieldByLabel[f.fldLabel] = f.fldid;
		fldIds.append(f.fldid);
	}
	return true;
}

bool TrkDb::readUsers()
{
	QSqlQuery query(db);
	if(!check(query.exec(
		"select "
		"usrId, "
		"usrClass, "
		"usrFlags, "
		"usrLoginId, "
		"usrFullNm, "
		"usrTel, "
		"usrEmail, "
		"usrPwd, "
		"usrComment, " 
		"usrLstLid, "
		"usrNfyTid, "
		"usrMailTid, "
		"usrViewTid, "
		"usrMailOpt, "
		"usrSrvMsg, "
		"usrProjMsg, "
		"usrParm1, "
		"usrParm2, "
		"usrParm3, "
		"usrParm4, "
		"usrParm5, "
		"usrParm6, "
		"usrParm7, "
		"usrParm8 "
		"from "+dbname+".dbo.trkusr")))
		return false;
	while(query.next())
	{
		QSqlRecord rec = query.record();
		TrkUser u;
		u.usrId       = rec.value(0).toInt();
		u.usrClass    = rec.value(1).toInt();
		u.usrFlags    = rec.value(2).toInt();
		u.usrLoginId  = rec.value(3).toString();
		u.usrFullNm   = rec.value(4).toString();
		/*
		u.usrTel      = rec.value(5).toString();
		u.usrEmail    = rec.value(6).toString();
		u.usrPwd      = rec.value(7).toString();
		u.usrComment  = rec.value(8).toString();
		u.usrLstLid   = rec.value(9).toInt();
		u.usrNfyTid   = rec.value(10).toInt();
		u.usrMailTid  = rec.value(11).toInt();
		u.usrViewTid  = rec.value(12).toInt();
		u.usrMailOpt  = query.value(13).toString();
		u.usrSrvMsg   = query.value(14).toInt();
		u.usrProjMsg  = query.value(15).toInt();
		u.usrParm1    = query.value(16).toInt();
		u.usrParm2    = query.value(17).toInt();
		u.usrParm3    = query.value(18).toInt();
		u.usrParm4    = query.value(19).toInt();
		u.usrParm5    = query.value(20).toInt();
		u.usrParm6    = query.value(21).toInt();
		u.usrParm7    = query.value(22).toInt();
		u.usrParm8    = query.value(23).toInt();
		*/
		users[u.usrId] = u;
	}
	return true;
}

bool TrkDb::readChoices()
{
	QSqlQuery query(db);
	QString qs = "select  "
		"chId, " // int An internal unique ID for this choice. [PK]
		"chClass, " // int 0 - normal 1 - open 2 - closed
		"chFlags, " // int Indicates whether the choice is deleted or not. 0 = non-deleted. 1 = deleted.
		"chLabel, " // varchar(32) The label of this choice. Choice labels are unique within the choice field.
		"chFldId, " // int Indicates the field in the trkfld table this choice refers to. Foreign key: fldId in the trkfld table.
		"chOrder, " // int Indicates the order the choice should appear in the list presented to the user. Lower numbered choices are displayed before higher numbered choices.
		"chValue " // int The weight associated with this choice. Tracker allows users to associate weights with choices, for the purpose of producing weighted reports. Value should be between 0 and 100.
		//chParm1 - chParm8 // int Reserved for future use.
		" from "+dbname+".dbo.trkch";
	if(!query.exec(qs))
	{
        QMessageBox::critical(0, "Error on database",
			qs+"\n"+query.lastError().text(), QMessageBox::Cancel);
		return false;
	}
	while(query.next())
	{
		TrkChoice t;
		QSqlRecord rec = query.record();
		t.id = rec.value(0).toInt();
		t.deleted = rec.value(2).toInt();
		t.label = rec.value(3).toString();
		t.order = rec.value(5).toInt();
		t.value = rec.value(6).toInt();
		int fieldId = rec.value(4).toInt();
		if(choices.contains(fieldId))
			choices[fieldId][t.id] = t;
		else
		{
			QHash<int, TrkChoice> col;
			col[t.id] = t;
			choices[fieldId] = col;
		}
	}
	return true;
}

bool TrkDb::readQueries()
{
	return queryModel.read(this);
	/*
	QSqlQuery query(db);
	query.prepare("select "
		"qryId, "
		"qryClass, "
		"qryFlags, "
		"qryTypeId, "
		"qryName, "
		"qryAuthId, "
		"qryOwnerId, "
		"qryCrDate, "
		"qryMdDate, "
		"qryTitle, "
		"qryCont, "
		"qryComment, "
		"qryDescr "
		"from trkqry"
		" where "
		" qryName is not null"
		" and qryName<>''"
		" and (qryAuthId = :cur or qryOwnerId = :cur or qryAuthId = 1)"
		);
	query.bindValue(":cur", curUserId);
	if(!query.exec())
        QMessageBox::critical(0, "Cannot read queries",
			query.lastError().text(), QMessageBox::Cancel);
	while(query.next())
	{
		TrkQuery q;
		q.qryId = query.value(0).toInt();
		q.qryClass = query.value(1).toInt();
		q.qryFlags = query.value(2).toInt();
		q.qryTypeId = query.value(3).toInt();
		q.qryName = query.value(4).toString();
		q.qryAuthId = query.value(5).toInt();
		q.qryOwnerId = query.value(6).toInt();
		q.qryCrDate = query.value(7).toInt();
		q.qryMdDate = query.value(8).toInt();
		q.qryTitle = query.value(9).toString();
		q.qryCont = query.value(10).toString();
		q.qryComment = query.value(11).toString();
		q.qryDescr = query.value(12).toString();
		queries[q.qryId] = q;
	}
	return true;
	*/
}

void splitCont(QStringList &list, QString &cont)
{
	QString res;
	for(int i=0; i<cont.size(); i++)
	{
		if(cont.at(i) == ',')
		{
			list << res;
			res = "";
		}
		else if(cont.at(i) == '"')
		{
			for(++i; i<cont.size() && cont.at(i) != '"'; i++)
				res += cont.at(i);
		}
		else
			res += cont.at(i);
	}
	if(!res.isEmpty())
		list << res;
}

QString TrkDb::getQueryById(int queryid)
{
	QStringList w;
	TFieldConds c(this);
	QString cont = queryModel.queries[queryid].qryCont; // index(queryid, TrkQryModel::QryCont).data().toString();
	//QString cont = queries[queryid].qryCont;
	//QMessageBox::information(0, "cont", cont, QMessageBox::Ok);
	QStringList list;
	splitCont(list, cont);
	QStringList::const_iterator i;
	i = list.constBegin();
	if(i == list.constEnd())
		return queryStr() + " where 1=0";
	//int isModuleSearch = i->toInt();
	i++;
	//int isKeywordSearch = i->toInt();
	i++;
	c.parseCondStr(i);

	QString res = c.makeQuery();
	if(!res.isEmpty())
		res = queryStr() + " where " + res;
	else
		res = queryStr();
	return res;
}

QString TrkDb::getQueryBySCRs(const QString& numbers)
{
	QString res,num;
	num = numbers.simplified();
	int f = fieldByLabel["Id"];
	res = "x"+QString::number(flds[f].fldSlot)+" in ("+num+")";
	if(!res.isEmpty() && !num.isEmpty())
		res = queryStr() + " where " + res;
	else
		res = queryStr();
	return res;
}


QString TrkDb::queryStr()
{
	QString res="select trkscrsl.id \"InternalId\", ";
	QString choices="";
	bool first=true;
	for(int i = 0; i<fldIds.count(); i++)
	{
		int id = fldIds[i];
		const TrkField &f = flds[id];
		/*QMessageBox::information(0,"Field",
			"["+f.fldLabel+"] = "+QString::number(id)
			+" row: "+QString::number(i)
			);*/
		if(f.fldFlags !=2)
		{
			if(!first)
				res += ", ";
			QString alias;
			switch(f.fldClass)
			{
			case 1: //choice
				alias = "ch_" + f.fldfield;
				choices += "left join "+dbname+".dbo.trkch as " + alias 
					+ " on " + f.fldfield + " = " + alias + ".chId ";
				res += alias + ".chLabel \"" + f.fldLabel + "\" ";
				break;
			case 4: //datetime
				res += "dateadd(second, "
					"case "+f.fldfield+" when 0 then NULL else " + f.fldfield + " end"
					+",'01/01/1970 04:00') \"" + f.fldLabel + "\" ";
				break;
			case 5:
			case 6:
			case 7:  // users
				alias = "us_"+f.fldfield;
				choices += "left join "+dbname+".dbo.trkusr as " + alias 
					+ " on " + f.fldfield + " = " + alias + ".usrId ";
				res += alias + ".usrFullNm \"" + f.fldLabel + "\" ";
				break;
			default:
				res += f.fldtable + "." + f.fldfield + " " + "\"" + f.fldLabel + "\" ";
			}
			first = false;
		}
	}
	res += " from "+dbname + ".dbo.trkscrsl left join "+dbname + ".dbo.trkscrst on trkscrsl.id = trkscrst.id "+choices;
	//QMessageBox::information(0, "Select", res, QMessageBox::Ok);
	return res;
}


bool TrkDb::readGroups()
{
	QSqlQuery query(db);
	if(!check(query.exec("select ugId, ugClass, ugFlags, ugName from "+dbname+".dbo.trkug")))
		return false;
	while(query.next())
	{
		TrkGroup g;
		QSqlRecord rec = query.record();
		g.ugId = rec.value(0).toInt();
		g.ugClass = rec.value(1).toInt();
		g.ugFlags = rec.value(2).toInt();
		g.ugName = rec.value(3).toInt();
		QSqlQuery u(db);
		u.prepare("select ugeUsrId from "+dbname+".dbo.trkuge where ugeUgId = :group");
		u.bindValue(":group",g.ugId);
		if(!check(u.exec()))
			return false;
		while(u.next())
		{
			g.users.append(u.value(0).toInt());
		}
		u.clear();
		groups[g.ugId]=g;
	}
	return true;
 }

int TrkDb::getFieldId(const QString &fldname)
{
	FieldCol::ConstIterator ff;
	for(ff=flds.constBegin(); ff!=flds.constEnd(); ff++)
		if(ff.value().fldLabel==fldname)
			return ff.key();
	return -1;
}

bool TrkDb::findScrs(const QList<int> &ids, QList<int> &found)
{
	QStringList idSList;
	QString numbers;
	for(int i=0; i<ids.count(); i++)
		idSList.append(QString::number(ids[i]));
	numbers = idSList.join(", ");

	DEBUGINFO("ID numbers", numbers);	

	QString res="select ";
	for(int i = 0; i<fldIds.count(); i++)
	{
		int id = fldIds[i];
		const TrkField &f = flds[id];
		if(f.fldVid == 10001 && f.fldFlags != 2)
		{
			res += f.fldfield;
			res += " from " + f.fldtable;
			res += " where "+f.fldfield + " in ("+numbers+")";
			QSqlQuery q(db);
			if(q.exec(res))
			{
				while(q.next())
				{
					found.append(q.value(0).toInt());
				}
				if(found.count())
					return true;
			}
		}
	}
	return false;
}

QList<class TrkDb *> TrkDb::dblist;

TrkDb *TrkDb::defaultDb()
{
	if(dblist.count())
		return dblist.at(0);
	return NULL;
}

// TField... ====================================================

void TFieldConds::parseCondStr(QStringList::const_iterator i)
{
	int fieldsCond = i->toInt();
	i++;
	for(int j=0; j<fieldsCond; j++)
	{
		//QMessageBox::information(0, "Field cont", *i, QMessageBox::Ok);
		int fieldId = i->toInt();
		i++;
		TrkField f = db->flds[fieldId];
		TCondField *cf = NULL;
		switch(f.fldClass)
		{
		case 1: // choice
			cf = new TCondChoice(db);
			break;
		case 2: // string
			cf = new TCondString(db);
			break;
		case 3: // number
			cf = new TCondNum(db);
			break;
		case 4: // date
			cf = new TCondDate(db);
			break;
		case 5: // submitter
		case 6: // owner
		case 7: // user
			cf = new TCondUser(db);
			break;
		case 8: // elapsed time
			cf = new TCondNum(db);
			break;
		}
		if(&cf)
		{
			QStringList fc=i->split(',');
			QStringList::const_iterator fi=fc.constBegin();
			cf->parseOp(f,fi);
			i++;
			cf->fid = fieldId;
			cf->rel = (i++)->toInt();
			cf->par = (i++)->toInt();
			conds.append(cf);
		}
	}
}

QString TFieldConds::makeQuery() const
{
	QString s;
	for(QList<TCond*>::const_iterator i=conds.constBegin(); i!=conds.constEnd(); i++)
	{
		TCond *c = *i;
		TCondField *f =  dynamic_cast<TCondField *>(c);
		if(i!=conds.constBegin())
			if(c->rel == 1)
				s += " AND ";
			else
				s += " OR ";
		if(f && f->par & 0x2)
			s+= '(';
		if(f && f->par & 0x1)
			s += " not (" + c->doMakeQuery() + ")";
		else
			s += " " + c->doMakeQuery();
		if(f && f->par & 0x4)
			s+= ')';
	}
#ifdef _DEBUG
	QMessageBox::information(0, "Where", s, QMessageBox::Ok);
#endif
	if(s.isEmpty())
		return "";
	return "("+s+")";
}

QString TCondField::makeQuery() const
{
	TrkField &f = db->flds[fid];
	return makeQueryField(f);
}

void TCondNum::parseOp(TrkField & /* f */, QStringList::const_iterator i)
{
	op=i->toInt();
	i++;
	switch(op)
	{
	case 1:
	case 3:
	case 4:
		value1 = *i++;
		break;
	case 2:
		value1 = *i++;
		value2 = *i++;
		break;
	}
}

QString TCondNum::makeQueryField(TrkField &f) const
{
	switch(op)
	{
	case 1:
		return f.fldfield + " = " + value1;
	case 2:
		return 
			"("
			+ f.fldfield + " >= " + value1
			+ " and "
			+ f.fldfield + " <= " + value2
			+ ")";
	case 3:
		return f.fldfield + " > " + value1;
	case 4:
		return f.fldfield + " < " + value1;
	}
	return "";
}

void TCondChoice::parseOp(TrkField & /* f */, QStringList::const_iterator i)
{
	op=i->toInt();
	i++;
	switch(op)
	{
	case 1:
		//w.append(f.fldfield + " = 0 ");
		break;
	case 3:
		int chcount = i->toInt();
		i++;
		for(int j=0; j<chcount; j++)
		{
			values.append((i++)->toInt());
		}
	}
}

QString TCondChoice::makeQueryField(TrkField &f) const
{
	QString res = "1=1";
	switch(op)
	{
	case 1:
		res = f.fldfield + " = 0 ";
		break;
	case 3:
		int chcount = values.count();
		//res=f.fldfield + " in (" ;
		res  = "(";
		for(int j=0; j<chcount; j++)
		{
			if(j)
				res +=" OR ";
			res += f.fldfield + " = " + QString::number(values[j]);
		}
		res += ")";
	}
	return res;
}

void TCondUser::parseOp(TrkField & /* f */, QStringList::const_iterator i)
{
	int count = (i++)->toInt();
	uint flags = i->toUInt();
	if (flags & 0x80000000)
	{
		byGroup = flags & 0x1;
		onlyActive = flags & 0x2;
		onlyDeleted = flags &0x4;
		i++;
	}
	else
	{
		byGroup = false;
		onlyActive = false;
		onlyDeleted =false;
	}
	for(int j=0; j<count; j++)
	{
		int id = (i++)->toInt();
		if(!id)
			id = 0 - (i++)->toInt();
		ids.append(id);
	}
}

QString TCondUser::makeQueryField(TrkField &f) const
{
	QString res;
	if(byGroup)
	{
		res = f.fldtable + "." + f.fldfield + " in (";
		QString list;
		for(int j=0; j<ids.count(); j++)
		{
			int id = ids[j];
			if(!db->groups.contains(id))
				continue;
			TrkGroup &g = db->groups[id];
			for(int iu=0; iu<g.users.count(); iu++)
			{
				if(iu)
					list+=",";
				int user = g.users[iu];
				list += QString::number(user);
			}
		}
		/*
		for(int j=0; j<ids.count(); j++)
		{
			int id = ids[j];
			if(j)
				res += ',';
			if(id>=0)
				res += QString::number(id);
			else switch(id)
			{
			case -1:
				res += "0";
				break;
			case -2:
				res += QString::number(db->curUserId);
				break;
			case -3:
				res += "0," +QString::number(db->curUserId);
				break;
			}
		}
		*/
		if(list.isEmpty())
			list = "-1";
		res += list + ")";
	}
	else
	{
		//res = f.fldtable + "." + f.fldfield + " in (";
		res = "(";
		for(int j=0; j<ids.count(); j++)
		{
			int id = ids[j];
			if(j)
				res += " OR ";
			res += f.fldtable + "." + f.fldfield + " = ";
			if(id>=0)
				res += QString::number(id);
			else switch(id)
			{
			case -1:
				res += "0";
				break;
			case -2:
				res += QString::number(db->curUserId);
				break;
			case -3:
				res += QString::number(db->curUserId);
				res += " OR " + f.fldtable + "." + f.fldfield + " = 0";
				break;
			}
		}
		res += ")";
	}
	return res;
}

void TCondDate::parseOp(TrkField & /* f */, QStringList::const_iterator i)
{
	/*
op, flag1, num1, [flag2, num2]
op: 1 = equals, 2 = between, 3 = greater than, 4 = less than.
If op = 1, 3 or 4, then only flag1 and num1 is used. 
If op = 2, then use both pairs.
flag1, flag2: 
	0 = use the next number as the date value; 
	1 = use current date/time as the date value; 
	2 = <<Unassigned>>.
num1, num2: the date value, if the flag is 0.
	*/
	op = (i++)->toInt();
	switch(op)
	{
	case 1:
	case 3:
	case 4:
		flags1 = (i++)->toInt();
		value1 = (i++)->toInt();
		break;
	case 2:
		flags1 = (i++)->toInt();
		value1 = (i++)->toInt();
		flags2 = (i++)->toInt();
		value2 = (i++)->toInt();
		break;
	}
}

QString TCondDate::makeQueryField(TrkField &f) const
{
	int v1,v2;
	switch(flags1)
	{
	case 0:
		v1 = value1;
	case 1:
		v1 = (QDate(1,1,1970).daysTo(QDate::currentDate())-value1)*24*60*60;
	case 2:
		v1 = 0;
	}
	switch(flags2)
	{
	case 0:
		v2 = value2;
	case 1:
		v2 = (QDate(1,1,1970).daysTo(QDate::currentDate())-value2)*24*60*60;
	case 2:
		v2 = 0;
	}
	QString res;
	switch(op)
	{
	case 1: //equals
		return f.fldfield + " = " + QString::number(v1);
	case 2:
		res += '(';
		if(v1)
			res+=f.fldfield + " >= " + QString::number(v1);
		if(v1 && v2)
			res+=" AND ";
		if(v2)
			res+=f.fldfield + " < " + QString::number(v2);
		res += ')';
		return res;
	case 3:
		return f.fldfield + " > " + v1;
	case 4:
		return f.fldfield + " < " + v1;
	}
	return "1=1";
}

void TCondString::parseOp(TrkField & /* f */, QStringList::const_iterator i)
{
	caseSens = (i++)->toInt();
	value = *i++;
}

QString TCondString::makeQueryField(TrkField &f) const
{
	QString mask = value;
	mask.replace("*","%");
	mask.replace("?",".");
	return f.fldfield + " like \'" + mask + "\'";
}

// TrkModel ===========================================

TrkModel::TrkModel(QObject *parent) 
	: 
	QObject(parent),
	queryModel(parent), 
	userFilter(parent),
	dependFilter(parent)
{
	query = NULL;
	conds = NULL;
	dependFilter.setSourceModel(&queryModel);
	userFilter.setSourceModel(&dependFilter);
	model = &userFilter;
	//model = &queryModel;
	//model = &dependFilter;
}

TrkModel::~TrkModel()
{
	if(query)
		delete query;
	if(conds)
		delete conds;
}

void TrkModel::setQuery(int queryId, TrkDb *db)
{
	trkdb=db;
	origStr = trkdb->getQueryById(queryId);
	if(query)
	{
		delete query;
		query = NULL;
	}
	query = new QSqlQuery(trkdb->db);
	if(conds)
	{
		delete conds;
		conds = NULL;
	}
	conds = new TFieldConds(trkdb);
	reread();
}

void TrkModel::setQueryById(const QString& numbers, TrkDb *db)
{
	trkdb=db;
	origStr = trkdb->getQueryBySCRs(numbers);
	if(query)
	{
		delete query;
		query = NULL;
	}
	query = new QSqlQuery(trkdb->db);
	if(conds)
	{
		delete conds;
		conds = NULL;
	}
	conds = new TFieldConds(trkdb);
	reread();
}

void TrkModel::reread()
{
	QString queryStr=origStr;
	QString cs=conds->makeQuery();
	if(!cs.isEmpty())
		queryStr += " AND " + cs;
	if(!query->exec(queryStr))
	{
		QMessageBox::critical(0, "Cannot open database",
			queryStr + "\n" 
			+ query->lastError().text(), 
			QMessageBox::Cancel);
		return;
	}
	queryModel.setQuery(*query);
}

QVariant TrkModel::fieldByName(const QString& field, int row)
{
	QSqlRecord rec=queryModel.record(row);
	return rec.field(field).value();
/*
	int cid=getColNum("Id");
	int cf=getColNum(field);
	if(cid==-1 || cf ==-1)
		return QVariant();
	for(int r=0; r<queryModel.rowCount(); r++)
	{
		if(queryModel.index(r,cid).data().toInt() == id)
			return queryModel.index(r,cf).data();
	}
	*/
}

int TrkModel::getColNum(const QString &fieldName)
{
	for(int i=0; i<queryModel.columnCount(); i++)
		if(!fieldName.compare(queryModel.headerData(i,Qt::Horizontal).toString(),
			Qt::CaseInsensitive))
			return i;
	return -1;
}

QSqlRecord TrkModel::record(const QModelIndex& index)
{
	return queryModel.record(index.row());
}

//TrkRecord TrkModel::record(int id)
//{
//	for(int c=0;  c<queryModel.columnCount(); c++)
//	{
//		int fid = trkdb->fldIds[c];
//		const TrkField &f = trkdb->flds[fid];
//		if(!f.fldfield.compare("Id"))
//		{
//			for(int r=0;  c<queryModel.columnCount(); c++)
//			return 
//		int fid = trkdb->fieldByLabel["Id"];
//
//}

TQNotesCol *TrkModel::notes(int row)
{
	QSqlQuery q(trkdb->db);
    TQNotesCol *res= new TQNotesCol;
	q.prepare(QString("select "
		"noteId, noteTitle, noteAuthId, noteText, "
		" dateadd(second, case noteCrDate when 0 then NULL else noteCrDate end,'01/01/1970 04:00'), "
		" dateadd(second, case noteMdDate when 0 then NULL else noteMdDate end,'01/01/1970 04:00') "
		" from trknote where noteTbId = 8000 and noteRecId=:id order by noteId"));
	int id = fieldByName("InternalId",row).toInt();
	q.bindValue(":id",id);
	if(!q.exec())
        QMessageBox::critical(0, "Cannot read notes",
			q.lastError().text(), QMessageBox::Cancel);
	while(q.next())
	{
        TQNote n;
		n.title = q.value(1).toString();
		n.text = q.value(3).toString();
		int a = q.value(2).toInt();
		const TrkUser &u =trkdb->users[a];
		n.author = u.usrFullNm.trimmed();
		n.crdate = q.value(4).toDateTime();
		n.mddate = q.value(5).toDateTime();
		res->append(n);
	}
	return res;
}

// TrkQryModel ========================================

int	TrkQryModel::columnCount(const QModelIndex & /* parent */) const
{
	return 9;
}

QVariant TrkQryModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid())
		return QVariant();
	int row=index.row();
	if(role ==Qt::UserRole)
		return queries[queryIds[row]].qryId;
	switch(role)
	{
	case Qt::DisplayRole:
		switch(index.column())
		{
		case QryId:
			return queries[queryIds[row]].qryId;
		case QryName:
			return queries[queryIds[row]].qryName; // char(64) The name of the query. If it is empty, then it is an embedded query from a report.
		case QryAuhId:
			return queries[queryIds[row]].qryAuthId; // int The user ID that originally authored this query. Foreign key: usrId in the trkusr table.
		case QryOwnerId:
			return queries[queryIds[row]].qryOwnerId; // int The user ID that currently owns this query. Stored separately from author ID for permission reasons and in case the original author is deleted. The initial owner is the author. Foreign key: usrId in the trkusr table.
		case QryCrDate:
			return queries[queryIds[row]].qryCrDate; // int The date and time this report was created. Tracker does not display this value anywhere.
		case QryMdDate:
			return queries[queryIds[row]].qryMdDate; // int The date and time this report was last modified. Tracker does not display this value anywhere.
		case QryCont:
			return queries[queryIds[row]].qryCont; // text A null terminated, comma-separated string representing the contents of the query. See Notes below.
		case QryComment:
			return queries[queryIds[row]].qryComment; // varchar(254) A comment about the query. Tracker displays it in the Query Run dialog.
		case QryDescr:
			return queries[queryIds[row]].qryDescr; // varchar(254) Reserved for future use.
		}
		break;
	case Qt::CheckStateRole:
		return QVariant();
		
	}
	return QVariant();
}

QVariant TrkQryModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	if(role !=Qt::DisplayRole)
		return QVariant();
	if(orientation != Qt::Horizontal)
		return QVariant();
	switch(section)
	{
	case QryId:
		return "Id";
	case QryName:
		return tr("Название");
	case QryAuhId:
		return tr("Автор");
	case QryOwnerId:
		return tr("Владелец");
	case QryCrDate:
		return tr("Создан");
	case QryMdDate:
		return tr("Изменен");
	case QryCont:
		return tr("Определение");
	case QryComment:
		return tr("Комментарий");
	case QryDescr:
		return tr("Описание");
	}
	return QVariant();
}

int	TrkQryModel::rowCount(const QModelIndex & /* parent */) const
{
	return queries.count();
}

QModelIndex	TrkQryModel::index ( int row, int column, const QModelIndex & /* parent */) const
{
	return createIndex(row,column);
}

QModelIndex TrkQryModel::parent(const QModelIndex & /* child */) const
{
	return QModelIndex();
}

bool TrkQryModel::read(TrkDb *trkdb)
{
	QSqlQuery query(trkdb->db);
	query.prepare("select "
		"qryId, "
		"qryClass, "
		"qryFlags, "
		"qryTypeId, "
		"qryName, "
		"qryAuthId, "
		"qryOwnerId, "
		"qryCrDate, "
		"qryMdDate, "
		"qryTitle, "
		"qryCont, "
		"qryComment, "
		"qryDescr "
		"from trkqry"
		" where "
		" qryName is not null"
		" and (qryAuthId = :cur or qryOwnerId = :cur or qryAuthId = 1)"
		" order by qryAuthId, qryName "
		);
	query.bindValue(":cur", trkdb->curUserId);
	if(!query.exec())
        QMessageBox::critical(0, "Cannot read queries",
			query.lastError().text(), QMessageBox::Cancel);
	while(query.next())
	{
		TrkQuery q;
		q.qryId = query.value(0).toInt();
		q.qryClass = query.value(1).toInt();
		q.qryFlags = query.value(2).toInt();
		q.qryTypeId = query.value(3).toInt();
		q.qryName = query.value(4).toString().trimmed();
		q.qryAuthId = query.value(5).toInt();
		q.qryOwnerId = query.value(6).toInt();
		q.qryCrDate = query.value(7).toInt();
		q.qryMdDate = query.value(8).toInt();
		q.qryTitle = query.value(9).toString().trimmed();
		q.qryCont = query.value(10).toString().trimmed();
		q.qryComment = query.value(11).toString().trimmed();
		q.qryDescr = query.value(12).toString().trimmed();
		queries[q.qryId] = q;
		queryIds.append(q.qryId);
	}
	return true;
}


// ==================== TrkRecord ================

TrkRecord::TrkRecord(TrkRecordSet *parent)
	:QObject(parent)
{
	recordset = parent;
}

QVariant TrkRecord::operator[](const QString& fieldName) const
{
	return data(fieldName);
}

QVariant TrkRecord::operator[](int fieldNum) const
{
	return data(fieldNum);
}

QVariant TrkRecord::data(const QString& fieldName) const
{
	int f = recordset->fieldNum(fieldName);
	return values[f];
}

QVariant TrkRecord::data(int fieldNum) const
{
	return values[fieldNum];
}

void TrkRecord::setValue(const QString& fieldName, const QVariant& value)
{
	int f = recordset->fieldNum(fieldName);
	values[f] =  value;
}

// ==================== TrkRecordSet ================

TrkRecordSet::TrkRecordSet(/*TrkDb *db,*/ QObject *parent)
:QObject(parent)
{
	//trkdb=db;
}
int TrkRecordSet::fieldNum(const QString &field) const
{
	return fields.indexOf(field);
}
QString TrkRecordSet::fieldName(int index) const
{
	return fields[index];
}
int TrkRecordSet::recCount() const
{
	return records.count();
}
int TrkRecordSet::fieldCount() const
{
	return fields.count();
}
QVariant TrkRecordSet::data(int record, int fieldNum) const
{
	TrkRecord *r = records[record];
	if(r)
		return r->data(fieldNum);
	return QVariant();
}

void TrkRecordSet::clear()
{
	while(!records.isEmpty())
	{
		TrkRecord *rec = records.takeFirst();
		delete rec;
	}
	fields.clear();
}

void TrkRecordSet::readQuery(QSqlQuery * query)
{
	clear();
	bool first=true;
	//query->first();
	while(query->next())
	{
		QSqlRecord qrec=query->record();
		if(first)
		{
			for(int i=0; i<qrec.count(); i++)
				fields.append(qrec.fieldName(i));
			first=false;
		}
		TrkRecord *trec=new TrkRecord(this);
		for(int i=0; i<qrec.count(); i++)
		{
			trec->setValue(qrec.fieldName(i),qrec.value(i));
		}
		records.append(trec);
	}
}

// ==================== TrkItemModel ================

TrkItemModel::TrkItemModel(TrkDb *db, QObject *parent)
: QAbstractItemModel(parent), scrs(), rset(/*db, */parent)
{
	trkdb = db;
}

TrkItemModel::~TrkItemModel()
{
	close();
}

int	TrkItemModel::columnCount(const QModelIndex & parent) const
{
	if(parent.isValid())
		return 0;
	return rset.fieldCount();
}

QVariant TrkItemModel::data(const QModelIndex & index, int role) const
{
	if(!index.isValid() || index.row()>=rowCount())
		return QVariant();
	switch(role)
	{
	case Qt::DisplayRole:
		return rset.data(index.row(), index.column());
	case Qt::CheckStateRole:
		if(index.column()==0)
		{
			int scr = rset.data(index.row(), 0).toInt();
			return QVariant(trkdb->selectedScr.contains(scr));
		}
		else
			return QVariant();
	}
	return QVariant();
}

QVariant TrkItemModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal)
		return QVariant();
	else if(role != Qt::DisplayRole)
		return QVariant();
	else
	{
		if(section <0 || section>=columnCount())
			return QVariant();
		return QVariant(rset.fieldName(section));
	}
}

int	TrkItemModel::rowCount(const QModelIndex & parent) const
{
	if(parent.isValid())
		return 0;
	return rset.recCount();
}

QModelIndex	TrkItemModel::index( int row, int column, const QModelIndex & parent) const
{
	if(parent.isValid())
		return QModelIndex();
	if(row<0 || row>=scrs.count())
		return  QModelIndex();
	if(column<0 || column>=trkdb->flds.count())
		return  QModelIndex();
	return createIndex(row, column, scrs[row]);
}

QModelIndex TrkItemModel::parent(const QModelIndex &) const
{
	return QModelIndex();
}

// -----------

bool TrkItemModel::openScrs(const QList<int> &ids)
{
	bool res = false;
	emit layoutAboutToBeChanged();
	close();
	QList<int> found;
	if(!trkdb->findScrs(ids,found))
	{
		emit layoutChanged();
		return false;
	}
	QStringList idSList;
	QString numbers;
	for(int i=0; i<found.count(); i++)
		idSList.append(QString::number(found[i]));
	numbers = idSList.join(", ");
	QString qsel = trkdb->getQueryBySCRs(numbers);
	QSqlQuery query(trkdb->db);
	if(query.exec(qsel))
	{
		rset.readQuery(&query);
		scrs = found;
		res = true;
	}
	else
		DEBUGERROR("Select by ID","Error:\n" + trkdb->db.lastError().text());
	emit layoutChanged();
	return res;
}

bool TrkItemModel::openScrs(const QStringList &lines)
{
	return openScrs(lines.join(" "));
}

bool TrkItemModel::openScrs(const QString &line)
{
	QList<int> ids;
	QString iline = line;
	iline.replace(QRegExp("[^0-9]")," ");
	QStringList ilist = iline.split(QChar(' '), QString::SkipEmptyParts);
	for(int i=0; i<ilist.count(); i++)
	{
		bool ok;
		int id = ilist.at(i).toInt(&ok);
		if(ok && !ids.contains(id))
			ids.append(id);
	}
	return openScrs(ids);
}

void TrkItemModel::close()
{
	emit layoutAboutToBeChanged();
	scrs.clear();
	rset.clear();
	emit layoutChanged();
}


//TrkNote::TrkNote()
//{
//}

//TrkNote::TrkNote(const TrkNote &src)
//{
//    title = src.title;
//    author = src.author;
//    text = src.text;
//    crdate = src.crdate;
//    mddate = src.mddate;
//    perm = src.perm;

//}
