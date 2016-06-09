#ifndef QUERYFIELDS_H
#define QUERYFIELDS_H

#include <QWidget>

namespace Ui {
class QueryFields;
}

class ModifyPanel;
class TQViewController;
class QAbstractButton;

class QueryFields : public QWidget
{
    Q_OBJECT
    
private:
    ModifyPanel *modifyPanel;
    TQViewController *controller;
public:
    explicit QueryFields(QWidget *parent = 0);
    ~QueryFields();
    void setViewController(TQViewController *viewController, int mode);
public slots:
    void applyChanges();
    void selectedRecordsChanged();
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void controllerDestroyed();

private:
    Ui::QueryFields *ui;
};

#endif // QUERYFIELDS_H
