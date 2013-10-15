#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>

class TrkToolRecord;
class TTRecordWindow;

#define NoteWidget_MIME "application/scrnote"

namespace Ui {
class NoteWidget;
}

class NoteWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit NoteWidget(TrkToolRecord *rec, QWidget *parent = 0);
    ~NoteWidget();
    Q_INVOKABLE QString noteTitle() const;
    Q_INVOKABLE QString noteText() const;
    Q_INVOKABLE int noteIndex() const;
public slots:
    void setNoteIndex(int newIndex);
    void setNoteTitle(const QString &title);
    void setNoteText(const QString &text);
    
signals:
    void changedNoteTitle(const QString &title);
    void changedNoteText();
    void submitTriggered(int index, const QString &title, const QString &text);
    void cancelTriggered(int index);

protected slots:
    void onSubmitClicked();
    void onCancelClicked();


private:
    Ui::NoteWidget *ui;
    TrkToolRecord *record;
    int index;
};

#endif // NOTEWIDGET_H
