email();

function email()
{
  //debugger;
  var app = new ActiveXObject("Outlook.Application");
  var ns = app.GetNamespace("MAPI");
  var myFolder = ns.GetDefaultFolder(4);
  var myFolder_2 = ns.GetDefaultFolder(5);
     
  mail = app.CreateItem(0);
  if(mail === null)
  	return;
  //format = app.olFormatHTML;
  format = 2;
  mail.BodyFormat = format;
  var template = query.project.optionValue("EmailTemplate");
  s = query.makeRecordsPage(records, template);
  mail.HTMLBody = s;


  subject ="";
  if(records.length === 1)
  {
      var rec = records[0];
      subject = "Запрос " + rec.recordId + " " + rec.value("Title");
  }
  else
  {
      subject = "Запросы ";
      for(i=0; i<records.length; i++)
      {
          var rec = records[i];
          if(i)
              subject += ", ";
          subject += rec.recordId;
      }
  }
  mail.Subject = subject;
  mail.Display(false);

  return;
}