#include "trkcondwidgets.h"

#include <QtGui>
#include "ui_trkchangedlg.h"
#include "ui_trkdatesdlg.h"
#include "trkview.h"

TrkKeywordCondDialog::TrkKeywordCondDialog(QWidget *parent) :
    QDialog(parent)
{
    QMutexLocker locker(&mutex);
    QHBoxLayout *hLay = new QHBoxLayout(this);
    QGroupBox *keyBox = new QGroupBox();
    hLay->addWidget(keyBox);
    {
        QFormLayout *bLay = new QFormLayout(keyBox);
        for(int i=0; i<8; i++)
        {
            QLabel *l = new QLabel(andOr(cond.isOr()));
            QLineEdit *e = new QLineEdit();
            bLay->addRow(l,e);
            keyLabels.append(l);
            keyEdits.append(e);
            connect(e, SIGNAL(editingFinished()),SLOT(keyEditFinished()));
        }
        cbCase = new QCheckBox(tr("Case sensitive"));
        bLay->addWidget(cbCase);
        connect(cbCase, SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
    }
    QVBoxLayout *mLay = new QVBoxLayout();
    hLay->addLayout(mLay);
    {
        QGroupBox *aoBox = new QGroupBox();
        mLay->addWidget(aoBox);
        {
            QHBoxLayout *aoLay = new QHBoxLayout(aoBox);
            rbOr = new QRadioButton(andOr(true));
            aoLay->addWidget(rbOr);
            rbAnd = new QRadioButton(andOr(false));
            aoLay->addWidget(rbAnd);
            connect(rbOr, SIGNAL(clicked()), SLOT(rbAndOrChecked()));
            connect(rbAnd, SIGNAL(clicked()), SLOT(rbAndOrChecked()));
        }

        QGroupBox *nBox = new QGroupBox();
        mLay->addWidget(nBox);
        {
            QVBoxLayout *nLay = new QVBoxLayout(nBox);
            cbTitle = new QCheckBox(tr("Title"));
            cbTitle->setChecked(cond.isKeyInRecTitles);
            connect(cbTitle,SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
            nLay->addWidget(cbTitle);

            cbDesc = new QCheckBox(tr("Description"));
            cbDesc->setChecked(cond.isKeyInDesc);
            connect(cbDesc,SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
            nLay->addWidget(cbDesc);

            cbNoteTitles = new QCheckBox(tr("Note Titles"));
            cbNoteTitles->setChecked(cond.isKeyInNoteTitles);
            connect(cbNoteTitles,SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
            nLay->addWidget(cbNoteTitles);

            cbNoteText = new QCheckBox(tr("Note Text"));
            cbNoteText->setChecked(cond.isKeyInNoteText);
            connect(cbNoteText,SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
            nLay->addWidget(cbNoteText);

            cbNoteOnly = new QCheckBox(tr("Note with Title only:"));
            cbNoteOnly->setChecked(cond.isKeyInNoteOnly);
            connect(cbNoteOnly,SIGNAL(toggled(bool)), SLOT(cbChecked(bool)));
            nLay->addWidget(cbNoteOnly);

            edOnlyTitle = new QComboBox();
            connect(edOnlyTitle, SIGNAL(editTextChanged(QString)), SLOT(onlyTitleEdited()));
            edOnlyTitle->setEditable(true);
            nLay->addWidget(edOnlyTitle);
        }
    }

    QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    bbox->setOrientation(Qt::Vertical);
    connect(bbox,SIGNAL(accepted()),SLOT(accept()));
    connect(bbox,SIGNAL(rejected()),SLOT(reject()));
    hLay->addWidget(bbox);
    resetControls();
}

void TrkKeywordCondDialog::setCondition(const TQCond &condition)
{
    QMutexLocker locker(&mutex);
    cond = condition;
    resetControls();
}

void TrkKeywordCondDialog::resetControls()
{
    for(int i = 0; i<keyEdits.count(); i++)
    {
        if(i < cond.keys.count())
            keyEdits[i]->setText(cond.keys[i]);
        else
            keyEdits[i]->clear();
    }
    cbCase->setChecked(cond.isKeyCase);
    rbOr->setChecked(!cond.isKeyAnd);
    rbAnd->setChecked(cond.isKeyAnd);
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(!cond.isKeyAnd));
    cbTitle->setChecked(cond.isKeyInRecTitles);
    cbDesc->setChecked(cond.isKeyInDesc);
    cbNoteTitles->setChecked(cond.isKeyInNoteTitles);
    cbNoteText->setChecked(cond.isKeyInNoteText);
    cbNoteOnly->setChecked(cond.isKeyInNoteOnly);
    edOnlyTitle->clear();
    TQAbstractRecordTypeDef *rdef = cond.recordType();
    if(rdef)
        edOnlyTitle->addItems(rdef->noteTitleList());
    edOnlyTitle->setEditText(cond.noteTitleSearch);
}

TQCond &TrkKeywordCondDialog::condition()
{
    return cond;
}


QString TrkKeywordCondDialog::andOr(bool value)
{
    return value ?
                tr("ИЛИ") :
                tr("И");
}

void TrkKeywordCondDialog::keyEditFinished()
{
    if(!mutex.tryLock())
        return;
    cond.keys.clear();
    foreach(QLineEdit *ed, keyEdits)
    {
        QString key = ed->text().trimmed();
        if(!key.isEmpty())
            cond.keys.append(key);
    }
    mutex.unlock();
}

void TrkKeywordCondDialog::rbAndOrChecked()
{
    if(!mutex.tryLock())
        return;
    cond.isKeyAnd = !rbOr->isChecked();
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(!cond.isKeyAnd));
    mutex.unlock();
}

void TrkKeywordCondDialog::cbChecked(bool state)
{
    if(!mutex.tryLock())
        return;
    QCheckBox *cb = qobject_cast<QCheckBox *>(sender());
    if(cb == cbTitle)
        cond.isKeyInRecTitles = state;
    else if(cb == cbDesc)
        cond.isKeyInDesc = state;
    else if(cb == cbNoteTitles)
    {
        cond.isKeyInNoteTitles = state;
        cond.isKeyInNoteOnly = false;
        cbNoteOnly->setChecked(false);
    }
    else if(cb == cbNoteText)
    {
        cond.isKeyInNoteText = state;
        cond.isKeyInNoteOnly = false;
        cbNoteOnly->setChecked(false);
    }
    else if(cb == cbNoteOnly)
    {
        cond.isKeyInNoteOnly = state;
        cond.isKeyInNoteTitles = false;
        cond.isKeyInNoteText = false;
        cbNoteTitles->setChecked(false);
        cbNoteText->setChecked(false);
    }
    else if(cb == cbCase)
    {
        cond.isKeyCase = state;
    }
    edOnlyTitle->setEnabled(cond.isKeyInNoteOnly);
    mutex.unlock();
}

void TrkKeywordCondDialog::onlyTitleEdited()
{
    if(!mutex.tryLock())
        return;
    cond.noteTitleSearch = edOnlyTitle->currentText();
    mutex.unlock();
}

// ====================================================================================================
TrkChangeCondDialog::TrkChangeCondDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::TrkChangeCondDialog),
      locks(0)
{
//    isInteractive = false;
    lockChanges();
    dummyAnyField = tr("-- Любое поле --");
    ui->setupUi(this);
    connect(ui->rbField, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbRecord, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbNote, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbModule, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbFile, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->btnAddNoteTitle,SIGNAL(clicked()),SLOT(btnAddNoteClicked()));
    connect(ui->coField,SIGNAL(currentIndexChanged(int)),SLOT(fieldIndexSelected(int)));
    connect(ui->rbDays,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(ui->rbDate,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(ui->rbDateTime,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(this,SIGNAL(fieldChanges()), SLOT(updateObjValues()), Qt::QueuedConnection);
    connect(ui->rbBetween,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbNotBetween,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbBefore,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbAfter,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->grDateTime,SIGNAL(toggled(bool)),SLOT(grDateTimeToggled(bool)));
    connect(ui->lwOldValues,SIGNAL(itemSelectionChanged()),SLOT(valuesSelected()));
    connect(ui->lwNewValues,SIGNAL(itemSelectionChanged()),SLOT(valuesSelected()));
    connect(ui->grAuthor,SIGNAL(toggled(bool)),SLOT(grAuthorToggled(bool)));
    connect(ui->coAuthor,SIGNAL(currentIndexChanged(int)),SLOT(coAuthorChanged(int)));

    connect(ui->lwNoteTitles,SIGNAL(itemSelectionChanged()),SLOT(noteTitlesSelected()));

    noteChangeTypes = tr("Любое изменение,0;Нота добавлена,1;Нота изменена,2;Нота удалена,3");
    fileChangeTypes = tr("Любое изменение,0;Файл добавлен,1;Файл удален,3");
    moduleChangeTypes = tr("Любое изменение,0;Модуль добавлен,1;Модуль изменен,2;Модуль удален,3;"
                           "Модуль check-in,4;Модуль check-out,5;Модулю присвоена метка версии,6");;
    // 0-any, 1-added, 2-modified,  3-removed, 4-checked in, 5-checked out, 6-assigned version label
//    isInteractive = true;
    unlockChanges();
}

void TrkChangeCondDialog::setCondition(const TQCond &condition)
{
//    isInteractive = false;
    lockChanges();
    cond = condition;
    updateUI();
//    isInteractive = true;
    unlockChanges();
}

void TrkChangeCondDialog::updateUI()
{
    lockChanges();
    ui->coChangeType->clear();
    ui->lwOldValues->clear();
    ui->lwNewValues->clear();
    ui->coAuthor->clear();
    ui->coField->clear();
    ui->coField->addItem(dummyAnyField, QVariant((int)0));
    TQAbstractRecordTypeDef *rDef = cond.queryDef->recordDef();
    if(rDef)
    {
        QStringList fNames = rDef->fieldNames();
        fNames.sort();
        foreach(const QString &f, fNames)
        {
            int vid = rDef->fieldVid(f);
            ui->coField->addItem(f, vid);
        }
    }
    updateObjValues();

    int i = 0, found = -1;
    foreach(const TQUser &user, cond.queryDef->project()->userList())
    {
        ui->coAuthor->addItem(user.displayName, user.id);
        if(cond.authorId == user.id)
            found = i;
        i++;
    }
    if(found != -1)
        ui->coAuthor->setCurrentIndex(found);
    ui->grAuthor->setChecked(cond.authorId != 0);
    setDateMode(cond.dateMode);
    setBetweenMode(cond.changeDate);
    resize(10,10);
    unlockChanges();
}

void TrkChangeCondDialog::updateObjValues()
{
    lockChanges();
    switch(cond.changeObject)
    {
    case TrkChangeCond::FieldChange:
    {
        if(cond.vid())
        {
            QString fname = cond.fieldName();
            int fi = ui->coField->findText(fname);
            ui->coField->setCurrentIndex(fi);
        }
        else
            ui->coField->setCurrentIndex(0);
        ui->rbField->setChecked(true);
        ui->frChangeType->setVisible(false);
        ui->frField->setVisible(true);
        ui->grName->setVisible(false);
        ui->grNoteTitles->setVisible(false);
        updateFieldValues();
        break;
    }
    case TrkChangeCond::RecordChange:
        ui->rbRecord->setChecked(true);
        ui->frChangeType->setVisible(false);
        ui->frField->setVisible(false);
        ui->grValues->setVisible(false);
        ui->grName->setVisible(false);
        ui->grNoteTitles->setVisible(false);
        break;
    case TrkChangeCond::NoteChange:
    {
        ui->rbNote->setChecked(true);
        ui->coChangeType->clear();
        foreach(QString line, noteChangeTypes.split(";"))
        {
            QStringList list= line.split(",");
            ui->coChangeType->addItem(list[0], list[1].toInt());
        }
        ui->lwNoteTitles->clear();
        QStringList notes = cond.queryDef->recordDef()->noteTitleList();
        notes.sort();
        ui->lwNoteTitles->addItems(notes);
        ui->grNoteTitles->setChecked(cond.noteTitles.size() != 0);
        foreach(const QString &title, cond.noteTitles)
        {
            QList<QListWidgetItem *>items = ui->lwNoteTitles->findItems(title,Qt::MatchFixedString);
            if(items.size())
                foreach(QListWidgetItem *item, items)
                    item->setSelected(true);
            else
            {
                QListWidgetItem * item = new QListWidgetItem(title, ui->lwNoteTitles);
                item->setSelected(true);
            }
        }
        ui->frChangeType->setVisible(true);
        ui->frField->setVisible(false);
        ui->grValues->setVisible(false);
        ui->grName->setVisible(false);
        ui->grNoteTitles->setVisible(true);
        break;
    }
    case TrkChangeCond::ModuleChange:
        ui->rbModule->setChecked(true);
        ui->coChangeType->clear();
        foreach(QString line, moduleChangeTypes.split(";"))
        {
            QStringList list= line.split(",");
            ui->coChangeType->addItem(list[0], list[1].toInt());
        }
        ui->leName->setText(cond.fileName);
        ui->grName->setChecked(!cond.fileName.isEmpty());
        ui->frChangeType->setVisible(true);
        ui->frField->setVisible(false);
        ui->grValues->setVisible(false);
        ui->grName->setVisible(true);
        ui->grNoteTitles->setVisible(false);
        break;
    case TrkChangeCond::FileChange:
        ui->rbFile->setChecked(true);
        ui->coChangeType->clear();
        foreach(QString line, fileChangeTypes.split(";"))
        {
            QStringList list= line.split(",");
            ui->coChangeType->addItem(list[0], list[1].toInt());
        }
        ui->leName->setText(cond.fileName);
        ui->grName->setChecked(!cond.fileName.isEmpty());
        ui->frChangeType->setVisible(true);
        ui->frField->setVisible(false);
        ui->grValues->setVisible(false);
        ui->grName->setVisible(true);
        ui->grNoteTitles->setVisible(false);
        break;
    }
    unlockChanges();
}

void TrkChangeCondDialog::updateFieldValues()
{
    lockChanges();
    QString tableName = cond.recordType()->fieldChoiceTable(cond.vid());
    if(!tableName.isEmpty())
    {
        TQChoiceList chTable = cond.choiceTable();

        QListWidgetItem *item;
        ui->lwOldValues->clear();
        item = new QListWidgetItem(tr("<Пусто>"), ui->lwOldValues);
        item->setData(Qt::UserRole, 0);
        item->setSelected(cond.choiceIds1.contains(0));
        ui->lwNewValues->clear();
        item = new QListWidgetItem(tr("<Пусто>"), ui->lwNewValues);
        item->setData(Qt::UserRole, 0);
        item->setSelected(cond.choiceIds2.contains(0));

        foreach(const TQChoiceItem& ch, chTable)
        {
            item = new QListWidgetItem(ch.displayText, ui->lwOldValues);
            item->setData(Qt::UserRole, ch.id);
            item->setSelected(cond.choiceIds1.contains(ch.id));
            item = new QListWidgetItem(ch.displayText, ui->lwNewValues);
            item->setData(Qt::UserRole, ch.id);
            item->setSelected(cond.choiceIds2.contains(ch.id));
        }
        ui->grValues->setVisible(true);
    }
    else
        ui->grValues->setVisible(false);
    unlockChanges();
}

void TrkChangeCondDialog::rbObjectToggled(bool value)
{
    if(!isInteractive())
        return;
    if(!value)
        return;
    if(sender() == ui->rbField)
        cond.changeObject = TrkChangeCond::FieldChange;
    else if(sender() == ui->rbRecord)
        cond.changeObject = TrkChangeCond::RecordChange;
    else if(sender() == ui->rbNote)
        cond.changeObject = TrkChangeCond::NoteChange;
    else if(sender() == ui->rbFile)
        cond.changeObject = TrkChangeCond::FileChange;
    else if(sender() == ui->rbModule)
        cond.changeObject = TrkChangeCond::ModuleChange;
    updateObjValues();
}

void TrkChangeCondDialog::btnAddNoteClicked()
{
    if(!isInteractive())
        return;
    QString note = QInputDialog::getText(this,
                                         tr("Введите свой заголовок"),
                                         tr("Заголовок"));
    if(!note.isEmpty())
    {
        QListWidgetItem *item = new QListWidgetItem(note,ui->lwNoteTitles);
        item->setSelected(true);
        ui->lwNoteTitles->setCurrentItem(item);
    }
}

void TrkChangeCondDialog::fieldIndexSelected(int index)
{
    if(!isInteractive())
        return;
    int vid = ui->coField->itemData(index).toInt();
    int condVid = cond.vid();
    if(vid != condVid)
    {
        cond.setVid(vid);
        updateFieldValues();
    }
}

void TrkChangeCondDialog::grDateTimeToggled(bool value)
{
    if(!isInteractive())
        return;
    if(value)
    {
        cond.dateMode = TrkChangeCond::Days;
        cond.changeDate = TrkChangeCond::AfterDate;
    }
    else
    {
        cond.dateMode = TrkChangeCond::Days;
        cond.changeDate = TrkChangeCond::AnyDate;
    }
    setDateMode(cond.dateMode);
    setBetweenMode(cond.changeDate);
}

void TrkChangeCondDialog::rbDaysToggled(bool value)
{ // hlBetween, hlNotBetween, hlBefore, hlAfter
    if(!isInteractive())
        return;
    if(!value)
        return;
    if(sender() == ui->rbDays)
        cond.dateMode = TrkChangeCond::Days;
    else if(sender() == ui->rbDate)
        cond.dateMode = TrkChangeCond::Date;
    else if(sender() == ui->rbDateTime)
        cond.dateMode = TrkChangeCond::DateTime;
    setDateMode(cond.dateMode);
    setBetweenMode(cond.changeDate);
}

void TrkChangeCondDialog::rbBetweenToggled(bool value)
{
    if(!isInteractive())
        return;
    if(!value)
    {
        QRadioButton *rb = qobject_cast<QRadioButton *>(sender());
        if(rb && !ui->rbBetween->isChecked()
                && !ui->rbNotBetween->isChecked()
                && !ui->rbBefore->isChecked()
                && !ui->rbAfter->isChecked())
            rb->setChecked(true);
        return;
    }
    if(sender() == ui->rbBetween)
        cond.changeDate = TrkChangeCond::BetweenDates;
    else if(sender() == ui->rbNotBetween)
        cond.changeDate = TrkChangeCond::NotBetweenDates;
    else if(sender() == ui->rbBefore)
        cond.changeDate = TrkChangeCond::BeforeDate;
    else if(sender() == ui->rbAfter)
        cond.changeDate = TrkChangeCond::AfterDate;
    setBetweenMode(cond.changeDate);
}

void TrkChangeCondDialog::sbDays1Changed(int days)
{
    if(!isInteractive())
        return;
    cond.days1 = days;
}

void TrkChangeCondDialog::sbDays2Changed(int days)
{
    if(!isInteractive())
        return;
    cond.days2 = days;
}

void TrkChangeCondDialog::deDate1Changed(QDateTime date)
{
    if(!isInteractive())
        return;
    cond.date1 = date;
}

void TrkChangeCondDialog::deDate2Changed(QDateTime date)
{
    if(!isInteractive())
        return;
    cond.date2 = date;
}

void TrkChangeCondDialog::valuesSelected()
{
    if(!isInteractive())
        return;
    QList<int> list;
    if(sender() == ui->lwOldValues)
    {
        foreach(QListWidgetItem *item, ui->lwOldValues->selectedItems())
        {
            int id = item->data(Qt::UserRole).toInt();
            list.append(id);
        }
        cond.choiceIds1 = list;
    }
    else
    {
        foreach(QListWidgetItem *item, ui->lwNewValues->selectedItems())
        {
            int id = item->data(Qt::UserRole).toInt();
            list.append(id);
        }
        cond.choiceIds2 = list;
    }
}

void TrkChangeCondDialog::grAuthorToggled(bool value)
{
    if(!isInteractive())
        return;
    if(value)
    {
        int id;
        id = ui->coAuthor->itemData(ui->coAuthor->currentIndex()).toInt();
        cond.authorId = id;
    }
    else
        cond.authorId = 0;
}

void TrkChangeCondDialog::coAuthorChanged(int index)
{
    if(!isInteractive())
        return;
    if(ui->grAuthor->isChecked())
    {
        int id = ui->coAuthor->itemData(index).toInt();
        cond.authorId = id;
    }
}

void TrkChangeCondDialog::noteTitlesSelected()
{
    if(!isInteractive())
        return;
    QStringList list;
    foreach(QListWidgetItem *item, ui->lwNoteTitles->selectedItems())
    {
        QString title = item->text();
        list.append(title);
    }
    cond.noteTitles = list;
}

void TrkChangeCondDialog::deleteDateEdits(QLayout *lay)
{
    QList<QWidget *>list;
    for(int i=0; i<lay->count(); i++)
    {
        QLayoutItem *item = lay->itemAt(i);
        if(item)
        {
            QWidget *w = item->widget();
            if(w)
                list.append(w);
        }
    }
    foreach(QWidget *w, list)
    {
        if(dateEdits.contains(w))
            dateEdits.removeAll(w);
        delete w;
    }
}

void TrkChangeCondDialog::setEnableDateEdits(QLayout *lay, bool value)
{
    for(int i=0; i<lay->count(); i++)
    {
        QLayoutItem *item = lay->itemAt(i);
        if(item)
        {
            QWidget *w = item->widget();
            if(w)
                w->setEnabled(value);
        }
    }
}

void TrkChangeCondDialog::setDateMode(TrkChangeCond::DateModeEnum mode)
{
    lockChanges();
    deleteDateEdits(ui->hlBetween);
    deleteDateEdits(ui->hlNotBetween);
    deleteDateEdits(ui->hlBefore);
    deleteDateEdits(ui->hlAfter);
    if(mode == TrkChangeCond::Days)
    {
        ui->rbDays->setChecked(true);
        QSpinBox *sb;
        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlBetween->insertWidget(0, sb);
        dateEdits.append(sb);
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays1Changed(int)));
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        sb = new QSpinBox(this);
        sb->setValue(cond.days2);
        ui->hlBetween->insertWidget(2, sb);
        dateEdits.append(sb);
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays2Changed(int)));
        ui->hlBetween->insertWidget(3, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlNotBetween->insertWidget(0, sb);
        dateEdits.append(sb);
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays1Changed(int)));
        sb = new QSpinBox(this);
        sb->setValue(cond.days2);
        ui->hlNotBetween->insertWidget(2, sb);
        dateEdits.append(sb);
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays2Changed(int)));
        ui->hlNotBetween->insertWidget(3, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlBefore->insertWidget(0, sb);
        dateEdits.append(sb);
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays1Changed(int)));
        ui->hlBefore->insertWidget(1, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlAfter->insertWidget(0, sb);
        dateEdits.append(sb);
        connect(sb,SIGNAL(valueChanged(int)),SLOT(sbDays1Changed(int)));
        ui->hlAfter->insertWidget(1, new QLabel(tr("дней назад")));
    }
    else if(mode == TrkChangeCond::Date)
    {
        ui->rbDate->setChecked(true);
        QDateEdit *de;
        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        de = new QDateEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(2, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate2Changed(QDateTime)));

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        de = new QDateEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(2, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate2Changed(QDateTime)));

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBefore->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlAfter->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
    }
    else if(mode == TrkChangeCond::DateTime)
    {
        ui->rbDateTime->setChecked(true);
        QDateTimeEdit *de;
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(2, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate2Changed(QDateTime)));

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(2, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate2Changed(QDateTime)));

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBefore->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlAfter->insertWidget(0, de);
        dateEdits.append(de);
        connect(de,SIGNAL(dateTimeChanged(QDateTime)),SLOT(deDate1Changed(QDateTime)));
    }
    unlockChanges();
}

void TrkChangeCondDialog::setBetweenMode(TrkChangeCond::ChangeDateEnum mode)
{
    ui->grDateTime->setEnabled(true);
//    ui->grDateTime->setCheckable(true);
    if(mode == TrkChangeCond::AnyDate)
    {
        ui->grDateTime->setChecked(false);
    }
    else
    {
        ui->grDateTime->setChecked(true);
        if(mode == TrkChangeCond::BetweenDates)
        {
            setEnableDateEdits(ui->hlBetween, true);
            setEnableDateEdits(ui->hlNotBetween, false);
            setEnableDateEdits(ui->hlBefore, false);
            setEnableDateEdits(ui->hlAfter, false);
            ui->rbBetween->setChecked(true);
        }
        else if(mode == TrkChangeCond::NotBetweenDates)
        {
            setEnableDateEdits(ui->hlBetween, false);
            setEnableDateEdits(ui->hlNotBetween, true);
            setEnableDateEdits(ui->hlBefore, false);
            setEnableDateEdits(ui->hlAfter, false);
            ui->rbNotBetween->setChecked(true);
        }
        else if(mode == TrkChangeCond::BeforeDate)
        {
            setEnableDateEdits(ui->hlBetween, false);
            setEnableDateEdits(ui->hlNotBetween, false);
            setEnableDateEdits(ui->hlBefore, true);
            setEnableDateEdits(ui->hlAfter, false);
            ui->rbBefore->setChecked(true);
        }
        else if(mode == TrkChangeCond::AfterDate)
        {
            setEnableDateEdits(ui->hlBetween, false);
            setEnableDateEdits(ui->hlNotBetween, false);
            setEnableDateEdits(ui->hlBefore, false);
            setEnableDateEdits(ui->hlAfter, true);
            ui->rbAfter->setChecked(true);
        }
    }
}

bool TrkChangeCondDialog::isInteractive()
{
    return locks<=0;
}

void TrkChangeCondDialog::lockChanges()
{
    locks++;
}

void TrkChangeCondDialog::unlockChanges()
{
    locks--;
}

TQCond &TrkChangeCondDialog::condition()
{
    return cond;
}

QSize TrkChangeCondDialog::minimumSizeHint() const
{
    QSize sz = QWidget::minimumSizeHint();
    int h = 0;
    h += ui->grAuthor->isVisible() ? ui->grAuthor->height() : 0;
    h += ui->grValues->isVisible() ? ui->grValues->height() : 0;
    h += ui->grName->isVisible() ? ui->grName->height() : 0;
    h += ui->grNoteTitles->isVisible() ? ui->grNoteTitles->height() : 0;
    h += ui->frChangeType->isVisible() ? ui->frChangeType->height() : 0;
    h += ui->frField->isVisible() ? ui->frField->height() : 0;
    h += ui->grDateTime->isVisible() ? ui->grDateTime->height() : 0;
    return QSize(sz.width(), h);
}

//===================================================================================
TrkDateCondDialog::TrkDateCondDialog(QWidget *parent)
    : QDialog(parent), locks(0),
      ui(new Ui::TrkDateCondDialog)
{
    ui->setupUi(this);
    connect(ui->cbEqual,SIGNAL(toggled(bool)),SLOT(sectionToggled(bool)));
    connect(ui->cbAfter,SIGNAL(toggled(bool)),SLOT(sectionToggled(bool)));
    connect(ui->cbBefore,SIGNAL(toggled(bool)),SLOT(sectionToggled(bool)));
    // section Equal
    connect(ui->rbEqualNull,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->rbEqualCurDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->rbEqualDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->dteEqual,SIGNAL(dateTimeChanged(QDateTime)),SLOT(dteChanged(QDateTime)));
    // section After
    connect(ui->rbAfterCurDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->rbAfterDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->sbAfterDays,SIGNAL(valueChanged(int)),SLOT(sbDaysChanged(int)));
    connect(ui->dteAfter,SIGNAL(dateTimeChanged(QDateTime)),SLOT(dteChanged(QDateTime)));
    // section Before
    connect(ui->rbBeforeCurDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->rbBeforeDate,SIGNAL(toggled(bool)),SLOT(rbToggled(bool)));
    connect(ui->sbBeforeDays,SIGNAL(valueChanged(int)),SLOT(sbDaysChanged(int)));
    connect(ui->dteBefore,SIGNAL(dateTimeChanged(QDateTime)),SLOT(dteChanged(QDateTime)));
}

void TrkDateCondDialog::setCondition(const TQCond &condition)
{
    lockChanges();
    cond = condition;
    ui->cbEqual->setChecked(cond.op == TQDateCond::Equals);
    ui->cbAfter->setChecked(cond.op == TQDateCond::GreaterThan
                            || cond.op == TQDateCond::Between);
    ui->cbBefore->setChecked(cond.op == TQDateCond::LessThan
                            || cond.op == TQDateCond::Between);
    ui->frEqual->setEnabled(ui->cbEqual->isChecked());
    ui->frAfter->setEnabled(ui->cbAfter->isChecked());
    ui->frBefore->setEnabled(ui->cbBefore->isChecked());
    if(cond.isDaysValue)
    {
        ui->rbEqualCurDate->setChecked(true);
        ui->rbBeforeCurDate->setChecked(true);
        ui->rbAfterCurDate->setChecked(true);
        ui->sbAfterDays->setValue(cond.days1);
        ui->sbBeforeDays->setValue(cond.days2);
    }
    else
    {
        ui->dteEqual->setDateTime(cond.value1);
        ui->dteAfter->setDateTime(cond.value1);
        ui->dteBefore->setDateTime(cond.value2);
        ui->rbEqualNull->setChecked(!cond.isCurrentDate1 && cond.value1.isNull());
        ui->rbEqualCurDate->setChecked(cond.isCurrentDate1);
        ui->rbEqualDate->setChecked(!cond.isCurrentDate1 && !cond.value1.isNull());
        ui->rbAfterCurDate->setChecked(cond.isCurrentDate1);
        ui->rbAfterDate->setChecked(!cond.isCurrentDate1);
        ui->rbBeforeCurDate->setChecked(cond.isCurrentDate2);
        ui->rbBeforeDate->setChecked(!cond.isCurrentDate2);
    }
    unlockChanges();
}

TQCond &TrkDateCondDialog::condition()
{
    return cond;
}

void TrkDateCondDialog::sectionToggled(bool value)
{
    if(!isInteractive())
        return;
    lockChanges();
    if(sender() == ui->cbEqual)
    {
        setDaysMode(false);
        ui->frEqual->setEnabled(value);
        if(value)
        {
            ui->cbAfter->setChecked(false);
            ui->cbBefore->setChecked(false);
        }
        else if(!ui->cbAfter->isChecked() && ui->cbBefore->isChecked())
            ui->cbAfter->setChecked(true);
    }
    else if(sender() == ui->cbAfter)
    {
        ui->frAfter->setEnabled(value);
        if(value)
            ui->cbEqual->setChecked(false);
        if(!value && !ui->cbBefore->isChecked())
            ui->cbBefore->setChecked(true);
    }
    else if (sender() == ui->cbBefore)
    {
        ui->frBefore->setEnabled(value);
        if(value)
            ui->cbEqual->setChecked(false);
        if(!value && !ui->cbAfter->isChecked())
            ui->cbAfter->setChecked(true);
    }
    ui->frEqual->setEnabled(ui->cbEqual->isChecked());
    ui->frAfter->setEnabled(ui->cbAfter->isChecked());
    ui->frBefore->setEnabled(ui->cbBefore->isChecked());
    if(ui->cbEqual->isChecked())
        cond.op = TQDateCond::Equals;
    else if(ui->cbAfter->isChecked() && ui->cbBefore->isChecked())
        cond.op = TQDateCond::Between;
    else if(ui->cbAfter->isChecked())
        cond.op = TQDateCond::GreaterThan;
    else if(ui->cbBefore->isChecked())
        cond.op = TQDateCond::LessThan;
    unlockChanges();
}

void TrkDateCondDialog::rbToggled(bool value)
{
    if(!isInteractive())
        return;
    if(!value)
        return;
    lockChanges();
    if(sender() == ui->rbEqualNull)
    {
        setDaysMode(false);
        cond.value1 = QDateTime();
        cond.isCurrentDate1 = false;
    }
    else if(sender() == ui->rbEqualCurDate)
    {
        setDaysMode(false);
        cond.days1 = 0;
//        cond.value1 = QDateTime::currentDateTime();
        cond.isCurrentDate1 = true;
    }
    else if(sender() == ui->rbEqualDate)
    {
        setDaysMode(false);
        cond.isCurrentDate1 = false;
        if(cond.value1.isNull())
            cond.value1 = QDateTime::currentDateTime();
        ui->dteEqual->setDateTime(cond.value1);
    }
    else if(sender() == ui->rbAfterCurDate)
    {
        cond.days1 = 0;
//        cond.value1 = QDateTime::currentDateTime();
        cond.isCurrentDate1 = true;
        ui->sbAfterDays->setValue(0);
    }
    else if(sender() == ui->rbAfterDate)
    {
        setDaysMode(false);
        cond.isCurrentDate1 = false;
        if(cond.value1.isNull())
            cond.value1 = QDateTime::currentDateTime();
        ui->dteAfter->setDateTime(cond.value1);
    }
    else if(sender() == ui->rbBeforeCurDate)
    {
        cond.days2 = 0;
//        cond.value2 = QDateTime::currentDateTime();
        cond.isCurrentDate2 = true;
        ui->sbBeforeDays->setValue(0);
    }
    else if(sender() == ui->rbBeforeDate)
    {
        setDaysMode(false);
        cond.isCurrentDate2 = false;
        if(cond.value2.isNull())
            cond.value2 = QDateTime::currentDateTime();
        ui->dteBefore->setDateTime(cond.value2);
    }
    unlockChanges();
}

void TrkDateCondDialog::sbDaysChanged(int value)
{
    if(!isInteractive())
        return;
    lockChanges();
    if(sender() == ui->sbAfterDays)
    {
        cond.days1 = value;
        ui->rbAfterCurDate->setChecked(true);
    }
    else if(sender() == ui->sbBeforeDays)
    {
        cond.days2 = value;
        ui->rbBeforeCurDate->setChecked(true);
    }
    if(value)
        setDaysMode(true);
    unlockChanges();
}

void TrkDateCondDialog::dteChanged(const QDateTime &value)
{
    if(!isInteractive())
        return;
    lockChanges();
    setDaysMode(false);
    if(sender() == ui->dteEqual)
    {
        cond.value1 = value;
        cond.isCurrentDate1 = false;
        ui->rbEqualDate->setChecked(true);
    }
    else if(sender() == ui->dteAfter)
    {
        cond.value1 = value;
        cond.isCurrentDate1 = false;
        ui->rbAfterDate->setChecked(true);
    }
    else if(sender() == ui->dteBefore)
    {
        cond.value2 = value;
        cond.isCurrentDate1 = false;
        ui->rbBeforeDate->setChecked(true);
    }
    unlockChanges();
}

bool TrkDateCondDialog::isInteractive()
{
    return locks<=0;
}

void TrkDateCondDialog::lockChanges()
{
    locks++;
}

void TrkDateCondDialog::unlockChanges()
{
    locks--;
}

void TrkDateCondDialog::setDaysMode(bool value)
{
    if(value)
    {
        cond.isDaysValue = true;
        cond.isCurrentDate1 = false;
        cond.isCurrentDate2 = false;
        if(ui->rbEqualDate->isChecked())
            ui->rbEqualCurDate->setChecked(true);
        ui->rbAfterCurDate->setChecked(true);
        ui->rbBeforeCurDate->setChecked(true);
    }
    else
    {
        cond.isDaysValue = false;
        cond.days1 = 0;
        cond.days2 = 0;
        ui->sbAfterDays->setValue(cond.days1);
        ui->sbBeforeDays->setValue(cond.days2);
    }
}

