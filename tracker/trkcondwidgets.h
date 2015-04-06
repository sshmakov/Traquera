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

#endif // TRKCONDWIDGETS_H
