#include "jiraqry.h"

JiraQry::JiraQry(TQAbstractProject *prj, int rectype)
    :TQQueryDef(prj, rectype)
{
}

QString JiraQry::queryLine()
{
    return jql;
}

void JiraQry::setQueryLine(const QString &line)
{
    jql = line;
}
