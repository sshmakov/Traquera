#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
}

void Highlighter::clearSyntax()
{
    highlightingRules.clear();
    syntaxName.clear();
    //rehighlight();
}

void Highlighter::setSyntax(const QString &syntax)
{
    clearSyntax();
    if(syntax == "C")
    {
        keywordFormat.setFontFixedPitch(true);
        classFormat.setFontFixedPitch(true);
        functionFormat.setFontFixedPitch(true);
        singleLineCommentFormat.setFontFixedPitch(true);
        multiLineCommentFormat.setFontFixedPitch(true);
        quotationFormat.setFontFixedPitch(true);


        HighlightingRule rule;

        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                        << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                        << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                        << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                        << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                        << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                        << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                        << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                        << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                        << "\\bvoid\\b" << "\\bvolatile\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        classFormat.setFontWeight(QFont::Bold);
        classFormat.setForeground(Qt::darkMagenta);
        rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(Qt::red);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::red);

        quotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

//        functionFormat.setFontItalic(true);
//        functionFormat.setForeground(Qt::blue);
//        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
//        rule.format = functionFormat;
//        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");
    }
    else if(syntax == "RSL")
    {
        HighlightingRule rule;

        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\barray\\b"
                        << "\\bclass\\b"
                        << "\\bconst\\b"
                        << "\\belif\\b"
                        << "\\belse\\b"
                        << "\\bend\\b"
                        << "\\bfile\\b"
                        << "\\bif\\b"
                        << "\\bimport\\b"
                        << "\\blocal\\b"
                        << "\\bmacro\\b"
                        << "\\bonError\\b"
                        << "\\bprivate\\b"
                        << "\\bpublic\\b"
                        << "\\brecord\\b"
                        << "\\breturn\\b"
                        << "\\bthis\\b"
                        << "\\bvar\\b"
                        << "\\bwhile\\b"
                        << "\\bwith\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern,Qt::CaseInsensitive);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        singleLineCommentFormat.setForeground(Qt::red);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::red);

        quotationFormat.setForeground(Qt::darkGreen);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

//        functionFormat.setFontItalic(true);
//        functionFormat.setForeground(Qt::blue);
//        rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
//        rule.format = functionFormat;
//        highlightingRules.append(rule);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");
    }
    else if(syntax == "SQL")
    {
        HighlightingRule rule;

        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns
                << "add"
                << "all"
                << "alter"
                << "and"
                << "any"
                << "as"
                << "asc"
                << "authorization"
                << "backup"
                << "begin"
                << "between"
                << "body"
                << "break"
                << "browse"
                << "bulk"
                << "by"
                << "cascade"
                << "case"
                << "check"
                << "checkpoint"
                << "close"
                << "clustered"
                << "coalesce"
                << "column"
                << "commit"
                << "committed"
                << "compute"
                << "confirm"
                << "constraint"
                << "contains"
                << "containstable"
                << "continue"
                << "controlrow"
                << "create"
                << "cross"
                << "current"
                << "current_date"
                << "current_time"
                << "current_timestamp"
                << "current_user"
                << "cursor"
                << "database"
                << "dbcc"
                << "deallocate"
                << "declare"
                << "default"
                << "delete"
                << "deny"
                << "desc"
                << "disk"
                << "distinct"
                << "distributed"
                << "double"
                << "drop"
                << "dummy"
                << "dynamic"
                << "else"
                << "encryption"
                << "end"
                << "errlvl"
                << "errorexit"
                << "escape"
                << "except"
                << "exec"
                << "exit"
                << "fast_forward"
                << "file"
                << "fillfactor"
                << "floppy"
                << "for"
                << "foreign"
                << "forward_only"
                << "freetext"
                << "freetexttable"
                << "from"
                << "full"
                << "function"
                << "goto"
                << "grant"
                << "group"
                << "having"
                << "holdlock"
                << "identity"
                << "identity_insert"
                << "identitycol"
                << "if"
                << "in"
                << "index"
                << "insensitive"
                << "insert"
                << "instead"
                << "intersect"
                << "into"
                << "is"
                << "isolation"
                << "join"
                << "key"
                << "keyset"
                << "kill"
                << "left"
                << "level"
                << "like"
                << "lineno"
                << "load"
                << "mirrorexit"
                << "national"
                << "nocheck"
                << "nonclustered"
                << "not"
                << "null"
                << "nullif"
                << "of"
                << "off"
                << "offsets"
                << "on"
                << "once"
                << "only"
                << "open"
                << "opendatasource"
                << "openquery"
                << "openrowset"
                << "optimistic"
                << "option"
                << "or"
                << "order"
                << "outer"
                << "over"
                << "package"
                << "percent"
                << "perm"
                << "permanent"
                << "pipe"
                << "plan"
                << "precision"
                << "prepare"
                << "primary"
                << "print"
                << "privileges"
                << "proc"
                << "procedure"
                << "processexit"
                << "public"
                << "raiserror"
                << "read"
                << "read_only"
                << "readtext"
                << "reconfigure"
                << "references"
                << "repeatable"
                << "replication"
                << "restore"
                << "restrict"
                << "return"
                << "returns"
                << "revoke"
                << "right"
                << "rollback"
                << "row"
                << "rowcount"
                << "rowguidecol"
                << "rule"
                << "save"
                << "schema"
                << "schemabinding"
                << "scroll_locks"
                << "select"
                << "serializable"
                << "session_user"
                << "set"
                << "setuser"
                << "shutdown"
                << "some"
                << "sql_variant"
                << "static"
                << "statistics"
                << "system_user"
                << "table"
                << "tape"
                << "temp"
                << "temporary"
                << "textsize"
                << "then"
                << "to"
                << "top"
                << "tran"
                << "transaction"
                << "trigger"
                << "truncate"
                << "tsequal"
                << "type_warning"
                << "uncommitted"
                << "union"
                << "unique"
                << "update"
                << "updatetext"
                << "use"
                << "values"
                << "varying"
                << "view"
                << "waitfor"
                << "when"
                << "where"
                << "while"
                << "with"
                << "work"
                << "writetext"
                << "xml"
                   ;
        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp("\\b"+pattern+"\\b",Qt::CaseInsensitive);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        classFormat.setFontWeight(QFont::Bold);
        classFormat.setForeground(Qt::darkMagenta);
        rule.pattern = QRegExp("\\bd[A-Za-z]+_dbt\\b",Qt::CaseInsensitive);
        rule.format = classFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(Qt::red);
        rule.pattern = QRegExp("--[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setForeground(Qt::red);

        quotationFormat.setForeground(Qt::darkGreen);
        rule.format = quotationFormat;
        rule.pattern = QRegExp("\"[^\"]*\"");
        highlightingRules.append(rule);
        rule.pattern = QRegExp("'[^']*'");
        highlightingRules.append(rule);

        commentStartExpression = QRegExp();
        commentEndExpression = QRegExp();
    }
    syntaxName = syntax;
    //rehighlight();
}

QString Highlighter::syntax()
{
    return syntaxName;
}

void Highlighter::highlightBlock(const QString &text)
{
    if(syntaxName.isEmpty())
        return;
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    if(!commentStartExpression.isEmpty())
    {
        int startIndex = 0;
        if (previousBlockState() != 1)
            startIndex = commentStartExpression.indexIn(text);

        while (startIndex >= 0) {
            int endIndex = commentEndExpression.indexIn(text, startIndex);
            int commentLength;
            if (endIndex == -1) {
                setCurrentBlockState(1);
                commentLength = text.length() - startIndex;
            } else {
                commentLength = endIndex - startIndex
                        + commentEndExpression.matchedLength();
            }
            setFormat(startIndex, commentLength, multiLineCommentFormat);
            startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
        }
    }
}
