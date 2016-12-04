#ifndef NOTEWIDGET_H
#define NOTEWIDGET_H

#include <QWidget>

class TQRecord;
class TTRecordWindow;

#define NoteWidget_MIME "application/scrnote"

namespace Ui {
class NoteWidget;
}

class QAbstractButton;

class NoteWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString noteTitle READ noteTitle WRITE setNoteTitle NOTIFY noteTitleChanged)
    Q_PROPERTY(QString noteText READ noteText WRITE setNoteText NOTIFY noteTextChanged)
    Q_PROPERTY(int noteIndex READ noteIndex WRITE setNoteIndex)
    Q_PROPERTY(bool buttonsVisible READ isButtonsVisible WRITE setButtonsVisible)
    Q_PROPERTY(bool titleVisible READ isTitleVisible WRITE setTitleVisible)
public:
    explicit NoteWidget(TQRecord *rec, QWidget *parent = 0);
    ~NoteWidget();
    QString noteTitle() const;
    QString noteText() const;
    int noteIndex() const;

    void setNoteIndex(int newIndex);
    void setNoteTitle(const QString &title);
    void setNoteText(const QString &text);

    bool isButtonsVisible() const;
    void setButtonsVisible(bool value);
    bool isTitleVisible() const;
    void setTitleVisible(bool value);

public slots:
    void submit();
    void cancel();

signals:
    void noteTitleChanged(int index, const QString &title);
    void noteTextChanged(int index);
    void submitTriggered(int index, const QString &title, const QString &text);
    void cancelTriggered(int index);

protected slots:
    void onTitleChanged(const QString &title);
    void onTextChanged();
    void onButtonClicked(QAbstractButton *button);

public:
    TQRecord *record;
    int index;

private:
    Ui::NoteWidget *ui;
};

#endif // NOTEWIDGET_H
