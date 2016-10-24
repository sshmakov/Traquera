#include "webform.h"
#include "ui_webform.h"
#include <QtCore>
#include <QNetworkRequest>
#include <QNetworkCookieJar>

class WebFormPrivate {
public:
    QEventLoop *eventLoop;
    bool callbackPressed;
    QUrl foundUrl;
    MyWebPage *page;
    WebFormPrivate()
        : eventLoop(0), callbackPressed(false)
    {}
};


// ===============  WebForm =====================
WebForm::WebForm(QWidget *parent) :
    QWidget(parent),
    d(new WebFormPrivate()),
    ui(new Ui::WebForm),
    checkedLink(),
    execMode(false)
{
    ui->setupUi(this);
    d->page = new MyWebPage(this);
    ui->webView->setPage(d->page);
    connect(d->page,SIGNAL(linkChecked(QUrl)),SLOT(onLinkClicked(QUrl)));
    connect(ui->webView,SIGNAL(linkClicked(QUrl)),SLOT(onLinkClicked(QUrl)));
    connect(ui->webView,SIGNAL(urlChanged(QUrl)),SLOT(onLinkClicked(QUrl)));
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    setWindowModality(Qt::WindowModal);
//    connect(ui->webView->page(),SIGNAL(downloadRequested(QNetworkRequest)),SLOT(onDownReq(QNetworkRequest)));
//    setWindowModality(Qt::ApplicationModal);
}

WebForm::~WebForm()
{
    delete d->page;
    delete d;
    delete ui;
}

bool WebForm::openUrl(const QUrl &url)
{
    ui->webView->setUrl(url);
    return true;
}

//void WebForm::setCheckedLink(const QString &link)
//{
//    checkedLink = link;
//}

bool WebForm::request(const QUrl &url, const QRegExp &callbackUrl)
{
    static QEventLoop eventLoop;
    if(d->eventLoop)
        return false;
    checkedLink = callbackUrl;
    d->callbackPressed = false;
    execMode = true;

    show();
    d->eventLoop = &eventLoop;
    ui->webView->load(url);
    QPointer<WebForm> guard = this;
    (void) eventLoop.exec(QEventLoop::DialogExec);
    bool res = false;
    if (!guard.isNull())
        res = d->callbackPressed;
    hide();
    d->eventLoop = 0;
    return res;
}

QUrl WebForm::foundUrl() const
{
    return d->foundUrl;
}

QList<QNetworkCookie> WebForm::cookies(const QUrl &url)
{
    QWebPage *page = ui->webView->page();
    if(!page)
        return QList<QNetworkCookie>();
    QNetworkAccessManager *man = page->networkAccessManager();
    if(!man)
        return QList<QNetworkCookie>();
    QNetworkCookieJar *jar = man->cookieJar();
    if(!jar)
        return QList<QNetworkCookie>();
    return jar->cookiesForUrl(url);
}

void WebForm::setNetworkAccessManager(QNetworkAccessManager *man)
{
    ui->webView->page()->setNetworkAccessManager(man);
}

void WebForm::closeEvent(QCloseEvent *event)
{
    if(d->eventLoop)
        d->eventLoop->exit();
    QWidget::closeEvent(event);
}

void WebForm::onLinkClicked(const QUrl &url)
{
    QString newUrl = url.toString();
    qDebug() << newUrl;
    if(!checkedLink.isEmpty() && newUrl.contains(checkedLink))
    {
        d->callbackPressed = true;
        d->foundUrl = url;
        emit checked();
        if(execMode)
            d->eventLoop->exit();
    }
}

//void WebForm::onDownReq(const QNetworkRequest &request)
//{
//    if(request.url().toString() == checkedLink)
//    {
//        d->callbackPressed = true;
//        emit checked();
//        if(execMode)
//            d->eventLoop->exit();
//    }
//}
