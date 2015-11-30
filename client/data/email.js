doc = ActiveXObject("Word.Document");     
app = ActiveXObject(doc.Application);
app.Visible = true;
cont = ActiveXObject(doc.Content)
rec = records[0];
debugger;
//Global.saveObjectDocumentation(cont, "data/cont.htm");
//Global.saveObjectDocumentation(doc, "data/doc.htm");
s = query.makeRecordsPage(records, "data/email.xq");
//s = rec.toJSON();
//cont.InsertAfter(s);
Global.setClipboardHTML(s);
cont.PasteSpecial(0,false,0,false,10);
//cont.InsertXML(s);



//app = ActiveXObject("Outlook.Application");
//itemType1 = app.olMailItem;
//itemType = 0;
//mail = ActiveXObject(app.CreateItem(itemType));
//if(!mail)
//{

//    QMessageBox::critical(this,tr("Ошибка"),
//        tr("Не создается письмо в Outlook"));
//    return;
//}
//format = 2;
//format = app.olFormatHTML;
//if(!format.isValid())
//    format=2;
//mail.BodyFormat = format;
//mail.HTMLBody = page;

//QString subject;
//if(records.count()==1)
//{
//    TQRecord *rec = qobject_cast<TQRecord *>(records[0]);
//    subject = QString(tr("Запрос %1. %2")).arg((int)(rec->recordId())).arg(rec->value("Title").toString());
//}
//else
//{
//    subject = tr("Запросы ");
//    for(int i=0; i<records.count(); i++)
//    {
//        const TQRecord *rec = qobject_cast<TQRecord *>(records[i]);
//        if(i)
//            subject += ", ";
//        subject += QString("%1").arg(rec->recordId());
//    }
//}
//mail->setProperty("Subject", subject);

//mail->dynamicCall("Display(QVariant)", true);
