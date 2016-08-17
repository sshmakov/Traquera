#ifndef QUERYFIELDS_H
#define QUERYFIELDS_H

#include <QWidget>
#include <tqbase.h>


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
    int mode;
public:
    explicit QueryFields(QWidget *parent = 0);
    ~QueryFields();
public slots:
    void applyChanges();
    void cancelChanges();
    void startChange();
    void selectedRecordsChanged();
    void setViewController(TQViewController *viewController, int viewMode = -1);
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void controllerDestroyed();
    void activatedField(const QString &fieldName);

private:
    Ui::QueryFields *ui;
};

#endif // QUERYFIELDS_H
