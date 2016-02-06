#ifndef TQQRYWID_H
#define TQQRYWID_H

#include <QDialog>
#include <tqbase.h>
#include <tqcondapi_global.h>
#include <tqcond.h>

namespace Ui {
class TQQueryWidget;
}

class QListWidgetItem;

class TQCONDAPISHARED_EXPORT TQQueryWidget : public QDialog
{
    Q_OBJECT
protected:
    TQAbstractRecordTypeDef *recDef;
    TQQueryDef *queryDef;
    bool modified;
public:
    explicit TQQueryWidget(QWidget *parent = 0);
    ~TQQueryWidget();

    void setRecordTypeDef(TQAbstractRecordTypeDef *def);
    void setQueryDefinition(TQQueryDef *def);
    TQQueryDef *queryDefinition();
    int currentField();
    int currentCIndex();
    TQCond *currentCondition();
    void setCurrentCondition(TQCond *cond);
    QString queryName() const;
    void setQueryName(const QString &name);
private slots:
    void refreshCList();
    void refreshCondLine(TQCond *cond);
    void refreshControls();
    void on_lwCond_itemActivated(QListWidgetItem *item);

    void on_btnAdd_clicked();

    void on_rbAnd_clicked(bool checked);

    void on_rbOr_clicked();

    void on_lwCond_currentRowChanged(int currentRow);

    void on_btnRemove_clicked();

    void on_btnMoveUp_clicked();

    void on_btnMoveDown_clicked();

    void on_cbOpenBracket_clicked(bool checked);

    void on_cbNot_clicked(bool checked);

    void on_cbCloseBracket_clicked(bool checked);

    void on_btnModify_clicked();

    void on_buttonBox_accepted();

private:
    Ui::TQQueryWidget *ui;
};

#endif // TQQRYWID_H
