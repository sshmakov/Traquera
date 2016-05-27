#include "logform.h"
#include "ui_logform.h"


static LogHandler handler;

LogForm::LogForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogForm)
{
    ui->setupUi(this);
    ui->textEdit->setDocument(&handler.doc);
}

LogForm::~LogForm()
{
    delete ui;
}


static void messageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        handler.cursor.insertText(QString("Debug: %1\n").arg(msg));
        break;
    case QtWarningMsg:
        handler.cursor.insertText(QString("Warning: %1\n").arg(msg));
        break;
    case QtCriticalMsg:
        handler.cursor.insertText(QString("Critical: %1\n").arg(msg));
        break;
    case QtFatalMsg:
        handler.cursor.insertText(QString("Fatal: %1\n").arg(msg));
        abort();
    }
}

LogHandler::LogHandler(QObject *parent)
    : QObject(parent), doc(), cursor(&doc), isInstalled(false)
{
}

LogHandler::~LogHandler()
{
    if(isInstalled)
        qInstallMsgHandler(oldHandler);
}

void LogHandler::installHandler()
{
    if(handler.isInstalled)
        return;
    handler.oldHandler = qInstallMsgHandler(messageOutput);
    handler.isInstalled = true;
}
