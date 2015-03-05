#ifndef SCRWIDG_H
#define SCRWIDG_H

#include "ui_scrwidg.h"
#include <QMainWindow>
#include <QWidget>
#include "trdefs.h"
#include "tqbase.h"
#include "trkdecorator.h"

class TrkToolRecord;

class ScrWidget : public QMainWindow, private Ui::ScrWidgForm
{
	Q_OBJECT
protected:
    EditDefList fieldEdits;
    TQNotesCol notes;
public:
    ScrWidget(const TQAbstractRecordTypeDef *typeDef, QWidget *parent = 0);
    virtual ~ScrWidget();
    TQRecord *record;
    void setRecord(TQRecord *newrec);
	void readRecord();
    void initEdits(const TQAbstractRecordTypeDef *typeDef);
protected:
    virtual void hideEvent(QHideEvent *event);
protected slots:
	void updateEditState();
private slots:
    void on_actionEdit_Record_triggered();
    void on_actionSave_Record_triggered();
    void on_actionCancel_Changes_triggered();
    void on_actionExit_triggered();
    void on_notesTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
};

#endif SCRWIDG_H
