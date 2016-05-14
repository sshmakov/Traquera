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

class TQCONDAPISHARED_EXPORT TQAbstractQWController : public QObject
{
public:
    TQAbstractQWController(QObject *parent = 0);
    virtual void setQueryDefinition(TQQueryDef *def) = 0;
    virtual TQQueryDef *queryDefinition() = 0;
    virtual int exec() = 0;
    virtual QString queryName() const = 0;
    virtual void setQueryName(const QString &name) = 0;
};


class TQQueryWidgetControllerPrivate;
class TQCONDAPISHARED_EXPORT TQQueryWidgetController : public TQAbstractQWController
{
    Q_OBJECT
public:
    TQQueryWidgetController(QObject *parent = 0);
    ~TQQueryWidgetController();
    void setQueryDefinition(TQQueryDef *def);
    TQQueryDef *queryDefinition();
    int exec();
    QString queryName() const;
    void setQueryName(const QString &name);
private:
    TQQueryWidgetControllerPrivate *d;
};

#endif // TQQRYWID_H
