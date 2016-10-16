#include "tqqrywid.h"
#include "ui_tqqrywid.h"
#include <QtGui>

TQQueryWidget::TQQueryWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TQQueryWidget)
{
    ui->setupUi(this);
    ui->lwFields->setSortingEnabled(true);
    modified = false;
}

TQQueryWidget::~TQQueryWidget()
{
    delete ui;
}

void TQQueryWidget::setRecordTypeDef(TQAbstractRecordTypeDef *def)
{
    recDef = def;
    ui->lwFields->clear();
    if(def)
    {
        foreach(QString fname, def->fieldNames())
        {
            int vid = def->fieldVid(fname);
            QListWidgetItem *item= new QListWidgetItem(fname, ui->lwFields);
            item->setData(Qt::UserRole, vid);
        }

//        foreach(int vid, def->fieldVids())
//        {
//            QString fname = def->fieldName(vid);
//            QListWidgetItem *item= new QListWidgetItem(fname, ui->lwFields);
//            item->setData(Qt::UserRole, vid);
//        }
        ui->lwFields->sortItems();
    }
}

void TQQueryWidget::setQueryDefinition(TQQueryDef *def)
{
    if(queryDef)
        disconnect(queryDef);
    queryDef = def;
    ui->lwCond->clear();
    if(queryDef)
    {
        setRecordTypeDef(queryDef->recordDef());
        QMenu *menu = new QMenu(this);
        QStringList list = queryDef->miscActions();
        QSignalMapper *mapper = new QSignalMapper(menu);
        connect(mapper, SIGNAL(mapped(QString)), queryDef, SLOT(miscActionTriggered(QString)));
        foreach(QString s, list)
        {
            QAction *a = menu->addAction(s);
            connect(a, SIGNAL(triggered()), mapper, SLOT(map()));
            mapper->setMapping(a, s);
        }
        QMenu *old = ui->btnAdd->menu();
        if(old)
            delete old;
        ui->btnAdd->setMenu(menu);
        connect(queryDef, SIGNAL(changed()), SLOT(refreshCList()));
    }
    else
        setRecordTypeDef(0);
    refreshCList();
    refreshControls();
    modified = false;
}

TQQueryDef *TQQueryWidget::queryDefinition()
{
    return queryDef;
}

int TQQueryWidget::currentField()
{
    QListWidgetItem *item = ui->lwFields->currentItem();
    if(!item)
        return 0;
    return item->data(Qt::UserRole).toInt();
}

int TQQueryWidget::currentCIndex()
{
    return ui->lwCond->currentIndex().row();
}

TQCond *TQQueryWidget::currentCondition()
{
    if(!queryDef)
        return 0;
    return queryDef->condition(currentCIndex());
}

void TQQueryWidget::setCurrentCondition(TQCond *cond)
{
    if(cond)
    {
        int i = queryDef->indexOf(cond);
        ui->lwCond->setCurrentRow(i);
    }
}

QString TQQueryWidget::queryName() const
{
    return ui->leQueryName->text();
}

void TQQueryWidget::setQueryName(const QString &name)
{
    ui->leQueryName->setText(name);
}

void TQQueryWidget::refreshCList()
{
    TQCond *cond = currentCondition();
    ui->lwCond->clear();
    if(queryDef)
    {
        ui->lwCond->addItems(queryDef->queryDefLines());
        setCurrentCondition(cond);
    }
    refreshControls();
}

void TQQueryWidget::refreshCondLine(TQCond *cond)
{
    int index = queryDef->indexOf(cond);
    if(index == -1)
        return;
    QListWidgetItem *item = ui->lwCond->item(index);
    if(item)
        item->setText(queryDef->condLine(index));
    refreshControls();
}

void TQQueryWidget::refreshControls()
{
    TQCond *cond = currentCondition();
    ui->rbAnd->setEnabled(cond);
    ui->rbOr->setEnabled(cond);
    ui->cbOpenBracket->setEnabled(cond);
    ui->cbNot->setEnabled(cond);
    ui->cbCloseBracket->setEnabled(cond);
    ui->btnModify->setEnabled(cond);
    ui->btnRemove->setEnabled(cond);
    if(!cond)
    {
        ui->pteQueryText->clear();
        return;
    }
    ui->rbAnd->setChecked(!cond->isOr());
    ui->rbOr->setChecked(cond->isOr());
    ui->cbOpenBracket->setChecked(cond->isOpenBracket());
    ui->cbNot->setChecked(cond->isNot());
    ui->cbCloseBracket->setChecked(cond->isCloseBracket());
    ui->pteQueryText->setPlainText(cond->condSubString());
}

void TQQueryWidget::on_lwCond_itemActivated(QListWidgetItem *item)
{
    int row = ui->lwCond->row(item);
    if(queryDef->editProperties(row))
    {
        modified = true;
        item->setText(queryDef->condLine(row));
        TQCond *cond = currentCondition();
        if(cond)
            ui->pteQueryText->setPlainText(cond->condSubString());
    }
}

void TQQueryWidget::on_btnAdd_clicked()
{
    int vid = currentField();
    if(!vid)
        return;

    TQCond *cond = queryDef->newCondition(vid);
    if(!cond)
        return;

    if(cond->editProperties())
    {
        queryDef->appendCondition(cond);
        refreshCList();
    }
    setCurrentCondition(cond);
}

void TQQueryWidget::on_rbAnd_clicked(bool checked)
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    cond->setIsOr(!checked);
    refreshCondLine(cond);
}

void TQQueryWidget::on_rbOr_clicked()
{
    on_rbAnd_clicked(false);
}

void TQQueryWidget::on_lwCond_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow)
    refreshControls();
}

void TQQueryWidget::on_btnRemove_clicked()
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    int i = queryDef->indexOf(cond);
    queryDef->removeCondition(i);
    refreshCList();
}

void TQQueryWidget::on_btnMoveUp_clicked()
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    int i = queryDef->indexOf(cond);
    if(!i)
        return;
    queryDef->takeCondition(i);
    queryDef->insertCondition(i-1, cond);
    refreshCList();
    setCurrentCondition(cond);
}

void TQQueryWidget::on_btnMoveDown_clicked()
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    int i = queryDef->indexOf(cond);
    if(i >= queryDef->count()-1)
        return;
    queryDef->takeCondition(i);
    queryDef->insertCondition(i+1, cond);
    refreshCList();
    setCurrentCondition(cond);
}

void TQQueryWidget::on_cbOpenBracket_clicked(bool checked)
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    cond->setIsOpenBracket(checked);
    refreshCondLine(cond);
}

void TQQueryWidget::on_cbNot_clicked(bool checked)
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    cond->setIsNot(checked);
    refreshCondLine(cond);
}

void TQQueryWidget::on_cbCloseBracket_clicked(bool checked)
{
    TQCond *cond = currentCondition();
    if(!cond)
        return;
    cond->setIsCloseBracket(checked);
    refreshCondLine(cond);
}

void TQQueryWidget::on_btnModify_clicked()
{
    QListWidgetItem *item = ui->lwCond->currentItem();
    if(item)
        on_lwCond_itemActivated(item);
}

void TQQueryWidget::on_buttonBox_accepted()
{
    if(!ui->leQueryName->text().trimmed().isEmpty())
        accept();
}

TQAbstractQWController::TQAbstractQWController(QObject *parent)
    : QObject(parent)
{
}


class TQQueryWidgetControllerPrivate
{
public:
    TQQueryWidget *dlg;
};

TQQueryWidgetController::TQQueryWidgetController(QObject *parent)
    : TQAbstractQWController(parent)
{
    d = new TQQueryWidgetControllerPrivate();
    d->dlg = new TQQueryWidget();
}

TQQueryWidgetController::~TQQueryWidgetController()
{
    delete d->dlg;
    delete d;
}

void TQQueryWidgetController::setQueryDefinition(TQQueryDef *def)
{
    d->dlg->setQueryDefinition(def);
}

TQQueryDef *TQQueryWidgetController::queryDefinition()
{
    return d->dlg->queryDefinition();
}

int TQQueryWidgetController::exec()
{
    return d->dlg->exec();
}

QString TQQueryWidgetController::queryName() const
{
    return d->dlg->queryName();
}

void TQQueryWidgetController::setQueryName(const QString &name)
{
    d->dlg->setQueryName(name);
}

