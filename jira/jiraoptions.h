#ifndef JIRAOPTIONS_H
#define JIRAOPTIONS_H

#include <QWidget>

namespace Ui {
class JiraOptions;
}

class JiraPlugin;
class JiraOptions : public QWidget
{
    Q_OBJECT
private:
    bool isModified;
    JiraPlugin *plugin;
public:
    explicit JiraOptions(QWidget *parent = 0);
    ~JiraOptions();
    
    bool event(QEvent *e);
    void saveChanges();
private slots:
    void on_leServer_textEdited();

    void on_tbKeyFileBrowse_clicked();

    void on_leKeyFile_textChanged(const QString &arg1);

    void on_leKeyPassword_textChanged(const QString &arg1);

private:
    Ui::JiraOptions *ui;
};

#endif // JIRAOPTIONS_H
