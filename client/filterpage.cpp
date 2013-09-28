#include "filterpage.h"
#include "flowlayout.h"

#include <QXmlSimpleReader>
#include <QXmlInputSource>
#include <QDomDocument>
#include <QGroupBox>
#include <QToolButton>
#include <QComboBox>

FilterPage::FilterPage(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	loadDefinitions();
}

void FilterPage::loadDefinitions()
{
	QXmlSimpleReader xmlReader;
	QFile *file = new QFile("data/tracker.xml");
	QXmlInputSource *source = new QXmlInputSource(file);
	QDomDocument dom;
	if(!dom.setContent(source,false))
		return;
	QDomElement doc = dom.documentElement();
	if(doc.isNull()) return;
	QDomElement filters = doc.firstChildElement("filters");
	if(filters.isNull()) return;
	QVBoxLayout *vLay = new QVBoxLayout(this); //new FlowLayout(this);
	QDomElement filter = filters.firstChildElement("filter");
	for(; !filter.isNull(); filter = filter.nextSiblingElement("filter"))
	{
		QString title = filter.attribute("title");
		QGroupBox *groupBox = new QGroupBox(title, this);
        //QHBoxLayout *lay = new QHBoxLayout(groupBox);
		//
		bool isMulty = filter.attribute("type") == "multy";
		if(isMulty)
		{
			QLayout *lay = new FlowLayout(groupBox);
			for(QDomElement item = filter.firstChildElement("item"); 
				!item.isNull(); 
				item = item.nextSiblingElement("item"))
			{
				QToolButton *btn = new QToolButton(groupBox);
				btn->setText(item.attribute("title"));
				//btn->setPopupMode(QToolButton::MenuButtonPopup);
				lay->addWidget(btn);
			}
		}
		else
		{
			QLayout *lay = new FlowLayout(groupBox);
			QComboBox *cb = new QComboBox(groupBox);
			for(QDomElement item = filter.firstChildElement("item"); 
				!item.isNull(); 
				item = item.nextSiblingElement("item"))
			{
				cb->addItem(item.attribute("title"));
			}
			lay->addWidget(cb);
			//QToolButton *btn = new QToolButton(groupBox);
		}
		vLay->addWidget(groupBox);
	}
	vLay->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
	delete source;
	delete file;
}