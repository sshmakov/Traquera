#ifndef FILESPAGE_H
#define FILESPAGE_H

#include <QWidget>

namespace Ui {
class FilesPage;
}

class Preview;
class TQRecord;
class QModelIndex;
class FilesPagePrivate;
class QTableWidgetItem;
class FilesPage : public QWidget
{
    Q_OBJECT
private:
    FilesPagePrivate *d;
    Preview *previewWidget;
public:
    explicit FilesPage(QWidget *parent = 0);
    ~FilesPage();
    void setModifyEnabled(bool value);
    bool isModifyEnabled() const;

public slots:
    void setRecord(TQRecord *record);
    void previewCurrentFile();
    void openCurrentFile();
    void saveCurrentFiles();
    void appendFiles();
    void startPreview();
    void stopPreview();

private slots:
    void on_actionSaveFileAs_triggered();
    void on_viewBtn_clicked();

    void on_tabWidget_currentChanged(QWidget *arg1);

    void on_filesTable_entered(const QModelIndex &index);

    void on_filesTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

private:
    Ui::FilesPage *ui;
};

#endif // FILESPAGE_H
