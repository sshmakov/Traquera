#include <QCoreApplication>

#include "trkview.h"
#include <QDomDocument>
#include <QtNetwork>
#include "main.h"

#ifdef USE_VLD
#include <vld.h>
#endif


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("windows-1251"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("windows-1251"));
    char * iniFile;
    if(argc > 1)
        iniFile = argv[1];
    else
        iniFile = "./trindex.ini";
    MainClass m(&a,iniFile);
    if(!m.isOk())
        return 1;
    m.start();
    return a.exec();
}

static QRegExp fReg("[\x00-\x08\x0B\x0C\x0E-\x1F]");

inline QString &filter(QString &src)
{
    return src.replace(fReg," ");
}

inline QString filter(const QString &src)
{
    return QString(src).replace(fReg," ");
}

inline QString &filterName(QString &src)
{
    return src.replace(' ',"_");
}

inline QString filterName(const QString &src)
{
    return QString(src).replace(' ',"_");
}

MainClass::MainClass(QObject *parent, char *iniFile)
    : QObject(parent), sout(stdout), sets(iniFile, QSettings::IniFormat), fieldMap()
{
    connect(this,SIGNAL(startProcess()),SLOT(process()),Qt::QueuedConnection);
    connect(this,SIGNAL(next()),SLOT(sendNextRecord()),Qt::QueuedConnection);
    connect(&man,SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));

    sets.setIniCodec(QTextCodec::codecForName("windows-1251"));

    QString
            dbType = sets.value("dbmsType").toString(),
            dbServer = sets.value("dbmsName").toString(),
            project = sets.value("project").toString(),
            user = sets.value("user").toString(),
            password = sets.value("password").toString();

    db=new TrkToolDB(0,this);
    db->setDbmsType(dbType);
    db->setDbmsServer(dbServer);
    db->setDbmsUser(sets.value("dbmsUser").toString(),
                      sets.value("dbmsPassword").toString());
    /*
    db->dbmsUser = sets.value("dbmsUser").toString();
    db->dbmsPassword = sets.value("dbmsPassword").toString();
    db->dbmsName = sets.value("dbmsName").toString(); //"SHMAKOVTHINK\\SQLEXPRESS";
    */
    uniqueField = sets.value("uniqueId","id").toString();
    url.setUrl(sets.value("solrUpdate","http://localhost:8983/solr/update").toString());
    inFile = url.isLocalFile();
    if(url.isEmpty())
    {
        inFile = true;
        localFile.open(stdout, QIODevice::WriteOnly | QIODevice::Append);
    }
    else if(inFile)
    {
        QString fileName = url.toLocalFile();
        localFile.setFileName(fileName);
        localFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }
    //QScopedPointer<TrkToolProject> prj(db->openProject(argv[1],argv[2],argv[3],argv[4]));

    prj = db->openProject(project,user,password);
    if(!prj->isOpened())
        sout << "Error opening project\n";
    prjName = prj->projectName();
    filterName(prjName);
    sets.beginGroup("FieldMap");
    QStringList keys = sets.childKeys();
    foreach(const QString &field, keys)
        fieldMap[field] = sets.value(field).toString();
    sets.endGroup();
}

void MainClass::start()
{
    emit startProcess();
}

#define TRK_FIELD_TYPE_TEXT TRK_FIELD_TYPE_STRING
#define TRK_FIELD_TYPE_MULTITEXT 10

const char *suffix[11] = {
            "_s", //TRK_FIELD_TYPE_NONE
            "_t", //TRK_FIELD_TYPE_CHOICE
            "_t", //TRK_FIELD_TYPE_STRING
            "_i", //TRK_FIELD_TYPE_NUMBER
            "_s", //TRK_FIELD_TYPE_DATE
            "_s", //TRK_FIELD_TYPE_SUBMITTER
            "_s", //TRK_FIELD_TYPE_OWNER
            "_s", //TRK_FIELD_TYPE_USER
            "_s", //TRK_FIELD_TYPE_ELAPSED_TIME
            "_s", //TRK_FIELD_TYPE_STATE
            "_txt" // TRK_FIELD_TYPE_MULTITEXT
            };


QDomElement eField(QDomDocument &xml, const QString &name, const QString &value, int ftype = TRK_FIELD_TYPE_STRING)
{

    QDomElement f = xml.createElement("field");
    f.setAttribute("name", name);
    QDomText text;
    if(ftype == TRK_FIELD_TYPE_NUMBER && value.isEmpty())
        text = xml.createTextNode(0);
    else
        text = xml.createTextNode(value);
    f.appendChild(text);
    return f;
}

QString MainClass::fieldNameTranslate(const QString &name, int ftype)
{
    QString res = fieldMap[name];
    if(res.isEmpty())
        return filterName(name) + QString(suffix[ftype]);
    return res;
}

QDomDocument MainClass::recFieldsXml(TrkToolRecord *rec)
{
    QDomDocument xml("doc");
    QDomElement root=xml.createElement("doc");
    xml.appendChild(root);
    QVariant v = rec->value("Id");
    int id = v.toInt();
    sout << tr("Posting SCR #%1\n").arg(id);
    root.appendChild(eField(xml,uniqueField,
                            QString("%1-%2")
                            .arg(prjName.replace(' ',"_"))
                            .arg(v.toString())));
    root.appendChild(eField(xml,fieldNameTranslate("project",TRK_FIELD_TYPE_NONE),prj->projectName()));
    TQAbstractRecordTypeDef *rdef = prj->recordTypeDef(rec->recordType());
    foreach(QString fi, rec->fields())
    {
//        int ftype = prj->fieldType(fi, rec->recordType());
        int fvid = rdef->fieldVid(fi);
        int ftype = rdef->fieldNativeType(fvid);
        root.appendChild(eField(xml, fieldNameTranslate(fi, ftype), //filterName(fi) + QString(suffix[ftype]),
                                filter(rec->value(fi).toString()),
                                ftype));
    }
    root.appendChild(eField(xml,fieldNameTranslate("Description",TRK_FIELD_TYPE_STRING), //"Description_s"
                            rec->description()));

    foreach(const TQNote &note, rec->notes())
    {
        root.appendChild(eField(xml, fieldNameTranslate("note", TRK_FIELD_TYPE_MULTITEXT), // "note_txt"
                                QString("%1 [%2 (%3)] %4")
                                .arg(filter(note.title))
                                .arg(note.crdate.toString(TT_DATETIME_FORMAT))
                                .arg(filter(note.author))
                                .arg(filter(note.text))));
    }
    return xml;
}

bool MainClass::isOk() const
{
    return prj->isOpened();
}


void MainClass::send(const QDomDocument &dom)
{
    //QUrl url(sets.value("solrUpdate","http://localhost:8983/solr/update").toString());
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    reply = man.post(req,dom.toString().toUtf8());
//    connect(reply,SIGNAL(finished()),this,SLOT(finished()));
//    connect(reply,SIGNAL(readyRead()),this,SLOT(finished()));
    //    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
}

void MainClass::write(const QDomDocument &dom)
{
    localFile.write(dom.toString().toUtf8());
    emit next();
}


void MainClass::process()
{
    //model = prj->openRecentModel(0,sets.value("query","All_SCRs").toString());
    list = prj->getQueryIds(sets.value("query","All SCRs").toString(), prj->defaultRecType());
    rowCount = list.count();
    sout << QString("Found %1 records\n").arg(rowCount);
    row = 0;
    emit next();
}

#define DELETE_RECORDS

void MainClass::sendNextRecord()
{
    if(row >= rowCount)
    {
        qApp->exit(0);
        return;
    }
    //PTrkToolRecord rec = model->at(row);
    TQRecord *trec = prj->createRecordById(list[row], prj->defaultRecType()); //model->at(row);
    if(!trec)
       return;
    PTrkToolRecord rec = qobject_cast<TrkToolRecord *>(trec);
    if(!rec)
    {
        delete trec;
        return;
    }
    QDomDocument xml = recFieldsXml(rec);
    rec->releaseBuffer();
    QDomDocument add("add");
    QDomElement root=add.createElement("add");
    root.setAttribute("overwrite", "true");
    add.appendChild(root);
    root.appendChild(xml.documentElement());
    //root.save(sout,4);
    if(inFile)
        write(add);
    else
        send(add);
    delete rec;
//#ifdef DELETE_RECORDS
    //model->removeRecordId(rec->recordId());
//#else
//#endif
    row++;
}


void MainClass::finished()
{
    if(reply->waitForReadyRead(500))
    {
        sout << reply->readAll();
    }
    QString res= QString("Error %1: %2\n").arg(reply->error()).arg(reply->errorString());
    delete reply;
    sout << res.toUtf8();
    emit next();
}

void MainClass::onError(QNetworkReply::NetworkError error)
{
    QString res = QString("Error %1: %2\n").arg(reply->error()).arg(reply->errorString());
    emit next();
}

void MainClass::finished(QNetworkReply *reply)
{
    /*
    if(reply->waitForReadyRead(500))
    {
        sout << reply->readAll();
    }
    QString res= QString("Error %1: %2\n").arg(reply->error()).arg(reply->errorString());
    sout << res.toUtf8();
    */
    sout << reply->readAll();
    emit next();
}


