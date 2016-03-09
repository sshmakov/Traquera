#include "filespage.h"
#include "ui_filespage.h"
#include "preview.h"
#include <tqbase.h>
#include "ttfileiconprovider.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

class FilesPagePrivate
{
public:
    TQRecord *rec;
    TQAbstractRecordTypeDef *def;
    QTimer previewTimer;
    int curFileIndex;
    bool modifyEnabled;
    QString title;
    FilesPagePrivate()
        : rec(0), def(0), modifyEnabled(false), curFileIndex(-1)
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

    ui->filesTable->horizontalHeader()->setResizeMode(0,QHeaderView::ResizeToContents);
    ui->filesTable->horizontalHeader()->setResizeMode(1,QHeaderView::ResizeToContents);

//    QVBoxLayout * v = new QVBoxLayout(ui->previewFrame);
//    v->setContentsMargins(0, 0, 0, 0);
//    v->setObjectName(QString::fromUtf8("verticalLayout"));

    previewWidget = new MasterPreview(this);
    previewWidget->setObjectName("previewWidget");
    ui->previewFrame->layout()->addWidget(previewWidget);
//    ui->prevewLayout->addWidget(previewWidget);
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

void FilesPage::setRecord(TQRecord *record)
{
    ui->filesTable->clearContents();
    ui->filesTable->setRowCount(0);
    d->def = 0;
    d->rec = record;
    if(d->rec)
    {
        d->def = d->rec->recordDef();
        QList<TQToolFile> files = d->rec->fileList();
        TTFileIconProvider prov;
        int row=0;
        foreach(const TQToolFile &file, files)
        {
            QFileInfo f(file.fileName);
            ui->filesTable->insertRow(row);
            ui->filesTable->setItem(row,FP_NAME,new QTableWidgetItem(prov.icon(f),f.fileName()));
            ui->filesTable->setItem(row,FP_CHANGE,new QTableWidgetItem(file.createDateTime.toString(d->def->dateTimeFormat())));
            ui->filesTable->setItem(row,FP_ORIGINALPATH,new QTableWidgetItem(f.dir().path()));
            row++;
        }
    }
    emit onTitleChange();
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
    if(!d->rec || !d->modifyEnabled)
        return;
    QStringList list = QFileDialog::getOpenFileNames(this);
    if(list.isEmpty())
        return;
    foreach(QString filePath, list)
    {
        if(!d->rec->appendFile(filePath))
            break;
    }
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

void FilesPage::on_filesTable_entered(const QModelIndex &index)
{
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
        TQToolFile file = d->rec->fileList().value(current->row());
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
    appendFiles();
}

void FilesPage::on_saveBtn_clicked()
{
    saveCurrentFiles();
}

void FilesPage::on_delBtn_clicked()
{

}

