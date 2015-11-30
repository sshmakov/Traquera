#ifndef TQPRJOPTDLG_H
#define TQPRJOPTDLG_H

#include <QDialog>

namespace Ui {
class TQProjectOptionsDialog;
}

class TQProjectOptionsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit TQProjectOptionsDialog(QWidget *parent = 0);
    ~TQProjectOptionsDialog();
    
private:
    Ui::TQProjectOptionsDialog *ui;
};

#endif // TQPRJOPTDLG_H
