#include "optionsform.h"
#include "ui_optionsform.h"

OptionsForm::OptionsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsForm),
    curWidget(0), curItem(0)
{
    ui->setupUi(this);
}

OptionsForm::~OptionsForm()
{
    delete ui;
}

QTreeWidgetItem *OptionsForm::getItem(const QString &path)
{
    return items.value(path,0);
}

QString OptionsForm::getParentPath(const QString &path)
{
    QStringList tree = path.split("/");
    if(tree.size())
        tree.removeLast();
    else
        return QString();
    return tree.join("/");
}

QString OptionsForm::getSectionPath(int option)
{
    switch(option)
    {
    case OptionGeneral:
        return tr("General");
    }
    return QString();
}

bool OptionsForm::registerWidget(const QString &path, GetOptionsWidgetFunc optionWidgetFunc)
{
    if(path.isEmpty() || items.contains(path))
        return false;
    QString parentPath = getParentPath(path);
    QString name = path.split("/").last();
    QTreeWidgetItem *item;
    QTreeWidgetItem *parentItem =0;
    if(!parentPath.isEmpty())
    {
        parentItem = getItem(parentPath);
        if(!parentItem)
        {
            if(!registerWidget(parentPath,0))
                return false;
            parentItem = getItem(parentPath);
        }
    }
    if(parentItem)
        item = new QTreeWidgetItem(parentItem);
    else
        item = new QTreeWidgetItem(ui->treeWidget);
    item->setData(0,Qt::EditRole, path);
    item->setData(0,Qt::DisplayRole, name);
    item->setData(0,Qt::UserRole, (int)optionWidgetFunc);
    items.insert(path, item);
    return true;
}

QWidget *OptionsForm::optionsWidget(const QString &path)
{
    if(!items.contains(path))
        return 0;
    bool ok;
    int p = items.value(path)->data(0,Qt::UserRole).toInt(&ok);
    if(!ok || !p)
        return 0;
    GetOptionsWidgetFunc func = (GetOptionsWidgetFunc)p;
    QWidget *widget = func(path);
    return widget;
}

bool OptionsForm::canCloseCurWidget(int r)
{
    if(curWidget)
    {
        bool isAccept = r == Accepted;
        QEvent e(QEvent::OkRequest);
        e.setAccepted(isAccept);
        if(!QApplication::sendEvent(curWidget,&e))
            return true;
        return e.isAccepted();
//        r = e.isAccepted() ? Accepted : Rejected;
    }
    return true;
}

void OptionsForm::done(int r)
{
    if(!canCloseCurWidget(r))
        return;
    QDialog::done(r);
}

void OptionsForm::accept()
{
    /*
    if(curWidget)
    {
        QEvent e(QEvent::OkRequest);
        e.setAccepted(true);
        if(!QApplication::sendEvent(curWidget,&e))
            return;
        if(!e.isAccepted())
            return;
    }
    */
    QDialog::accept();
}

int OptionsForm::exec()
{
    ui->treeWidget->expandAll();
    return QDialog::exec();
}

void OptionsForm::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(curItem == current)
        return;
    curItem = current;
    if(curWidget)
    {
        int r = Rejected;
        if(!canCloseCurWidget(r))
        {
            curItem = previous;
            ui->treeWidget->setCurrentItem(previous);
            return;
        }
        if(!curWidget->close())
        {
            ui->treeWidget->setCurrentItem(previous);
            return;
        }
        curWidget = 0;
    }
    QString path = current->data(0,Qt::EditRole).toString();
    bool ok;
    int p = current->data(0,Qt::UserRole).toInt(&ok);
    if(!ok || !p)
        return;
    GetOptionsWidgetFunc func = (GetOptionsWidgetFunc)p;
    curWidget = func(path);
    if(!curWidget)
        return;
    ui->scrollArea->setWidget(curWidget);
}
