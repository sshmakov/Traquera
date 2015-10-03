#ifndef WEBFORM_H
#define WEBFORM_H

#include <QWidget>
#include <QtGui>
#include <QWebPage>

namespace Ui {
class WebForm;
}

class WebFormPrivate;
class QUrl;
class QNetworkRequest;


class MyWebPage: public QWebPage
{
    Q_OBJECT
signals:
    void linkChecked(const QUrl &url) const;
public:
    MyWebPage(QObject *parent = 0)
        :QWebPage(parent)
    {}
    ~MyWebPage()
    {}
    QString userAgentForUrl(const QUrl &url) const
    {
        emit linkChecked(url);
        return QWebPage::userAgentForUrl(url);
    }
};


class WebForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit WebForm(QWidget *parent = 0);
    ~WebForm();
    bool openUrl(const QUrl &url);
//    void setCheckedLink(const QString &link);
    bool request(const QUrl &url, const QRegExp &callbackUrl);
    QUrl foundUrl() const;
protected:
    void closeEvent(QCloseEvent *event);
//    static authorize()
protected slots:
    void onLinkClicked(const QUrl &url);
//    void onDownReq( const QNetworkRequest & request );
signals:
    void checked();
private:
    WebFormPrivate *d;
    Ui::WebForm *ui;
    QRegExp checkedLink;
    bool execMode;
};

#endif // WEBFORM_H
