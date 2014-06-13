#include "scrwidg.h"
#include "trkview.h"
#include "trkdecorator.h"
#include <QDomDocument>
#include <QFile>
#include <QLineEdit>
#include <QFormLayout>
#include <QXmlSimpleReader>
#include <QScrollArea>

ScrWidget::ScrWidget(const AbstractRecordTypeDef *typeDef, QWidget *parent)
:QMainWindow(parent), record(NULL)
{
	setupUi(this);
    fieldTabs->setCurrentIndex(0);
    tabPanels->setCurrentIndex(0);
    //editRecordButton->setDefaultAction(actionEdit_Record);
    btnCommitNote->setDefaultAction(actionCommit_Note_Changes);
    btnRevertNote->setDefaultAction(actionRevert_Note_Changes);
    initEdits(typeDef);
}

ScrWidget::~ScrWidget()
{
    if(record)
    {
        if(record->mode() != TrkToolRecord::View)
            record->cancel();
        fieldEdits.connectToRecord(0);
        record->removeLink();
    }
    record = NULL;
}

void ScrWidget::setRecord(TrkToolRecord *newrec)
{
	if(record)
	{
        record->removeLink();
		record = NULL;
	}
	record = newrec;
    record->addLink();
    fieldEdits.connectToRecord(record);
	connect(record,SIGNAL(changedState(TrkToolRecord::RecMode)),
		this,SLOT(updateEditState()));
    readRecord();
    updateEditState();
}

void ScrWidget::readRecord()
{
	descEdit->setPlainText(record->description());
    notesTable->clearContents();
    notes = record->notes();
    int row=0;
    foreach(const TrkNote &note, notes)
    {
        notesTable->insertRow(row);
        QTableWidgetItem *item;
        item = new QTableWidgetItem(note.crdate.toString(Qt::SystemLocaleShortDate));
        notesTable->setItem(row, 0, item);
        item = new QTableWidgetItem(note.author);
        notesTable->setItem(row, 1, item);
        item = new QTableWidgetItem(note.title);
        notesTable->setItem(row, 2, item);
        row++;
    }
    /*
	QList<EditDef>::const_iterator ei;
    for(ei=fieldEdits.edits.constBegin(); ei!=fieldEdits.edits.constEnd(); ei++)
	{
		QLineEdit *ed = qobject_cast<QLineEdit *>(ei->edit);
        if(ed)
            ed->setText(record->value(ei->title).toString());
	}
    */
}

void ScrWidget::updateEditState()
{
    bool view = (record->mode() == TrkToolRecord::View);
    bool edit = (record->mode() == TrkToolRecord::Edit);
    bool insert = (record->mode() == TrkToolRecord::Insert);

    decorator->updateState(record, fieldEdits);
    /*

	descEdit->setReadOnly(view);
	QList<EditDef>::const_iterator ei;
    for(ei=fieldEdits.edits.constBegin(); ei!=fieldEdits.edits.constEnd(); ei++)
	{
		QLineEdit *ed = qobject_cast<QLineEdit *>(ei->edit);
		ed->setReadOnly(view);
		//setText(record->value(ei->title).toString());
	}
    */


    actionCancel_Changes->setEnabled(edit || insert);
    actionSave_Record->setEnabled(edit || insert);
    actionEdit_Record->setEnabled(view);
}


void ScrWidget::initEdits(const AbstractRecordTypeDef *typeDef)
{
    decorator->fillEditPanels(fieldTabs, typeDef, fieldEdits, false);
}

void ScrWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    deleteLater();
}

void ScrWidget::on_actionEdit_Record_triggered()
{
    if(!record)
        return;
    if(record->mode() == TrkToolRecord::View)
        record->updateBegin();
    updateEditState();
}

void ScrWidget::on_actionSave_Record_triggered()
{
    //if(record->mode() == TrkToolRecord::Insert && record->mode() == TrkToolRecord::Edit)
    record->commit();
    updateEditState();
}

void ScrWidget::on_actionCancel_Changes_triggered()
{
    record->cancel();
    updateEditState();
}

void ScrWidget::on_actionExit_triggered()
{
    hide();
}

void ScrWidget::on_notesTable_currentCellChanged(int currentRow, int /* currentColumn */, int previousRow, int /* previousColumn */)
{
    if(previousRow!=currentRow)
        if(currentRow>=0)
            noteEdit->setPlainText(notes[currentRow].text);
        else
            noteEdit->setPlainText("");
}
