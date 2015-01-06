#ifndef TQCOLSDIALOG_H
#define TQCOLSDIALOG_H

#include <QDialog>

class QHeaderView;
class QAbstractItemView;
class QListWidgetItem;

namespace Ui {
class TQColsDialog;
}

class TQColsDialog : public QDialog
{
    Q_OBJECT
public:
    struct Col {
    public:
        QString colName;
        int pos;
        int logicalIndex;
        int width;
        bool isHidden;
    };

    typedef QMap<int, TQColsDialog::Col> ColList;


    explicit TQColsDialog(QWidget *parent = 0);
    ~TQColsDialog();

    int exec(QHeaderView *hv);
//    ColList result() const;
    
protected:

private slots:
    void on_btnAdd_clicked();
    void on_btnDel_clicked();
    void on_btnAllAdd_clicked();
    void on_btnAllDel_clicked();
    void on_btnMoveUp_clicked();
    void on_btnMoveDown_clicked();
    void on_listShow_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_editWidth_editingFinished();

private:
    Ui::TQColsDialog *ui;
};

#endif // TQCOLSDIALOG_H
