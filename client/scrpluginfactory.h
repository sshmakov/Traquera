#ifndef SCRPLUGINFACTORY_H
#define SCRPLUGINFACTORY_H

#include <QWebPluginFactory>

class TTRecordWindow;
class QNetworkAccessManager;
class TrkToolRecord;
class QWebView;

class ScrPluginFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    explicit ScrPluginFactory(QWebView *webView, TTRecordWindow *parent);
    void setRecord(TrkToolRecord *rec);
    QObject *create(const QString &mimeType, const QUrl &url,
                    const QStringList &argumentNames,
                    const QStringList &argumentValues) const;
    QList<QWebPluginFactory::Plugin> plugins() const;

private slots:
    void saveNote(int index, const QString &title, const QString &text);
    void resetNote(int index);

private:
    QNetworkAccessManager *manager;
    TrkToolRecord *record;
    TTRecordWindow *editor;
    QWebView *web;
};

#endif // SCRPLUGINFACTORY_H
