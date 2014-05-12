#ifndef TQPLUGINWIDGET_H
#define TQPLUGINWIDGET_H

#include <QWidget>

class TQPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TQPluginWidget(QWidget *parent = 0);
    void initWidgets();
signals:
    
public slots:
    
};

#endif // TQPLUGINWIDGET_H
