#ifndef TTRECWINDOW_H
#define TTRECWINDOW_H

#include <QMainWindow>
#include <trkview.h>

namespace Ui {
class TTRecordWindow;
}

class ModifyPanel;
class TrkToolRecord;
class ScrPluginFactory;

class TTRecordWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(TrkToolRecord *record READ getRecord)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(bool changed READ isChanged WRITE setChanged)
public:
    explicit TTRecordWindow(QWidget *parent = 0);
    ~TTRecordWindow();
    void setTypeDef(const RecordTypeDef *recDef);
    Q_INVOKABLE TrkToolRecord *getRecord();
    void setRecord(TrkToolRecord *rec);
    Q_INVOKABLE bool setNote(int index, const QString &title, const QString &text);
    Q_INVOKABLE QString noteTitle(int index);
    Q_INVOKABLE QString noteText(int index);
    Q_INVOKABLE bool setValue(const QString &field, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &field);
    Q_INVOKABLE QString description();
    Q_INVOKABLE bool setDescription(const QString &desc);
    Q_INVOKABLE bool enableModify();
    Q_INVOKABLE bool writeChanges();
    bool isChanged();
    void setChanged(bool value);


protected:
    ModifyPanel *props;
    RecordTypeDef *recDef;
    TrkToolRecord *a_record;
    ScrPluginFactory *factory;
    bool changed;
    struct NoteValue {
        QString title;
        QString text;
    };

    QHash<int,NoteValue> newText;
    virtual void closeEvent(QCloseEvent *event);
    bool isNoteEntered();
    bool doSetNote(int index, const QString &title, const QString &text);
    void applyNewNote();

protected slots:
    void refreshState();
    void valueChanged();
    void refreshValues();
    bool addNewNote(const QString &title, const QString &text);
    void postCurValue();

private slots:
    void on_actionEditRecord_triggered();
    void on_actionSaveChanges_triggered();
    void on_actionCancelChanges_triggered();

    void on_actionClose_triggered();

    void on_addNoteButton_clicked();

    void on_actionSaveExit_triggered();

public slots:
    void populateJavaScriptWindowObject();
    void cancel();
    void commit();


private:
    Ui::TTRecordWindow *ui;
};

#endif // TTRECWINDOW_H
