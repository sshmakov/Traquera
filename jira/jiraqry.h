#ifndef JIRAQRY_H
#define JIRAQRY_H

#include <tqcond.h>

class JiraQry: public TQQueryDef
{
    Q_OBJECT
public:
    JiraQry(TQAbstractProject *prj, int rectype);
    QString queryLine();
    void setQueryLine(const QString &line);
private:
    QString jql;
};

#endif // JIRAQRY_H
