#include "uniondevice.h"

class UnionDevicePrivate
{
public:
    QList<QIODevice *> childs;
    int current;
    UnionDevicePrivate()
        : current(0)
    {

    }
};

UnionDevice::UnionDevice(QObject *parent)
    : QIODevice(parent), d(new UnionDevicePrivate())
{

}

UnionDevice::~UnionDevice()
{
    delete d;
}

void UnionDevice::appendDevice(QIODevice *device)
{
    d->childs.append(device);
}

void UnionDevice::appendDevice(QList<QIODevice *> list)
{
    d->childs.append(list);
}

qint64 UnionDevice::readData(char *data, qint64 maxSize)
{
    if(d->current < 0 || d->current >= d->childs.size())
        return -1;
    qint64 res = 0;
    char *buf = data;
    while(maxSize>0 && d->current < d->childs.size())
    {
        QIODevice *dev = d->childs.value(d->current);
        qint64 readed = 0;
        while(maxSize>0)
        {
            readed = dev->read(buf, maxSize);
            if(readed <= 0)
            {
                d->current++;
                break;
            }
            res += readed;
            maxSize -= readed;
            buf += readed;
        }
    }
    return res;
}

qint64 UnionDevice::writeData(const char *data, qint64 maxSize)
{
    Q_UNUSED(data)
    Q_UNUSED(maxSize)
    return -1;
}

