#include "tqcondwidgets.h"
#include <QtGui>
#include "tqbase.h"

TQAbstractCondDialog::TQAbstractCondDialog(QWidget *parent)
    : QDialog(parent)
{
}

TQNumberCondDialog::TQNumberCondDialog(QWidget *parent) :
    TQAbstractCondDialog(parent)
{
    opTexts.append(tr("?"));
    opTexts.append(tr("="));
    opTexts.append(tr("between"));
    opTexts.append(tr(">="));
    opTexts.append(tr("<="));

    vLay = new QVBoxLayout();
    lay = new QHBoxLayout();
    fLabel = new QLabel();
    lay->addWidget(fLabel);
    opBtn = new QPushButton();
    opMenu = new QMenu(this);
    for(int i=1; i<opTexts.count(); i++)
        opMenu->addAction(opTexts[i]);
    connect(opMenu,SIGNAL(triggered(QAction*)),SLOT(opBtnMenu(QAction*)));
    opBtn->setMenu(opMenu);
    lay->addWidget(opBtn);
    numEdit1 = new QLineEdit();
    connect(numEdit1,SIGNAL(editingFinished()),SLOT(valueChanged()));
    lay->addWidget(numEdit1);
    andLabel = new QLabel();
    andLabel->setText(tr("and"));
    lay->addWidget(andLabel);
    numEdit2 =new QLineEdit();
    connect(numEdit2,SIGNAL(editingFinished()),SLOT(valueChanged()));
    lay->addWidget(numEdit2);
    vLay->addLayout(lay);
    box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->setOrientation(Qt::Horizontal);
    connect(box,SIGNAL(accepted()),SLOT(accept()));
    connect(box,SIGNAL(rejected()),SLOT(reject()));
    vLay->addWidget(box);
    setLayout(vLay);
}

void TQNumberCondDialog::setCondition(const TQCond &condition)
{
    cond = condition;
    fLabel->setText(cond.queryDef->recordDef()->fieldName(cond.vid()));
    opBtn->setText(opTexts[cond.op]);
    numEdit1->setText(cond.value1.toString());
    numEdit2->setText(cond.value2.toString());
    bool isBetween = (cond.op == TQNumberCond::Between);
    andLabel->setEnabled(isBetween);
    numEdit2->setEnabled(isBetween);
}

TQCond &TQNumberCondDialog::condition()
{
    return cond;
}


void TQNumberCondDialog::opBtnMenu(QAction *action)
{
    QString text = action->text();
    for(int i = 0; i<opTexts.count(); i++)
        if(text == opTexts[i])
        {
            opBtn->setText(text);
            cond.op = i;
            bool isBetween = (cond.op == TQNumberCond::Between);
            andLabel->setEnabled(isBetween);
            numEdit2->setEnabled(isBetween);
            return;
        }
}

void TQNumberCondDialog::valueChanged()
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(sender());
    if(edit == numEdit1)
    {
        cond.value1 = edit->text().toInt();
        edit->setText(cond.value1.toString());
    }
    else if(edit == numEdit2)
    {
        cond.value2 = edit->text().toInt();
        edit->setText(cond.value2.toString());
    }
}


TQChoiceCondDialog::TQChoiceCondDialog(QWidget *parent)
    :TQAbstractCondDialog(parent)
{
    QVBoxLayout *lay = new QVBoxLayout(this);
    box = new QGroupBox();
    QVBoxLayout *boxLay = new QVBoxLayout(box);
    rbNull = new QRadioButton(tr("Null"));
    boxLay->addWidget(rbNull);
    rbAny = new QRadioButton(tr("Any"));
    boxLay->addWidget(rbAny);
    rbSelected = new QRadioButton(tr("Selected:"));
    boxLay->addWidget(rbSelected);
    vList = new QListWidget();
    vList->setSelectionMode(QAbstractItemView::MultiSelection);
    lay->addWidget(box);
    lay->addWidget(vList);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->setOrientation(Qt::Horizontal);
    connect(box,SIGNAL(accepted()),SLOT(accept()));
    connect(box,SIGNAL(rejected()),SLOT(reject()));
    lay->addWidget(box);

    QSignalMapper *mapper = new QSignalMapper(this);
    connect(rbNull,SIGNAL(clicked()),mapper,SLOT(map()));
    connect(rbAny,SIGNAL(clicked()),mapper,SLOT(map()));
    connect(rbSelected,SIGNAL(clicked()),mapper,SLOT(map()));
    mapper->setMapping(rbNull,TQChoiceCond::Null);
    mapper->setMapping(rbAny,TQChoiceCond::Any);
    mapper->setMapping(rbSelected,TQChoiceCond::Selected);
    connect(mapper,SIGNAL(mapped(int)),SLOT(opChecked(int)));

    connect(vList,SIGNAL(itemSelectionChanged()),this,SLOT(selectionChanged()));
}

void TQChoiceCondDialog::setCondition(const TQCond &condition)
{
    cond = condition;
    QString chTable = cond.queryDef->recordDef()->fieldChoiceTable(cond.vid());
    choices = cond.queryDef->recordDef()->choiceTable(chTable);
    QMutexLocker locker(&mutex);
    vList->clear();
    foreach(const TQChoiceItem &item, choices)
    {
        QListWidgetItem *line =new QListWidgetItem(vList);
        line->setText(item.displayText);
        QVariant value = QVariant(item.fieldValue);
        line->setData(Qt::UserRole, value);
        if(cond.values.contains(item))
            line->setSelected(true);
    }
    rbNull->setChecked(cond.op == TQChoiceCond::Null);
    rbAny->setChecked(cond.op == TQChoiceCond::Any);
    rbSelected->setChecked(cond.op == TQChoiceCond::Selected);
    vList->setEnabled(cond.op == TQChoiceCond::Selected);
}

TQCond &TQChoiceCondDialog::condition()
{
    return cond;
}

void TQChoiceCondDialog::opChecked(int op)
{
    if(!mutex.tryLock())
        return;
    cond.op = op;
    vList->setEnabled(cond.op == TQChoiceCond::Selected);
    mutex.unlock();
}

void TQChoiceCondDialog::selectionChanged()
{
    if(!mutex.tryLock())
        return;
    cond.values.clear();
    foreach(QListWidgetItem *line, vList->selectedItems())
    {
        QVariant value = line->data(Qt::UserRole);
        foreach(const TQChoiceItem &item, choices)
            if(item.fieldValue == value)
            {
                cond.values.append(item);
                break;
            }
    }
    mutex.unlock();
}


TQUserCondDialog::TQUserCondDialog(QWidget *parent)
    : TQAbstractCondDialog(parent)
{
    QVBoxLayout *vLay = new QVBoxLayout(this);
    QGroupBox *grbox = new QGroupBox();
    QHBoxLayout *grLay = new QHBoxLayout(grbox);
    rbUsers = new QRadioButton(tr("Пользователи"));
    grLay->addWidget(rbUsers);
    rbGroups = new QRadioButton(tr("Группы"));
    grLay->addWidget(rbGroups);
    vLay->addWidget(grbox);
    connect(rbUsers,SIGNAL(clicked()), SLOT(usersChecked()));
    connect(rbGroups,SIGNAL(clicked()), SLOT(groupsChecked()));



    vList = new QListWidget();
    vList->setSelectionMode(QAbstractItemView::MultiSelection);
    vLay->addWidget(vList);

    statebox = new QGroupBox();
    QVBoxLayout *boxLay = new QVBoxLayout(statebox);
    rbActive = new QRadioButton(tr("Только активные"));
    boxLay->addWidget(rbActive);
    rbDeleted = new QRadioButton(tr("Только удаленные"));
    boxLay->addWidget(rbDeleted);
    rbAny = new QRadioButton(tr("Любые"));
    boxLay->addWidget(rbAny);
    vLay->addWidget(statebox);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->setOrientation(Qt::Horizontal);
    connect(box,SIGNAL(accepted()),SLOT(accept()));
    connect(box,SIGNAL(rejected()),SLOT(reject()));
    vLay->addWidget(box);

    QSignalMapper *mapper = new QSignalMapper(this);
    connect(rbActive,SIGNAL(clicked()),mapper,SLOT(map()));
    connect(rbAny,SIGNAL(clicked()),mapper,SLOT(map()));
    connect(rbDeleted,SIGNAL(clicked()),mapper,SLOT(map()));
    mapper->setMapping(rbActive,1);
    mapper->setMapping(rbAny,0);
    mapper->setMapping(rbDeleted,-1);
    connect(mapper,SIGNAL(mapped(int)),SLOT(stateChecked(int)));

    connect(vList,SIGNAL(itemSelectionChanged()),SLOT(selectionChanged()));
}

void TQUserCondDialog::setCondition(const TQCond &condition)
{
    QMutexLocker locker(&mutex);
    cond = condition;
    refresh();
}

TQCond &TQUserCondDialog::condition()
{
    return cond;
}

void TQUserCondDialog::refresh()
{
    vList->clear();
    QListWidgetItem *line;
    if(cond.isGroups)
    {
        rbGroups->setChecked(true);
        rbUsers->setChecked(false);
        TQGroupList groups =  cond.queryDef->recordDef()->project()->userGroups();
        foreach(const TQGroup &item, groups)
        {
            line = new QListWidgetItem(vList);
            line->setText(item.name);
            line->setData(Qt::UserRole, item.id);
            if(cond.ids.contains(item.id))
                line->setSelected(true);
        }
    }
    else
    {
        rbGroups->setChecked(false);
        rbUsers->setChecked(true);
        if(cond.isCurrentAvailable)
        {
            line = new QListWidgetItem(vList);
            line->setText(tr("<Текущий>"));
            line->setData(Qt::UserRole, -1);
            if(cond.isCurrentIncluded)
                line->setSelected(true);
        }
        if(cond.isNullAvailable)
        {
            line = new QListWidgetItem(vList);
            line->setText(tr("<Пустой>"));
            line->setData(Qt::UserRole, -2);
            if(cond.isNullIncluded)
                line->setSelected(true);
        }
        QString chTable = cond.queryDef->recordDef()->fieldChoiceTable(cond.vid());
        TQChoiceList choices = cond.queryDef->recordDef()->choiceTable(chTable);
        foreach(const TQChoiceItem &item, choices)
        {
            line = new QListWidgetItem(vList);
            line->setText(item.displayText);
            line->setData(Qt::UserRole, item.id);
            if(cond.ids.contains(item.id))
                line->setSelected(true);
        }
    }
    rbActive->setChecked(cond.isActiveIncluded && !cond.isDeletedIncluded);
    rbDeleted->setChecked(!cond.isActiveIncluded && cond.isDeletedIncluded);
    rbAny->setChecked((cond.isActiveIncluded && cond.isDeletedIncluded)
                      || (!cond.isActiveIncluded && !cond.isDeletedIncluded));
}

void TQUserCondDialog::stateChecked(int state)
{
    if(!mutex.tryLock())
        return;
    cond.isActiveIncluded = state >= 0;
    cond.isDeletedIncluded = state <= 0;
    mutex.unlock();
}

void TQUserCondDialog::selectionChanged()
{
    if(!mutex.tryLock())
        return;
    cond.ids.clear();
    foreach(QListWidgetItem *line, vList->selectedItems())
    {
        int id = line->data(Qt::UserRole).toInt();
        if(id == -1)
            cond.isCurrentIncluded = true;
        else if(id == -2)
            cond.isNullIncluded = true;
        else
            cond.ids.append(id);
    }
    mutex.unlock();
}

void TQUserCondDialog::usersChecked()
{
    if(!mutex.tryLock())
        return;
    cond.isGroups = false;
    refresh();
    mutex.unlock();
}

void TQUserCondDialog::groupsChecked()
{
    if(!mutex.tryLock())
        return;
    cond.isGroups = true;
    refresh();
    mutex.unlock();
}


TQStringCondDialog::TQStringCondDialog(QWidget *parent)
    : TQAbstractCondDialog(parent)
{
    QVBoxLayout *vLay = new QVBoxLayout(this);
    QVBoxLayout *lay = new QVBoxLayout();
    vLay->addLayout(lay);
    label = new QLabel();
    lay->addWidget(label);
    edit = new QLineEdit();
    lay->addWidget(edit);
    caseSensitive = new QCheckBox("Case sensitive");
    lay->addWidget(caseSensitive);

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->setOrientation(Qt::Horizontal);
    connect(box,SIGNAL(accepted()),SLOT(accept()));
    connect(box,SIGNAL(rejected()),SLOT(reject()));
    vLay->addWidget(box);

    connect(edit, SIGNAL(editingFinished()), SLOT(editFinished()));
    connect(caseSensitive, SIGNAL(toggled(bool)), SLOT(caseChanged()));
}

void TQStringCondDialog::setCondition(const TQCond &condition)
{
    cond = condition;
    label->setText(cond.fieldName()+":");
    edit->setText(cond.value);
    caseSensitive->setChecked(cond.isCaseSensitive);
}

TQCond &TQStringCondDialog::condition()
{
    return cond;
}

void TQStringCondDialog::editFinished()
{
    cond.value = edit->text();
}

void TQStringCondDialog::caseChanged()
{
    cond.isCaseSensitive = caseSensitive->isChecked();
}

