#ifndef _TRDEFS_H_
#define _TRDEFS_H_

#include <QSet>
#include <QList>
#include <QStringList>

typedef QSet<int> ScrSet;
typedef QMap<int, ScrSet> ScrSetList;

inline ScrSet scrStringToSet(const QString & scrString)
{
	QString s = scrString;
	s.replace(QRegExp("[^0-9 ,]")," ");
	s.replace(' ',',');
	QStringList list = s.split(',',QString::SkipEmptyParts);
	ScrSet scrSet;
	for(int i=0; i<list.count(); i++)
	{
		bool ok;
		int n=list[i].toInt(&ok);
		if(ok)
			scrSet << n;
	}
	return scrSet;
}

class QWidget;

#endif //#ifndef _TRDEFS_H_
