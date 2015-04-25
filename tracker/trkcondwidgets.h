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
    void fieldSelected(const QString &field);
    void rbDaysToggled(bool value);
    void rbBetweenToggled(bool value);
private:
    void deleteDateEdits(QLayout *lay);
    void setDateMode(TrkChangeCond::DateModeEnum mode);
protected:
    bool isInteractive();
    void lockChanges();
    void unlockChanges();
signals:
    void fieldChanges();

protected:
    Ui::TrkChangeCondDialog *ui;
};

#endif // TRKCONDWIDGETS_H
