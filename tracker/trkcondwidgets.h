#ifndef TRKCONDWIDGETS_H
#define TRKCONDWIDGETS_H

#include <QtGui>
#include "trkcond.h"

class TrkKeywordCondDialog : public QDialog
{
    Q_OBJECT
protected:
    TrkKeywordCond cond;
    QList<QLabel *> keyLabels;
    QList<QLineEdit *> keyEdits;
    QRadioButton *rbOr, *rbAnd;
    QCheckBox *cbCase, *cbTitle, *cbDesc, *cbNoteTitles, *cbNoteText, *cbNoteOnly;
    QComboBox *edOnlyTitle;
    QMutex mutex;
public:
    explicit TrkKeywordCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    void resetControls();
    TQCond &condition();
protected:
    QString andOr(bool value);
protected slots:
    void keyEditFinished();
    void rbAndOrChecked();
    void cbChecked(bool state);
    void onlyTitleEdited();

signals:
    
public slots:
    
};

namespace Ui {
class TrkChangeCondDialog;
class TrkDateCondDialog;
}

class TrkChangeCondDialog : public QDialog
{
    Q_OBJECT
protected:
    TrkChangeCond cond;
    QString recordChangeTypes, fieldChangeTypes, noteChangeTypes, moduleChangeTypes, fileChangeTypes;

    QList<QWidget *> dateEdits;
    QMutex mutex;
    int locks;
    QString dummyAnyField;
public:
    explicit TrkChangeCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
    QSize minimumSizeHint() const;
protected slots:
    void updateUI();
    void updateObjValues();
    void updateFieldValues();
    void rbObjectToggled(bool value);
    void btnAddNoteClicked();
    void fieldIndexSelected(int index);
    void grDateTimeToggled(bool value);
    void rbDaysToggled(bool value);
    void rbBetweenToggled(bool value);
    void sbDays1Changed(int days);
    void sbDays2Changed(int days);
    void deDate1Changed(QDateTime date);
    void deDate2Changed(QDateTime date);
    void valuesSelected();
    void grAuthorToggled(bool value);
    void coAuthorChanged(int index);
    void noteTitlesSelected();
private:
    void deleteDateEdits(QLayout *lay);
    void setEnableDateEdits(QLayout *lay, bool value);
    void setDateMode(TrkChangeCond::DateModeEnum mode);
    void setBetweenMode(TrkChangeCond::ChangeDateEnum mode);
protected:
    bool isInteractive();
    void lockChanges();
    void unlockChanges();
signals:
    void fieldChanges();

protected:
    Ui::TrkChangeCondDialog *ui;
};


class TrkDateCondDialog: public QDialog
{
    Q_OBJECT
protected:
    TQDateCond cond;
public:
    explicit TrkDateCondDialog(QWidget *parent = 0);
    void setCondition(const TQCond &condition);
    TQCond &condition();
protected slots:
    void sectionToggled(bool value);
    void rbToggled(bool value);
    void sbDaysChanged(int value);
    void dteChanged(const QDateTime &value);
protected:
    int locks;
    bool isInteractive();
    void lockChanges();
    void unlockChanges();
    void setDaysMode(bool value);
protected:
    Ui::TrkDateCondDialog *ui;
};

#endif // TRKCONDWIDGETS_H
