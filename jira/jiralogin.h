#ifndef JIRALOGIN_H
#define JIRALOGIN_H

#include <QDialog>

namespace Ui {
class JiraLogin;
}

class JiraLoginPrivate;

class JiraLogin : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QString connectString READ connectString WRITE setConnectString)
    Q_PROPERTY(QString connectSaveString READ connectSaveString)
private:
    JiraLoginPrivate *d;
public:
    explicit JiraLogin(QWidget *parent = 0);
    ~JiraLogin();
    QString connectString() const;
    QString connectSaveString() const;
    void setConnectString(const QString &string);
    QStringList readProjects();
private slots:
    void on_cbMethod_currentIndexChanged(int index);
    void on_tbRefreshProjects_clicked();

private:
    Ui::JiraLogin *ui;
};

#endif // JIRALOGIN_H
