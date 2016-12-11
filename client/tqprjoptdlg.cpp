#include "tqprjoptdlg.h"
#include "ui_tqprjoptdlg.h"
#include <ttglobal.h>
#include <tqbase.h>
#include <QFileDialog>

class TQProjectOptionsDialogPrivate
{
public:
    TQAbstractProject *project;
};

TQProjectOptionsDialog::TQProjectOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TQProjectOptionsDialog)
{
    d = new TQProjectOptionsDialogPrivate();
    ui->setupUi(this);
    connect(ui->tbView,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbEdit,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbPrint,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbEmail,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbEmailScript,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbGroups,SIGNAL(clicked()),SLOT(browseClicked()));
}

TQProjectOptionsDialog::~TQProjectOptionsDialog()
{
    delete ui;
    delete d;
}

void TQProjectOptionsDialog::setProject(TQAbstractProject *prj)
{
    d->project = prj;
    if(!d->project)
        return;
    ui->leView->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_VIEW_TEMPLATE).toString()));
    ui->leEdit->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_EDIT_TEMPLATE).toString()));
    ui->lePrint->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_PRINT_TEMPLATE).toString()));
    ui->leEmail->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_EMAIL_TEMPLATE).toString()));
    ui->leEmailScript->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_EMAIL_SCRIPT).toString()));
    ui->leGroups->setText(
                QDir::toNativeSeparators(
                    d->project->optionValue(TQOPTION_GROUP_FILE).toString()));
}

int TQProjectOptionsDialog::exec()
{
    if(!d->project)
        return QDialog::Rejected;
    int res = QDialog::exec();
    if(res == QDialog::Accepted)
    {
        d->project->setOptionValue(TQOPTION_VIEW_TEMPLATE,
                                   QDir::fromNativeSeparators(ui->leView->text()));
        d->project->setOptionValue(TQOPTION_EDIT_TEMPLATE,
                                   QDir::fromNativeSeparators(ui->leEdit->text()));
        d->project->setOptionValue(TQOPTION_PRINT_TEMPLATE,
                                   QDir::fromNativeSeparators(ui->lePrint->text()));
        d->project->setOptionValue(TQOPTION_EMAIL_TEMPLATE,
                                   QDir::fromNativeSeparators(ui->leEmail->text()));
        d->project->setOptionValue(TQOPTION_EMAIL_SCRIPT,
                                   QDir::fromNativeSeparators(ui->leEmailScript->text()));
        d->project->setOptionValue(TQOPTION_GROUP_FILE,
                                   QDir::fromNativeSeparators(ui->leGroups->text()));
    }
}

void TQProjectOptionsDialog::browseClicked()
{
    bool xq = true, js = false;
    QObject *tb = sender();
    QLineEdit *edit = 0;
    if(tb == ui->tbEdit)
        edit = ui->leEdit;
    else if(tb == ui->tbView)
        edit = ui->leView;
    else if(tb == ui->tbPrint)
        edit = ui->lePrint;
    else if(tb == ui->tbEmail)
        edit = ui->leEmail;
    else if(tb == ui->tbEmailScript)
    {
        js = true;
        edit = ui->leEmailScript;
    }
    else if(tb == ui->tbGroups)
    {
        edit = ui->leGroups;
        xq = false;
    }
    if(!edit)
        return;
    QString oldFile = edit->text();
    QString newFile =
            QFileDialog::getOpenFileName(this, tr("Выберите файл"),
                                         oldFile,
                                         js ? tr("JavaScript (*.js);;Все файлы (*.*)")
                                            : xq ? tr("XQuery (*.xq);;Все файлы (*.*)")
                                                 : tr("XML (*.xml);;Все файлы (*.*)"));
    if(!newFile.isEmpty())
        edit->setText(QDir::toNativeSeparators(newFile));
}
