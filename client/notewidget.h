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
    void submit();
    void cancel();

public slots:
    void setNoteIndex(int newIndex);
    void setNoteTitle(const QString &title);
    void setNoteText(const QString &text);
    
signals:
    void changedNoteTitle(int index, const QString &title);
    void changedNoteText(int index);
    void submitTriggered(int index, const QString &title, const QString &text);
    void cancelTriggered(int index);

protected slots:
    void onTitleChanged(const QString &title);
    void onTextChanged();
    void onSubmitClicked();
    void onCancelClicked();

public:
    TrkToolRecord *record;
    int index;

private:
    Ui::NoteWidget *ui;
};

#endif // NOTEWIDGET_H
