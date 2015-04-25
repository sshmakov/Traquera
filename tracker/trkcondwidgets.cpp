#include "trkcondwidgets.h"

#include <QtGui>
#include "ui_trkchangedlg.h"
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
            nLay->addWidget(edOnlyTitle);
        }
    }

    QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    bbox->setOrientation(Qt::Vertical);
    connect(bbox,SIGNAL(accepted()),SLOT(accept()));
    connect(bbox,SIGNAL(rejected()),SLOT(reject()));
    hLay->addWidget(bbox);
}

void TrkKeywordCondDialog::setCondition(const TQCond &condition)
{
    QMutexLocker locker(&mutex);
    cond = condition;
    for(int i = 0; i<keyEdits.count(); i++)
    {
        if(i < cond.keys.count())
            keyEdits[i]->setText(cond.keys[i]);
        else
            keyEdits[i]->clear();
    }
    cbCase->setChecked(cond.isKeyCase);
    rbOr->setChecked(!cond.isOr());
    rbAnd->setChecked(cond.isOr());
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(cond.isOr()));
    cbTitle->setChecked(cond.isKeyInRecTitles);
    cbDesc->setChecked(cond.isKeyInDesc);
    cbNoteTitles->setChecked(cond.isKeyInNoteTitles);
    cbNoteText->setChecked(cond.isKeyInNoteText);
    cbNoteOnly->setChecked(cond.isKeyInNoteOnly);
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
    cond.setIsOr(rbOr->isChecked());
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(cond.isOr()));
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
//    cond.noteTitleSearch = edOnlyTitle->currentText();
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
    ui->setupUi(this);
    connect(ui->rbField, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbRecord, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbNote, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbModule, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->rbFile, SIGNAL(toggled(bool)),SLOT(rbObjectToggled(bool)));
    connect(ui->btnAddNoteTitle,SIGNAL(clicked()),SLOT(btnAddNoteClicked()));
    connect(ui->coField,SIGNAL(currentIndexChanged(QString)),SLOT(fieldSelected(QString)));
    connect(ui->rbDays,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(ui->rbDate,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(ui->rbDateTime,SIGNAL(toggled(bool)),SLOT(rbDaysToggled(bool)));
    connect(this,SIGNAL(fieldChanges()), SLOT(updateObjValues()), Qt::QueuedConnection);
    connect(ui->rbBetween,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbNotBetween,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbBefore,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));
    connect(ui->rbAfter,SIGNAL(toggled(bool)),SLOT(rbBetweenToggled(bool)));

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
    QStringList fNames = cond.queryDef->recordDef()->fieldNames();
    fNames.sort();
    ui->coField->addItems(fNames);
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
        QString fname = cond.fieldName();
        int fi = ui->coField->findText(fname);
        ui->coField->setCurrentIndex(fi);

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
        TQChoiceList list = cond.recordType()->choiceTable(tableName);

        QListWidgetItem *item;
        ui->lwOldValues->clear();
        item = new QListWidgetItem(tr("<Пусто>"), ui->lwOldValues);
        item->setData(Qt::UserRole, 0);
        ui->lwNewValues->clear();
        item = new QListWidgetItem(tr("<Пусто>"), ui->lwNewValues);
        item->setData(Qt::UserRole, 0);

        foreach(const TQChoiceItem& ch, list)
        {
            item = new QListWidgetItem(ch.displayText, ui->lwOldValues);
            item->setData(Qt::UserRole, ch.id);
            item->setSelected(cond.oldValues.contains(QString::number(ch.id)));
            item = new QListWidgetItem(ch.displayText, ui->lwNewValues);
            item->setData(Qt::UserRole, ch.id);
            item->setSelected(cond.newValues.contains(QString::number(ch.id)));
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

void TrkChangeCondDialog::fieldSelected(const QString &field)
{
    if(!isInteractive())
        return;
    int vid =  cond.queryDef->recordDef()->fieldVid(field);
    int condVid = cond.vid();
    if(vid != condVid)
    {
        cond.setVid(vid);
        updateFieldValues();
    }
}

void TrkChangeCondDialog::rbDaysToggled(bool value)
{ // hlBetween, hlNotBetween, hlBefore, hlAfter
    if(!isInteractive())
        return;
    if(!value)
        return;
    if(sender() == ui->rbDays)
        setDateMode(TrkChangeCond::Days);
    else if(sender() == ui->rbDate)
        setDateMode(TrkChangeCond::Date);
    else if(sender() == ui->rbDateTime)
        setDateMode(TrkChangeCond::DateTime);
}

void TrkChangeCondDialog::rbBetweenToggled(bool value)
{
    if(!isInteractive())
        return;
    if(!value)
        return;
    if(sender() == ui->rbBetween)
    {
        ui->hlBetween->setEnabled(true);
        ui->hlNotBetween->setEnabled(false);
        ui->hlBefore->setEnabled(false);
        ui->hlAfter->setEnabled(false);
    }
    else if(sender() == ui->rbNotBetween)
    {
        ui->hlBetween->setEnabled(false);
        ui->hlNotBetween->setEnabled(true);
        ui->hlBefore->setEnabled(false);
        ui->hlAfter->setEnabled(false);
    }
    else if(sender() == ui->rbBefore)
    {
        ui->hlBetween->setEnabled(false);
        ui->hlNotBetween->setEnabled(false);
        ui->hlBefore->setEnabled(true);
        ui->hlAfter->setEnabled(false);
    }
    else if(sender() == ui->rbAfter)
    {
        ui->hlBetween->setEnabled(false);
        ui->hlNotBetween->setEnabled(false);
        ui->hlBefore->setEnabled(false);
        ui->hlAfter->setEnabled(true);
    }
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

void TrkChangeCondDialog::setDateMode(TrkChangeCond::DateModeEnum mode)
{
    lockChanges();
    deleteDateEdits(ui->hlBetween);
    deleteDateEdits(ui->hlNotBetween);
    deleteDateEdits(ui->hlBefore);
    deleteDateEdits(ui->hlAfter);
    if(mode == TrkChangeCond::Days)
    {
        QSpinBox *sb;
        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlBetween->insertWidget(0, sb);
        dateEdits.append(sb);
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        sb = new QSpinBox(this);
        sb->setValue(cond.days2);
        ui->hlBetween->insertWidget(2, sb);
        dateEdits.append(sb);
        ui->hlBetween->insertWidget(3, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlNotBetween->insertWidget(0, sb);
        dateEdits.append(sb);
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        sb = new QSpinBox(this);
        sb->setValue(cond.days2);
        ui->hlNotBetween->insertWidget(2, sb);
        dateEdits.append(sb);
        ui->hlNotBetween->insertWidget(3, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlBefore->insertWidget(0, sb);
        dateEdits.append(sb);
        ui->hlBefore->insertWidget(1, new QLabel(tr("дней назад")));

        sb = new QSpinBox(this);
        sb->setValue(cond.days1);
        ui->hlAfter->insertWidget(0, sb);
        dateEdits.append(sb);
        ui->hlAfter->insertWidget(1, new QLabel(tr("дней назад")));
    }
    else if(mode == TrkChangeCond::Date)
    {
        QDateEdit *de;
        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(0, de);
        dateEdits.append(de);
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        de = new QDateEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(2, de);
        dateEdits.append(de);

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(0, de);
        dateEdits.append(de);
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        de = new QDateEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(2, de);
        dateEdits.append(de);

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBefore->insertWidget(0, de);
        dateEdits.append(de);

        de = new QDateEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlAfter->insertWidget(0, de);
        dateEdits.append(de);
    }
    else if(mode == TrkChangeCond::DateTime)
    {
        QDateTimeEdit *de;
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(0, de);
        dateEdits.append(de);
        ui->hlBetween->insertWidget(1, new QLabel(tr("и")));
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlBetween->insertWidget(2, de);
        dateEdits.append(de);

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(0, de);
        dateEdits.append(de);
        ui->hlNotBetween->insertWidget(1, new QLabel(tr("и после")));
        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date2);
        de->setCalendarPopup(true);
        ui->hlNotBetween->insertWidget(2, de);
        dateEdits.append(de);

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlBefore->insertWidget(0, de);
        dateEdits.append(de);

        de = new QDateTimeEdit(this);
        de->setDateTime(cond.date1);
        de->setCalendarPopup(true);
        ui->hlAfter->insertWidget(0, de);
        dateEdits.append(de);
    }
    unlockChanges();
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

