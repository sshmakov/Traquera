#ifndef TTRECWINDOW_H
#define TTRECWINDOW_H

#include <QMainWindow>
//#include <trkview.h>
#include "tqbase.h"
#include <tqrecordviewcontroller.h>

namespace Ui {
class TTRecordWindow;
}

#define TQ_DESC_INDEX (-1)
#define TQ_FIRST_NOTE 0
#define TQ_NEW_NOTE (-2)
#define TQ_NO_INDEX (-3)


class ModifyPanel;
class TrkToolRecord;
class ScrPluginFactory;
//class FilesPage;
class TTRecordWindowPrivate;

class TTRecordWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(TQRecord *record READ record)
    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(bool changed READ isChanged WRITE setChanged)
    Q_PROPERTY(int mode READ mode NOTIFY modeChanged)
    Q_PROPERTY(TQViewController *controller READ controller)
    Q_PROPERTY(const TQAbstractRecordTypeDef *recordTypeDef READ recordTypeDef)
private:
    TTRecordWindowPrivate *d;
public:
    explicit TTRecordWindow(QWidget *parent = 0);
    ~TTRecordWindow();
    void setRecordTypeDef(const TQAbstractRecordTypeDef *recDef, int mode);
    const TQAbstractRecordTypeDef *recordTypeDef() const;
    Q_INVOKABLE TQRecord *record();
    void setRecord(TQRecord *rec);
    Q_INVOKABLE QString noteTitle(int index);
    Q_INVOKABLE QString noteText(int index);
    Q_INVOKABLE bool setValue(const QString &field, const QVariant &value);
    Q_INVOKABLE QVariant value(const QString &field);
    Q_INVOKABLE QString description();
    Q_INVOKABLE bool enableModify();
    Q_INVOKABLE bool writeChanges();
    Q_INVOKABLE QObjectList getSelectedRecords();
    bool writeDraftChanges();
    bool isChanged();
    void setChanged(bool value);
    int mode();
    TQViewController *controller();

public slots:
    bool setDescription(const QString &desc);
    bool setNote(int index, const QString &title, const QString &text);
    bool addNote(const QString &title, const QString &text);
    bool startEditDescription();
    bool startEditNote(int index);
    bool endEditNote(bool commitChanges = true);
    void resetNoteEditor();

signals:
    void noteChanged(int index, const QString &title, const QString &text);
    void noteStateChanged(int index, const QString &newState);
    void recordAdded(TQRecord *record);
    void recordModified(TQRecord *record);
    void recordChanged(TQRecord *newRecord);
    void modeChanged(int mode);

protected:
    ModifyPanel *props;
//    FilesPage *filesPage;
    TQAbstractRecordTypeDef *recDef;
    TQRecord *a_record;
    ScrPluginFactory *factory;
    bool changed;
    struct NoteValue {
        QString title;
        QString text;
    };

    QHash<int,NoteValue> newText;
    int noteInEdit;
    QTabBar *tabBar;
    int titleVid;
    QString titleFieldName;
    int editorMode;

    void initWidgets();
    virtual void closeEvent(QCloseEvent *event);
    bool isNoteEntered();
    //bool doSetNote(int index, const QString &title, const QString &text);
//    void applyNewNote();
    void showNoteEditor(bool show);

protected slots:
    void refreshState();
    void valueChanged();
    void recordStateChanged();
    void noteEditing(int index);
    void canceledEditing(int index);
    void refreshValues();
    bool addNewNote(const QString &title, const QString &text);
    void postCurValue();
    void titleChanged(const QString &value);
    void fullReload();

private slots:
    void on_actionEditRecord_triggered();
    void on_actionSaveChanges_triggered();
    void on_actionCancelChanges_triggered();
    void on_actionClose_triggered();
    void on_addNoteButton_clicked();
    void on_actionSaveExit_triggered();
    void on_newNoteButton_clicked();
    void on_cancelNoteButton_clicked();

    void on_actionRefresh_triggered();

public slots:
    void populateJavaScriptWindowObject();
    void cancel();
    void commit();
    void addDetailTab(QWidget *tab, const QString &title, const QIcon &icon = QIcon());
    void addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon);
    void setDetailTabTitle(QWidget *tab, const QString &title);


private:
    Ui::TTRecordWindow *ui;
};

#endif // TTRECWINDOW_H
