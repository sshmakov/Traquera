#ifndef CLIPUTIL_H_

#include <QtCore>
//#include "../tracker/trkview.h"

class TrkToolRecord;

QString copyRecordsToHTMLTable(const QObjectList &records, const QStringList &fields, bool withHeaders = true);


#endif
