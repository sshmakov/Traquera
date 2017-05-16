#include "notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) :
    QDialog(parent),
    Ui::NoteDialog()
{
    setupUi(this);
    addAction(actionAddNote);
}

NoteDialog::~NoteDialog()
{
    //delete ui;
}

void NoteDialog::setNoteTitleVisible(bool value)
{
    titleEdit->setVisible(value);
}

void NoteDialog::on_actionAddNote_triggered()
{
    accept();
}
