#include "idinput.h"
#include "ui_idinput.h"
#include "ttutils.h"

IdInput::IdInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IdInput)
{
    ui->setupUi(this);
}

IdInput::~IdInput()
{
    delete ui;
}

void IdInput::setData(const QString &text)
{
    ui->idEdit->setPlainText(text);
}

QString IdInput::getData()
{
    QString input = ui->idEdit->toPlainText();
    input = onlyInts(input);
    return input;
}
