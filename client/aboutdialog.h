#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();
    QString regName() const;
    void setRegName(const QString &name);
    QString regKey() const;
    void setRegKey(const QString &key);
    void setText(const QString &text);
    void setEditEnabled(bool enable);
    int exec();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
