#ifndef PREVIEWFACTORY_H
#define PREVIEWFACTORY_H

#include <QtCore>
#include "preview.h"

class PreviewFactory : public QObject
{
    Q_OBJECT
    QHash<QString, QString> typeByExt;
public:
    explicit PreviewFactory(QObject *parent = 0);
    Preview *createPreview(const QString &file, QWidget *parent = 0);
signals:
    
public slots:
    
};

#endif // PREVIEWFACTORY_H
