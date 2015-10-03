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
    QList<QPair<QString, QNetworkProxy::ProxyType> > proxyTypes;
    QNetworkProxy cur;
    ProxyOptionsPrivate();
};

ProxyOptionsPrivate::ProxyOptionsPrivate()
{
    proxyTypes.append(qMakePair(QObject::tr("Нет"), QNetworkProxy::NoProxy));
    proxyTypes.append(qMakePair(QObject::tr("Системный"), QNetworkProxy::DefaultProxy));
    proxyTypes.append(qMakePair(QObject::tr("SOCKS5"), QNetworkProxy::Socks5Proxy));
    proxyTypes.append(qMakePair(QObject::tr("HTTP only"), QNetworkProxy::HttpCachingProxy));
    proxyTypes.append(qMakePair(QObject::tr("HTTPS"), QNetworkProxy::HttpProxy));
    cur = QNetworkProxy::applicationProxy();
}

ProxyOptions::ProxyOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProxyOptions), d(new ProxyOptionsPrivate())
{
    ui->setupUi(this);
    setAttribute( Qt::WA_DeleteOnClose, true);

//    man = ttglobal()->networkManager();
//    cur = pset;
    ui->leServer->setText(d->cur.hostName());
    ui->lePort->setValue(d->cur.port());
    ui->leLogin->setText(d->cur.user());
    ui->lePassword->setText(d->cur.password());
    QPair<QString, QNetworkProxy::ProxyType> p;
    int curIndex = -1, i=0;
    QNetworkProxy::ProxyType curType = d->cur.type();
    foreach(p, d->proxyTypes)
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
    delete d;
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
    QNetworkProxy::setApplicationProxy(d->cur);
}

void ProxyOptions::saveSettings()
{
}

void ProxyOptions::loadSettings()
{
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
    if(index>=0 && index < d->proxyTypes.size())
    {
        QPair<QString, QNetworkProxy::ProxyType> p = d->proxyTypes.value(index);
        d->cur.setType(p.second);
    }
    else
        d->cur.setType(QNetworkProxy::NoProxy);
}

void ProxyOptions::on_leServer_editingFinished()
{
    d->cur.setHostName(ui->leServer->text());
}

void ProxyOptions::on_lePort_editingFinished()
{
    d->cur.setPort(ui->lePort->value());
}

void ProxyOptions::on_leLogin_editingFinished()
{
    d->cur.setUser(ui->leLogin->text());
}

void ProxyOptions::on_lePassword_editingFinished()
{
    d->cur.setPassword(ui->lePassword->text());
}


