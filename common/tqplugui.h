#ifndef TQPLUGUI_H
#define TQPLUGUI_H

#include <QtCore>

class QWidget;
class QString;

typedef QWidget *(*GetOptionsWidgetFunc)(const QString &optionsPath);

//Q_DECLARE_METATYPE(GetOptionsWidgetFunc)

enum TQOptionsEnum {
    OptionGeneral = 1
};


#endif // TQPLUGUI_H
