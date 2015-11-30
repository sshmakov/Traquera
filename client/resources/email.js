debugger;

obj = new global.CreateObject("Outlook.Application");

itemType = obj.olMailItem;
mail = new app.CreateItem(itemType);
//if(!mail)
//{

//    QMessageBox::critical(this,tr("Ошибка"),
//        tr("Не создается письмо в Outlook"));
//    return;
//}
format = app.olFormatHTML;
//if(!format.isValid())
//    format=2;
mail.BodyFormat = format;
mail.HTMLBody = page;

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
