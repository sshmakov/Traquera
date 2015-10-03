#ifndef JIRAOPTIONS_H
#define JIRAOPTIONS_H

#include <QWidget>

namespace Ui {
class JiraOptions;
}

class JiraOptions : public QWidget
{
    Q_OBJECT
private:
    bool isModified;
public:
    explicit JiraOptions(QWidget *parent = 0);
    ~JiraOptions();
    
    bool event(QEvent *e);
    void saveChanges();
private slots:
    void on_leServer_textEdited();

private:
    Ui::JiraOptions *ui;
};

#endif // JIRAOPTIONS_H
