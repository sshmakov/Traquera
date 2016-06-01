#ifndef FILESPAGE_H
#define FILESPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QtGui>

namespace Ui {
class FilesPage;
}

class Preview;
class TQRecord;
class QModelIndex;
class FilesPagePrivate;
class QTableWidgetItem;
class FileListWidget;

class FilesPage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title NOTIFY onTitleChange)
private:
    FilesPagePrivate *d;
    Preview *previewWidget;
public:
    explicit FilesPage(QWidget *parent = 0);
    ~FilesPage();
    void setModifyEnabled(bool value);
    bool isModifyEnabled() const;
    QList<int> currentRows() const;
    QString title() const;
protected:
//    void dropEvent(QDropEvent *e);

signals:
    void onTitleChange();

public slots:
    void setRecord(TQRecord *record);
    void refreshFiles();
    void refreshFiles2();
    void previewCurrentFile();
    void openCurrentFile();
    void saveCurrentFiles();
    void appendFiles();
    void deleteCurrentFiles();
    void startPreview();
    void stopPreview();
    void detachRecord();
protected slots:
    void recordChangedState();

private slots:
    void on_actionSaveFileAs_triggered();
    void on_viewBtn_clicked();

    void on_tabWidget_currentChanged(QWidget *arg1);

    void on_filesTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_addBtn_clicked();

    void on_saveBtn_clicked();

    void on_delBtn_clicked();

    void on_filesTable_doubleClicked(const QModelIndex &index);

private:
    Ui::FilesPage *ui;
};

class FileListWidget : public QListWidget {
    Q_OBJECT

public:
    FileListWidget(QWidget *parent = 0);

    void setNewCurrentItem();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *event);
    // new methods:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent*);
    void dragMoveEvent(QDragMoveEvent*);

private:
    QListWidgetItem *newCurrentItem;
    // new members:
    QPoint startPos;
    void startDrag();
};

#endif // FILESPAGE_H
