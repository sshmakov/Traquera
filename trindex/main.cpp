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
    QString iniFile;
    if(argc)
        iniFile = argv[1];
    else
        iniFile = "./trindex.ini";
    MainClass m(&a,iniFile);
    if(!m.isOk())
        return 1;
    m.start();
    return a.exec();
}

MainClass::MainClass(QObject *parent, const QString &iniFile)
    : QObject(parent), sout(stdout), sets(iniFile, QSettings::IniFormat)
{
    connect(this,SIGNAL(startProcess()),SLOT(process()),Qt::QueuedConnection);
    connect(this,SIGNAL(next()),SLOT(sendNextRecord()),Qt::QueuedConnection);
    connect(&man,SIGNAL(finished(QNetworkReply*)),this,SLOT(finished(QNetworkReply*)));

    sets.setIniCodec(QTextCodec::codecForName("windows-1251"));

    db=new TrkToolDB(this);
    db->dbmsUser = sets.value("dbmsUser").toString();
    db->dbmsPassword = sets.value("dbmsPassword").toString();
    db->dbmsName = sets.value("dbmsName").toString(); //"SHMAKOVTHINK\\SQLEXPRESS";
    //QScopedPointer<TrkToolProject> prj(db->openProject(argv[1],argv[2],argv[3],argv[4]));
    QString
            dbType = sets.value("dbmsType").toString(),
            project = sets.value("project").toString(),
            user = sets.value("user").toString(),
            password = sets.value("password").toString();
     prj = db->openProject(dbType,project,user,password);
    if(!prj->isOpened())
        sout << "Error opening project\n";
}

void MainClass::start()
{
    emit startProcess();
}

const char *suffix[10] = {
            "",   //TRK_FIELD_TYPE_NONE
            "_t", //TRK_FIELD_TYPE_CHOICE
            "_t", //TRK_FIELD_TYPE_STRING
            "_i", //TRK_FIELD_TYPE_NUMBER
            "_s", //TRK_FIELD_TYPE_DATE
            "_s", //TRK_FIELD_TYPE_SUBMITTER
            "_s", //TRK_FIELD_TYPE_OWNER
            "_s", //TRK_FIELD_TYPE_USER
            "_s", //TRK_FIELD_TYPE_ELAPSED_TIME
            "_s"  //TRK_FIELD_TYPE_STATE
            };


QDomElement eField(QDomDocument &xml, const QString &name, const QString &value)
{

    QDomElement f = xml.createElement("field");
    f.setAttribute("name", name);
    QDomText text = xml.createTextNode(value);
    f.appendChild(text);
    return f;
}

QDomDocument MainClass::recFieldsXml(TrkToolRecord *rec)
{
    QDomDocument xml("doc");
    QDomElement root=xml.createElement("doc");
    xml.appendChild(root);
    QVariant v = rec->value("Id");
    int id = v.toInt();
    sout << tr("Posting SCR #%1\n").arg(id);
    root.appendChild(eField(xml,"id",
                            QString("%1-%2")
                            .arg(prj->name.replace(' ',"_"))
                            .arg(v.toString())));
    root.appendChild(eField(xml,"project_s",prj->name));
    foreach(const QString &fi, rec->fields())
    {
        int ftype = prj->fieldType(fi, rec->recordType());
        root.appendChild(eField(xml,fi + QString(suffix[ftype]),
                                rec->value(fi).toString()));
    }
    root.appendChild(eField(xml,"Description_t", rec->description()));

    foreach(const TrkNote &note, rec->getNotes())
    {
        root.appendChild(eField(xml, "note_txt",
                                QString("%1 [%2 (%3)] %4")
                                .arg(note.title)
                                .arg(note.crdate.toString(TT_DATETIME_FORMAT))
                                .arg(note.author)
                                .arg(note.text)));
    }
    return xml;
}

bool MainClass::isOk() const
{
    return prj->isOpened();
}


void MainClass::send(const QDomDocument &dom)
{
    QUrl url(sets.value("solrUpdate","http://localhost:8983/solr/update").toString());
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/xml");
    reply = man.post(req,dom.toString().toUtf8());
//    connect(reply,SIGNAL(finished()),this,SLOT(finished()));
//    connect(reply,SIGNAL(readyRead()),this,SLOT(finished()));
//    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(onError(QNetworkReply::NetworkError)));
}


void MainClass::process()
{
    model = prj->openRecentModel(0,sets.value("query","All_SCRs").toString());
    row = 0;
    emit next();
}

void MainClass::sendNextRecord()
{
    if(row >= model->rowCount())
    {
        qApp->exit(0);
        return;
    }
    PTrkToolRecord rec = model->at(row);
    QDomDocument xml = recFieldsXml(rec);
    QDomDocument add("add");
    QDomElement root=add.createElement("add");
    root.setAttribute("overwrite", "true");
    add.appendChild(root);
    root.appendChild(xml.documentElement());
    //root.save(sout,4);
    send(add);
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
    QString res= QString("Error %1: %2\n").arg(reply->error()).arg(reply->errorString());
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


