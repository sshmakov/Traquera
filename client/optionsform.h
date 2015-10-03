#ifndef OPTIONSFORM_H
#define OPTIONSFORM_H

#include <QDialog>
#include <QtCore>
#include <ttglobal.h>

namespace Ui {
class OptionsForm;
}

class OptionsPage//: public QObject
{
    //Q_OBJECT
public:
    virtual QWidget *getWidget(const QString &optionsPath) = 0;
    virtual bool execCommand(int command) = 0;
};

class QTreeWidgetItem;

class OptionsForm : public QDialog
{
    Q_OBJECT
protected:
    QMap<QString, QTreeWidgetItem *> items;
    QWidget *curWidget;
    QTreeWidgetItem *curItem;
public:

    explicit OptionsForm(QWidget *parent = 0);
    ~OptionsForm();

    QTreeWidgetItem *getItem(const QString &path);
    QString getParentPath(const QString &path);
    QString getSectionPath(int option);
    bool registerWidget(const QString &path, GetOptionsWidgetFunc optionWidgetFunc);
    QWidget *optionsWidget(const QString &path);
    bool canCloseCurWidget(int r);

    void done(int r);
public slots:
    void accept();
    int exec();
private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    Ui::OptionsForm *ui;
};

#endif // OPTIONSFORM_H
