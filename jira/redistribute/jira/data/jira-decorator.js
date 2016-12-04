/*
  Функции работы с запросами JIRA
*/

function JiraDecorator()
{
    this.projectUrl = winContr.record.project.db.dbmsServer();
    this.browseIssueUrl = this.projectUrl+"browse/";
    this.key = winContr.record.project.jiraProjectKey();
    this.record = winContr.record;
    winContr.record.changedState.connect(this.onRecordStateChanged);
    if(winContr.record.mode !== 0)
        $("issueTitle").html("Новый запрос"); 
    this.textReplace = [
      [/\{code:title=([^|}]+)([|\s\S]*?)\}\n*([\s\S]*?)\{code\}/gi, 
       '<div class="jiraCodeTitle">$1</div><hr/><div class="jiraCode">$3</div>'],
      [/\{code\}\n*([\s\S]*?)\{code\}/gi, '<div class="jiraCode">$1</div>'],
      [/\{\{([\s\S]*?)\}\}/gi, '<span class="jiraCodeInline">$1</span>'],
      [/\{quote\}\n*([\s\S]*?)\{quote\}/gi, '<div class="jiraQuote">$1</div>'],
      [/\{noformat\}\n*([\s\S]*?)\{noformat\}/gi, '<div class="jiraNoFormat">$1</div>'],
      [/(ftp:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
      [/(mailto:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
      [ new RegExp("(\\s)(" + this.key + "-[0-9]+\\b)", "gi"),  '$1<span class="jiraIssueLink">$2</span>'],
      [ new RegExp("^(" + this.key + "-[0-9]+\\b)", "gi"),      '<span class="jiraIssueLink">$1</span>'],
      [ new RegExp("\\b("+this.browseIssueUrl + ")(" + this.key + "-[0-9]+\\b)", "gi"), 
       '<span class="jiraIssueLink">$2</span>'],
      [/(https?:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>']
    ];
}

JiraDecorator.prototype = new TextDecorator();

JiraDecorator.prototype.init = function ()
{
}

JiraDecorator.prototype.decorateNote = function (index)
{
/*
  var re1 = new RegExp("(\\b" + this.key + "-[0-9]+\\b)", "g");
  var pro1 = '<span class="jiraIssueLink">';
  var pro2 = '</span>';
  $(".test-note-text").html(function(noteIndex, html) {
    return html.replace(re1, '' + pro1 + '$1' + pro2);
  });

  $(".jiraIssueLink").contextmenu(function(event) {
      var key = this.issue;
      menuSCR(key,event);
      return false;
   })
*/
}

JiraDecorator.prototype.decorateAll = function()
{
  //var key = view.controller.currentRecord.project.jiraProjectKey();
  debugInfo(this.key);


/*
  var re1 = new RegExp("(\\b" + this.key + "-[0-9]+\\b)", "gi");
  var to1 = '<span class="jiraIssueLink">$1</span>';

  var re2 = /\{code:title=([\s\S]*?)\}\n*([\s\S]*?)\{code\}/gi;
  var to2 = '<div class="jiraCodeTitle">$1</div><hr/><div class="jiraCode">$2</div>';

  var re3 = /\{code\}\n*([\s\S]*?)\{code\}/gi;
  var to3 = '<div class="jiraCode">$1</div>';

  var re4 = /\{noformat\}\n*([\s\S]*?)\{noformat\}/gi;
  var to4 = '<div class="jiraNoFormat">$1</div>';
*/

  $(".noteText").html(function(index, old) {
    var i;
    var s = old;
    var arr = textDecorator.textReplace;
    for(i=0; i<arr.length ; i++)
    {
      var re = arr[i][0];
      var to = arr[i][1];
      s = s.replace(re, to);
    }
    return s;

/*    var s = old;
    s = s.replace(re1, to1);
    s = s.replace(re2, to2);
    s = s.replace(re3, to3);
    s = s.replace(re4, to4);
    return s;*/
  });

  $(".jiraIssueLink").contextmenu(function(event) {
      var key = this.innerHTML;
      menuSCR(key,event);
      return false;
   });

  this.loadChangeLog();
}

JiraDecorator.prototype.loadChangeLog = function()
{
  var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=id&expand=changelog");
  var hist = res.changelog.histories;
  var i;
  var text = "";
  for(i=0; i<hist.length; i++)
  {
    var v = hist[i];
    var a = "<td>" + v.created + "</td><td>" + v.author.displayName + "</td>";
    var items = v.items;
    var j;
    for(j=0; j<items.length; j++)
    {
      text += "<tr>"+a+"<td>Changed " + items[j].field + " from '" + items[j].fromString + "' to '" + items[j].toString + "'</td></tr>\n"
    }
  }
  $("#changelog").html("<table cols='3' width='100%'><tbody>" + text + "</tbody></table>");
  return text;
}

