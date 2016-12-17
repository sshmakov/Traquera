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

    $(".jiraIssueLink").contextmenu(function(event) {
        var key = this.textContent;
        menuSCR(key,event);
        return false;
    });

    $(".jiraIssueLink").click(function(event) {
        var key = this.textContent;
        var id = key.replace(/^.+-([0-9]+)/g, "$1");
        query.openRecordId(id,event);
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

