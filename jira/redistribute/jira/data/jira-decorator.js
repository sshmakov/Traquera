/*
  Функции работы с запросами JIRA
*/

function JiraDecorator()
{
    this.key = winContr.record.project.jiraProjectKey();
    winContr.record.changedState.connect(this.onRecordStateChanged);
    if(winContr.record.mode !== 0)
        $("issueTitle").html("Новый запрос");
}

JiraDecorator.prototype = new TextDecorator();

JiraDecorator.prototype.decorateNote = function (index)
{
    var re1 = new RegExp("(\\b" + this.key + "-[0-9]+\\b)", "g");
    var pro1 = '<span class="jiraIssueLink">';
    var pro2 = '</span>';
    $(".test-note-text").html(function(noteIndex, html) {
        return html.replace(re1, '' + pro1 + '$1' + pro2);
    });

    $(".jiraIssueLink").contextmenu(function(event) {
        var key = this.innerText;
        menuSCR(key,event);
        return false;
    })
}

JiraDecorator.prototype.decorateAll = function()
{
    //var key = view.controller.currentRecord.project.jiraProjectKey();
    debugInfo(this.key);
    var re1 = new RegExp("(\\b" + this.key + "-[0-9]+\\b)", "g");
    var pro1 = '<span class="jiraIssueLink">';
    var pro2 = '</span>';
    $(".noteText").each(function(index, item) {
        var s = item.innerHTML;
        s = s.replace(re1, '' + pro1 + '$1' + pro2);
        item.innerHTML = s;
    });

    $(".jiraIssueLink").contextmenu(function(event) {
        var key = this.innerText;
        menuSCR(key,event);
        return false;
    })
}



window.textDecorator = new JiraDecorator();
