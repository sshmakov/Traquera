#include "messager.h"
#include <QMessageBox>

QAbstractMessageHandler *sysMessager = 0;

Messager::Messager(QObject *parent)
    : QAbstractMessageHandler(parent)
{
}

void Messager::handleMessage(QtMsgType type, const QString &description, const QUrl &identifier, const QSourceLocation &sourceLocation)
{
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
    QMessageBox::information(0, "info", text);
}
