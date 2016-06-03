#include "jirafinduser.h"
#include "ui_jirafinduser.h"
#include <QCompleter>
#include "jirausermodel.h"
#include <jiradb.h>

class JiraFindUserPrivate
{
public:
    QCompleter *completer;
    JiraUserModel *model;
    QTimer *timer;
};


JiraFindUser::JiraFindUser(JiraProject *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JiraFindUser),
    d(new JiraFindUserPrivate())
{
    ui->setupUi(this);
    d->model = new JiraUserModel(project);
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    d->timer->setInterval(500);
    connect(d->timer,SIGNAL(timeout()),SLOT(refreshModel()));

    d->completer = new QCompleter(this);
    d->completer->setModel(d->model);
    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
//    ui->lineEdit->setCompleter(d->completer);
    ui->comboBox->setCompleter(d->completer);
//    connect(ui->lineEdit,SIGNAL(textChanged(QString)),d->timer,SLOT(start()));

//    ui->comboBox->setModel(d->model);
    connect(ui->comboBox,SIGNAL(editTextChanged(QString)),d->timer,SLOT(start()));
}

JiraFindUser::~JiraFindUser()
{
    delete ui;
    delete d;
}

QString JiraFindUser::text()
{
//    return ui->lineEdit->text();
    return ui->comboBox->currentText();
}

void JiraFindUser::refreshModel()
{
//    d->model->refresh(ui->lineEdit->text());
    d->model->refresh(ui->comboBox->currentText());
}

class JiraUserComboBoxPrivate
{
public:
    QCompleter *completer;
    JiraUserModel *model;
    QTimer *timer;
};

JiraUserComboBox::JiraUserComboBox(JiraProject *project, QWidget *parent)
    : QComboBox(parent), d(new JiraUserComboBoxPrivate())
{
    setDuplicatesEnabled(false);
    setEditable(true);
    setEnabled(true);

    d->model = new JiraUserModel(project);
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    d->timer->setInterval(500);
    connect(d->timer,SIGNAL(timeout()),SLOT(refreshModel()));

    d->completer = new QCompleter(this);
    d->completer->setModel(d->model);
    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
    setCompleter(d->completer);
    connect(this,SIGNAL(editTextChanged(QString)),d->timer,SLOT(start()));
}

JiraUserComboBox::~JiraUserComboBox()
{
    delete d->model;
    delete d;
}

void JiraUserComboBox::refreshModel()
{
    d->model->refresh(currentText());
}
