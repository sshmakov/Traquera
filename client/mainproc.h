#ifndef MAINPROC_H
#define MAINPROC_H
#include <ttglobal.h>
#include <QMainWindow>
#include <tqbase.h>
#include <QtXmlPatterns>

class MainWindow;
class MainProcPrivate;
class MainProc: public TTMainProc
{
    MainProcPrivate *d;
public:
    MainWindow *mainWin;
    QAbstractMessageHandler *messageHandler;

    MainProc(MainWindow *win);
    ~MainProc();
    virtual QMainWindow *mainWindow();
    virtual TQAbstractProject *currentProject();
    virtual bool insertViewTab(QWidget *view, QWidget *tab, const QString &title);
    virtual bool addPropWidget(QWidget *widget);
    QVariant createActiveX(const QString &objectName, QObject *parent);
    QAbstractMessageHandler *messager() const;
    QString makeXMLController(QXmlQuery *xquery, const QString &xqCodePath, TQViewController *controller=0) const;
    QString makeXmlQuery(QXmlQuery *xquery, const QString &xqCodePath, const QObjectList &records) const;
    QString makeXmlQuery(QXmlQuery *xquery, const QString &xqCodePath, TQRecord *record) const;
    TQEditorFactory *fieldEditorFactory() const;
    void populateScriptEngine(QScriptEngine *engine);
};

extern MainProc *mainProc;

#endif // MAINPROC_H
