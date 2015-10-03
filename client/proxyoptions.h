#ifndef PROXYOPTIONS_H
#define PROXYOPTIONS_H

#include <QWidget>
#include <QtGui>

namespace Ui {
class ProxyOptions;
}

class QNetworkAccessManager;
class QNetworkProxy;

/*
class ProxySet;

class ProxySet
{
public:
    int proxyType;
    QString user, password;
    QString host;
    int port;
    QList<QPair<QString, int> > proxyTypes;
    ProxySet();
    ProxySet &operator =(const ProxySet &src);
    static void saveSettings();
    static void loadSettings();
    static void resetAppProxy();
    void assignToProxy(QNetworkProxy &proxy);
    void getFromProxy(const QNetworkProxy &proxy);
};
*/

class ProxyOptionsPrivate;

class ProxyOptions : public QWidget
{
    Q_OBJECT
    
public:
    explicit ProxyOptions(QWidget *parent = 0);
    ~ProxyOptions();
    bool event(QEvent *event);

    static QWidget *proxyOptionsFunc(const QString &optionsPath)
    {
        return new ProxyOptions();
    }

    void applyChanges();
    static void saveSettings();
    static void loadSettings();

private slots:
//    void on_chUseProxy_clicked(bool checked);

//    void on_chNeedLogin_clicked(bool checked);

    void on_cbProxyType_currentIndexChanged(int index);

    void on_leServer_editingFinished();

    void on_lePort_editingFinished();

    void on_leLogin_editingFinished();

    void on_lePassword_editingFinished();

private:
    Ui::ProxyOptions *ui;
    ProxyOptionsPrivate *d;
//    QNetworkAccessManager *man;
//    ProxySet cur;
    void setLoginNeeded(bool need);
};



#endif // PROXYOPTIONS_H
