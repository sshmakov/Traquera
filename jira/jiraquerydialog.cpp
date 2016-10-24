#include "jiraquerydialog.h"
#include "ui_jiraquerydialog.h"
#include "jiraqry.h"
#include "jiradb.h"

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
    if(jQry)
    {
        jQry->setName(ui->leName->text());
        jQry->setQueryLine(ui->pleJQL->toPlainText());
    }
}

JiraQueryDialogController::JiraQueryDialogController(JiraProject *project, int recType)
    : TQAbstractQWController(project)
{
    dlg = new JiraQueryDialog();
    dlg->jQry = new JiraQry(project, recType);
}

JiraQueryDialogController::~JiraQueryDialogController()
{
    delete dlg;
}

void JiraQueryDialogController::setQueryDefinition(TQQueryDef *def)
{
    JiraQry *q = qobject_cast<JiraQry *>(def);
    dlg->jQry = q;
    if(!q)
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
    if(dlg->jQry)
        dlg->jQry->setName(name);
}


