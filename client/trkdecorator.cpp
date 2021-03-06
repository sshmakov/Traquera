#include "trkdecorator.h"
#include <QtGui>
#include <QStringList>
#include <QDomElement>
#include <QXmlSimpleReader>
//#include <QTabWidget>
//#include <QGridLayout>
//#include <QFormLayout>
//#include <QHeaderView>
//#include <QScrollArea>
//#include "trkview.h"
#include "settings.h"
#include "ttglobal.h"
#include "querypage.h"

TQDecorator *decorator;

TQDecorator::TQDecorator(QObject *parent)
    : QObject(parent)
//    , prj(0)
{
}

void TQDecorator::fillEditPanels(QTabWidget *tabs, const TQAbstractRecordTypeDef * recDef, EditDefList &def, bool onlyView)
{
    //QList<EditDef> edits;
    //const QStringList & fieldList = record->fields();

    if(def.isFilled && def.recType == recDef->recordType())
        return;
    def.onlyView = onlyView;
    clearEdits(tabs, def);
//    QXmlSimpleReader xmlReader;
    //QFile *file = new QFile("data/tracker.xml");
    QIODevice *file = recDef->defineSource();
    if(!file)
        return;
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
                    TQAbstractFieldType fdef = recDef->getFieldType(fname);
                    EditDef f;
                    QWidget *w;
                    //QLineEdit *ie = 0;
                    if(!onlyView && fdef.isChoice())
                    {
                        TQChoiceBox *c = new TQChoiceBox(tabs, fname);
                        //c->setInsertPolicy(QComboBox::NoInsert);
                        //ie = new QLineEdit();
                        //c->setLineEdit(ie);
                        //c->setEditable(false);
                        TQChoiceItem choice;
                        foreach(choice,fdef.choiceList())
                        {
                            c->addItem(choice.displayText, choice.fieldValue /*  QVariant((int)(choice.order)) */);
                        }
                        w = c;
                    }
                    else if(fdef.simpleType() == TQ::TQ_FIELD_TYPE_DATE)
                        w = new TQDateTimeEdit(tabs, fname);
                    else
                        w = new TQStringEdit(tabs, fname);
                    f.edit = w;
                    //f.internalEditor = ie;
                    f.fieldName = fname;
                    f.title = fname;
                    f.vid = fdef.virtualID();
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

void TQDecorator::readValues(TQRecord *record, EditDefList &def)
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

void TQDecorator::updateState(TQRecord *record, EditDefList &def)
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

void TQDecorator::clearEdits(QTabWidget *tabs, EditDefList &def)
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

void TQDecorator::fieldEditChanged()
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

static const TQAbstractRecordTypeDef::TQFieldRole basicRoles[8] = {
    TQAbstractRecordTypeDef::IdField,
    TQAbstractRecordTypeDef::StateField,
    TQAbstractRecordTypeDef::SubmitDateTimeField,
    TQAbstractRecordTypeDef::SubmitterField,
    TQAbstractRecordTypeDef::OwnerField,
    TQAbstractRecordTypeDef::TitleField,
    TQAbstractRecordTypeDef::RecordTypeField,
    TQAbstractRecordTypeDef::PriorityField
};


void TQDecorator::loadViewDef(QueryPage *page)
{
    const QString KEY = Settings_Grid;
    //QXmlSimpleReader xmlReader;
    TQAbstractProject *newPrj = page->project();
//    if(prj == newPrj)
//        return;
//    prj = newPrj;

    QHeaderView *hv = page->queryView->horizontalHeader();
    TQScopeSettings sets = newPrj->projectSettings();
    QVariant gridSet = sets->value(KEY);
    bool isGridRestored=false;
    QByteArray tempBuf = hv->saveState();
    if(gridSet.isValid())
        isGridRestored = hv->restoreState(gridSet.toByteArray());

    if(!isGridRestored)
    {
//        hv->restoreState(tempBuf);
//        QString groupFile = sets->value("GroupFile", "data/tracker.xml").toString();
        QString groupFile = newPrj->optionValue(TQOPTION_GROUP_FILE).toString();
//        QString groupFile = newPrj->property("groupFile").toString();
        QFile file(groupFile);
        QXmlInputSource source(&file);
        QDomDocument dom;
        if(!dom.setContent(&source,false))
        {
            const TQAbstractRecordTypeDef *def = page->recordTypeDef();
            if(def)
            {
                int nextCol = 0;
                for(int i=0; i<sizeof(basicRoles); i++)
                {
                    int vid = def->roleVid(basicRoles[i]);
                    if(vid != TQ::TQ_NO_VID)
                    {
                        QString flabel = def->fieldName(vid);
                        int colNum = findColumn(hv, flabel);
                        if(colNum>=0)
                        {
                            int from = hv->visualIndex(colNum);
                            hv->moveSection(from, nextCol++);
//                            bool ok;
//                            int size = col.attribute("size").toInt(&ok);
//                            if(ok)
//                                hv->resizeSection(colNum,size);
                        }
                    }
                }
                for(; nextCol<hv->count(); nextCol++)
                {
                    int log=hv->logicalIndex(nextCol);
                    hv->hideSection(log);
                }
            }
        }
        else
        {
            QDomElement doc = dom.documentElement();
            if(doc.isNull())
                return;
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
    }
    hv->setMovable(true);
    hv->setDefaultAlignment(Qt::AlignLeft);
    sets->setValue(KEY, hv->saveState());
    //    isDefLoaded = true;
}

bool TQDecorator::saveState(QueryPage *page)
{
    const QString KEY = Settings_Grid;
    //QXmlSimpleReader xmlReader;
    TQAbstractProject *newPrj = page->project();
    TQScopeSettings sets = newPrj->projectSettings();
    QHeaderView *hv = page->queryView->horizontalHeader();
    QByteArray tempBuf = hv->saveState();
    sets->setValue(KEY, tempBuf);
    return true;
}

FieldGroupsDef TQDecorator::loadGroups(const TQAbstractRecordTypeDef *recDef)
{
    FieldGroupsDef res;
    res.loadGroupsDef(recDef);
    return res;
    /*
    FieldGroupsDef res = loadGroupsXML(recDef);
    if(res.isValid())
        return res;
    QString fname;
    QStringList fieldList = recDef->fieldNames();
    QStringList baseList = recDef->project()->baseRecordFields(recDef->recordType()).values();
    QStringList flist;
    for(int i=0; i<6; i++)
    {
        int vid = recDef->roleVid(basicRoles[i]);
        if(vid != TQ::TQ_NO_VID)
        {
            QString flabel = recDef->fieldName(vid);
            flist.append(flabel);
            baseList.removeAll(flabel);
        }
    }
    flist += baseList;
    res.groups.append(tr("Основные"));
    res.fieldsByGroup.append(flist);
    QSet<QString> fieldSet = fieldList.toSet();
    QSet<QString> baseSet = flist.toSet();
    fieldList = (fieldSet - baseSet).toList();
    qSort(fieldList);
    res.groups.append(tr("Другие"));
    res.fieldsByGroup.append(fieldList);
    return res;
    */
}

FieldGroupsDef TQDecorator::loadGroupsXML(const TQAbstractRecordTypeDef *recDef)
{
    FieldGroupsDef res;
    QScopedPointer<QIODevice> file(recDef->defineSource());
    if(file.isNull())
        return res;
    QScopedPointer<QXmlInputSource> source(new QXmlInputSource(file.data()));
    QDomDocument dom;
    if(!dom.setContent(source.data(),false))
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
                res.groups.append(panel.attribute("title", QString("Группа %1").arg(res.groups.count()+1)));
                res.fieldsByGroup.append(f_in_p);
                used.append(f_in_p);
            }
        }
    }
    return res;
}

// ======================= TrkFieldEdit ================================
TQFieldEdit::TQFieldEdit(const QString &fieldName)
    : field(fieldName), rec(0)
{
}

void TQFieldEdit::connectToRecord(TQRecord *record, int fieldVid)
{
    rec = record;
}

void TQFieldEdit::valueChanged(const QString &fieldName, const QVariant &newValue)
{
//    if(rec)
//        rec->setValue(fieldName, newValue);
}


// ======================= TrkStringEdit ================================
TQStringEdit::TQStringEdit(QWidget *parent, const QString &fieldName)
    :QLineEdit(parent), TQFieldEdit(fieldName)
{
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(onTextChanged(QString)));
}

void TQStringEdit::clearValues()
{
    clear();
}

void TQStringEdit::setValues(const QVariant &value)
{
    setText(value.toString());
}

QVariant TQStringEdit::values() const
{
    return QVariant(text());
}

void TQStringEdit::setReadOnly(bool readOnly)
{
    QLineEdit::setReadOnly(readOnly);
}

void TQStringEdit::onTextChanged(const QString &text)
{
    valueChanged(field, QVariant(text));
}

// ======================= TrkChoiceBox ================================

TQChoiceBox::TQChoiceBox(QWidget *parent, const QString &fieldName)
    :QComboBox(parent), TQFieldEdit(fieldName)
{
    setInsertPolicy(QComboBox::NoInsert);
    QLineEdit *ie = new QLineEdit();
    setLineEdit(ie);
    setEditable(true);
    ie->setReadOnly(true);
    connect(this,SIGNAL(editTextChanged(QString)),this,SLOT(onEditTextChanged(QString)));
    connect(this,SIGNAL(currentIndexChanged(QString)),this,SLOT(onEditTextChanged(QString)));
}

void TQChoiceBox::clearValues()
{
    setEditText("");
}

void TQChoiceBox::setValues(const QVariant &value)
{
    setEditText(value.toString());
}

QVariant TQChoiceBox::values() const
{
    return QVariant(currentText());
}

void TQChoiceBox::setReadOnly(bool /* readOnly */)
{
}

void TQChoiceBox::onEditTextChanged(const QString &text)
{
    valueChanged(field, QVariant(text));
}

// ======================= EditDefList ================================

void EditDefList::connectToRecord(TQRecord *record)
{
    if(rec!=record)
    {
        if(rec)
            disconnect(rec,SIGNAL(changed()),this,SLOT(recordChanged()));
        rec = record;
        QList<EditDef>::const_iterator ei;
        for(ei=edits.constBegin(); ei!=edits.constEnd(); ei++)
        {
            TQFieldEdit *ed = qobject_cast<TQFieldEdit *>(ei->edit);
            if(ed)
                ed->connectToRecord(rec, ei->vid);
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
        TQFieldEdit *ed = qobject_cast<TQFieldEdit *>(ei->edit);
        if(ed)
            if(rec)
                ed->setValues(rec->value(ei->title));
            else
                ed->clearValues();
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

//============================ TQDateTimeEdit ================================
TQDateTimeEdit::TQDateTimeEdit(QWidget *parent, const QString &fieldName)
    : QDateTimeEdit(parent), TQFieldEdit(fieldName)
{
    setCalendarPopup(true);
    setDisplayFormat(tr("dd.MM.yyyy hh:mm:ss"));
}

void TQDateTimeEdit::clearValues()
{
    clear();
}

void TQDateTimeEdit::setValues(const QVariant &value)
{
    QDateTime dt = value.toDateTime();
    setDateTime(dt);
}

QVariant TQDateTimeEdit::values() const
{
    return QVariant(dateTime());
}

void TQDateTimeEdit::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly)
}

void TQDateTimeEdit::onDateTimeChanged(const QDateTime &value)
{
    valueChanged(field, QVariant(value));
}

void FieldGroupsDef::clear()
{
    groups.clear();
    fieldsByGroup.clear();
    other.clear();
}

void FieldGroupsDef::loadGroupsDef(const TQAbstractRecordTypeDef *recDef)
{
    clear();
    if(!recDef)
        return;
    QByteArray buf;
    buf = recDef->optionValue(TQOPTION_GROUP_XML).toByteArray();
    if(buf.isEmpty())
    {
        QString fileName = recDef->project()->optionValue(TQOPTION_GROUP_FILE).toString();
        QFile file(fileName);
        if(file.exists() && file.open(QFile::ReadOnly))
            buf = file.readAll();
    }
    if(buf.isEmpty() || !setGroupsDef(recDef, buf))
        setDefaultGroupsDef(recDef);
}

void FieldGroupsDef::saveGroupsDef(const TQAbstractRecordTypeDef *recDef)
{
    QByteArray buf = groupsDef();
    recDef->setOptionValue(TQOPTION_GROUP_XML, buf);
}

bool FieldGroupsDef::setDefaultGroupsDef(const TQAbstractRecordTypeDef *recDef)
{
    clear();
    QStringList fieldList = recDef->fieldNames();
    QStringList baseList = recDef->project()->baseRecordFields(recDef->recordType()).values();
    QStringList flist;
    for(int i=0; i<sizeof(basicRoles); i++)
    {
        int vid = recDef->roleVid(basicRoles[i]);
        if(vid != TQ::TQ_NO_VID)
        {
            QString flabel = recDef->fieldName(vid);
            flist.append(flabel);
            baseList.removeAll(flabel);
        }
    }
    flist += baseList;
    groups.append(tr("Основные"));
    fieldsByGroup.append(flist);
    QSet<QString> fieldSet = fieldList.toSet();
    QSet<QString> baseSet = flist.toSet();
    fieldList = (fieldSet - baseSet).toList();
    qSort(fieldList);
    groups.append(tr("Другие"));
    fieldsByGroup.append(fieldList);
    return true;
}

bool FieldGroupsDef::setGroupsDef(const TQAbstractRecordTypeDef *recDef, QByteArray &buf)
{
    clear();
    /*
    QString fileName = recDef->project()->optionValue(TQOPTION_GROUP_FILE).toString();
    QFile file(fileName);
    if(!file.exists())
        return false;
    QScopedPointer<QXmlInputSource> source(new QXmlInputSource(&file));
    QDomDocument dom;
    if(!dom.setContent(source.data(),false))
        return false;
    */
    QDomDocument dom;
    if(!dom.setContent(buf,false))
        return false;
    QDomElement doc = dom.documentElement();
    if(doc.isNull())
        return false;
    QStringList fieldList = recDef->fieldNames();
    QDomElement panels = doc.firstChildElement("panels");
    if(panels.isNull())
        return false;
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
            other = panel.attribute("title",tr("Другие"));
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
            QString t = panel.attribute("title", tr("Группа %1").arg(groups.count()+1));
            groups.append(t);
            fieldsByGroup.append(f_in_p);
            used.append(f_in_p);
        }
    }
    /*
    if(other.isEmpty())
    {
        QStringList f_in_p;
        other = tr("Другие");
        for(int i=0; i<fieldList.count(); i++)
        {
            QString fname = fieldList[i].trimmed();
            if(!used.contains(fname,Qt::CaseInsensitive))
            {
                f_in_p.append(fname);
            }
        }
        if(f_in_p.count())
        {
            groups.append(other);
            fieldsByGroup.append(f_in_p);
        }
    }
    */
    return true;
}

QByteArray FieldGroupsDef::groupsDef() const
{
    /*
<?xml version="1.0" encoding="utf-8"?>
<tracker>
  <panels>
    <panel title="Главное" cols="2">
      <field name="Id"/>
      <field name="Title"/>
      <field name="Request Type"/>
      <field name="Submitter"/>
      <field name="Submit Department"/>
      <field name="Owner"/>
      <field name="Submit Date"/>
      <field name="Close Date"/>
      <field name="State"/>
      <field name="Current State"/>
      <field name="Severity"/>
    </panel>

    <panel title="Источник" cols="2">
      <field name="Submitter"/>
      <field name="Found In Build"/>
      <field name="Found In Update"/>
    </panel>
    <panel title="Другое" type="other"/>
  </panels>
     */
    QDomDocument doc("groups");
    QDomElement root = doc.createElement("groups");
    doc.appendChild(root);
    QDomElement panels = doc.createElement("panels");
    root.appendChild(panels);
    for(int i =0; i<groups.size(); i++)
    {
        QString group = groups.value(i);
        QDomElement tagGroup = doc.createElement("panel");
        tagGroup.setAttribute("title", group);
        if(group == other)
            tagGroup.setAttribute("type","other");
        else
        {
            QStringList fields = fieldsByGroup.value(i);
            foreach(QString fname, fields)
            {
                QDomElement tagField = doc.createElement("field");
                tagField.setAttribute("name", fname);
                tagGroup.appendChild(tagField);
            }
        }
        panels.appendChild(tagGroup);
    }
    return doc.toByteArray();
}
