#include "ttrecwindow.h"
#include "ui_ttrecwindow.h"
#include "modifypanel.h"
#include "trkview.h"
#include "settings.h"
#include "ttglobal.h"
#include "scrpluginfactory.h"

#include <QtWebKit>

const QString TTRecordState = "TTRecWinState";
const QString TTRecordGeometry = "TTRecWinGeometry";

//QScriptEngine engine;
   //QScriptEngineDebugger debugger;

TTRecordWindow::TTRecordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TTRecordWindow)
{
    ui->setupUi(this);
    props = new ModifyPanel(this);
    props->setButtonsVisible(false);
    connect(props,SIGNAL(dataChanged()),SLOT(valueChanged()));
    ui->dockWidget->setWidget(props);
    ui->dockWidget->setWindowTitle(tr("Свойства"));
    setAttribute(Qt::WA_DeleteOnClose,true);
    if(settings->contains(TTRecordGeometry))
        restoreGeometry(settings->value(TTRecordGeometry).toByteArray());
    if(settings->contains(TTRecordState))
        restoreState(settings->value(TTRecordState).toByteArray());
    ui->webView->setUrl(QUrl("about:blank"));
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(populateJavaScriptWindowObject()));
    connect(ui->webView,SIGNAL(statusBarMessage(QString)),
            ttglobal(),SLOT(statusBarMessage(QString)));
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    factory = new ScrPluginFactory(ui->webView,this);
    ui->webView->page()->setPluginFactory(factory);
    connect(ui->noteTextEdit,SIGNAL(textChanged()),this,SLOT(valueChanged()));

    //ui->noteTextEdit->addAction(ui->actionSaveExit);
    changed = false;
}

TTRecordWindow::~TTRecordWindow()
{
    delete ui;
}

void TTRecordWindow::closeEvent(QCloseEvent *event)
{
    if(a_record && a_record->isEditing())
    {
        if(isChanged())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("Запрос был изменен"));
            msgBox.setInformativeText(tr("Хотите записать изменения?"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();
            switch (ret) {
            case QMessageBox::Save:
                // Save was clicked
                commit();
                break;
            case QMessageBox::Discard:
                // Don't Save was clicked
                cancel();
                break;
            case QMessageBox::Cancel:
                // Cancel was clicked
                event->ignore();
                return;
            default:
                // should never be reached
                event->ignore();
                return;
            }
        }
        else
            a_record->cancel();
    }
    settings->setValue(TTRecordState, saveState());
    settings->setValue(TTRecordGeometry, saveGeometry());
    QWidget::closeEvent(event);
}

bool TTRecordWindow::isNoteEntered()
{
    return !ui->noteTextEdit->toPlainText().trimmed().isEmpty();
}

void TTRecordWindow::setTypeDef(const RecordTypeDef *recDef)
{
    props->loadDefinitions(recDef);
}

TrkToolRecord *TTRecordWindow::getRecord()
{
    return a_record;
}

void TTRecordWindow::setRecord(TrkToolRecord *rec)
{
    a_record = rec;
    factory->setRecord(rec);
    refreshValues();
    /*
    props->fillValues(a_record);
    QString html = a_record->toHTML("data/edit.xq");
    ui->webView->setHtml(html);
    ui->titleBox->clear();
    ui->titleBox->addItems(a_record->project()->noteTitles);
    if(settings->contains("LastNote"))
        ui->titleBox->setEditText(settings->value("LastNote").toString());
    changed = false;
    */
    refreshState();
    setWindowTitle(QString(tr("Запрос %1")).arg(rec->recordId()));
}

bool TTRecordWindow::setNote(int index, const QString &title, const QString &text)
{
    return doSetNote(index, title, text);
    /*
    if(enableModify())
    {
        NoteValue nv;
        nv.title = title;
        nv.text = text;
        newText[index] = nv;
        changed = true;
        return true;
    }
    return false;
    */
}

QString TTRecordWindow::noteTitle(int index)
{
    return a_record->noteTitle(index);
}

QString TTRecordWindow::noteText(int index)
{
    return a_record->noteText(index);
}

bool TTRecordWindow::setValue(const QString &field, const QVariant &value)
{
    if(!enableModify())
        return false;
    props->setFieldValue(field,value);
    changed = true;
    return true;
}

QVariant TTRecordWindow::value(const QString &field)
{
    return props->fieldValue(field);
}

QString TTRecordWindow::description()
{
    return a_record->description();
}

bool TTRecordWindow::setDescription(const QString &desc)
{
    if(enableModify())
    {
        NoteValue nv;
        nv.text = desc;
        newText[-1] = nv;
        changed = true;
        return true;
    }
    return false;
}

void TTRecordWindow::refreshState()
{
    bool edit = a_record->isEditing();
    ui->actionEditRecord->setChecked(edit);
    ui->actionEditRecord->setEnabled(!edit);
    ui->actionSaveChanges->setEnabled(edit);
    ui->actionCancelChanges->setEnabled(edit);
}

void TTRecordWindow::valueChanged()
{
    if(enableModify())
        setChanged(true);
    else
        props->resetAll();
}

void TTRecordWindow::refreshValues()
{
    a_record->refresh();
    props->fillValues(a_record);
    QString html = a_record->toHTML("data/edit.xq");
    ui->webView->setHtml(html);
    ui->titleBox->clear();
    ui->titleBox->addItems(a_record->project()->noteTitles);
    if(settings->contains("LastNote"))
        ui->titleBox->setEditText(settings->value("LastNote").toString());
    //changed = false;
}

bool TTRecordWindow::addNewNote(const QString &title, const QString &text)
{
    if(enableModify())
        if(a_record->appendNote(title,text))
        {
            setChanged(true);
            return true;
        }
    return false;
}

bool TTRecordWindow::doSetNote(int index, const QString &title, const QString &text)
{
    if(enableModify())
        if(a_record->setNote(index,title,text))
        {
            setChanged(true);
            return true;
        }
    return false;
}

void TTRecordWindow::applyNewNote()
{
    if(isNoteEntered())
        if(addNewNote(ui->titleBox->currentText(),ui->noteTextEdit->toPlainText()))
            ui->noteTextEdit->setPlainText("");
}

void TTRecordWindow::postCurValue()
{
}

void TTRecordWindow::cancel()
{
    if(a_record->cancel())
    {
        /*
        props->resetAll();
        props->fillValues(a_record);
        changed = false;
        */
        refreshValues();
    }
    refreshState();
}

void TTRecordWindow::commit()
{
    if(writeChanges() && a_record->commit())
    {
        props->resetAll();
        props->fillValues(a_record);
        setRecord(a_record);

    }
    refreshState();
}

void TTRecordWindow::on_actionEditRecord_triggered()
{
    enableModify();
}

void TTRecordWindow::on_actionSaveChanges_triggered()
{
    commit();
}

void TTRecordWindow::on_actionCancelChanges_triggered()
{
    cancel();
}

void TTRecordWindow::populateJavaScriptWindowObject()
{
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("editor", this);
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("record", a_record);
}


bool TTRecordWindow::enableModify()
{
    bool res=false;
    if(a_record->mode() == TrkToolRecord::View)
        res = a_record->updateBegin();
    else
        res = true;
    refreshState();
    return res;
}

bool TTRecordWindow::writeChanges()
{
    if(a_record->mode() == TrkToolRecord::View)
        return false;
    applyNewNote();
    a_record->setValues(props->changes());
    foreach(int key, newText.keys())
    {
        if(key==-1)
        {
            if(!a_record->setDescription(newText.value(key).text))
                return false;
        }
        else if(key>=0)
        {
            if(!a_record->setNote(key,newText.value(key).title,newText.value(key).text))
                return false;
        }
        else if(key<0)
        {
            if(!a_record->appendNote(newText.value(key).title,newText.value(key).text))
                return false;
        }
    }
    return true;
}

bool TTRecordWindow::isChanged()
{
    return changed;
}

void TTRecordWindow::setChanged(bool value)
{
    changed = value;
}

void TTRecordWindow::on_actionClose_triggered()
{
    close();
}

void TTRecordWindow::on_addNoteButton_clicked()
{
    applyNewNote();
    refreshValues();
}

void TTRecordWindow::on_actionSaveExit_triggered()
{
    if(isChanged() || isNoteEntered())
        commit();
    refreshValues();
    close();
}
