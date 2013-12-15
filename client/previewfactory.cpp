#include "previewfactory.h"
#include "settings.h"

#include <QtCore>
#include <QtXml>

PreviewFactory::PreviewFactory(QObject *parent) :
    QObject(parent)
{
    QFile file("data/init.xml");
    QXmlInputSource source(&file);
    QDomDocument dom;
    if(dom.setContent(&source,false))
    {
        QDomElement doc = dom.documentElement();
        if(!doc.isNull())
        {
            QDomElement p = doc.firstChildElement("preview");
            if(!p.isNull())
            {
                QDomElement v = p.firstChildElement("viewer");
                while(!v.isNull())
                {
                    QString type = v.attribute("type").toLower();
                    QString exts = v.attribute("ext");
                    QString syntax = v.attribute("syntax");
                    foreach(QString ext, exts.split(QChar(',')))
                        typeByExt[ext.toLower().trimmed()] = type + "|" + syntax;
                    v = v.nextSiblingElement("viewer");
                }
            }
        }
    }
}

Preview *PreviewFactory::createPreview(const QString &file, QWidget *parent)
{
    Preview *res;
    QString ext,type;
    QFileInfo info(file);
    //QString filePath = QDir::toNativeSeparators(info.absoluteFilePath());
    ext = info.suffix().toLower();
    type = typeByExt[ext];
    if(!type.isEmpty())
    {
        QStringList a = type.split('|');
        type = a[0];
        QString syntax = a[1];
        if(type == "image")
            return new PreviewImage(parent);
        if(type == "text")
        {
            PreviewTxt *res = new PreviewTxt(parent);
            res->setSyntax(syntax);
            return res;
        }
    }
    if(ext.isEmpty())
        return 0;
    ext = "." + ext;
    if(!PreviewActiveX::previewClass(ext).isEmpty())
    {
        res = new PreviewActiveX(parent);
        //res->setSourceFile(file);
        return res;
    }
    return 0;
}
