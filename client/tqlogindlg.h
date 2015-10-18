#ifndef TQLOGINDLG_H
#define TQLOGINDLG_H

#include <QDialog>

namespace Ui {
class TQLoginDlg;
}

class TQLoginDlgPrivate;
class QAuthenticator;

class TQLoginDlg : public QDialog
{
    Q_OBJECT
private:
    TQLoginDlgPrivate *d;
public:
    explicit TQLoginDlg(QWidget *parent = 0);
    ~TQLoginDlg();
    QString user() const;
    void setUser(const QString &user);
    QString password() const;
    void setPassword(const QString &password);
    QString project() const;
    void setProject(const QString &project);
    void setAuthenticator(const QAuthenticator *authenticator);
    void assignToAuthenticator(QAuthenticator *authenticator) const;
private:
    Ui::TQLoginDlg *ui;
};

#endif // TQLOGINDLG_H
