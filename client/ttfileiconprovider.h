#ifndef TTFILEICONPROVIDER_H
#define TTFILEICONPROVIDER_H

#include <QFileIconProvider>

class TTFileIconProvider : public QFileIconProvider
{
public:
    TTFileIconProvider();
    virtual QIcon icon(const QFileInfo &info) const;
protected:
#ifdef Q_WS_WIN
    QIcon getWinIcon(const QFileInfo &fileInfo) const;
#endif
};

#endif // TTFILEICONPROVIDER_H
