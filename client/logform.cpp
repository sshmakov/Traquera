#include "logform.h"
#include "ui_logform.h"


static LogHandler handler;

LogForm::LogForm(QWidget *parent) :
    QWidget(parent),
    end(true),
    ui(new Ui::LogForm)
{
    ui->setupUi(this);
    ui->textEdit->setDocument(&handler.doc);
    ui->textEdit->moveCursor(QTextCursor::End);
    connect(ui->textEdit, SIGNAL(textChanged()), SLOT(contentChanged()));
    connect(ui->textEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), SLOT(scrollChanged()));
}

LogForm::~LogForm()
{
    delete ui;
}

void LogForm::contentChanged()
{
    if(isEndPos())
        ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->maximum());
//        ui->textEdit->moveCursor(QTextCursor::End);
}

void LogForm::scrollChanged()
{
    int pos  = ui->textEdit->verticalScrollBar()->value();
    end = pos >= ui->textEdit->verticalScrollBar()->maximum();
}

bool LogForm::isEndPos()
{
    return end;
//    int pos  = ui->textEdit->verticalScrollBar()->value();
//    return pos >= ui->textEdit->verticalScrollBar()->maximum();
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
#ifndef QT_DEBUG
    if(isInstalled)
        qInstallMsgHandler(oldHandler);
#endif
}

void LogHandler::installHandler()
{
    if(handler.isInstalled)
        return;
#ifndef QT_DEBUG
    handler.oldHandler = qInstallMsgHandler(messageOutput);
#endif
    handler.isInstalled = true;
    TQDebug::registerMsgWriter(stringOutput);
}

