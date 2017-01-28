#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include <QPushButton>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

QString AboutDialog::regName() const
{
    return ui->leName->text();
}

void AboutDialog::setRegName(const QString &name)
{
    ui->leName->setText(name);
}

QString AboutDialog::regKey() const
{
    return ui->leKey->text();
}

void AboutDialog::setRegKey(const QString &key)
{
    ui->leKey->setText(key);
}

void AboutDialog::setText(const QString &text)
{
    ui->lText->setText(text);
}

void AboutDialog::setEditEnabled(bool enable)
{
    ui->leName->setReadOnly(!enable);
    ui->leKey->setReadOnly(!enable);
    if(!enable)
    {
        QPushButton *btn = ui->buttonBox->button(QDialogButtonBox::Cancel);
        if(btn)
            btn->setVisible(false);
    }
}

int AboutDialog::exec()
{
    if(!QDialog::exec())
        return QDialog::Rejected;
    if(ui->leKey->isReadOnly())
        return QDialog::Rejected;
    return QDialog::Accepted;
}
