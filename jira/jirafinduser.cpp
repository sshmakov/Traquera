#include "jirafinduser.h"
#include "ui_jirafinduser.h"
#include <QtGui>
#include "jirausermodel.h"
#include <jiradb.h>
#include <tqdebug.h>

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

class JiraUserEditPrivate
{
public:
    JiraProject *project;
    const JiraRecTypeDef *recordDef;
//    QFrame *frame;
//    QListView *popupList;
//    QCompleter *completer;
    JiraUserModel *model;
    QTimer *timer;
    QVariant curValue;
    QString link;
};

JiraUserEdit::JiraUserEdit(JiraProject *project, const JiraRecTypeDef *recordDef, QWidget *parent)
    : TQSearchBox(parent), d(new JiraUserEditPrivate())
{
//    setDuplicatesEnabled(false);
//    setEditable(true);
//    setEnabled(true);

    d->project = project;
    d->recordDef = recordDef;
    d->model = new JiraUserModel(project);
    setModel(d->model);
    d->timer = new QTimer(this);
    d->timer->setSingleShot(true);
    d->timer->setInterval(500);
    connect(d->timer,SIGNAL(timeout()),SLOT(refreshModel()));
//    setModel(d->model);

//    d->completer = new QCompleter(this);
//    d->completer->setModel(d->model);
//    d->completer->setCaseSensitivity(Qt::CaseInsensitive);
//    setCompleter(d->completer);
//    setCompleter(0);


    /*
    d->frame = new QFrame(0, Qt::Tool | Qt::FramelessWindowHint);
    installEventFilter(d->frame);
    QHBoxLayout *lay = new QHBoxLayout(d->frame);
    lay->setSpacing(0);
    lay->setContentsMargins(0,0,0,0);
    d->popupList = new QListView(d->frame);
    d->popupList->setModel(d->model);
    lay->addWidget(d->popupList);
    */
//    connect(this, SIGNAL(modelIndexSelected(QModelIndex)), SLOT(slotItemClicked(QModelIndex)));
//    connect(d->popupList, SIGNAL(clicked(QModelIndex)), SLOT(slotItemClicked(QModelIndex)));
//    d->listWidget->setWindowFlags(Qt::Popup);

    connect(this,SIGNAL(textChanged(QString)),d->timer,SLOT(start()));
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(slotTextChanged(QString)));
//    connect(this,SIGNAL(editTextChanged(QString)),d->timer,SLOT(start()));
//    connect(this,SIGNAL(editTextChanged(QString)),this,SLOT(slotTextChanged(QString)));

}

JiraUserEdit::~JiraUserEdit()
{
//    delete d->frame;
    delete d->model;
    delete d;
}

void JiraUserEdit::setCompleteLink(const QString &link)
{
    d->link = link;
    d->model->setCompleteLink(link);
}

QVariant JiraUserEdit::currentUser() const
{
//    QModelIndex index = currentIndex();
//    if(index.isValid())
//        return index.data(Qt::UserRole);
    return d->curValue;
//    int i = currentIndex();
//    tqDebug() << QString::number(i) << currentText() << d->model->index(i,0).data(Qt::EditRole).toString();

//    return currentText();
}

void JiraUserEdit::setCurrentUser(const QVariant &user)
{
    d->curValue = user;
    QString fullName = d->project->userFullName(user.toString());
    setText(fullName);
    //    setEditText(user.toString());
}

void JiraUserEdit::setFilterText(const QString &text)
{
    d->model->refresh(text);
}

void JiraUserEdit::popupItemSelected(const QModelIndex &index)
{
    d->curValue = index.data(Qt::EditRole);
    TQSearchBox::popupItemSelected(index);
}

/*
bool JiraUserEdit::event(QEvent *event)
{
    if(event->type() == QEvent::ShortcutOverride)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
//        qDebug() << "shortcut" << keyEvent->key() << keyEvent->modifiers();
        if(keyEvent->key() == Qt::Key_Down
                && keyEvent->modifiers() == Qt::NoModifier)
        {
            qDebug() << "shortcut accept";
            d->frame->activateWindow();
            d->popupList->setFocus();
            d->frame->update();
//            keyEvent->accept();
//            return true;
        }
    }
    bool res = TQSearchBox::event(event);
//    qDebug() << event->type() << event->isAccepted() << res;
    return res;
}

bool JiraUserEdit::eventFilter(QObject *obj, QEvent *event)
{
    qDebug() << event->type() << event->isAccepted();
    return false;
}

void JiraUserEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    qDebug() << "keypress" << keyEvent->key() << keyEvent->modifiers();
    if(keyEvent->key() == Qt::Key_Down
            && keyEvent->modifiers() == Qt::NoModifier)
    {
        qDebug() << "keypress accept";
//        d->popupList->setFocus();
//        d->frame->update();
//        keyEvent->accept();
        return;
    }
    QLineEdit::keyPressEvent(keyEvent);
}

void JiraUserEdit::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    showPopup();
}

void JiraUserEdit::hideEvent(QHideEvent *event)
{
    hidePopup();
    QWidget::hideEvent(event);
}

void JiraUserEdit::showPopup()
{
//    QComboBox::showPopup();
//    return;

    QRect rec = geometry();

    // get the two possible list points and height
    QRect screen = QApplication::desktop()->screenGeometry(this);
    QPoint above = this->mapToGlobal(QPoint(0,0));
//    int aboveHeight = above.y() - screen.y();
    QPoint below = this->mapToGlobal(QPoint(0,rec.height()));
//    int belowHeight = screen.bottom() - below.y();

    // first activate it with height 1px to get all the items initialized
    QRect rec2;
    rec2.setTopLeft(below);
    rec2.setWidth(rec.width());
    rec.setHeight(100);
    d->frame->setGeometry(rec2);
    d->frame->raise();
    d->frame->show();
}

void JiraUserEdit::hidePopup()
{
//    QComboBox::hidePopup();
//    return;

    d->popupList->hide();
}

*/

void JiraUserEdit::refreshModel()
{
//    d->model->refresh(currentText());
    d->model->refresh(text());
}

void JiraUserEdit::slotTextChanged(const QString &newText)
{
    tqDebug() << "New text:" << newText;
}

void JiraUserEdit::slotItemClicked(const QModelIndex &index)
{
    if(index.isValid())
        setCurrentUser(index.data(Qt::EditRole));
    else
        setCurrentUser(QVariant(QVariant::String));
}
