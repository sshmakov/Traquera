#ifndef SCRPLUGINFACTORY_H
#define SCRPLUGINFACTORY_H

#include <QWebPluginFactory>

class TTRecordWindow;
class QNetworkAccessManager;
class TrkToolRecord;
class QWebView;
class NoteWidget;

class ScrPluginFactory : public QWebPluginFactory
{
    Q_OBJECT
public:
    explicit ScrPluginFactory(QWebView *webView, TTRecordWindow *parent);
    ~ScrPluginFactory();
    void setRecord(TrkToolRecord *rec);
    QObject *create(const QString &mimeType, const QUrl &url,
                    const QStringList &argumentNames,
                    const QStringList &argumentValues) const;
    QList<QWebPluginFactory::Plugin> plugins() const;
    bool closeEdits(bool withSave = false);

private slots:
    void saveNote(int index, const QString &title, const QString &text);
    void resetNote(int index);
    void changedNote(int index);
    void destroyedEdit();

signals:
    void savedChanges(int index);
    void canceledChanges(int index);
    void changedNoteEditor(int index);

private:
    QNetworkAccessManager *manager;
    TrkToolRecord *record;
    TTRecordWindow *editor;
    QWebView *web;
    QObjectList *views;
};

#endif // SCRPLUGINFACTORY_H
