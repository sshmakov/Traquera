#include "planfilesform.h"
#include "ui_planfilesform.h"
#include "planfiles.h"
#include <QDialog>
#include "plans.h"
#include <QFileDialog>

PlanFilesForm::PlanFilesForm(/*PlanPlugin *plugObject, */QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlanFilesForm)
{
    ui->setupUi(this);
    ui->projectTableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

PlanFilesForm::~PlanFilesForm()
{
    delete ui;
}

int PlanFilesForm::execute(QWidget *parent, PlanFilesModel *model)
{
    QDialog dlg(parent);
    dlg.resize(600,200);
    PlanFilesForm form(parent);
    form.setModel(model);
    QHBoxLayout lay;
    dlg.setLayout(&lay);
    lay.addWidget(&form);
    return dlg.exec();
}

void PlanFilesForm::setModel(PlanFilesModel *model)
{
    projects = model;
    ui->projectTableView->setModel(projects);
}

QItemSelectionModel * PlanFilesForm::getSelectionModel()
{
    return ui->projectTableView->selectionModel();
}

void PlanFilesForm::on_projectAddBtn_clicked()
{
    emit addBtn_clicked();
    /*
    QString selectedFilter;
    QStringList fileNames= QFileDialog::getOpenFileNames(this,
        tr("Добавить план"),
        ".",
        tr(
            "Проекты Microsoft Project (*.mpp)"
            //  ";;Книги Microsoft Excel (*.xls)"
        ),
        &selectedFilter
        );
    for(int f=0; f<fileNames.count(); f++)
        projects->addPlan(fileNames[f],false);
    */
}

void PlanFilesForm::on_projectOpenBtn_clicked()
{
    emit openMSP_clicked();
    /*
    PrjItemModel *model = new PrjItemModel(this);
    if(model->open("",false))
    {
        projects->addPlan(model->fileName);
        planModel.addPrjModel(model);
        projects->models[model->fileName]=model;
    }
    else
        delete model;
    */
}

void PlanFilesForm::on_projectDelBtn_clicked()
{
    emit delBtn_clicked();
    /*
    QItemSelectionModel *is=ui->projectTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=ii.count()-1; i>=0; i--)
    {
        //QString file = ii[i].data().toString();
        projects->removeRow(ii[i].row());
    }
    ui->projectTableView->clearSelection();
    //saveSettings();
    */
}

void PlanFilesForm::on_projectLoadBtn_clicked()
{
    emit loadBtn_clicked();
    /*
    QItemSelectionModel *is=ui->projectTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=0; i<ii.count(); i++)
        projects->loadPlan(ii[i].row());
    */
}

void PlanFilesForm::on_projectShowBtn_clicked()
{
    emit showBtn_clicked();
    /*
    QItemSelectionModel *is=ui->projectTableView->selectionModel();
    QModelIndexList ii = is->selectedRows();
    for(int i=0; i<ii.count(); i++)
        emit showPlan_clicked(projects->plans[ii[i].row()].file);
    */
}
