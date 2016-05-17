#ifndef TQPRJOPTDLG_H
#define TQPRJOPTDLG_H

#include <QDialog>

namespace Ui {
class TQProjectOptionsDialog;
}

class TQAbstractProject;
class TQProjectOptionsDialogPrivate;

class TQProjectOptionsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TQProjectOptionsDialog(QWidget *parent = 0);
    ~TQProjectOptionsDialog();
    void setProject(TQAbstractProject *prj);
    int exec();
private slots:
    void browseClicked();

private:
    TQProjectOptionsDialogPrivate *d;

private:
    Ui::TQProjectOptionsDialog *ui;
};

#endif // TQPRJOPTDLG_H
