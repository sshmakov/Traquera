#ifndef TTGLOBAL_H
#define TTGLOBAL_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

class MainWindow;
class TrkToolProject;
class QSettings;
class QTableView;
class QMenu;

class TTGlobal : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSettings* settings READ settings)
private:
    explicit TTGlobal(QObject *parent = 0);
    QSqlDatabase userDb;
    QString userDbType;
    QString userDbPath;
    QString userDbUser;
    QString userDbPassword;
    QSettings *settingsObj;
public:
    MainWindow *mainWindow;
    //TrkToolProject *currentProject;
    static TTGlobal *global();
    QSqlDatabase userDatabase();
    Q_INVOKABLE QString toOemString(const QString &s);
    Q_INVOKABLE QSettings *settings();
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
protected:
    QMultiHash <QString, QPair<QObject*,QString> > handlers;
public:
    Q_INVOKABLE bool registerEventHandler(const QString &event, QObject *obj, const QString &method);
    Q_INVOKABLE bool unregisterEventHandler(const QString &event, QObject *obj, const QString &method);
public slots:
    void handleEvent(const QString &event,
                     QGenericReturnArgument ret,
                     QGenericArgument val0 = QGenericArgument( 0 ), QGenericArgument val1 = QGenericArgument(),
                     QGenericArgument val2 = QGenericArgument(), QGenericArgument val3 = QGenericArgument(),
                     QGenericArgument val4 = QGenericArgument(), QGenericArgument val5 = QGenericArgument(),
                     QGenericArgument val6 = QGenericArgument(), QGenericArgument val7 = QGenericArgument(),
                     QGenericArgument val8 = QGenericArgument(), QGenericArgument val9 = QGenericArgument());

    /* Plugins */
protected:
    QObjectList plugins;
public slots:
    void loadPlugins();
    void appendContextMenu(QMenu *menu);
    void queryViewOpened(QWidget *widget, QTableView *view, const QString &recType = "scr");
    void recordOpened(QWidget *widget, const QString &recType = "scr");
    bool insertViewTab(QWidget *view, QWidget *tab, const QString &title = "");
};


inline TTGlobal *ttglobal()
{
    return TTGlobal::global();
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
