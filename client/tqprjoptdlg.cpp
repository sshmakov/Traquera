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
    connect(ui->tbEdit,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbEmail,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbGroups,SIGNAL(clicked()),SLOT(browseClicked()));
    connect(ui->tbView,SIGNAL(clicked()),SLOT(browseClicked()));
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
    ui->leView->setText(d->project->optionValue(TQOPTION_VIEW_TEMPLATE,
                                                ttglobal()->optionDefaultValue(TQOPTION_VIEW_TEMPLATE)).toString());
    ui->leEdit->setText(d->project->optionValue(TQOPTION_EDIT_TEMPLATE,
                        ttglobal()->optionDefaultValue(TQOPTION_EDIT_TEMPLATE)).toString());
    ui->leEmail->setText(d->project->optionValue(TQOPTION_EMAIL_TEMPLATE,
                         ttglobal()->optionDefaultValue(TQOPTION_EMAIL_TEMPLATE)).toString());
    ui->leGroups->setText(d->project->optionValue(TQOPTION_GROUP_FIELDS,
                          ttglobal()->optionDefaultValue(TQOPTION_GROUP_FIELDS)).toString());
}

int TQProjectOptionsDialog::exec()
{
    if(!d->project)
        return QDialog::Rejected;
    int res = QDialog::exec();
    if(res == QDialog::Accepted)
    {
        d->project->setOptionValue(TQOPTION_VIEW_TEMPLATE, ui->leView->text());
        d->project->setOptionValue(TQOPTION_EDIT_TEMPLATE, ui->leEdit->text());
        d->project->setOptionValue(TQOPTION_EMAIL_TEMPLATE, ui->leEmail->text());
        d->project->setOptionValue(TQOPTION_GROUP_FIELDS, ui->leGroups->text());
    }
}

void TQProjectOptionsDialog::browseClicked()
{
    bool xq = true;
    QObject *tb = sender();
    QLineEdit *edit = 0;
    if(tb == ui->tbEdit)
        edit = ui->leEdit;
    else if(tb == ui->tbEmail)
        edit = ui->leEmail;
    else if(tb == ui->tbView)
        edit = ui->leView;
    else if(tb == ui->tbGroups)
    {
        edit = ui->leGroups;
        xq = false;
    }
    if(!edit)
        return;
    QString oldFile = edit->text();
    QString newFile = QFileDialog::getOpenFileName(this, tr("Выберите файл"),
                                 oldFile,
                                 xq ? tr("XQuery (*.xq);;Все файлы (*.*)")
                                    : tr("XML (*.xml);;Все файлы (*.*)"));
    if(!newFile.isEmpty())
        edit->setText(newFile);
}
