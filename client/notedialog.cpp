#include "notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) :
    QDialog(parent),
    Ui::NoteDialog()
{
    setupUi(this);
}

NoteDialog::~NoteDialog()
{
    //delete ui;
}
