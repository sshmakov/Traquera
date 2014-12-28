#include "scrpluginfactory.h"
#include "notewidget.h"
#include <QtNetwork>
#include <QtWebKit>
#include <QObjectList>

#include "ttrecwindow.h"

ScrPluginFactory::ScrPluginFactory(QWebView *webView, TTRecordWindow *parent) :
    QWebPluginFactory(parent)
{
    editor = parent;
    web = webView;
    manager = new QNetworkAccessManager(this);
    views = new QObjectList();
}

ScrPluginFactory::~ScrPluginFactory()
{
    delete views;
}

void ScrPluginFactory::setRecord(TQRecord *rec)
{
    record = rec;
}

QObject *ScrPluginFactory::create(const QString &mimeType, const QUrl &url, const QStringList &argumentNames, const QStringList &argumentValues) const
{
    if (mimeType != NoteWidget_MIME)
        return 0;

    int noteIndex = argumentValues[argumentNames.indexOf("noteIndex")].toInt();
    NoteWidget *view = new NoteWidget(record, 0); //argumentValues[argumentNames.indexOf("type")]);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(finished()), view, SLOT(updateModel()));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    if(noteIndex ==-1)
    {
        view->setNoteTitle("Description");
        view->setNoteText(record->description());
    }
    else
    {
        view->setNoteTitle(record->noteTitle(noteIndex));
        view->setNoteText(record->noteText(noteIndex));
    }
    view->setNoteIndex(noteIndex);
    views->append(view);
    //connect(view,SIGNAL(changedNoteTitle(QString))
    connect(view,SIGNAL(submitTriggered(int,QString,QString)),SLOT(saveNote(int,QString,QString)));
    connect(view,SIGNAL(cancelTriggered(int)),SLOT(resetNote(int)));
    connect(view,SIGNAL(destroyed()),this,SLOT(destroyedEdit()));
    connect(view,SIGNAL(changedNoteTitle(int,QString)),SLOT(changedNote(int)));
    connect(view,SIGNAL(changedNoteText(int)),SLOT(changedNote(int)));
    QWebFrame *frame = web->page()->mainFrame();
    QString obj = "wid" + QString::number(noteIndex);
    view->setProperty("JSObject", obj);
    frame->addToJavaScriptWindowObject(obj, view);
    return view;
}

QList<QWebPluginFactory::Plugin> ScrPluginFactory::plugins() const
{
    QWebPluginFactory::MimeType mimeType;
    mimeType.name = NoteWidget_MIME;
    mimeType.description = "Note";
    //mimeType.fileExtensions = QStringList() << "csv";

    QWebPluginFactory::Plugin plugin;
    plugin.name = "Note editor";
    //plugin.description = "A CSV file Web plugin.";
    plugin.mimeTypes = QList<MimeType>() << mimeType;

    return QList<QWebPluginFactory::Plugin>() << plugin;
}

bool ScrPluginFactory::closeEdits(bool withSave)
{
    foreach(QObject *v, *views)
    {
        NoteWidget *view = qobject_cast<NoteWidget *>(v);
        if(view)
            if(withSave)
                view->submit();
            else
                view->cancel();
    }
    return true;
}

void ScrPluginFactory::saveNote(int index, const QString &title, const QString &text)
{
//    QObject *s = sender();
//    QVariant obj = s->property("JSObject");
//    if(obj.isValid())
//    {
//        QWebFrame *frame = web->page()->mainFrame();
//        frame->evaluateJavaScript("closeNoteWidget("+obj.toString()+ ");\n");
//    }
    QWebFrame *frame = web->page()->mainFrame();
    frame->evaluateJavaScript("submitNoteWidget("+QString::number(index)+ ");\n");
    //editor->setNote(index, title, text);
    emit savedChanges(index);
}

void ScrPluginFactory::resetNote(int index)
{
//    QObject *s = sender();
    QWebFrame *frame = web->page()->mainFrame();
    frame->evaluateJavaScript("closeNoteWidget("+QString::number(index)+ ");\n");
    emit canceledChanges(index);
    //s->deleteLater();
//    QObject *s = sender();
//    QVariant obj = s->property("JSObject");
//    if(obj.isValid())
//    {
//        QWebFrame *frame = web->page()->mainFrame();
//        frame->evaluateJavaScript("closeNoteWidget("+obj.toString()+ ");\n");
    //    }
}

void ScrPluginFactory::changedNote(int index)
{
    emit changedNoteEditor(index);
}

void ScrPluginFactory::destroyedEdit()
{
    QObject *s = sender();
    views->removeAll(s);
}
