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
    var arr = [
                [/\{code:title=([^|}]+)([|\s\S]*?)\}\n*([\s\S]*?)\{code\}/gi,
                 '<div class="jiraCodeTitle">$1</div><hr/><div class="jiraCode">$3</div>'],
                [/\{code:[A-Za-z]+\}\n*([\s\S]*?)\{code\}/gi, '<div class="jiraCode">$1</div>'],
                [/\{code\}\n*([\s\S]*?)\{code\}/gi, '<div class="jiraCode">$1</div>'],
                [/\{\{([\s\S]*?)\}\}/gi, '<span class="jiraCodeInline">$1</span>'],
                [/\{quote\}\n*([\s\S]*?)\{quote\}/gi, '<div class="jiraQuote">$1</div>'],
                [/\{noformat\}\n*([\s\S]*?)\{noformat\}/gi, '<div class="jiraNoFormat">$1</div>'],
                [ new RegExp("\\b(?:"+this.browseIssueUrl + ")?(" + this.key + "-[0-9]+\\b)", "gi"),
                 '<a href="#" class="jiraIssueLink">$1</a>']
            ];
    if(typeof this.replaceArray !== "object")
        this.replaceArray = [];
    this.replaceArray = arr.concat(this.replaceArray);
}

JiraDecorator.prototype = new TextDecorator();
JiraDecorator.prototype.txtDecorateAll = TextDecorator.prototype.decorateAll;

JiraDecorator.prototype.init = function ()
{
}

JiraDecorator.prototype.decorateAll = function()
{
    this.txtDecorateAll();

    /*
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
    });
    */
    
    //this.loadInfo();

    $(".jiraIssueLink").contextmenu(function(event) {
        var key = this.textContent;
        menuIssue(key,event);
        return false;
    });

    $(".jiraIssueLink").click(function(event) {
        var key = this.textContent;
        var id = key.replace(/^.+-([0-9]+)/g, "$1");
        var projectkey = key.replace(/(\w+)-[0-9]+/,'$1');
        if(projectkey === winContr.record.project.jiraProjectKey())
          query.openRecordId(id,event);
        else {
          var url = $(this).attr("href");
          if(url !== "#" && url !== "")
            global.shell('cmd /c start '+url);
        }
        return false;
    });

    $(".jiraIssueLink").each(function(index, el) {
        var key = el.textContent;
        var url = record.project.db.dbmsServer() + "browse/" + key;
	$(el).attr("href",url);
    });

    $(".jiraExtLink").attr("href",record.project.db.dbmsServer() + "browse/" + record.jiraKey());
    $(".jiraExtLink").click(function() {
        var url = $(this).attr("href");
	if(url !== "#" && url !== "")
		global.shell('cmd /c start '+url);
	return false;
    });

}

function isoToDate(s)
{
  var dt = new Date(s.substr(0,4), s.substr(5,2)*1-1, s.substr(8,2)*1, s.substr(11,2)*1, s.substr(14,2)*1, s.substr(17,2)*1 );
  return dt;
  /*
  var lang = navigator.browserLanguage || navigator.language || navigator.userLanguage || "en";
  lang = lang.substr(0,2);
  if(lang === "ru")
    return s.substr(8,2) + "." + s.substr(5,2) + "." + s.substr(0,4) + " " + s.substr(11,8);
  return s.substr(0,10) + " " + s.substr(11,8);
  */
}

function dateToString(dt)
{
  var lang = navigator.browserLanguage || navigator.language || navigator.userLanguage || "en";
  lang = lang.substr(0,2);
  var d = dt.getDate();
  if(d < 10)
    d = "0" + d;
  var m = dt.getMonth()+1;
  if(m < 10)
    m = "0" + m;
  if(lang === "ru")
    return d + "." + m + "." + dt.getFullYear();
  return dt.toDateString();
}

function timeToString(dt)
{
  return dt.toLocaleTimeString();
}

JiraDecorator.prototype.loadInfo = function(res)
{
    var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=id,parent,issuelinks,subtasks&expand=changelog");
    this.loadLinks(res);
    this.loadSubtasks(res);
    this.loadChangeLog(res);
    this.loadParent(res);
}

JiraDecorator.prototype.loadChangeLog = function(res)
{
    //var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=id&expand=changelog");
    var hist = res.changelog.histories;
    var i;
    var text = "";
    var dt = "", tm = "";
    var author = "";
    for(i=hist.length-1; i>=0; i--)
    {
        var v = hist[i];
        var newdt = isoToDate(v.created);
        if(dateToString(newdt) !== dt)
        {
            dt = dateToString(newdt);
            text += "<tr><th colspan='2'>" + dt + "</th><td/></tr>\n"
            tm = "";
            author = "";
        }
        var stime;
        var ns = timeToString(newdt);
        if(ns !== tm)
          stime = ns;
        else
          stime = "&nbsp;";
        var sa = "&nbsp;";
        if(author !== v.author.displayName)
        {
           author = v.author.displayName;
           sa = author;
        }
        //var a = "<td>" + stime+"/"+ns+"/"+tm + "</td><td>" + v.author.displayName + "</td>";
        tm = ns;
        var items = v.items;
        var j;
        for(j=0; j<items.length; j++)
        {
            text += "<tr>";
            if(j>0)
              text += "<td>&nbsp;</td><td>&nbsp;</td>";
            else
              text += "<td>" + sa + "</td><td>" + stime + "</td>";
            var fromS = items[j].fromString;
            if(fromS && fromS.length > 50)
              fromS = fromS.substr(0,40)+'...';
            var toS = items[j].toString;
            if(toS && toS.length > 50)
              toS = toS.substr(0,40)+'...';
            text += "<td><b>" + items[j].field + "</b>: '<i>" + fromS + "</i>' ==&gt; '<i>" + toS + "</i>'</td></tr>\n"
        }
    }
    $("#changelog").html("<table cols='3' width='100%' cellpadding='2em' cellspacing='0'><tbody>" + text + "</tbody></table>");
    return text;
}

JiraDecorator.prototype.loadLinks = function(res)
{
    //var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=issuelinks");
    var links = res.fields.issuelinks;
    var i;
    var text = "";
    for(i=0; i<links.length; i++)
    {
       var l = links[i];
       if(typeof l.outwardIssue !== "undefined")
         text += "<li>"+ l.type.outward +" <a class='jiraIssueLink' href='" + l.outwardIssue.self + "'>" + l.outwardIssue.key + "</a> " + l.outwardIssue.fields.summary + '</li>\n';
       else
         text += "<li>"+ l.type.inward +" <a class='jiraIssueLink' href='" + l.inwardIssue.self + "'>" + l.inwardIssue.key + "</a> " + l.inwardIssue.fields.summary + '</li>\n';
    }
    if(text !== "")
      $("#linksBody").html("<ul>" + text + "</ul>");
}

JiraDecorator.prototype.loadSubtasks = function(res)
{
    //var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=subtasks");
    var tasks = res.fields.subtasks;
    var i;
    var text = "";
    var keys = "";
    for(i=0; i<tasks.length; i++)
    {
       var t = tasks[i];
       text += "<li><a class='jiraIssueLink' href='" + t.self + "'>" + t.key + "</a> "+ t.fields.summary + " (" + t.fields.status.name +")</li>\n";
       if(keys === "")
         keys += t.key;
       else
         keys += ", " + t.key;
    }
    if(text !== "")
      $("#tasksBody").html("<ul>" + text + "</ul>");
}

JiraDecorator.prototype.loadParent = function(res)
{
    //var res = this.record.project.serverGet("rest/api/2/issue/"+this.record.jiraKey() +"?fields=parent");
    var parent = res.fields.parent;
    if(parent && typeof parent.key !== "undefined")
      $("#parentLink").html("(по <a class='jiraIssueLink' href='" + parent.key + "'>"+ parent.key +"</a>)");
    else
      $("#parentLink").hide();
}

