#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include <QDialog>
#include "ui_notedialog.h"

namespace Ui {
class NoteDialog;
}

class NoteDialog : public QDialog, public Ui::NoteDialog
{
    Q_OBJECT
    Q_PROPERTY(bool noteTitleVisible WRITE setNoteTitleVisible)
public:
    explicit NoteDialog(QWidget *parent = 0);
    ~NoteDialog();
    void setNoteTitleVisible(bool value);


private slots:
    void on_actionAddNote_triggered();
};

#endif // NOTEDIALOG_H
