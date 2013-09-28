#ifndef _FILTERPAGE_H_
#define _FILTERPAGE_H_

#include "ui_filters.h"

class FilterPage: public QWidget, public Ui_FilterWidget
{
	Q_OBJECT
public:
	FilterPage(QWidget *parent = 0);
	void loadDefinitions();
};

#endif