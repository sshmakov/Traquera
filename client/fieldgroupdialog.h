#ifndef FIELDGROUPDIALOG_H
#define FIELDGROUPDIALOG_H

#include <QDialog>

namespace Ui {
class FieldGroupDialog;
}

class FieldGroupDialogPrivate;

class FieldGroupDialog : public QDialog
{
    Q_OBJECT
protected:
    FieldGroupDialogPrivate *d;
public:
    explicit FieldGroupDialog(QWidget *parent = 0);
    ~FieldGroupDialog();
    void setFieldList(const QStringList &fields);
    void setGroupsDef(const QByteArray &buf);
    QByteArray groupsDef() const;
protected:
    void updateFieldsState();
    bool eventFilter(QObject *obj, QEvent *ev);

private slots:
    void on_btnAdd_clicked();

    void on_btnAddGroup_clicked();

    void on_btnUp_clicked();

    void on_btnDown_clicked();

    void on_btnDel_clicked();

    void on_cHideUsed_clicked();

private:
    Ui::FieldGroupDialog *ui;
};

#endif // FIELDGROUPDIALOG_H
