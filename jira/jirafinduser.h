#ifndef JIRAFINDUSER_H
#define JIRAFINDUSER_H

#include <QWidget>
#include <QDialog>
#include <QComboBox>

namespace Ui {
class JiraFindUser;
}

class JiraProject;
class JiraFindUserPrivate;

class JiraFindUser : public QDialog
{
    Q_OBJECT
    
public:
    explicit JiraFindUser(JiraProject *project, QWidget *parent = 0);
    ~JiraFindUser();
    QString text();
private slots:
    void refreshModel();
private:
    JiraFindUserPrivate *d;
    Ui::JiraFindUser *ui;
};

class JiraUserComboBoxPrivate;

class JiraUserComboBox: public QComboBox
{
    Q_OBJECT
private:
    JiraUserComboBoxPrivate *d;
public:
    JiraUserComboBox(JiraProject *project, QWidget *parent = 0);
    ~JiraUserComboBox();
private slots:
    void refreshModel();

};

#endif // JIRAFINDUSER_H
