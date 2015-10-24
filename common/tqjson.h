#ifndef TQJSON_H
#define TQJSON_H

#include <QObject>
#include <QVariant>
#include <tqplugin_global.h>

class TQJsonPrivate;

class TQPLUGIN_SHARED TQJson : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString charset READ charset WRITE setCharset)
private:
    TQJsonPrivate *d;
public:
    explicit TQJson(QObject *parent = 0);
    ~TQJson();
    Q_INVOKABLE QString charset() const;
    Q_INVOKABLE void setCharset(const QString &charset);
    Q_INVOKABLE void reset();
    Q_INVOKABLE QString toString(const QVariant &value);
    Q_INVOKABLE QByteArray toByteArray(const QVariant &value);
    Q_INVOKABLE QVariant toVariant(const QString &string);
    Q_INVOKABLE QVariant toVariant(const QByteArray &buf);
    Q_INVOKABLE bool isError() const;
    Q_INVOKABLE QString errorString() const;
signals:
    
public slots:
    
};



#endif // TQJSON_H
