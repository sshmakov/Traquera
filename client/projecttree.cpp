#include "projecttree.h"
#include <tqbase.h>
#include "ttfolders.h"
#include <ttglobal.h>
#include "tqlogindlg.h"
#include <QtWebKit>
#include <tqjson.h>

TQProjectTreeItem::TQProjectTreeItem(TQOneProjectTree *projectTree)
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

bool TQProjectTreeItem::open(const QString &connString)
{
    if(isOpened())
        return true;
    if(data.prj)
    {
        delete data.prj;
        data.prj = 0;
    }
    QString cs = connString;
    if(cs.isEmpty())
        cs = data.connectString;
    updateParams(cs);
    if(!data.db)
        data.db = TQAbstractDB::createDbClass(data.dbClass, data.tree);
    if(!data.db)
        return false;
    data.tree->connect(data.db,
                       SIGNAL(projectAuthenticationRequired(TQAbstractDB*,QString,QAuthenticator*)),
                       data.tree,
                       SLOT(onProjectAuthenticationRequired(TQAbstractDB*,QString,QAuthenticator*)),
                       Qt::DirectConnection);
    data.prj = data.db->openConnection(cs);
    if(!data.prj || !data.prj->isOpened())
    {
        if(data.prj)
        {
            delete data.prj;
            data.prj = 0;
        }
        delete data.db;
        data.db = 0;
        return false;
    }
    if(data.recType < 0)
        data.recType = data.prj->defaultRecType();
    return true;
    /*
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
    if(!data.params.contains("RecordType"))
        data.recType = data.prj->defaultRecType();
    return true;
    */
}

void TQProjectTreeItem::setConnectString(const QString &string)
{
    data.connectString = string;
    updateParams(string);
}

void TQProjectTreeItem::updateParams(const QString &string)
{
    TQJson parser;
    QVariantMap map = parser.toVariant(string).toMap();
    foreach(QString key, map.keys())
    {
        data.params.insert(key, map.value(key).toString());
    }
    data.dbClass = data.params.value("DBClass");
    data.projectName = data.params.value("Project");
    QString sRecType = data.params.value("RecordType");
    bool okRecType;
    data.recType = sRecType.toInt(&okRecType);
    if(!okRecType)
        data.recType = -1;
}

// ================== TQProjectTree ===================
TQOneProjectTree::TQOneProjectTree(QObject *parent)
    : UnionModel(parent), info(this)
{
}

/*
void TQProjectTree::setProject(TQAbstractProject *prj, int recordType)
{
    info.data.prj = prj;
//    info.db = prj->db;
    info.data.recType = recordType;
}
*/

void TQOneProjectTree::setConnectString(const QString &connectString)
{
    info.setConnectString(connectString);
}

QString TQOneProjectTree::connectString() const
{
    return info.data.connectString;
}

/*
void TQProjectTree::setProject(const QString &dbClass, const QString &projectName, const QString &connectString)
{
    info.data.dbClass = dbClass;
    info.data.projectName = projectName;
    info.data.connectString = connectString;
}
*/

void TQOneProjectTree::setItem(const TQProjectTreeItem &item)
{
    beginResetModel();
    if(info.data.prj)
        delete info.data.prj;
    info = item;
    info.data.tree = this;
    endResetModel();
}

const TQProjectTreeItem &TQOneProjectTree::item() const
{
    return info;
}


TQAbstractProject *TQOneProjectTree::project() const
{
    return info.data.prj;
}

QString TQOneProjectTree::projectTitle() const
{
    return info.data.projectName;
}

int TQOneProjectTree::recordType() const
{
    return info.data.recType;
}

bool TQOneProjectTree::isOpened() const
{
    return info.isOpened();
}

bool TQOneProjectTree::isAutoOpen() const
{
    QString s = info.data.params.value("AutoLogin","false");
    return QString::compare(s,"true",Qt::CaseInsensitive) == 0;
}

bool TQOneProjectTree::open(const QString &connectString)
{
    if(!info.open(connectString))
        return false;
    emit projectOpened();
    beginResetModel();
    folders = new TTFolderModel(this);
    QSqlDatabase db = ttglobal()->userDatabase();
    folders->setDatabaseTable(db,"folders",project()->projectName());
    appendSourceModel(folders,tr("������ �����"));


    userModel = new TrkQryFilter(this);
    userModel->setSourceQueryModel(project()->queryModel(recordType()),TrkQryFilter::UserOnly);
    appendSourceModel(userModel,tr("������ �������"));
    int rows = userModel->rowCount();

    publicModel = new TrkQryFilter(this);
    publicModel->setSourceQueryModel(project()->queryModel(recordType()),TrkQryFilter::PublicOnly);
    appendSourceModel(publicModel,tr("����� �������"));
    rows = publicModel->rowCount();
    setMaxColCount(1);    
    endResetModel();

    return true;
}

void TQOneProjectTree::close()
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
    emit projectClosed();
}

void TQOneProjectTree::onProjectAuthenticationRequired(TQAbstractDB *db, const QString &projectName, QAuthenticator *authenticator)
{
    TQLoginDlg dlg;
    dlg.setAuthenticator(authenticator);
    if(dlg.exec())
        dlg.assignToAuthenticator(authenticator);
}

// ================ TQProjectsTree ====================
TQProjectsTree::TQProjectsTree(QObject *parent)
    : UnionModel(parent), selModel(0)
{
}

int TQProjectsTree::appendProject(TQOneProjectTree *prjTree)
{
    return appendSourceModel(prjTree, prjTree->projectTitle()).row();
}

void TQProjectsTree::removeProject(TQOneProjectTree *prjTree)
{
    removeSourceModel(prjTree);
    if(prjTree == selModel)
        selModel = 0;
}

void TQProjectsTree::setSelectedModel(QAbstractItemModel *model)
{
    selModel = model;
    UnionModel::setSelectedModel(model);
}

QVariant TQProjectsTree::data(const QModelIndex &proxyIndex, int role) const
{
    if(role == Qt::DisplayRole)
        return UnionModel::data(proxyIndex, role);
    QModelIndex pp = proxyIndex.parent();
    if(pp.isValid())
        return UnionModel::data(proxyIndex, role);
    TQOneProjectTree *prjTree = qobject_cast<TQOneProjectTree *>(sourceModel(proxyIndex));
    if(!prjTree)
        return QVariant();
    if(role == Qt::FontRole)
    {
        if(prjTree == selModel)
        {
            QFont boldFont;
            boldFont.setBold(true);
            return boldFont;
        }
        return QFont();
    }
    if(role == Qt::ForegroundRole)
    {
        if(!prjTree->isOpened())
        {
            QBrush gr;
            gr.setColor(Qt::gray);
            return gr;
        }
        return QVariant();
    }
    return QVariant();

    /*
    if(!proxyIndex.isValid())
        return QVariant();
    int id = proxyIndex.internalId();
    if(!info.contains(id))
        return QVariant();
    const MapInfo &m = info[id];
    if(m.parentId<0)
    {
        if(proxyIndex.column()==0 && proxyIndex.row()<titles.count())
        {
            switch(role)
            {
            case Qt::FontRole:
                QFont boldFont;
                if(m.model == selModel)
                    boldFont.setBold(true);
                return boldFont;
            }
        }
        return QVariant();
    }
    QModelIndex si = mapToSource(proxyIndex);
    return si.data(role);
    */
}

