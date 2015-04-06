#ifndef TQCONDWIDGETS_H
#define TQCONDWIDGETS_H

#include <QtGui>
#include "tqcond.h"

class TQNumberCond;


class TQAbstractCondDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TQAbstractCondDialog(QWidget *parent = 0);
    virtual void setCondition(const TQCond &condition) = 0;
    virtual TQCond &condition() = 0;
};

class TQNumberCondDialog : public TQAbstractCondDialog
{
    Q_OBJECT
protected:
    TQNumberCond cond;
    QLabel *fLabel;
    QPushButton *opBtn;
    QMenu *opMenu;
    QVBoxLayout *vLay;
    QHBoxLayout *lay;
    QLineEdit *numEdit1;
    QLineEdit *numEdit2;
    QLabel *andLabel;
    QStringList opTexts;
    QDialogButtonBox *box;
public:
    explicit TQNumberCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
    
signals:
    
public slots:

private slots:
    void opBtnMenu(QAction *action);
    void valueChanged();


};

class TQChoiceCondDialog : public TQAbstractCondDialog
{
    Q_OBJECT
protected:
    TQChoiceCond cond;

    QGroupBox *box;
    QRadioButton *rbNull, *rbAny, *rbSelected;
    QListWidget *vList;
    QMutex mutex;
public:
    explicit TQChoiceCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
protected slots:
    void opChecked(int op);
    void selectionChanged();
};

class TQUserCondDialog : public TQAbstractCondDialog
{
    Q_OBJECT
protected:
    TQUserCond cond;

    QGroupBox *box;
    QRadioButton *rbActive, *rbDeleted, *rbAny;
    QListWidget *vList;
    QMutex mutex;
public:
    explicit TQUserCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
protected slots:
    void stateChecked(int state);
    void selectionChanged();
};

class TQStringCondDialog : public TQAbstractCondDialog
{
    Q_OBJECT
protected:
    TQStringCond cond;

    QLabel *label;
    QLineEdit *edit;
    QCheckBox *caseSensitive;
    QMutex mutex;
public:
    explicit TQStringCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
protected slots:
    void editFinished();
    void caseChanged();
};

#endif // TQCONDWIDGETS_H
