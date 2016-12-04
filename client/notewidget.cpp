#include "notewidget.h"
#include "ui_notewidget.h"
//#include <trkview.h>
#include "tqbase.h"
#include <QPlainTextEdit>
#include <QPushButton>

NoteWidget::NoteWidget(TQRecord *rec, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteWidget)
{
    ui->setupUi(this);
    record = rec;
    ui->titleEdit->addItems(record->typeDef()->noteTitleList());
    connect(ui->titleEdit,SIGNAL(editTextChanged(QString)),SLOT(onTitleChanged(QString)));
    connect(ui->noteEdit,SIGNAL(textChanged()),SLOT(onTextChanged()));
    connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),SLOT(onButtonClicked(QAbstractButton*)));
    QAction *a;
    a = new QAction(this);
    a->setShortcut(QKeySequence("Ctrl+Return"));
    a->setShortcutContext(Qt::WidgetShortcut);
    connect(a, SIGNAL(triggered(bool)), SLOT(submit()));
    ui->titleEdit->addAction(a);
    ui->noteEdit->addAction(a);

    a = new QAction(this);
    a->setShortcut(QKeySequence("Escape"));
    a->setShortcutContext(Qt::WidgetShortcut);
    connect(a, SIGNAL(triggered(bool)), SLOT(cancel()));
    ui->titleEdit->addAction(a);
    ui->noteEdit->addAction(a);
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

void NoteWidget::submit()
{
    QPushButton *btn;
    btn = ui->buttonBox->button(QDialogButtonBox::Save);
    if(!btn)
        btn = ui->buttonBox->button(QDialogButtonBox::Ok);
    if(btn)
        btn->click();
}

void NoteWidget::cancel()
{
    QPushButton *btn;
    btn = ui->buttonBox->button(QDialogButtonBox::Cancel);
    if(btn)
        btn->click();
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

bool NoteWidget::isButtonsVisible() const
{
    return ui->buttonBox->isVisible();
}

void NoteWidget::setButtonsVisible(bool value)
{
    ui->buttonBox->setVisible(value);
}

bool NoteWidget::isTitleVisible() const
{
    return ui->titleWidget->isVisible();
}

void NoteWidget::setTitleVisible(bool value)
{
    ui->titleWidget->setVisible(value);
}


void NoteWidget::onButtonClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton sb = ui->buttonBox->standardButton(button);
    if(sb == QDialogButtonBox::NoButton)
        return;
    if(sb == QDialogButtonBox::Save || sb == QDialogButtonBox::Ok)
        emit submitTriggered(noteIndex(), noteTitle(), noteText());
    else if(sb == QDialogButtonBox::Cancel)
        emit cancelTriggered(noteIndex());
}


void NoteWidget::onTitleChanged(const QString &title)
{
    emit noteTitleChanged(noteIndex(), title);
}

void NoteWidget::onTextChanged()
{
    emit noteTextChanged(noteIndex());
}
