#include "messager.h"
#include <QMessageBox>
#include <tqdebug.h>

QAbstractMessageHandler *sysMessager = 0;

Messager::Messager(QObject *parent)
    : QAbstractMessageHandler(parent)
{
}

void Messager::handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation)
{
    Q_UNUSED(identifier)
    Q_UNUSED(type)

    QString text;
    text += description;
    /*
    if(identifier.isValid())
    {
        text += '\n';
        text += identifier.toString();
    }
    */
    if(!sourceLocation.isNull())
    {
        text += '\n';
        text += QString("%1 (%2/%3)").arg(sourceLocation.uri().toString()).arg(sourceLocation.line()).arg(sourceLocation.column());
    }
    switch (type) {
    case QtDebugMsg:
        tqDebug() << text;
        break;
    case QtWarningMsg:
        tqWarning() << text;
        break;
    case QtCriticalMsg:
        tqCritical() << text;
        break;
    case QtFatalMsg:
        tqFatal() << text;
        break;
    default:
        tqInfo() << text;
        break;
    }
    QMessageBox::information(0, "info", text);
}
