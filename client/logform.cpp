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
        if(TQDebug::verboseLevel() >= TQDebugLevel)
            handler.cursor.insertText(QString("Debug: %1\n").arg(msg));
        break;
    case QtWarningMsg:
        if(TQDebug::verboseLevel() >= TQWarningLevel)
            handler.cursor.insertText(QString("Warning: %1\n").arg(msg));
        break;
    case QtCriticalMsg:
        if(TQDebug::verboseLevel() >= TQCriticalLevel)
            handler.cursor.insertText(QString("Critical: %1\n").arg(msg));
        break;
    case QtFatalMsg:
        if(TQDebug::verboseLevel() >= TQFatalLevel)
            handler.cursor.insertText(QString("Fatal: %1\n").arg(msg));
        abort();
    default:
        handler.cursor.insertText(QString::fromLocal8Bit(msg) + "\n");
    }
}

static void stringOutput(int level, const QString &string)
{
    handler.cursor.insertText(string);
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
    TQDebug::registerMsgWriter(stringOutput);
}

