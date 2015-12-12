/*
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
*/

email();

function email()
{
//debugger;
var app = ActiveXObject("Outlook.Application");
var ns = ActiveXObject(app.GetNamespace("MAPI"));
var myFolder = ActiveXObject(ns.GetDefaultFolder(4));
var myFolder_2 = ActiveXObject(ns.GetDefaultFolder(5));
   
//itemType1 = app.olMailItem;
itemType = 0;
mail = ActiveXObject(app.CreateItem(itemType));
if(mail == null)
	return;
//Global.saveObjectDocumentation(mail, "data/mail.htm");

//{

//    QMessageBox::critical(this,tr("Ошибка"),
//        tr("Не создается письмо в Outlook"));
//    return;
//}
format = 2;
//format = app.olFormatHTML;
//if(!format.isValid())
//    format=2;
mail.BodyFormat = format;
s = query.makeRecordsPage(records, "data/email.xq");
mail.HTMLBody = s;


subject ="";
if(records.length == 1)
{
    var rec = records[0];
    subject = "Запрос " + rec.recordId() + " " + rec.value("Title");
}
else
{
    subject = "Запросы ";
    for(i=0; i<records.length; i++)
    {
        var rec = records[i];
        if(i)
            subject += ", ";
        subject += rec.recordId();
    }
}
mail.Subject = subject;
mail.Display(true);

//var hasSent = mail.Sent;
//app = null;

return;

}