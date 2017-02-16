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
  var format = 2;
  mail.BodyFormat = format;
  var template = query.project.optionValue("EmailTemplate");
  var body = query.makeRecordsPage(records, template);


  var subject ="";
  if(records.length === 1)
  {
      var rec = records[0];
      subject = rec.key + " " + rec.title;
      var url = rec.project.db.dbmsServer() + "browse/" + rec.jiraKey();
      body = "<a href='"+ url +"'>" + url +"</a>" + body;
  }
  else
  {
      subject = "";
      var list = "<div>";
      for(i=0; i<records.length; i++)
      {
          var rec = records[i];
          if(i)
              subject += ", ";
          subject += rec.key;
          var url = rec.project.db.dbmsServer() + "browse/" + rec.jiraKey();
          list += "<a href='"+ url +"'>" + url +"</a><br/>";
      }
      list += "</div>";
      body = list + body;
  }
  mail.HTMLBody = "<p>&nbsp;</p><hr/>" + body;
  mail.Subject = subject;
  mail.Display(false);

  return;
}