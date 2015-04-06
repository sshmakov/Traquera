#include "trkcondwidgets.h"

#include <QtGui>

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
            QLabel *l = new QLabel(andOr(cond.isAnd));
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
            rbOr = new QRadioButton(andOr(false));
            aoLay->addWidget(rbOr);
            rbAnd = new QRadioButton(andOr(true));
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
    rbOr->setChecked(!cond.isAnd);
    rbAnd->setChecked(cond.isAnd);
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(cond.isAnd));
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
                tr("And") :
                tr("Or");
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
    cond.isAnd = rbAnd->isChecked();
    foreach(QLabel *l, keyLabels)
        l->setText(andOr(cond.isAnd));
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
        cond.isKeyInNoteText= false;
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
