#include <QStringList>
#include <QLineEdit>
#include <QComboBox>
#include <QDomElement>
#include <QXmlSimpleReader>
#include <QTabWidget>
#include <QGridLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QScrollArea>
#include "trkview.h"
#include "trkdecorator.h"
#include "settings.h"

TrkDecorator *decorator;

TrkDecorator::TrkDecorator(QObject *parent) :
    QObject(parent)
{
}

void TrkDecorator::fillEditPanels(QTabWidget *tabs, const RecordTypeDef * recDef, EditDefList &def, bool onlyView)
{
    //QList<EditDef> edits;
    //const QStringList & fieldList = record->fields();

    if(def.isFilled && def.recType == recDef->recordType())
        return;
    def.onlyView = onlyView;
    clearEdits(tabs, def);
    QXmlSimpleReader xmlReader;
    QFile *file = new QFile("data/tracker.xml");
    QXmlInputSource *source = new QXmlInputSource(file);
    QDomDocument dom;
    if(!dom.setContent(source,false))
        return ;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return ;

    QStringList fieldList = recDef->fieldNames();


    QDomElement panels = doc.firstChildElement("panels");
    if(!panels.isNull())
    {
        QStringList used;
        for(QDomElement panel = panels.firstChildElement("panel");
            !panel.isNull();
            panel = panel.nextSiblingElement("panel"))
        {
            QList<EditDef> ed;
            if(panel.attribute("type") != "other")
            {
                for(QDomElement field = panel.firstChildElement("field");
                    !field.isNull();
                    field = field.nextSiblingElement("field"))
                {
                    QString fname = field.attribute("name");
                    if(!fieldList.contains(fname,Qt::CaseInsensitive))
                        continue;
                    TrkFieldDef fdef = recDef->getFieldDef(fname);
                    EditDef f;
                    QWidget *w;
                    //QLineEdit *ie = 0;
                    if(!onlyView && fdef.isChoice())
                    {
                        TrkChoiceBox *c = new TrkChoiceBox(tabs, fname);
                        //c->setInsertPolicy(QComboBox::NoInsert);
                        //ie = new QLineEdit();
                        //c->setLineEdit(ie);
                        //c->setEditable(false);
                        TrkToolChoice choice;
                        foreach(choice,fdef.choiceList())
                        {
                            c->addItem(choice.displayText, choice.fieldValue /*  QVariant((int)(choice.order)) */);
                        }
                        w = c;
                    }
                    else
                        w = new TrkStringEdit(tabs, fname);
                    f.edit = w;
                    //f.internalEditor = ie;
                    f.fieldName = fname;
                    f.title = fname;
                    ed.append(f);
                    //f.fieldcol = fieldList.indexOf(f.title);
                }
            }
            else
            {
                for(int i=0; i<fieldList.count(); i++)
                {
                    QString fname = fieldList[i].trimmed();
                    if(!used.contains(fname,Qt::CaseInsensitive))
                    {
                        EditDef f;
                        f.edit = new QLineEdit();
                        f.title = fname;
                        //f.fieldcol = i;
                        ed.append(f);
                    }
                }
            }
            if(ed.count())
            {
                QString s_cols = panel.attribute("cols");
                bool ok;
                int cols = s_cols.toUInt(&ok);
                if(!ok || !cols)
                    cols = 1;
                int rows = ed.count() / cols;
                if(ed.count() % cols)
                    rows++;
                QWidget *inp = new QWidget();
                inp->setGeometry(QRect(0, 0, 295, 200));
                inp->setAutoFillBackground(true);
                QGridLayout *grid=new QGridLayout(inp);
                QFormLayout *lay = new QFormLayout();
                int gridcol=0;
                for(int fi=0; fi<ed.count(); fi++)
                {
                    if(fi && !(fi % rows))
                    {
                        grid->addLayout(lay,0,gridcol++,1,1);
                        lay = new QFormLayout();
                    }
                    lay->addRow(ed[fi].title,ed[fi].edit);
                    def.edits.append(ed[fi]);
                    used.append(ed[fi].title);
                }
                grid->addLayout(lay,0,gridcol);
                def.panels.append(inp);
                tabs->addTab(inp, panel.attribute("title"));
            }
        }
    }
    delete source;
    delete file;
    //return edits;
}

void TrkDecorator::readValues(TrkToolRecord *record, EditDefList &def)
{
    def.connectToRecord(record);
    def.readValues();
    /*
    //descEdit->setPlainText(record->description());
    QList<EditDef>::const_iterator ei;
    for(ei=def.edits.constBegin(); ei!=def.edits.constEnd(); ei++)
    {
        QLineEdit *ed = qobject_cast<QLineEdit *>(ei->edit);
        if(ed)
            ed->setText(record->value(ei->title).toString());
    }
    updateState(record, def);
    */
}

void TrkDecorator::updateState(TrkToolRecord *record, EditDefList &def)
{
    def.connectToRecord(record);
    def.updateState();
    /*
    if(def.onlyView)
        return;
    QList<EditDef>::const_iterator ei;
    for(ei=def.edits.constBegin(); ei!=def.edits.constEnd(); ei++)
    {
        QLineEdit *ed = qobject_cast<QLineEdit *>(ei->edit);
        QComboBox *cb = qobject_cast<QComboBox *>(ei->edit);
        bool isReadOnly = record->isFieldReadOnly(ei->fieldName);
        if(ed)
        {
            ed->setReadOnly(isReadOnly);
            //ed->setEnabled(!isReadOnly);
        }
        else if(cb)
        {
            cb->setEnabled(!isReadOnly);
            //ed = qobject_cast<QLineEdit *>(ei->internalEditor);
            //ed->setReadOnly(isReadOnly);
            //ed->setEnabled(true);
        }
    }
    */
}

void TrkDecorator::clearEdits(QTabWidget *tabs, EditDefList &def)
{
    EditDef ed;
    foreach(ed, def.edits)
    {
        delete ed.edit;
    }
    def.edits.clear();
    QWidget *panel;
    foreach(panel, def.panels)
    {
        int i = tabs->indexOf(panel);
        if(i>=0)
        {
            tabs->removeTab(i);
            delete panel;
        }
    }
}

void TrkDecorator::fieldEditChanged()
{
}

static int findColumn(QHeaderView *hv, const QString &label)
{
    //QHeaderView *hv = horizontalHeader();
    QAbstractItemModel *model = hv->model();
    for(int i=0; i<model->columnCount(); i++)
    {
        if(!QString::compare(
            label.trimmed(),
            model->headerData(i,Qt::Horizontal).toString().trimmed()))
            return i;
    }
    return -1;
}

void TrkDecorator::loadViewDef(QTableView *view)
{
    const QString KEY = Settings_Grid;
    QXmlSimpleReader xmlReader;
    QFile *file = new QFile("data/tracker.xml");
    QXmlInputSource *source = new QXmlInputSource(file);
    QDomDocument dom;
    if(!dom.setContent(source,false))
        return;
    QDomElement doc = dom.documentElement();
    if(doc.isNull()) return;
    QHeaderView *hv = view->horizontalHeader();
    QVariant gridSet = settings->value(KEY);
    bool isGridRestored=false;
    QByteArray tempBuf = hv->saveState();
    if(gridSet.isValid())
        isGridRestored = hv->restoreState(gridSet.toByteArray());
    if(!isGridRestored)
    {
        hv->restoreState(tempBuf);
        QDomElement grid = doc.firstChildElement("grid");
        if(!grid.isNull())
        {
            int nextCol=0;
            for(QDomElement col = grid.firstChildElement("col");
                !col.isNull();
                col = col.nextSiblingElement("col"))
            {
                QString flabel = col.attribute("field");
                int colNum = findColumn(hv, flabel);
                if(colNum>=0)
                {
                    int from = hv->visualIndex(colNum);
                    hv->moveSection(from, nextCol++);
                    bool ok;
                    int size = col.attribute("size").toInt(&ok);
                    if(ok)
                        hv->resizeSection(colNum,size);
                }
            }
            for(; nextCol<hv->count(); nextCol++)
            {
                int log=hv->logicalIndex(nextCol);
                hv->hideSection(log);
            }
        }
    }
    hv->setMovable(true);
    hv->setDefaultAlignment(Qt::AlignLeft);
    settings->setValue(KEY, hv->saveState());
//    isDefLoaded = true;
    delete source;
    delete file;
}

FieldGroupsDef TrkDecorator::loadGroups(const RecordTypeDef *recDef)
{
    FieldGroupsDef res;
    QXmlSimpleReader xmlReader;
    QFile *file = new QFile("data/tracker.xml");
    QXmlInputSource *source = new QXmlInputSource(file);
    QDomDocument dom;
    if(!dom.setContent(source,false))
        return res;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return res;

    QStringList fieldList = recDef->fieldNames();
    QDomElement panels = doc.firstChildElement("panels");
    if(!panels.isNull())
    {
        QStringList used;
        for(QDomElement panel = panels.firstChildElement("panel");
            !panel.isNull();
            panel = panel.nextSiblingElement("panel"))
        {
            QStringList f_in_p;
            if(panel.attribute("type") != "other")
            {
                for(QDomElement field = panel.firstChildElement("field");
                    !field.isNull();
                    field = field.nextSiblingElement("field"))
                {
                    QString fname = field.attribute("name");
                    if(!fieldList.contains(fname,Qt::CaseInsensitive))
                        continue;
                    f_in_p.append(fname);
                }
            }
            else
            {
                for(int i=0; i<fieldList.count(); i++)
                {
                    QString fname = fieldList[i].trimmed();
                    if(!used.contains(fname,Qt::CaseInsensitive))
                    {
                        f_in_p.append(fname);
                    }
                }
            }
            if(f_in_p.count())
            {
                res.groups.append(panel.attribute("title", QString("Ãðóïïà %1").arg(res.groups.count()+1)));
                res.fieldsByGroup.append(f_in_p);
                used.append(f_in_p);
            }
        }
    }
    delete source;
    delete file;
    return res;
}

// ======================= TrkFieldEdit ================================
TrkFieldEdit::TrkFieldEdit(const QString &fieldName)
    : field(fieldName), rec(0)
{
}

void TrkFieldEdit::connectToRecord(TrkToolRecord *record)
{
    rec = record;
}

void TrkFieldEdit::valueChanged(const QString &fieldName, const QVariant &newValue)
{
//    if(rec)
//        rec->setValue(fieldName, newValue);
}


// ======================= TrkStringEdit ================================
TrkStringEdit::TrkStringEdit(QWidget *parent, const QString &fieldName)
    :QLineEdit(parent), TrkFieldEdit(fieldName)
{
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
}

void TrkStringEdit::clearValue()
{
    clear();
}

void TrkStringEdit::setValue(const QVariant &value)
{
    setText(value.toString());
}

QVariant TrkStringEdit::value() const
{
    return QVariant(text());
}

void TrkStringEdit::setReadOnly(bool readOnly)
{
    QLineEdit::setReadOnly(readOnly);
}

void TrkStringEdit::onTextChanged(const QString &text)
{
    valueChanged(field, QVariant(text));
}

// ======================= TrkChoiceBox ================================

TrkChoiceBox::TrkChoiceBox(QWidget *parent, const QString &fieldName)
    :QComboBox(parent), TrkFieldEdit(fieldName)
{
    setInsertPolicy(QComboBox::NoInsert);
    QLineEdit *ie = new QLineEdit();
    setLineEdit(ie);
    setEditable(true);
    ie->setReadOnly(true);
    connect(this,SIGNAL(editTextChanged(QString)),this,SLOT(onEditTextChanged(QString)));
    connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(onEditTextChanged(QString)));
}

void TrkChoiceBox::clearValue()
{
    setEditText("");
}

void TrkChoiceBox::setValue(const QVariant &value)
{
    setEditText(value.toString());
}

QVariant TrkChoiceBox::value() const
{
    return QVariant(currentText());
}

void TrkChoiceBox::setReadOnly(bool /* readOnly */)
{
}

void TrkChoiceBox::onEditTextChanged(const QString &text)
{
    valueChanged(field, QVariant(text));
}

// ======================= EditDefList ================================

void EditDefList::connectToRecord(TrkToolRecord *record)
{
    if(rec!=record)
    {
        if(rec)
            disconnect(rec,SIGNAL(changed()),this,SLOT(recordChanged()));
        rec = record;
        QList<EditDef>::const_iterator ei;
        for(ei=edits.constBegin(); ei!=edits.constEnd(); ei++)
        {
            TrkFieldEdit *ed = qobject_cast<TrkFieldEdit *>(ei->edit);
            if(ed)
                ed->connectToRecord(rec);
        }
        if(rec)
        {
            connect(rec,SIGNAL(changed()),this,SLOT(recordChanged()));
            connect(rec,SIGNAL(destroyed()),this,SLOT(recordDestroyed()));
        }
    }
    readValues();
}

void EditDefList::readValues()
{

    QList<EditDef>::const_iterator ei;
    for(ei=edits.constBegin(); ei!=edits.constEnd(); ei++)
    {
        TrkFieldEdit *ed = qobject_cast<TrkFieldEdit *>(ei->edit);
        if(ed)
            if(rec)
                ed->setValue(rec->value(ei->title));
            else
                ed->clearValue();
    }
    updateState();
}

void EditDefList::updateState()
{
    if(onlyView)
        return;
    QList<EditDef>::const_iterator ei;
    for(ei=edits.constBegin(); ei!=edits.constEnd(); ei++)
    {
        QLineEdit *ed = qobject_cast<QLineEdit *>(ei->edit);
        QComboBox *cb = qobject_cast<QComboBox *>(ei->edit);
        bool isReadOnly = rec==0 || rec->isFieldReadOnly(ei->fieldName);
        if(ed)
        {
            ed->setReadOnly(isReadOnly);
            //ed->setEnabled(!isReadOnly);
        }
        else if(cb)
        {
            cb->setEnabled(!isReadOnly);
            //cb->setEditable(isReadOnly);
            //ed = qobject_cast<QLineEdit *>(ei->internalEditor);
            //ed->setReadOnly(isReadOnly);
            //ed->setEnabled(true);
        }
    }
}

void EditDefList::clear()
{
    rec=0;
}

void EditDefList::recordChanged()
{
    readValues();
}

void EditDefList::recordDestroyed()
{
    rec = 0;
}


EditDefList::~EditDefList()
{
    connectToRecord(0);
}
