#ifndef TTGLOBAL_H
#define TTGLOBAL_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>
#include <tqplugui.h>
#include <tqplugin_global.h>

class QMainWindow;
class MainWindow;
class TrkToolProject;
class TQAbstractProject;
class QSettings;
class QTableView;
class QMenu;
class QWidget;
class QNetworkAccessManager;

class TTGlobalPrivate;
class TQOAuth;
class QScriptEngine;
class QAxScriptManager;
class QAxScript;
class QAxObject;

class TTMainProc
{
public:
    virtual QMainWindow *mainWindow() = 0;
    virtual TQAbstractProject *currentProject() = 0;
    virtual bool insertViewTab(QWidget *view, QWidget *tab, const QString &title) = 0;
    virtual bool addPropWidget(QWidget *widget) = 0;
};


class TQPLUGIN_SHARED TTGlobal : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("ClassID", "{6DC8CE91-BFD5-4189-81E0-DDCE0F98A54C}")
    Q_CLASSINFO("InterfaceID", "{2D3A28A5-E55E-4b2b-9A03-320A7B98B76D}")
    Q_CLASSINFO("EventsID", "{3B53E3FC-853D-4b47-9C79-BF00F365A43B}")

    Q_PROPERTY(QSettings* settings READ settings)
    Q_PROPERTY(QObject *mainWindowObj READ mainWindowObj)
    Q_PROPERTY(QString clipboardText READ getClipboardText WRITE setClipboardText)
    Q_PROPERTY(QString currentProjectName READ curProjectName)
private:
    TTGlobalPrivate *d;
public:
    TTMainProc *proc;
    //TrkToolProject *currentProject;
    explicit TTGlobal(QObject *parent = 0);
//    static TTGlobal *global();
    QSqlDatabase userDatabase();
    Q_INVOKABLE QString toOemString(const QString &s);
    Q_INVOKABLE QSettings *settings();
    Q_INVOKABLE QMainWindow *mainWindow();
    Q_INVOKABLE QObject *mainWindowObj();
    Q_INVOKABLE bool addPropWidget(QWidget *prop);
    Q_INVOKABLE QString getClipboardText() const;
    Q_INVOKABLE void setClipboardText(const QString &text) const;
    Q_INVOKABLE QString getClipboardHTML() const;
    Q_INVOKABLE void setClipboardHTML(const QString &text) const;
    Q_INVOKABLE QString curProjectName() const;
    Q_INVOKABLE QObject *getRecord(int id);
    Q_INVOKABLE bool registerOptionsWidget(const QString &path, void *funcPtr);
    Q_INVOKABLE QNetworkAccessManager *networkManager() const;
    QMap<QString, GetOptionsWidgetFunc> optionsWidgets() const;
    Q_INVOKABLE QVariant CreateObject(const QString &objectName);
    Q_INVOKABLE QString saveObjectDocumentation(QObject *object, const QString &fileName = QString()) const;
protected:
    void readInitSettings();
    void upgradeUserDB();
    bool executeBatchFile(const QString &fileName);
signals:
    
public slots:
    void showError(const QString &text);
    void statusBarMessage(const QString& text);
    void shell(const QString &command);
    int shellLocale(const QString &command, const QString &locale = QString());

    /* Events */
public:
    Q_INVOKABLE QString initFileValue(const QString &elementPath, const QString &attr = QString());
    Q_INVOKABLE bool registerEventHandler(const QString &event, QObject *obj, const QString &method);
    Q_INVOKABLE bool unregisterEventHandler(const QString &event, QObject *obj, const QString &method);
    Q_INVOKABLE QObject *oauth();
    QScriptEngine *newScriptEngine();
    QAxScriptManager *newAxScriptManager();
protected:
    void initLibraryPath();
    void addLibraryPath(const QString &path);

protected:
    QStringList libDirs;
    TQOAuth *m_oauth;
public slots:
    void handleEvent(const QString &event,
                     QGenericReturnArgument ret,
                     QGenericArgument val0 = QGenericArgument( 0 ), QGenericArgument val1 = QGenericArgument(),
                     QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
                     QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
                     QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
                     QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

    /* Plugins */
public slots:
    void loadPlugins();
    bool loadSinglePlugin(const QDir &dir);
    void appendContextMenu(QMenu *menu);
    void queryViewOpened(QWidget *widget, QTableView *view, const QString &recType = "scr");
    void recordOpened(QWidget *widget, const QString &recType = "scr");
    bool insertViewTab(QWidget *view, QWidget *tab, const QString &title = "");
    void pluginError(const QString &pluginName, const QString &msg);
protected slots:
    void axScriptError( QAxScript * script, int code, const QString & description, int sourcePosition, const QString & sourceText );
};


TQPLUGIN_SHARED TTGlobal *ttglobal();

inline void ttShowError(const QString &error)
{
//    QMetaObject::invokeMethod(ttglobal(), "showError", Qt::DirectConnection, Q_ARG(QString, error));
    ttglobal()->showError(error);
}

inline void SQLError(const QSqlError &error)
{
    ttglobal()->showError(error.text());
}

inline void Error(const QString& text)
{
    ttglobal()->showError(text);
}



#endif // TTGLOBAL_H
