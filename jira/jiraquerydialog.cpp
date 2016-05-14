#include "jiraquerydialog.h"
#include "ui_jiraquerydialog.h"
#include "jiraqry.h"

JiraQueryDialog::JiraQueryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JiraQueryDialog)
{
    ui->setupUi(this);
}

JiraQueryDialog::~JiraQueryDialog()
{
    delete ui;
}

void JiraQueryDialog::on_buttonBox_accepted()
{
    jQry->setName(ui->leName->text());
    jQry->setQueryLine(ui->leName->text());
}

JiraQueryDialogController::JiraQueryDialogController(QObject *parent)
{
    dlg = new JiraQueryDialog();
}

JiraQueryDialogController::~JiraQueryDialogController()
{
    delete dlg;
}

void JiraQueryDialogController::setQueryDefinition(TQQueryDef *def)
{
    dlg->jQry = qobject_cast<JiraQry *>(def);
    if(!dlg->jQry)
    {
        dlg->ui->leName->clear();
        dlg->ui->pleJQL->clear();
    }
    else
    {
        dlg->ui->leName->setText(dlg->jQry->name());
        dlg->ui->pleJQL->setPlainText(dlg->jQry->queryLine());
    }
}

TQQueryDef *JiraQueryDialogController::queryDefinition()
{
    return dlg->jQry;
}

int JiraQueryDialogController::exec()
{
    return dlg->exec();
}

QString JiraQueryDialogController::queryName() const
{
    return dlg->ui->leName->text();
}

void JiraQueryDialogController::setQueryName(const QString &name)
{
    dlg->ui->leName->setText(name);
}


