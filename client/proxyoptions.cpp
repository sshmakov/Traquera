#include "proxyoptions.h"
#include "ui_proxyoptions.h"
#include "ttglobal.h"
#include <QtNetwork>

/*
static ProxySet pset;
static QNetworkProxy appProxy;

ProxySet::ProxySet()
{
    proxyTypes.append(qMakePair(QObject::tr("No proxy"), (int)QNetworkProxy::NoProxy));
    proxyTypes.append(qMakePair(QObject::tr("Default"), (int)QNetworkProxy::DefaultProxy));
    proxyTypes.append(qMakePair(QObject::tr("SOCKS5"), (int)QNetworkProxy::Socks5Proxy));
    proxyTypes.append(qMakePair(QObject::tr("HTTP only"), (int)QNetworkProxy::HttpCachingProxy));
    proxyTypes.append(qMakePair(QObject::tr("HTTPS"), (int)QNetworkProxy::HttpProxy));
               ;
    port = 80;
    host = "proxy";
    proxyType = QNetworkProxy::DefaultProxy;
}

ProxySet &ProxySet::operator =(const ProxySet &src)
{
    proxyType = src.proxyType;
    host = src.host;
    port = src.port;
    user = src.user;
    password = src.password;
    return *this;
}

void ProxySet::saveSettings()
{
    pset.assignToProxy(appProxy);
//    QNetworkProxy::setApplicationProxy(appProxy);
}

void ProxySet::loadSettings()
{
    QNetworkProxy p = QNetworkProxy::applicationProxy();
    pset.getFromProxy(p);
}

void ProxySet::resetAppProxy()
{
    QNetworkProxy p = QNetworkProxy::applicationProxy();
    pset.assignToProxy(p);
    QNetworkProxy::setApplicationProxy(p);
}

void ProxySet::assignToProxy(QNetworkProxy &proxy)
{
    if(!&proxy)
        return;
    proxy.setType((QNetworkProxy::ProxyType)proxyType);
    proxy.setHostName(host);
    proxy.setPort(port);
    proxy.setUser(user);
    proxy.setPassword(password);
}

void ProxySet::getFromProxy(const QNetworkProxy &proxy)
{
    if(!&proxy)
        return;
    proxyType = proxy.type();
    host = proxy.hostName();
    port = proxy.port();
    user = proxy.user();
    password = proxy.password();
}

 */

class ProxyOptionsPrivate
{
public:
//    QNetworkProxy cur;
    QNetworkProxy::ProxyType ptype;
    QString hostname;
    int port;
    QString user,password;
    ProxyOptionsPrivate();
};

static ProxyOptionsPrivate options;

ProxyOptionsPrivate::ProxyOptionsPrivate()
{
    QNetworkProxy cur = QNetworkProxy::applicationProxy();
    ptype = cur.type();
    hostname = cur.hostName();
    port = cur.port();
    user = cur.user();
    password = cur.password();
}

ProxyOptions::ProxyOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProxyOptions), d(&options)
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose, true);

//    man = ttglobal()->networkManager();
//    cur = pset;
    ui->leServer->setText(d->hostname);
    ui->lePort->setValue(d->port);
    ui->leLogin->setText(d->user);
    ui->lePassword->setText(d->password);
    proxyTypes
            << qMakePair(QObject::tr("Нет"), QNetworkProxy::NoProxy)
            << qMakePair(QObject::tr("Системный"), QNetworkProxy::DefaultProxy)
            << qMakePair(QObject::tr("SOCKS5"), QNetworkProxy::Socks5Proxy)
            << qMakePair(QObject::tr("HTTP only"), QNetworkProxy::HttpCachingProxy)
            << qMakePair(QObject::tr("HTTPS"), QNetworkProxy::HttpProxy);
    QPair<QString, QNetworkProxy::ProxyType> p;
    int curIndex = -1, i=0;
    QNetworkProxy::ProxyType curType = d->ptype;
    foreach(p, proxyTypes)
    {
        ui->cbProxyType->addItem(p.first, p.second);
        if(curType == p.second)
            curIndex = i;
        i++;
    }
    ui->cbProxyType->setCurrentIndex(curIndex);
}

ProxyOptions::~ProxyOptions()
{
//    delete d;
    delete ui;
}

bool ProxyOptions::event(QEvent *event)
{
    if(event->type() == QEvent::OkRequest && event->isAccepted())
    {
        applyChanges();
        saveSettings();
        event->accept();
        return true;
    }
    return QWidget::event(event);
}

void ProxyOptions::applyChanges()
{
    QNetworkProxy p;
    p.setType(options.ptype);
    p.setHostName(options.hostname);
    p.setPort(options.port);
    p.setUser(options.user);
    p.setPassword(options.password);
    QNetworkProxy::setApplicationProxy(p);
}

void ProxyOptions::saveSettings()
{
//    QNetworkProxy p = QNetworkProxy::applicationProxy();
    QSettings sets;
    sets.beginGroup("Proxy");
    QNetworkProxy::ProxyType curType = options.ptype;
    sets.setValue("Type",(int)curType);
    sets.setValue("HostName", options.hostname);
    sets.setValue("Port", options.port);
    sets.setValue("User", options.user);
    sets.setValue("Password", options.password);
}

void ProxyOptions::loadSettings()
{
    QNetworkProxy p = QNetworkProxy::applicationProxy();
    QNetworkProxy::ProxyType curType = p.type();
    QSettings sets;
    sets.beginGroup("Proxy");
    if(!sets.contains("Type"))
        return;
    options.ptype = (QNetworkProxy::ProxyType)sets.value("Type").toInt();
    options.hostname = sets.value("HostName").toString();
    options.port = sets.value("Port").toInt();
    options.user = sets.value("User").toString();
    if(sets.contains("Password"))
        options.password = sets.value("Password").toString();
    p.setType(options.ptype);
    p.setHostName(options.hostname);
    p.setPort(options.port);
    p.setUser(options.user);
    p.setPassword(options.password);
    QNetworkProxy::setApplicationProxy(p);
}

//void ProxyOptions::on_chUseProxy_clicked(bool checked)
//{
//    ui->cbProxyType->setEnabled(checked);
//    ui->chNeedLogin->setEnabled(checked);
//    setLoginNeeded(checked && ui->chNeedLogin->isEnabled());
//}

//void ProxyOptions::setLoginNeeded(bool need)
//{
//    bool loginEnabled = ui->chUseProxy->isChecked() && need && !ui->chUseNTLM->isChecked();
//    ui->leLogin->setEnabled(loginEnabled);
//    ui->lePort->setEnabled(loginEnabled);
//    ui->chUseNTLM->setEnabled(need);
//}

//void ProxyOptions::on_chNeedLogin_clicked(bool checked)
//{
//    setLoginNeeded(checked);
//}

void ProxyOptions::on_cbProxyType_currentIndexChanged(int index)
{
    if(index>=0 && index < proxyTypes.size())
    {
        QPair<QString, QNetworkProxy::ProxyType> p = proxyTypes.value(index);
        d->ptype = p.second;
    }
    else
        d->ptype = QNetworkProxy::NoProxy;
}

void ProxyOptions::on_leServer_editingFinished()
{
    d->hostname = ui->leServer->text();
}

void ProxyOptions::on_lePort_editingFinished()
{
    d->port = ui->lePort->value();
}

void ProxyOptions::on_leLogin_editingFinished()
{
    d->user = ui->leLogin->text();
}

void ProxyOptions::on_lePassword_editingFinished()
{
    d->password = ui->lePassword->text();
}


