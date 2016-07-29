#include "ttrecwindow.h"
#include "ui_ttrecwindow.h"
#include "modifypanel.h"
//#include "trkview.h"
#include "tqbase.h"
#include "settings.h"
#include "ttglobal.h"
#include "scrpluginfactory.h"

#include <QtWebKit>
#include <QtXmlPatterns>
#include <filespage.h>
//#include "tqplanswidget.h"

const QString TTRecordState = "TTRecWinState";
const QString TTRecordGeometry = "TTRecWinGeometry";

//QScriptEngine engine;
   //QScriptEngineDebugger debugger;

class TTRecordWindowPrivate {
public:
    TQRecordViewController *controller;
    const TQAbstractRecordTypeDef *recDef;
    QWebInspector *inspector;
};


TTRecordWindow::TTRecordWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TTRecordWindow),
    d(new TTRecordWindowPrivate())
{
    ui->setupUi(this);
    initWidgets();
    d->controller = new TQRecordViewController(this);
    d->inspector = new QWebInspector();
//    connect(this,SIGNAL(recordChanged(TQRecord*)),d->controller,SLOT(onViewRecordChanged(TQRecord*)));
//    connect(d->controller,SIGNAL(detailTabTitleChanged(QWidget*,QString)),SLOT(setDetailTabTitle(QWidget*,QString)));

    //ui->noteTextEdit->addAction(ui->actionSaveExit);
    changed = false;

    QAction *action;
    action = ui->webView->pageAction(QWebPage::Copy);
    if(action)
    {
#ifdef Q_WS_WIN
        action->setShortcuts(QList<QKeySequence>()
                             << QKeySequence("Ctrl+Insert")
                             << QKeySequence("Ctrl+C")
                             );
#else
        action->setShortcut(QKeySequence::Copy);
#endif
        ui->webView->addAction(action);
    }
    action = ui->webView->pageAction(QWebPage::Reload);
    if(action)
    {
        action->setShortcut(QKeySequence("F5"));
        ui->webView->addAction(action);
    }
    /*
    action = ui->webView->pageAction(QWebPage::ReloadAndBypassCache);
    if(action)
    {
        action->setShortcut(QKeySequence("Ctrl+F5"));
        ui->webView->addAction(action);
    }
    */
    /*
#ifdef QT_DEBUG
    action = new QAction("Refresh", this);
    ui->webView->addAction(action);
    connect(action, SIGNAL(triggered()), SLOT(fullReload()));
#endif
    */
    ttglobal()->emitViewOpened(this, d->controller);
}

TTRecordWindow::~TTRecordWindow()
{
    if(d->inspector)
    {
        d->inspector->close();
//        d->inspector->setPage(0);
//        delete d->inspector;
//        d->inspector = 0;
    }
    delete ui->webView;
    if(a_record)
    {
        QObject *rec = a_record;
        a_record = 0;
        delete rec;
    }
    delete ui;
    delete d;
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
    QSettings *settings = ttglobal()->settings();
    settings->setValue(TTRecordState, saveState());
    settings->setValue(TTRecordGeometry, saveGeometry());
    QWidget::closeEvent(event);
}

bool TTRecordWindow::isNoteEntered()
{
    return !ui->noteTextEdit->toPlainText().trimmed().isEmpty();
}

void TTRecordWindow::showNoteEditor(bool show)
{
    if(show)
    {
        ui->actionsBar->hide();
        ui->editorFrame->show();
        //ui->addNoteButton->hide();
    }
    else
    {
        ui->actionsBar->show();
        ui->editorFrame->hide();
        //ui->addNoteButton->show();
    }

}

void TTRecordWindow::setRecordTypeDef(const TQAbstractRecordTypeDef *recDef, int mode)
{
    if(d->recDef == recDef && editorMode == mode)
        return;
    d->recDef = recDef;
    Q_ASSERT(recDef != 0);
    props->setRecordDef(recDef, mode);
    props->fillValues(QObjectList() << a_record);
    titleVid = recDef->roleVid(TQAbstractRecordTypeDef::TitleField);
    if(titleVid)
        titleFieldName = recDef->fieldName(titleVid);
    else
        titleFieldName.clear();
    if(titleFieldName.isEmpty())
        ui->labelRecordTitle->setText(tr("Тема:"));
    else
        ui->labelRecordTitle->setText(titleFieldName+":");
}

const TQAbstractRecordTypeDef *TTRecordWindow::recordTypeDef() const
{
    return d->recDef;
}

TQRecord *TTRecordWindow::record()
{
    return a_record;
}

void TTRecordWindow::setRecord(TQRecord *rec)
{
    a_record = rec;
    if(rec->mode() == TQRecord::Edit)
        setRecordTypeDef(rec->typeEditDef(), rec->mode());
    else
        setRecordTypeDef(rec->typeDef(), rec->mode());
    factory->setRecord(rec);
    refreshValues();
    refreshState();
    setWindowTitle(QString(tr("Запрос %1 %2")).arg(rec->recordId()).arg(rec->title()));
    connect(a_record, SIGNAL(changedState(int)), SLOT(refreshState()));
    emit recordChanged(a_record);
}

bool TTRecordWindow::setNote(int index, const QString &title, const QString &text)
{
    bool res = a_record->setNote(index,title,text);
    if(res)
        emit noteChanged(index, title, text);
    return res;
}

bool TTRecordWindow::addNote(const QString &title, const QString &text)
{
    if(!enableModify())
        return false;
    return a_record->addNote(title, text) != -1;
}

bool TTRecordWindow::removeNote(int index)
{
    if(!enableModify())
        return false;
    return a_record->removeNote(index);
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
        return a_record->setDescription(desc);
    return false;
    /*
    {
        NoteValue nv;
        nv.text = desc;
        newText[-1] = nv;
        changed = true;
        return true;
    }
    return false;
    */
}

void TTRecordWindow::refreshState()
{
    bool edit = a_record->isEditing();
    ui->actionEditRecord->setChecked(edit);
    ui->actionEditRecord->setEnabled(!edit);
    ui->actionSaveChanges->setEnabled(edit);
    ui->actionCancelChanges->setEnabled(edit);
    bool changed = editorMode != a_record->mode();
    editorMode = a_record->mode();
    if(editorMode == TQRecord::Edit)
        setRecordTypeDef(a_record->typeEditDef(), a_record->mode());
    else
        setRecordTypeDef(a_record->typeDef(), a_record->mode());
    if(changed)
        emit modeChanged(editorMode);
}

void TTRecordWindow::valueChanged()
{
    if(enableModify())
    {
        setChanged(true);
        ui->recordTitleEdit->setText(props->fieldValue(titleFieldName).toString());
    }
    else
        props->resetAll();
}

void TTRecordWindow::recordStateChanged()
{
    if(a_record)
        refreshState();
}

void TTRecordWindow::noteEditing(int index)
{
    //noteInEdit[index] = true;
}

void TTRecordWindow::canceledEditing(int index)
{
    //noteInEdit.remove(index);
}



static QString xmlToHTML(TQRecord *record, const QString &xqCodeFile)
{
    QDomDocument xml = record->toXML();
    QFile xq(xqCodeFile);
    xq.open(QIODevice::ReadOnly);
    QFile groupXML(record->project()->optionValue(TQOPTION_GROUP_FILE).toString()); // "data/tracker.xml");
    bool isGroupOpened = groupXML.open(QIODevice::ReadOnly);
    QXmlQuery query;
//#ifdef CLIENT_APP
//    query.setMessageHandler(sysMessager);
//#endif


    QString page;
    //QString src=xml.toString();
    QByteArray ba=xml.toByteArray();
    QBuffer buf;
    buf.setData(ba);
    buf.open(QIODevice::ReadOnly);
    query.bindVariable("scrdoc",&buf);
    if(isGroupOpened)
        query.bindVariable("def",&groupXML);
    query.setQuery(&xq);
    //query.setQuery(&xq, QUrl::fromLocalFile(xq.fileName()));
    query.evaluateTo(&page);
#ifdef QT_DEBUG
    QFile testXml("!testEdit.xml");
    testXml.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream textOut(&testXml);
    xml.save(textOut,4);

    QFile testRes("!testResult.html");
    testRes.open(QIODevice::WriteOnly | QIODevice::Text);
    //QTextStream textOutHTML(&testRes);
    testRes.write(page.toLocal8Bit());
#endif
    return page;
}


void TTRecordWindow::refreshValues()
{
    //a_record->refresh();
    ui->recordTitleEdit->blockSignals(true);
    props->fillValues(QObjectList() << a_record);
    if(titleVid)
        ui->recordTitleEdit->setText(a_record->value(titleVid).toString());
    else
        ui->recordTitleEdit->clear();
    ui->recordTitleEdit->blockSignals(false);

    QString xqFile = a_record->project()->optionValue(TQOPTION_EDIT_TEMPLATE).toString();
    QString html = xmlToHTML(a_record, xqFile);
    QUrl baseUrl = QUrl::fromUserInput(xqFile);
    ui->webView->setHtml(html, baseUrl);
    d->inspector->setPage(ui->webView->page());


/*
    QAction *action = ui->webView->pageAction(QWebPage::Copy);
#ifdef Q_WS_WIN
    copyAction->setShortcuts(QList<QKeySequence>()
                             << QKeySequence("Ctrl+Insert")
                             << QKeySequence("Ctrl+C")
                             );
#else
    copyAction->setShortcut(QKeySequence::Copy);
#endif
    ui->webView->addAction(copyAction);
    ui->webView->addAction(ui->webView->pageAction(QWebPage::Reload));
*/

    ui->titleBox->clear();
    const TQAbstractRecordTypeDef *def;
    def = a_record->typeDef();
    if(def)
    {
        ui->titleBox->addItems(def->noteTitleList());
    }
    QSettings *settings = ttglobal()->settings();
    if(settings->contains("LastNote"))
        ui->titleBox->setEditText(settings->value("LastNote").toString());
    //changed = false;
}

bool TTRecordWindow::addNewNote(const QString &title, const QString &text)
{
    if(enableModify())
        if(a_record->addNote(title,text))
        {
            setChanged(true);
            return true;
        }
    return false;
}

//bool TTRecordWindow::doSetNote(int index, const QString &title, const QString &text)
//{
//    if(enableModify())
//        if(a_record->setNote(index,title,text))
//        {
//            setChanged(true);
//            return true;
//        }
//    return false;
//}

//void TTRecordWindow::applyNewNote()
//{
//    if(isNoteEntered())
//        if(addNewNote(ui->titleBox->currentText(),ui->noteTextEdit->toPlainText()))
//            ui->noteTextEdit->setPlainText("");
//}

void TTRecordWindow::postCurValue()
{
}

void TTRecordWindow::titleChanged(const QString &value)
{
    if(titleFieldName.isEmpty())
        return;
    if(enableModify())
    {
        props->blockSignals(true);
        props->setFieldValue(titleFieldName, value);
        props->blockSignals(false);
    }
}

void TTRecordWindow::fullReload()
{
    QWebPage *page = ui->webView->page();
    if(page)
    {
        QNetworkAccessManager *man = page->networkAccessManager();
        if(man)
        {
            QAbstractNetworkCache *cache = man->cache();
            if(cache)
                cache->clear();
        }
    }
    refreshValues();
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
    int mode = a_record->mode();
    if(writeChanges() && a_record->commit())
    {
        props->resetAll();
        props->fillValues(QObjectList() << a_record);
        setRecord(a_record);
        if(mode == TQRecord::Insert)
            emit recordAdded(a_record);
        if(mode == TQRecord::Edit)
            emit recordModified(a_record);

    }
    refreshState();
}

void TTRecordWindow::addDetailTab(QWidget *tab, const QString &title, const QIcon &icon)
{
    ui->tabWidget->addTab(tab,icon,title);
//    connect(this,SIGNAL(recordChanged(TQRecord*)),tab,SLOT(setRecord(TQRecord*)));
    /*
    QVBoxLayout *lay = new QVBoxLayout();
    TQPlansWidget *plans = new TQPlansWidget(this);
    lay->addWidget(plans);
    plans->setParentObject(this);
    plans->setGlobalObject(this);
    ui->tabPlans->setLayout(lay);
    */
}

void TTRecordWindow::addDetailWidgets(QWidget *topWidget, QWidget *pageWidget, const QString &title, const QIcon &icon)
{
    QWidget *page;
    if(topWidget && pageWidget)
    {
        page = new QWidget(this);
//        QHBoxLayout *lay = new QHBoxLayout(page);
        QSplitter *split = new QSplitter(page);
        split->addWidget(topWidget);
        split->addWidget(pageWidget);
    }
    else if(topWidget)
        page = topWidget;
    else if(pageWidget)
        page = pageWidget;
    else
        return;
    ui->tabWidget->addTab(page,icon,title);
}

void TTRecordWindow::setDetailTabTitle(QWidget *tab, const QString &title)
{
    int index = ui->tabWidget->indexOf(tab);
    if(index == -1)
        return;
    ui->tabWidget->setTabText(index, title);
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
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("view", this);
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("record", a_record);
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("global", ttglobal());
}


bool TTRecordWindow::enableModify()
{
    bool res=false;
    if(a_record->mode() == TQRecord::View)
        res = a_record->updateBegin();
    else
        res = true;
    refreshState();
    return res;
}

bool TTRecordWindow::writeChanges()
{
    if(a_record->mode() == TQRecord::View)
        return false;
    //applyNewNote();
    factory->closeEdits(true);
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
            if(a_record->addNote(newText.value(key).title,newText.value(key).text) == -1)
                return false;
        }
    }
    return true;
}

QObjectList TTRecordWindow::getSelectedRecords()
{
    QObjectList list;
    if(a_record)
        list.append(a_record);
    return list;
}

bool TTRecordWindow::writeDraftChanges()
{
    //foreach(int index, noteInEdit.keys())
    return false;
}

bool TTRecordWindow::isChanged()
{
    return changed || (record() && record()->isModified());
}

void TTRecordWindow::setChanged(bool value)
{
    changed = value;
}

int TTRecordWindow::mode()
{
    return editorMode;
}

TQViewController *TTRecordWindow::controller()
{
    return d->controller;
}

bool TTRecordWindow::startEditDescription()
{
    return startEditNote(TQ_DESC_INDEX);
}

bool TTRecordWindow::startEditNote(int index)
{
    if(index == TQ_NO_INDEX)
        return false;
    if(!enableModify())
        return false;
    if(index == TQ_DESC_INDEX)
    {
        ui->labelTitle->setText("Description");
        ui->titleBox->setVisible(false);
        ui->noteTextEdit->setPlainText(a_record->description());
        noteInEdit = index;
        showNoteEditor(true);
        return true;
    }
    ui->titleBox->clear();
    const TQAbstractRecordTypeDef *def = a_record->typeEditDef();
    if(def)
        ui->titleBox->addItems(def->noteTitleList());
    if(index == TQ_NEW_NOTE)
    {
        QSettings *settings = ttglobal()->settings();
        if(settings->contains("LastNote"))
            ui->titleBox->setEditText(settings->value("LastNote").toString());
        ui->noteTextEdit->setPlainText("");
        noteInEdit = index;
        showNoteEditor(true);
        return true;
    }
    if(index >= TQ_FIRST_NOTE)
    {
        ui->titleBox->setEditText(a_record->noteTitle(index - TQ_FIRST_NOTE));
        ui->noteTextEdit->setPlainText(a_record->noteText(index - TQ_FIRST_NOTE));
        noteInEdit = index;
        showNoteEditor(true);
        return true;
    }
    return false;
}

bool TTRecordWindow::endEditNote(bool commitChanges)
{
    QSettings *settings = ttglobal()->settings();
    if(commitChanges)
    {
        if(noteInEdit == TQ_DESC_INDEX)
        {
            if(!setDescription(ui->noteTextEdit->toPlainText()))
                return false;
        }
        else if (noteInEdit == TQ_NEW_NOTE)
        {
            if(!addNote(ui->titleBox->currentText(),
                        ui->noteTextEdit->toPlainText()))
                return false;
            settings->setValue("LastNote",ui->titleBox->currentText());
        }
        else if (noteInEdit >= TQ_FIRST_NOTE)
        {
            if(!setNote(noteInEdit - TQ_FIRST_NOTE,
                        ui->titleBox->currentText(),
                        ui->noteTextEdit->toPlainText()))
                return false;
            settings->setValue("LastNote",ui->titleBox->currentText());
        }
    }
    resetNoteEditor();
    showNoteEditor(false);
    return true;
}

void TTRecordWindow::resetNoteEditor()
{
    ui->labelTitle->setText(tr("New note"));
    ui->titleBox->clear();
    ui->titleBox->setVisible(true);
    ui->noteTextEdit->clear();
    noteInEdit = TQ_NO_INDEX;
}

void TTRecordWindow::on_actionClose_triggered()
{
    close();
}

void TTRecordWindow::on_addNoteButton_clicked()
{
    //applyNewNote();
    endEditNote(true);
    refreshValues();
}

void TTRecordWindow::on_actionSaveExit_triggered()
{
    if(isChanged() || isNoteEntered())
        commit();
    refreshValues();
    close();
}

void TTRecordWindow::on_newNoteButton_clicked()
{
    startEditNote(TQ_NEW_NOTE);
}


void TTRecordWindow::initWidgets()
{
    QSettings *settings = ttglobal()->settings();
    props = new ModifyPanel(this);
    props->setButtonsVisible(false);
    connect(props,SIGNAL(dataChanged()),SLOT(valueChanged()));
    connect(ui->recordTitleEdit,SIGNAL(textChanged(QString)),SLOT(titleChanged(QString)));
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
    factory = new ScrPluginFactory(ui->webView,this);
    ui->webView->page()->setPluginFactory(factory);
    connect(factory,SIGNAL(changedNoteEditor(int)),SLOT(noteEditing(int)));
    //connect(factory,SIGNAL(canceledChanges(int)),SLOT());
    connect(ui->noteTextEdit,SIGNAL(textChanged()),this,SLOT(valueChanged()));
    showNoteEditor(false);

    /* moved to plugins
    QVBoxLayout *lay = new QVBoxLayout();
    TQPlansWidget *plans = new TQPlansWidget(this);
    lay->addWidget(plans);
    plans->setParentObject(this);
    plans->setGlobalObject(this);
    ui->tabPlans->setLayout(lay);
    */

//    tabBar = new QTabBar(ui->tabPlaceWidget);
//    tabBar->addTab(QIcon(":/images/file.png"),tr("Текст"));
//    tabBar->addTab(QIcon(":/images/plan.png"),tr("Планы"));
//    tabBar->addTab(tr("Файлы"));

//    QBoxLayout *lay = qobject_cast<QBoxLayout *>(ui->tabPlaceWidget->layout());
//    lay->insertWidget(0,tabBar);
    ui->tabWidget->setCurrentIndex(0);

//    filesPage = new FilesPage();
//    addDetailTab(filesPage, "Files!");

}

void TTRecordWindow::on_cancelNoteButton_clicked()
{
    endEditNote(false);
}

void TTRecordWindow::on_actionRefresh_triggered()
{
    fullReload();
}
