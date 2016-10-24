#ifndef JIRAQUERYDIALOG_H
#define JIRAQUERYDIALOG_H

#include <QDialog>
#include <tqqrywid.h>

namespace Ui {
class JiraQueryDialog;
}

class JiraQry;
class JiraProject;

class JiraQueryDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit JiraQueryDialog(QWidget *parent = 0);
    ~JiraQueryDialog();
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::JiraQueryDialog *ui;
    JiraQry *jQry;
    friend class JiraQueryDialogController;
};

class JiraQueryDialogController : public TQAbstractQWController
{
    Q_OBJECT
public:
    JiraQueryDialogController(JiraProject *project, int recType);
    ~JiraQueryDialogController();
    void setQueryDefinition(TQQueryDef *def);
    TQQueryDef *queryDefinition();
    int exec();
    QString queryName() const;
    void setQueryName(const QString &name);
private:
    JiraQueryDialog *dlg;
};

#endif // JIRAQUERYDIALOG_H
