#ifndef MODIFYPANEL_H
#define MODIFYPANEL_H

//#include "trkview.h"
//#include "trkdecorator.h"
#include "querypage.h"
#include <QWidget>
#include <QTableWidget>

namespace Ui {
class ModifyPanel;
}

class ModifyPanel : public QWidget
{
    Q_OBJECT

protected:
//    TrkToolModel *a_model;
    const TQAbstractRecordTypeDef *rdef;
    QueryPage *queryPage;

    QStringList fieldGroups;
    QList<QStringList> groupFields;
    struct ModifyRow
    {
        bool isGroup;
        bool isDifferent;
        bool isChanged;
        bool isEditable;
        int fieldType;
        QString fieldName;
        QString displayValue;
        QVariant fieldValue;
        QVariant newValue;
    };

    QList<ModifyRow> rows;
    bool a_readOnly;
    int curMode;
public:
    explicit ModifyPanel(QWidget *parent = 0);
    ~ModifyPanel();
    
//    void setQueryPage(QueryPage *page);
//    void setModel(TrkToolModel *newModel);
    void setRecordDef(const TQAbstractRecordTypeDef *typeDef, int mode);
    //void fillValues(TrkToolRecord *record);
    void fillValues(const QObjectList &records);
    int mode() const;
    QTableWidget *tableWidget();
    const ModifyRow &fieldRow(int row)
    {
        return rows[row];
    }
    const TQAbstractFieldType fieldDef(int row) const;
    const TQAbstractFieldType fieldDef(const QString &fieldName) const;
    void setRowValue(int row, const QVariant &value, int role = Qt::EditRole);
    QString fieldName(const QModelIndex &index);
    QVariant fieldValue(const QString &fieldName);
    QVariantHash changes();
    void setChanges(const QVariantHash &newChanges);
    void setButtonsVisible(bool visible);
signals:
    void applyButtonPressed();
    void repeatButtonClicked();
    void dataChanged();
protected:
    void fillTable();
public slots:
    void setFieldValue(const QString &fieldName, const QVariant &value);
    void resetField(const QString& fieldName);
    void resetAll();
    void clearField(const QString& fieldName);
private slots:
    void on_actionApplyChanges_triggered();

    void on_actionRevertChanges_triggered();

    void on_actionRepeatChanges_triggered();

    void on_actionRevertField_triggered();

    void on_actionClearField_triggered();

private:
    Ui::ModifyPanel *ui;

    friend class TTDelegate;
};

#endif // MODIFYPANEL_H
