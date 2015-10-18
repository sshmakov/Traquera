#ifndef TRKPLUGIN_H
#define TRKPLUGIN_H

#include <QObject>
#include "tracker_global.h"

class TRACKERSHARED_EXPORT TrkPlugin: public QObject
{
    Q_OBJECT
public:
    TrkPlugin();
    Q_INVOKABLE void initPlugin(QObject *obj, const QString &modulePath);
    Q_INVOKABLE bool saveSettings();
    Q_INVOKABLE bool loadSettings();
};

#endif // TRKPLUGIN_H
