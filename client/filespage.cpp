#include "filespage.h"
#include "ui_filespage.h"
#include "preview.h"
#include <tqbase.h>
#include "ttfileiconprovider.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include <tqviewcontroller.h>

class FilesPagePrivate
{
public:
    TQRecord *rec;
    const TQAbstractRecordTypeDef *def;
    QTimer previewTimer;
    int curFileIndex;
    bool modifyEnabled;
    QString title;
    TQViewController *controller;

//    FilesListWidget *fileView;
    FilesPagePrivate()
        : rec(0), def(0), modifyEnabled(false), curFileIndex(-1), controller(0)
    {
        previewTimer.setSingleShot(true);
    }
};

enum FilesColumns {
    FP_NAME = 0,
    FP_CHANGE,
    FP_ORIGINALPATH
};

FilesPage::FilesPage(QWidget *parent) :
    QWidget(parent),
    d(new FilesPagePrivate),
    ui(new Ui::FilesPage)
{
    ui->setupUi(this);
//    d->fileView = new FilesListWidget(this);
//    ui->layFiles->addWidget(d->fileView);
    ui->filesTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->filesTable->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);

//    QVBoxLayout * v = new QVBoxLayout(ui->previewFrame);
//    v->setContentsMargins(0, 0, 0, 0);
//    v->setObjectName(QString::fromUtf8("verticalLayout"));

    previewWidget = new MasterPreview(this);
    previewWidget->setObjectName("previewWidget");
    ui->previewFrame->layout()->addWidget(previewWidget);
//    ui->hLay->addWidget(new FileListWidget());
    connect(&d->previewTimer, SIGNAL(timeout()), SLOT(previewCurrentFile()));
    d->title = tr("Файлы (%1)");
}

FilesPage::~FilesPage()
{
    delete ui;
    delete d;
}

void FilesPage::setModifyEnabled(bool value)
{
    d->modifyEnabled = value;
}

bool FilesPage::isModifyEnabled() const
{
    return d->modifyEnabled;
}

QList<int> FilesPage::currentRows() const
{
    QList<int> rows;
    QList<QTableWidgetItem *> items = ui->filesTable->selectedItems();
    if(items.isEmpty())
        return rows;
    QSet<int> rowSet;
    foreach(QTableWidgetItem *item, items)
        rowSet.insert(item->row());
    rows = rowSet.toList();
    qSort(rows);
    return rows;
}

QString FilesPage::title() const
{
    return d->title.arg(ui->filesTable->rowCount());
}

void FilesPage::setViewController(TQViewController *contr)
{
    d->controller = contr;
    connect(d->controller, SIGNAL(currentRecordChanged(TQRecord*)), SLOT(setRecord(TQRecord*)));
}

void FilesPage::setRecord(TQRecord *record)
{
//    ui->filesTable->clearContents();
//    ui->filesTable->setRowCount(0);
    d->def = 0;
    if(d->rec)
        disconnect(d->rec,0, this, 0);
    d->rec = record;
    if(d->rec)
    {
        d->def = d->rec->typeDef();
        connect(d->rec, SIGNAL(changed(int)), SLOT(refreshFiles()));
        connect(d->rec, SIGNAL(destroyed()), SLOT(detachRecord()));
        connect(d->rec, SIGNAL(changedState(int)), SLOT(recordChangedState()));
    }
    refreshFiles();
//    refreshFiles2();
}

void FilesPage::refreshFiles()
{
    stopPreview();
    ui->filesTable->clearContents();
    if(d->rec)
    {
        ui->filesTable->setRowCount(0);
        QList<TQAttachedFile> files = d->rec->fileList();
        TTFileIconProvider prov;
        int row=0;
        foreach(const TQAttachedFile &file, files)
        {
            QFileInfo f(file.fileName);
            ui->filesTable->insertRow(row);
            ui->filesTable->setItem(row,FP_NAME,new QTableWidgetItem(prov.icon(f),f.fileName()));
            ui->filesTable->setItem(row,FP_CHANGE,new QTableWidgetItem(file.createDateTime.toString(d->def->dateTimeFormat())));
            ui->filesTable->setItem(row,FP_ORIGINALPATH,new QTableWidgetItem(f.dir().path()));
            row++;
        }
        setModifyEnabled(d->rec->mode() != TQRecord::View);
    }
    else
        setModifyEnabled(false);
    emit onTitleChange();
}

void FilesPage::refreshFiles2()
{
    /*
    stopPreview();
    d->fileView->clear();
    if(d->rec)
    {
//        ui->filesTable->setRowCount(0);
        QList<TQAttachedFile> files = d->rec->fileList();
        TTFileIconProvider prov;
        int row=0;
        foreach(const TQAttachedFile &file, files)
        {
            QFileInfo f(file.fileName);
            QListWidgetItem *item = new QListWidgetItem(d->fileView);
//            d->fileView->insertItem(item);
            item->setIcon(prov.icon(f));
            item->setText(f.fileName());
//            ui->filesTable->insertRow(row);
//            ui->filesTable->setItem(row,FP_NAME,new QTableWidgetItem(prov.icon(f),f.fileName()));
//            ui->filesTable->setItem(row,FP_CHANGE,new QTableWidgetItem(file.createDateTime.toString(d->def->dateTimeFormat())));
//            ui->filesTable->setItem(row,FP_ORIGINALPATH,new QTableWidgetItem(f.dir().path()));
            row++;
        }
        setModifyEnabled(d->rec->mode() != TQRecord::View);
    }
    else
        setModifyEnabled(false);
    emit onTitleChange();
    */
}

void FilesPage::previewCurrentFile()
{
    QTableWidgetItem *fileItem = ui->filesTable->currentItem();
    if(fileItem && fileItem->isSelected())
    {
        if(d->rec)
        {
            QString fileName = fileItem->text();
            QString tempFile = QDir::temp().absoluteFilePath(fileName);
            if(d->rec->saveFile(fileItem->row(), tempFile))
            {
                previewWidget->setSourceFile(tempFile);
                return;
            }
        }
    }
    previewWidget->clear();
}

void FilesPage::openCurrentFile()
{
    QModelIndex fileIndex = ui->filesTable->currentIndex();
    if(!fileIndex.isValid())
        return;
    stopPreview();
    if(!d->rec)
        return;
    QString fileName = ui->filesTable->item(fileIndex.row(),FP_NAME)->text();
    QString tempFile = QDir::temp().absoluteFilePath(fileName);
    if(d->rec->saveFile(fileIndex.row(), tempFile))
    {
        tempFile = QDir::toNativeSeparators(tempFile);
        //QUrl url = QUrl::fromUserInput(tempFile);
        QUrl url = QUrl::fromLocalFile(tempFile);
        //tempFile = url.toString();
        QDesktopServices::openUrl(url);
    }
}

void FilesPage::saveCurrentFiles()
{
    QList<int> rows = currentRows();
    if(rows.count() == 1)
    {
        QTableWidgetItem *item = ui->filesTable->item(rows.first(), FP_NAME);
        if(!item)
            return;
        QString fileName = item->text();
        fileName = QFileDialog::getSaveFileName(this,
                                              QString() /*"Сохранить файл..."*/,
                                              fileName);
        if(!fileName.isEmpty())
            d->rec->saveFile(item->row(), fileName);
        return;
    }
    QString path = QFileDialog::getExistingDirectory(this);
    if(path.isEmpty())
        return;
    QDir dir(path);
    foreach(int row, rows)
    {
        QTableWidgetItem *item = ui->filesTable->item(row, FP_NAME);
        if(!item)
            return;
        QString fileName = item->text();
        fileName = dir.filePath(fileName);
        if(!d->rec->saveFile(row, fileName))
            return;
    }
}

void FilesPage::appendFiles()
{
    if(!d->controller->beginModifySection())
        return;
    QStringList list = QFileDialog::getOpenFileNames(this);
    if(list.isEmpty())
    {
        d->controller->cancelModifySection();
        return;
    }
    foreach(QString filePath, list)
    {
        if(d->rec->appendFile(filePath) < 0)
            break;
    }
    d->controller->submitModifySection();
    refreshFiles();
}

void FilesPage::deleteCurrentFiles()
{
    QList<int> rows = currentRows();
    if(!rows.size())
        return;
    QStringList files;
    QString text;
    if(rows.size()==1)
    {
        QTableWidgetItem *item = ui->filesTable->item(rows.first(), FP_NAME);
        text = tr("Удалить файл '%1'?").arg(item->text());
    }
    else
        text = tr("Удалить файл: %1").arg(rows.size());

    if(QMessageBox::Ok == QMessageBox::question(this, tr("Удаление файлов"),text))
    {
        qSort(rows.begin(), rows.end(), qGreater<int>());
        foreach(int row, rows)
        {
            if(!d->rec->removeFile(row))
                break;
        }
    }
    refreshFiles();
}

void FilesPage::startPreview()
{
    d->previewTimer.start(100);
}

void FilesPage::stopPreview()
{
    d->previewTimer.stop();
    previewWidget->clear();
}

void FilesPage::detachRecord()
{
    disconnect(d->rec, 0, this, 0);
    stopPreview();
    d->def = 0;
    d->rec = 0;
    ui->filesTable->clearContents();
}

void FilesPage::recordChangedState()
{
    setModifyEnabled(d->rec->mode() != TQRecord::View);
}

void FilesPage::on_actionSaveFileAs_triggered()
{
    saveCurrentFiles();
}

void FilesPage::on_tabWidget_currentChanged(QWidget *arg1)
{
    if(arg1 == ui->tabPreview)
        startPreview();
    else
        stopPreview();
}

void FilesPage::on_filesTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if(ui->tabWidget->currentWidget() == ui->tabPreview)
    {
        if(!current)
            stopPreview();
        else if(!previous || current->row() != previous->row())
            startPreview();
    }
    else if(current && d->rec)
    {
        TQAttachedFile file = d->rec->fileList().value(current->row());
        ui->fileDescrEdit->setText(
                    tr("Файл: %1\nИзменен: %2")
                    .arg(file.fileName)
                    .arg(file.createDateTime.toString(d->def->dateTimeFormat())));
    }
    else
        ui->fileDescrEdit->clear();
}

void FilesPage::on_viewBtn_clicked()
{
    startPreview();
}

void FilesPage::on_addBtn_clicked()
{
//    emit addFileClicked();
    appendFiles();
}

void FilesPage::on_saveBtn_clicked()
{
    saveCurrentFiles();
}

void FilesPage::on_delBtn_clicked()
{
    deleteCurrentFiles();
}

void FilesPage::on_filesTable_doubleClicked(const QModelIndex &index)
{
    openCurrentFile();
}


// ======================== FileListWidget ===============================
FileListWidget::FileListWidget(QWidget *parent) :
    QListWidget(parent)
{
    setAcceptDrops(true);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setAlternatingRowColors(true);
}

void FileListWidget::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls() && e->source() != this)
        e->acceptProposedAction();
}


void FileListWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls() && event->source() != this) {
        QList<QUrl> urls = event->mimeData()->urls();
        foreach (QUrl url, urls)
            newCurrentItem = (new QListWidgetItem(url.toLocalFile(), this));
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void FileListWidget::setNewCurrentItem()
{
    setCurrentItem(newCurrentItem);
}

// new methods:

void FileListWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        startPos = e->pos();
    QListWidget::mousePressEvent(e);
}

void FileListWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton) {
        int distance = (e->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            startDrag();
    }
    QListWidget::mouseMoveEvent(e);
}

void FileListWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}


void FileListWidget::startDrag()
{
    QListWidgetItem *item = currentItem();
    if (item) {
        QFileInfo fi(item->text());
        QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
        QMimeData *mimeData = new QMimeData;
        mimeData->setUrls(QList<QUrl>() << url);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction);
    }
}

