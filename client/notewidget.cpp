#include "notewidget.h"
#include "ui_notewidget.h"
#include <trkview.h>
#include <QPlainTextEdit>

NoteWidget::NoteWidget(TrkToolRecord *rec, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteWidget)
{
    ui->setupUi(this);
    record = rec;
    ui->titleEdit->addItems(record->typeDef()->project()->noteTitles);
    connect(ui->titleEdit,SIGNAL(editTextChanged(QString)),SIGNAL(changedNoteTitle(QString)));
    connect(ui->noteEdit,SIGNAL(textChanged()),SIGNAL(changedNoteText()));
    connect(ui->submitButton,SIGNAL(clicked()),SLOT(onSubmitClicked()));
    connect(ui->cancelButton,SIGNAL(clicked()),SLOT(onCancelClicked()));
}

NoteWidget::~NoteWidget()
{
    delete ui;
}

QString NoteWidget::noteTitle() const
{
    return ui->titleEdit->currentText();
}

QString NoteWidget::noteText() const
{
    return ui->noteEdit->toPlainText();
}

int NoteWidget::noteIndex() const
{
    return index;
}

void NoteWidget::setNoteIndex(int newIndex)
{
    index = newIndex;
    ui->label->setVisible(index != -1);
    ui->titleEdit->setEnabled(index != -1);
}

void NoteWidget::setNoteTitle(const QString &title)
{
    ui->titleEdit->setEditText(title);
}

void NoteWidget::setNoteText(const QString &text)
{
    ui->noteEdit->setPlainText(text);
}


void NoteWidget::onSubmitClicked()
{
    emit submitTriggered(noteIndex(), noteTitle(), noteText());
}

void NoteWidget::onCancelClicked()
{
    emit cancelTriggered(noteIndex());
}
