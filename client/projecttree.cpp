#include "projecttree.h"
#include <tqbase.h>
#include "ttfolders.h"
#include <ttglobal.h>
#include "tqlogindlg.h"
#include <QtWebKit>
#include <tqjson.h>

TQProjectTreeItem::TQProjectTreeItem(TQProjectTree *projectTree)
    //: data.tree(projectTree), data.dbClass(), data.connectString(), data.projectName(), data.prj(0), data.db(0)
{
    data.tree = projectTree;
    data.prj = 0;
    data.db = 0;
}

TQProjectTreeItem &TQProjectTreeItem::operator =(const TQProjectTreeItem &src)
{
    data = src.data;
    return *this;
}

bool TQProjectTreeItem::isOpened() const
{
    return (data.prj && data.prj->isOpened());
}

bool TQProjectTreeItem::open()
{
    if(isOpened())
        return true;
    if(data.prj)
    {
        delete data.prj;
        data.prj = 0;
    }
    TQJson parser;
    QVariantMap map = parser.toVariant(data.connectString).toMap();
    foreach(QString key, map.keys())
    {
        data.params.insert(key, map.value(key).toString());
    }

//    QStringList values = data.connectString.split(";");
//    foreach(QString v, values)
//    {
//        int p = v.indexOf("=");

//        QString par = v.left(p);
//        QString val = v.mid(p+1);
//        data.params.insert(par,val);
//    }
    QString sRecType = data.params.value("RecordType");
    bool okRecType;
    data.recType = sRecType.toInt(&okRecType);
    if(!data.db)
        data.db = TQAbstractDB::createDbClass(data.dbClass, data.tree);
    if(!data.db)
        return false;
    data.tree->connect(data.db,
                       SIGNAL(projectAuthenticationRequired(TQAbstractDB*,QString,QAuthenticator*)),
                       data.tree,
                       SLOT(onProjectAuthenticationRequired(TQAbstractDB*,QString,QAuthenticator*)),
                       Qt::DirectConnection);
    QString dbType = data.params.value("DBType");
    if(!dbType.isEmpty())
        data.db->setDbmsType(dbType);
    QString dbServer = data.params.value("DBServer");
    if(!dbServer.isEmpty())
        data.db->setDbmsServer(dbServer);
    QString user, pass;
    user = data.params.value("DBUser");
    pass = data.params.value("DBPass");
    data.db->setDbmsUser(user,pass);
    data.prj = data.db->openProject(data.projectName, data.params.value("User"), data.params.value("Password"));
    if(!data.prj)
        return false;
    if(!data.prj->isOpened())
    {
        delete data.prj;
        data.prj = 0;
        return false;
    }
    if(!okRecType)
        data.recType = data.prj->defaultRecType();
    return true;
}

// ================== TQProjectTree ===================
TQProjectTree::TQProjectTree(QObject *parent)
    : UnionModel(parent), info(this)
{
}

void TQProjectTree::setProject(TQAbstractProject *prj, int recordType)
{
    info.data.prj = prj;
//    info.db = prj->db;
    info.data.recType = recordType;
}

void TQProjectTree::setProject(const QString &dbClass, const QString &projectName, const QString &connectString)
{
    info.data.dbClass = dbClass;
    info.data.projectName = projectName;
    info.data.connectString = connectString;
}

void TQProjectTree::setItem(const TQProjectTreeItem &item)
{
    beginResetModel();
    if(info.data.prj)
        delete info.data.prj;
    info = item;
    info.data.tree = this;
    endResetModel();
}


TQAbstractProject *TQProjectTree::project() const
{
    return info.data.prj;
}

int TQProjectTree::recordType() const
{
    return info.data.recType;
}

bool TQProjectTree::isOpened() const
{
    return info.isOpened();
}

bool TQProjectTree::open()
{
    if(!info.open())
        return false;
    folders = new TTFolderModel(this);
    QSqlDatabase db = ttglobal()->userDatabase();
    folders->setDatabaseTable(db,"folders",info.data.prj->projectName());
    appendSourceModel(folders,tr("Личные папки"));


    userModel = new TrkQryFilter(this);
    userModel->setSourceQueryModel(info.data.prj->queryModel(info.data.recType),TrkQryFilter::UserOnly);
    appendSourceModel(userModel,tr("Личные выборки"));
    int rows = userModel->rowCount();

    publicModel = new TrkQryFilter(this);
    publicModel->setSourceQueryModel(info.data.prj->queryModel(info.data.recType),TrkQryFilter::PublicOnly);
    appendSourceModel(publicModel,tr("Общие выборки"));
    rows = publicModel->rowCount();
    setMaxColCount(1);

    return true;
}

void TQProjectTree::close()
{
    if(!isOpened())
        return;
    beginResetModel();

    if(folders)
    {
        removeSourceModel(folders);
        delete folders;
        folders = 0;
    }
    if(userModel)
    {
        removeSourceModel(userModel);
        delete userModel;
        userModel = 0;
    }
    if(publicModel)
    {
        removeSourceModel(publicModel);
        delete publicModel;
        publicModel = 0;
    }
    if(info.data.prj)
    {
        delete info.data.prj;
        info.data.prj = 0;
    }
    endResetModel();
}

void TQProjectTree::onProjectAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator)
{
    TQLoginDlg dlg;
    dlg.setAuthenticator(authenticator);
    if(dlg.exec())
        dlg.assignToAuthenticator(authenticator);
}


