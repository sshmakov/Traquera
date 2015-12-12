debugger;
doc = ActiveXObject("Word.Document");     
app = ActiveXObject(doc.Application);
app.Visible = true;
cont = ActiveXObject(doc.Content);
rec = records[0];
s = rec.toJSON();
cont.InsertAfter(s);



//app = ActiveXObject("Outlook.Application");
//itemType1 = app.olMailItem;
//itemType = 0;
//mail = ActiveXObject(app.CreateItem(itemType));
debugger;
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
