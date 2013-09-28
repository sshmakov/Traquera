#ifndef TTGLOBAL_H
#define TTGLOBAL_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QString>

class MainWindow;
class TrkToolProject;

class TTGlobal : public QObject
{
    Q_OBJECT
private:
    explicit TTGlobal(QObject *parent = 0);
    QSqlDatabase userDb;
    QString userDbType;
    QString userDbPath;
    QString userDbUser;
    QString userDbPassword;
public:
    MainWindow *mainWindow;
    //TrkToolProject *currentProject;
    static TTGlobal *global();
    QSqlDatabase userDatabase();
protected:
    void readInitSettings();
    void upgradeUserDB();
    bool executeBatchFile(const QString &fileName);
signals:
    
public slots:
    void showError(const QString &text);
    void statusBarMessage(const QString& text);
    void shell(const QString &command);
};


inline TTGlobal *ttglobal()
{
    return TTGlobal::global();
}

inline void SQLError(const QSqlError &error)
{
    TTGlobal::global()->showError(error.text());
}

inline void Error(const QString& text)
{
    TTGlobal::global()->showError(text);
}

#endif // TTGLOBAL_H
