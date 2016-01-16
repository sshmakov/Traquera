// CommaNum.cpp : Defines the entry point for the application.
//

#include "cliputil.h"

#include <QtCore>

#include <QClipboard>
#include <QApplication>

//#include "../tracker/trkview.h"
#include "tqbase.h"

#define TABLE_HEADER "<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>"
#define ROW_START    "<tr>"
#define CELL_START   "<td valign='top'>"

QString copyRecordsToHTMLTable(const QObjectList &records, const QStringList &fields, bool withHeaders)
{
    QString html;
    html = TABLE_HEADER;
    if(withHeaders)
    {
        html.append(ROW_START);
        foreach(const QString &f, fields)
            html.append(QString(CELL_START "%1</td>").arg(f));
        html.append("</tr>");
    }
    foreach(QObject *obj, records)
    {
        TQRecord *rec = (TQRecord *)obj;
        html.append(ROW_START);
        foreach(const QString &f, fields)
            html.append(QString(CELL_START "%1</td>").arg(rec->value(f).toString()));
        html.append("</tr>");
    }
    html.append("</table>");
    return html;
}

#ifdef AAAA_CLIP
void ClipToUnique(const QString & format)
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    if(mimeData->hasFormat(format))
    {
        QByteArray data = mimeData->data(format);
        QString clipText = QString::fromAscii(data.constData());
        clipboard->setText(
                    intListToString(
                        uniqueIntList(
                            clipText)));
    }
}

int compare(const void *p1, const void *p2)
{
	int i = *(const int*)p1;
	int j = *(const int*)p2;
	return i == j ? 0 : i<j ? -1 : 1;
}

void freemem(void *p)
{
	if(p)
		free(p);
}

void MakeDiesList(void)
{
	HGLOBAL   hglb; 
	LPSTR    lpstr; 
	UINT getfrm=CF_TEXT;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
	{
		if (!IsClipboardFormatAvailable(CF_CSV)) 
			return;
		getfrm=CF_CSV;
	}
	if (!OpenClipboard(hWnd)) 
		return; 
	hglb = GetClipboardData(getfrm); 
	if (hglb != NULL) 
	{ 
		LPSTR buf=NULL;
		int len=0;
		lpstr = (LPSTR)GlobalLock(hglb); 
		if (lpstr != NULL) 
		{ 
			buf = GetUnique(lpstr);
			len=strlen(buf);
		}
		GlobalUnlock(hglb); 
		int *arr=(int*)malloc(len*sizeof(int));
		int count=ParseToIntArr(buf,arr);
		freemem(buf);
		LPSTR newbuf=ArrToUnique(arr,count,true);
		freemem(arr);
		SetToClip(newbuf);
		freemem(newbuf);
	} 
	CloseClipboard(); 
}

class CSVRecord
{
public:
	LPSTR *values;
	int count;
	void setValue(int index, LPSTR newvalue) // index=0..
	{
		if(index>=count)
		{
			LPSTR *nv =(LPSTR *)malloc(sizeof(LPSTR *)*(index+1));
			memset(nv,0,sizeof(LPSTR *)*(index+1));
			memcpy(nv,values,sizeof(LPSTR *)*count);
			if(values)
				free(values);
			values=nv;
			count=index+1;
		}
		if(values[index])
			free(values[index]);
		values[index]=(LPSTR)malloc(strlen(newvalue)+1);
		strcpy(values[index],newvalue);
				
	}
	CSVRecord()
	{
		values=NULL;
		count=0;
	}
	~CSVRecord()
	{
		for(int i=0; i<count; i++)
		{
			if(values[i])
				free(values[i]);
		}
	}
};

class CSVSet
{
public:
	CSVRecord **records;
	int rows;
	int cols;
	CSVSet()
	{
		records=NULL;
		rows=0;
		cols=0;
	}
	void clear()
	{
		for(int i =0; i<rows; i++)
			delete records[i];
		rows=0;
		cols=0;
		records=NULL;
	}
	~CSVSet()
	{
		clear();
	}
	void parse(LPSTR text);
	void setValue(int r, int c, LPSTR text) // r=0..., c=0..
	{
		if(r >= rows)
		{
			CSVRecord **nr = (CSVRecord**)malloc(sizeof(CSVRecord*)*(r+1));
			memset(nr,0, sizeof(CSVRecord*)*(r+1));
			memcpy(nr,records, sizeof(CSVRecord*)*rows);
			if(records)
				free(records);
			records=nr;
			rows=r+1;
			records[r]=new CSVRecord();
		}
		records[r]->setValue(c,text);
		if(c>=cols)
			cols=c+1;
	}
	LPSTR value(int r, int c)
	{
		if(r<0 || r>=rows || c<0 || c>=cols)
			return "";
		CSVRecord *v=records[r];
		if(!v || c>=v->count)
			return "";
		LPSTR res = v->values[c];
		if(!res)
			return "";
		return res;
	}
	LPSTR valueByName(int r, LPSTR colname)
	{
		if(r<1 || r>=rows)
			return "";
		CSVRecord *h=records[0];
		if(!h)
			return "";
		int col=-1;
		for(int i=0; i<h->count; i++)
			if(_stricmp(h->values[i],colname)==0)
			{
				col=i;
			}
		CSVRecord *v=records[r];
		if(!v || col<0 || col>=v->count)
			return "";
		return v->values[col];
	}
};

void CSVSet::parse(LPSTR text)
{
	LPSTR p=text;
	const int maxlen=1024;
	CHAR buf[1024];
	LPSTR beg=NULL;
	int row=0;
	while(*p)
	{
		LPSTR k=strchr(p,13);
		if(!k)
		{
			k=strchr(p,10);
			if(!k)
				k=p+strlen(p);
		}
		int col=0;
		while(*p && p<k)
		{
			if(*p != '"')
			{
				beg=p;
				LPSTR n=strchr(p,',');
				if(!n)
					n=p+strlen(p);
				if(n>k)
					n=k;
				p=n;
				int len=p-beg;
				if(len>=maxlen)
					len=maxlen-1;
				strncpy(buf,beg,len);
				buf[len]=0;
				setValue(row,col++,buf);
				if(*p==',')
					p++;
			}
			else
			{
				p++;
				beg=p;
				LPSTR dest=buf;
				int len=0;
				bool is_end=false;
				while(!is_end && len<maxlen-1)
				{
					CHAR c=*p++;
					switch(c)
					{
					case '"':
						if(*p=='"')
						{
							*dest++='"';
							len++;
							p++;
						}
						else
						{
							*dest++=0;
							len++;
							is_end=true;
						}
						break;
					//case ',':
					//	*dest++=0;
					//	len++;
					//	is_end=true;
					//	break;
					default:
						*dest++=c;
						len++;
					}
				}
				buf[len]=0;
				setValue(row,col++,buf);
				if(*p==',')
					p++;
			}
		}
		if(*p==13)
			p++;
		if(*p==10)
			p++;
		row++;
	}
}



LPSTR *addline(LPSTR *dest, LPSTR src)
{
	int newlen= strlen(*dest)+ strlen(src);
	LPSTR res=(LPSTR)malloc(newlen+1);
	strcpy(res,*dest);
	strcat(res,src);
	free(*dest);
	*dest=res;
	return dest;
}



void SetToClipHTML(LPSTR text)
{
	const LPSTR header=
		"Version:0.9\r\n"
		"StartHTML:-1\r\n"
		"EndHTML:-1\r\n"
		"StartFragment:000081\r\n"
		"EndFragment:";
	int widelen=strlen(text)+1;
	int widesize=widelen*sizeof(WCHAR);
	LPWSTR wide=(LPWSTR)malloc(widesize);
	MultiByteToWideChar(1251,0,text,-1,wide,widelen);
	int utflen=widelen*4+1;
	LPSTR utf=(LPSTR)malloc(utflen);
	int utfcount = WideCharToMultiByte(CP_UTF8,0,wide,-1,utf,utflen,NULL,NULL);
	//int utfcount = UnicodeToUtf8(wide,utf,utflen);
	utf[utfcount]=0;
	free(wide);

	int hlen=strlen(header);
	char end[16];
	_itoa(utfcount+hlen+2+6+100000,end,10);
	end[0]--;
	LPSTR res=(LPSTR)malloc(1);
	res[0]=0;
	addline(&res,header);
	addline(&res,end);
	addline(&res,"\r\n");
	addline(&res,utf);
	//SetToClip(res);
	SetToClip(res,CF_HTML);
	free(res);
}

/*
void CSVToTable(LPSTR text, LPSTR *list, LPSTR *html)
{
	CSVSet set;
	set.parse(text);
	*html=(LPSTR)malloc(1);
	*html[0]=0;
	addline(html,"<html>\r\n<body>\r\n<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>\r\n");
	*list=(LPSTR)malloc(1);
	*list[0]=0;
	for(int r=1; r<set.rows; r++)
	{
		addline(html,"<tr>");
		LPSTR id = set.valueByName(r,"Id");
		LPSTR title = set.valueByName(r,"Title");
		if((id && *id) || (title && *title))
		{
			addline(html,"<td valign='top'>");
			addline(html,id);
			addline(html,"</td>");
			addline(html,"<td valign='top'>");
			addline(html,title);
			addline(html,"</td>");
			addline(list,id);
			addline(list,"\t");
			addline(list,title);
			addline(list,"\r\n");
		}
		addline(html,"</tr>\r\n");
	}
	addline(html,"</table>\r\n</body>\r\n</html>\r\n");
}


*/

void MakeTableIdTitle()
{
	HGLOBAL   hglb; 
	LPSTR    lpstr; 
	UINT getfrm=CF_TEXT;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
	{
		if (!IsClipboardFormatAvailable(CF_CSV)) 
			return;
		getfrm=CF_CSV;
	}
	if (!OpenClipboard(hWnd)) 
		return; 
	hglb = GetClipboardData(getfrm); 
	if (hglb != NULL) 
	{ 
		LPSTR list=NULL;
		LPSTR html=NULL;
		int len=0;
		lpstr = (LPSTR)GlobalLock(hglb); 
		if (lpstr != NULL) 
		{ 
			//CSVToTable(lpstr,&list,&html);
			CSVSet set;
			set.parse(lpstr);
			html=(LPSTR)malloc(1);
			html[0]=0;
			addline(&html,"<html>\r\n<body>\r\n<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>\r\n");
			list=(LPSTR)malloc(1);
			list[0]=0;
			for(int r=1; r<set.rows; r++)
			{
				addline(&html,"<tr>");
				LPSTR id = set.valueByName(r,"Id");
				LPSTR title = set.valueByName(r,"Title");
				if((id && *id) || (title && *title))
				{
					addline(&html,"<td valign='top'>");
					addline(&html,id);
					addline(&html,"</td>");
					addline(&html,"<td valign='top'>");
					addline(&html,title);
					addline(&html,"</td>");
					addline(&list,id);
					addline(&list,"\t");
					addline(&list,title);
					addline(&list,"\r\n");
				}
				addline(&html,"</tr>\r\n");
			}
			addline(&html,"</table>\r\n</body>\r\n</html>\r\n");
		}
		GlobalUnlock(hglb); 
		SetToClipHTML(html);
		SetToClip(list,CF_TEXT,false);
		freemem(list);
		freemem(html);
	} 
	CloseClipboard(); 
}

void MakeTableFull()
{
	HGLOBAL   hglb; 
	LPSTR    lpstr; 
	UINT getfrm=CF_TEXT;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
	{
		if (!IsClipboardFormatAvailable(CF_CSV)) 
			return;
		getfrm=CF_CSV;
	}
	if (!OpenClipboard(hWnd)) 
		return; 
	hglb = GetClipboardData(getfrm); 
	if (hglb != NULL) 
	{ 
		LPSTR list=NULL;
		LPSTR html=NULL;
		int len=0;
		lpstr = (LPSTR)GlobalLock(hglb); 
		if (lpstr != NULL) 
		{ 
			//CSVToTable(lpstr,&list,&html);
			CSVSet set;
			set.parse(lpstr);
			html=(LPSTR)malloc(1);
			html[0]=0;
			addline(&html,"<html>\r\n<body>\r\n<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>\r\n");
			list=(LPSTR)malloc(1);
			list[0]=0;
			for(int r=0; r<set.rows; r++)
			{
				addline(&html,"<tr>");
				for(int c=0; c<set.cols; c++)
				{
					LPSTR v=set.value(r,c);
					addline(&html,"<td valign='top'>");
					addline(&html,v);
					addline(&html,"</td>");
					if(c)
						addline(&list, "\t");
					addline(&list, v);
				}
				addline(&html,"</tr>\r\n");
				addline(&list, "\r\n");
			}
			addline(&html,"</table>\r\n</body>\r\n</html>\r\n");
		}
		GlobalUnlock(hglb); 
		SetToClipHTML(html);
		SetToClip(list,CF_TEXT,false);
		freemem(list);
		freemem(html);
	} 
	CloseClipboard(); 
}

void DiffMemoryTable()
{
	HGLOBAL   hglb; 
	LPSTR    lpstr; 
	UINT getfrm=CF_TEXT;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
	{
		if (!IsClipboardFormatAvailable(CF_CSV)) 
			return;
		getfrm=CF_CSV;
	}
	if (!OpenClipboard(hWnd)) 
		return; 
	hglb = GetClipboardData(getfrm); 
	if (hglb != NULL) 
	{ 
		LPSTR buf=NULL;
		int len=0;
		lpstr = (LPSTR)GlobalLock(hglb); 
		if (lpstr != NULL) 
		{ 
			buf = GetUnique(lpstr);
			if(buf)
				len=strlen(buf);
		}
		GlobalUnlock(hglb); 
		int memcount=0;
		int bufcount=0;
		int *bufarr=NULL;
		int *memarr=NULL;
		if(len)
		{
			bufarr=(int*)malloc(len*sizeof(int));
			memset(bufarr,0,len*sizeof(int));
		}
		if(buf && bufarr)
			bufcount=ParseToIntArr(buf,bufarr);
		int memlen=0;
		if(memory)
			memlen=strlen(memory);
		if(memlen && memory)
		{
			memarr=(int*)malloc(memlen*sizeof(int));
			memset(memarr,0,memlen*sizeof(int));
			memcount=ParseToIntArr(memory,memarr);
		}
		//int rescount=0;
		int i=0, j=0;
		char val[20];
		const LPSTR td="<td valign='top'>";
		const LPSTR ed="</td>";
		const LPSTR crlf="\r\n";
		const LPSTR empty="<td valign='top'></td>";
		LPSTR html=(LPSTR)malloc(1);
		html[0]=0;
		addline(&html,"<html>\r\n<body>\r\n<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>\r\n");
		LPSTR list=(LPSTR)malloc(1);
		list[0]=0;
		addline(&html,
			"<tr>"
			"<td valign='top'>Память</td>"
			"<td valign='top'>Буфер</td>"
			"<td valign='top'>Совпадающие</td>"
			"<td valign='top'>Нет в памяти</td>"
			"<td valign='top'>Нет в буфере</td>"
			"</tr>\r\n"
			);
		while(i<bufcount || j<memcount)
		{
			addline(&html,"<tr>");
			if(i<bufcount)
			{
				if(j<memcount)
				{
					if(bufarr[i]<memarr[j])
					{
						_itoa(bufarr[i++],val,10);
						addline(&html,empty);
						addline(&html,td);	addline(&html,val);	addline(&html,ed);
						addline(&html,empty);
						addline(&html,td);	addline(&html,val);	addline(&html,ed);
						addline(&html,empty);
						addline(&list,"\t");
						addline(&list,val);
						addline(&list,crlf);
					}
					else if(bufarr[i]>memarr[j])
					{
						_itoa(memarr[j++],val,10);
						addline(&html,td); addline(&html,val); addline(&html,ed);
						addline(&html,empty);
						addline(&html,empty);
						addline(&html,empty);
						addline(&html,td); addline(&html,val); addline(&html,ed);
						addline(&list,val);
						addline(&list,"\t");
						addline(&list,"\r\n");
					}
					else
					{
						_itoa(bufarr[i++],val,10);
						j++;
						addline(&html,td); addline(&html,val); addline(&html,ed);
						addline(&html,td); addline(&html,val); addline(&html,ed);
						addline(&html,td); addline(&html,val); addline(&html,ed);
						addline(&html,empty);
						addline(&html,empty);
						addline(&list,val);
						addline(&list,"\t");
						addline(&list,val);
						addline(&list,"\r\n");
					}
				}
				else
				{
					_itoa(bufarr[i++],val,10);
					addline(&html,empty);
					addline(&html,td); addline(&html,val); addline(&html,ed);
					addline(&html,empty);
					addline(&html,td); addline(&html,val); addline(&html,ed);
					addline(&html,empty);
					addline(&list,"\t");
					addline(&list,val);
					addline(&list,"\r\n");
				}
			}
			else
			{
				_itoa(memarr[j++],val,10);
				addline(&html,td); addline(&html,val); addline(&html,ed);
				addline(&html,empty);
				addline(&html,empty);
				addline(&html,empty);
				addline(&html,td); addline(&html,val); addline(&html,ed);
				addline(&list,val);
				addline(&list,"\t");
				addline(&list,"\r\n");
			}
			addline(&html,"</tr>");
		}
		addline(&html,"</table>\r\n</body>\r\n</html>\r\n");
		freemem(memarr);
		SetToClipHTML(html);
		SetToClip(list,CF_TEXT,false);
		freemem(html);
		freemem(list);
		freemem(bufarr);
		freemem(buf);
	} 
	CloseClipboard(); 
}



void MakeColumnList(void)
{
	HGLOBAL   hglb; 
	LPSTR    lpstr; 
	UINT getfrm=CF_TEXT;

	if (!IsClipboardFormatAvailable(CF_TEXT)) 
	{
		if (!IsClipboardFormatAvailable(CF_CSV)) 
			return;
		getfrm=CF_CSV;
	}
	if (!OpenClipboard(hWnd)) 
		return; 
	hglb = GetClipboardData(getfrm); 
	if (hglb != NULL) 
	{ 
		LPSTR buf=NULL;
		int len=0;
		lpstr = (LPSTR)GlobalLock(hglb); 
		len=strlen(lpstr);
		int *arr=(int*)malloc(len*sizeof(int));
		int count=0;
		if (lpstr != NULL) 
		{ 
			count=ParseToIntArr(lpstr,arr);
			qsort(arr,count,sizeof(arr[0]),compare);
		}
		GlobalUnlock(hglb); 
		LPSTR html =(LPSTR)malloc(1);
		html[0]=0;
		addline(&html,"<html>\r\n<body>\r\n<table border=1 cellpadding=3 cellspacing=0 bordercolor=#000000 style='border-collapse:collapse;border:none'>\r\n");
		LPSTR list =(LPSTR)malloc(1);
		list[0]=0;
		int prev=0;
		bool first=true;
		for(int i=0; i<count; i++)
		{
			if(arr[i] < 10000 || arr[i] == prev)
				continue;
			addline(&html,"<tr><td valign='top'>");
			char dest[20];
			_itoa(arr[i],dest,10);
			addline(&html,dest);
			addline(&html,"</td></tr>\r\n");
			addline(&list,dest);
			addline(&list,"\r\n");
			prev=arr[i];
		}
		addline(&html,"</table>\r\n</body>\r\n</html>\r\n");
		SetToClipHTML(html);
		SetToClip(list,CF_TEXT,false);
		freemem(html);
		freemem(list);
	}
	CloseClipboard(); 
}




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case ICON_NOTIFY:
		switch (lParam)
		{
		case WM_LBUTTONDOWN:
			DoIt();
			break;
		case WM_RBUTTONUP:
			//DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			POINT pt;
			GetCursorPos(&pt);
            //int x = GET_X_LPARAM(lParam); 
            //int y = GET_Y_LPARAM(lParam); 
			HMENU menu;
			HMENU submenu;
			menu = LoadMenu(hInst,MAKEINTRESOURCE(IDC_POPUP));
			submenu = GetSubMenu(menu,0);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(submenu,
                     TPM_RIGHTBUTTON,
                     pt.x,
                     pt.y,
                     0,
                     hWnd,
                     NULL);
			PostMessage(hWnd, WM_NULL, 0, 0);
			DestroyMenu(menu);
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_COMMA:
			MakeCommaList();
			break;
		case IDM_STORE:
			SetMemory();
			break;
		case IDM_ADD:
			AddMemory();
			break;
		case IDM_DIFF:
			DiffMemory();
			break;
		case IDM_DUP:
			DupMemory();
			break;
		case IDM_GET:
			GetMemory();
			break;
		case IDM_DIES:
			MakeDiesList();
			break;
		case IDM_CLEAR:
			ClearMemory();
			break;
		case IDM_LIST:
			MakeTableIdTitle();
			break;
		case IDM_HTML:
			MakeTableFull();
			break;
		case IDM_TEXT:
			MakeSimpleText();
			break;
		case IDM_CMP:
			DiffMemoryTable();
			break;
		case IDM_COLUMN:
			MakeColumnList();
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			RemoveIcon(hWnd);
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
#endif
