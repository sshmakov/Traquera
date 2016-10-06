#ifndef TQAPPLICATION_H
#define TQAPPLICATION_H

#include <QApplication>



class TQApplication : public QApplication
{
    Q_OBJECT
public:
    TQApplication(int & argc, char ** argv);
    bool notify(QObject *obj, QEvent *ev);
};

#endif // TQAPPLICATION_H
