#ifndef UNIONDEVICE_H
#define UNIONDEVICE_H

#include <QIODevice>

class UnionDevicePrivate;
class UnionDevice : public QIODevice
{
    Q_OBJECT
private:
    UnionDevicePrivate *d;
public:
    UnionDevice(QObject *parent);
    ~UnionDevice();
    void appendDevice(QIODevice *device);
    void appendDevice(QList<QIODevice *>list);
protected:
    qint64	readData(char * data, qint64 maxSize);
    qint64	writeData(const char * data, qint64 maxSize);
};

#endif // UNIONDEVICE_H
