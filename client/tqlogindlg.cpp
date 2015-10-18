#include "tqlogindlg.h"
#include "ui_tqlogindlg.h"
#include <QtWebKit>

class TQLoginDlgPrivate
{
public:
    TQLoginDlgPrivate()
    {

    }
};

TQLoginDlg::TQLoginDlg(QWidget *parent) :
    QDialog(parent),
    d(new TQLoginDlgPrivate()),
    ui(new Ui::TQLoginDlg)
{
    ui->setupUi(this);
}

TQLoginDlg::~TQLoginDlg()
{
    delete ui;
    delete d;
}

QString TQLoginDlg::user() const
{
    return ui->leUser->text();
}

void TQLoginDlg::setUser(const QString &user)
{
    ui->leUser->setText(user);
}

QString TQLoginDlg::password() const
{
    return ui->lePassword->text();
}

void TQLoginDlg::setPassword(const QString &password)
{
    ui->lePassword->setText(password);
}

QString TQLoginDlg::project() const
{
    return ui->lProject->text();
}

void TQLoginDlg::setProject(const QString &project)
{
    ui->lProject->setText(project);
}

void TQLoginDlg::setAuthenticator(const QAuthenticator *authenticator)
{
    setProject(authenticator->option("project").toString());
    setUser(authenticator->user());
    setPassword(authenticator->password());
}

void TQLoginDlg::assignToAuthenticator(QAuthenticator *authenticator) const
{
    authenticator->setUser(user());
    authenticator->setPassword(password());
}
