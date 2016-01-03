#include "jiraoptions.h"
#include "ui_jiraoptions.h"
#include "jiradb.h"
#include <QMessageBox>
#include <QFileDialog>

JiraOptions::JiraOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JiraOptions)
{
    ui->setupUi(this);
    plugin = JiraPlugin::plugin();
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->leServer->setText(plugin->servers.join("; "));
    ui->leKeyFile->setText(plugin->keyFile);
    ui->leKeyPassword->setText(plugin->keyPass);
    isModified = false;
}

JiraOptions::~JiraOptions()
{
    delete ui;
}

bool JiraOptions::event(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::OkRequest:
        if(isModified)
            if(e->isAccepted())
            {
                saveChanges();
                return true;
            }
            else
            {
                int button = QMessageBox::information(this, tr("?"), tr("Save changes"),
                                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                            QMessageBox::Yes);
                switch(button)
                {
                case QMessageBox::No:
                    e->accept();
                    return true;
                case QMessageBox::Yes:
                    e->accept();
                    saveChanges();
                    return true;
                case QMessageBox::Cancel:
                    e->ignore();
                    return true;
                }
            }
        e->accept();
        return true;
        /*
    case QEvent::Hide:
        if(isModified)
        {
            int button = QMessageBox::information(this, tr("?"), tr("Сохранить изменения"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                                        QMessageBox::Yes);
            switch(button)
            {
            case QMessageBox::No:
                isModified = false;
                break;
            case QMessageBox::Yes:
                saveChanges();
                break;
            case QMessageBox::Cancel:
                e->ignore();
                return true;
            }
        }
        */
    }
    return QWidget::event(e);
}

void JiraOptions::saveChanges()
{
    QString server = ui->leServer->text();
    plugin->servers = server.split(QRegExp("\\s*;\\s*"));
    plugin->keyFile = ui->leKeyFile->text();
    plugin->keyPass = ui->leKeyPassword->text();
    plugin->saveSettings();
    isModified = false;
}

void JiraOptions::on_leServer_textEdited()
{
    isModified = true;
}

void JiraOptions::on_tbKeyFileBrowse_clicked()
{
    QString keyFile = QFileDialog::getOpenFileName(this, tr("Выберите файл ключа"),
                                 QString(),
                                 tr("PEM файлы (*.pem);;Все файлы (*.*)"));
    if(!keyFile.isEmpty())
    {
        ui->leKeyFile->setText(keyFile);
        isModified = true;
    }
}

void JiraOptions::on_leKeyFile_textChanged(const QString &arg1)
{
    isModified = true;
}

void JiraOptions::on_leKeyPassword_textChanged(const QString &arg1)
{
    isModified = true;
}
