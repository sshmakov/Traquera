#ifndef JIRAFINDUSER_H
#define JIRAFINDUSER_H

#include <QWidget>
#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <../tqgui/tqsearchbox.h>

namespace Ui {
class JiraFindUser;
}

class JiraProject;
class JiraRecTypeDef;
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

class JiraUserEditPrivate;

class JiraUserEdit: public TQSearchBox
{
    Q_OBJECT
    Q_PROPERTY(QVariant currentUser READ currentUser WRITE setCurrentUser USER true)
private:
    JiraUserEditPrivate *d;
public:
    JiraUserEdit(JiraProject *project, const JiraRecTypeDef *recordDef, QWidget *parent = 0);
    ~JiraUserEdit();
    void setCompleteLink(const QString &link);
    QVariant currentUser() const;
    void setCurrentUser(const QVariant &user);
    void setFilterText(const QString &text);
protected:
//    bool event(QEvent *event);
//    bool eventFilter(QObject *obj, QEvent *event);
//    void keyPressEvent(QKeyEvent *keyEvent);
//    void showEvent(QShowEvent * event );
//    void hideEvent(QHideEvent * event );
//    void showPopup();
//    void hidePopup();
    void popupItemSelected(const QModelIndex &index);
private slots:
    void refreshModel();
    void slotTextChanged(const QString &newText);
    void slotItemClicked(const QModelIndex &index);

};

#endif // JIRAFINDUSER_H
