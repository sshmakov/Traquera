#include "tqprjoptdlg.h"
#include "ui_tqprjoptdlg.h"

TQProjectOptionsDialog::TQProjectOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TQProjectOptionsDialog)
{
    ui->setupUi(this);
}

TQProjectOptionsDialog::~TQProjectOptionsDialog()
{
    delete ui;
}
