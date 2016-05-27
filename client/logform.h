#ifndef LOGFORM_H
#define LOGFORM_H

#include <QWidget>
#include <QTextDocument>
#include <QtGui>

namespace Ui {
class LogForm;
}

class LogForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit LogForm(QWidget *parent = 0);
    ~LogForm();

private:
    Ui::LogForm *ui;
};

class LogHandler: public QObject
{
    Q_OBJECT
private:
    QtMsgHandler oldHandler;
    bool isInstalled;
public:
    QTextDocument doc;
    QTextCursor cursor;
    LogHandler(QObject *parent = 0);
    ~LogHandler();
    static void installHandler();
};

#endif // LOGFORM_H
