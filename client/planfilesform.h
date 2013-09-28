#ifndef PLANFILESFORM_H
#define PLANFILESFORM_H

#include <QWidget>

namespace Ui {
class PlanFilesForm;
}

class PlanFilesModel;

class PlanFilesForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit PlanFilesForm(QWidget *parent = 0);
    ~PlanFilesForm();

    static int execute(QWidget *parent, PlanFilesModel *model);
    void setModel(PlanFilesModel *model);
protected:
    PlanFilesModel *projects;
signals:
    void openMSP_clicked();
    void showPlan_clicked(const QString &fileName);
private slots:
    void on_projectAddBtn_clicked();

    void on_projectOpenBtn_clicked();

    void on_projectDelBtn_clicked();

    void on_projectLoadBtn_clicked();

    void on_projectShowBtn_clicked();

private:
    Ui::PlanFilesForm *ui;
};

#endif // PLANFILESFORM_H
