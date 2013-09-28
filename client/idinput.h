#ifndef IDINPUT_H
#define IDINPUT_H

#include <QDialog>

namespace Ui {
class IdInput;
}

class IdInput : public QDialog
{
    Q_OBJECT
    
public:
    explicit IdInput(QWidget *parent = 0);
    ~IdInput();
    void setData(const QString &text);
    QString getData();
private:
    Ui::IdInput *ui;
};

#endif // IDINPUT_H
