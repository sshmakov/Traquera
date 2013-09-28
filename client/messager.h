#ifndef MESSAGER_H
#define MESSAGER_H

#include <QAbstractMessageHandler>

class Messager : public QAbstractMessageHandler
{
public:
    Messager(QObject * parent =0);
protected:
    virtual void	handleMessage ( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation );
};

extern QAbstractMessageHandler *sysMessager;

#endif // MESSAGER_H
