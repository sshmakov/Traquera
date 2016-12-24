/*
  Декоратор запросов PVCS Tracker
*/

function TrkDecorator()
{
    this.record = winContr.record;
    winContr.record.changedState.connect(this.onRecordStateChanged);
    if(winContr.record.mode !== 0)
        $("issueTitle").html("Новый запрос"); 
    var arr = [
      [/^#([0-9]+)/g, '<a href="#" class="scrLink">#$1</a>'],
      [/([\s\b,.^])#([0-9]+)/g, '$1<a href="#" class="scrLink">#$2</a>'],
    ];
    if(typeof this.replaceArray !== "object")
      this.replaceArray = [];
    this.replaceArray = this.replaceArray.concat(arr);
}

TrkDecorator.prototype = new TextDecorator();
TrkDecorator.prototype.txtDecorateAll = TextDecorator.prototype.decorateAll;

TrkDecorator.prototype.init = function ()
{
}

TrkDecorator.prototype.decorateNote = function (index)
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

TrkDecorator.prototype.decorateAll = function()
{
    if(typeof this.txtDecorateAll === "function")
       this.txtDecorateAll();

  // oncontextmenu="javascript:return menuSCR($1, event);"
  $(".scrLink").contextmenu(function(event) {
      var id = this.textContent.replace(/#/,"");
      return textDecorator.menuSCR(id,event);
   });

   // onclick="javascript:query.openRecordId($1);"
  $(".scrLink").click(function(event) {
      var id = this.textContent.replace(/#/,"");
      return query.openRecordId(id,event);
   });

}

TrkDecorator.prototype.menuSCR = function(scrid, evt) 
{ 
    // Блокируем всплывание события contextmenu
    evt = evt || window.event;
    evt.cancelBubble = true;
    // Показываем собственное контекстное меню
    var menu = contextMenuId;
    var rec = global.getRecord(scrid);
    var title = "", fix = "", state = "";
    if(rec)
    {
        title = rec.title;
        state = rec.value("State");
    }
    else
        title = "<i>(не найден)</i>";
    if(fix && fix !== "0")
    {
        state = state + " " + fix;
    }
    var html = "";
    html = '<ul class="SimpleContextMenu">';
    html += "<li><b>Запрос " + scrid + '</b> <span class="shortInfo">' + state +"</span><br/>"
            + title + "</li>";
    html += '<li><a href="#" onclick="copyToClip('+scrid+'); return false;">Копировать номер</a></li>';
    html += '<li><a href="#" onclick="query.openRecordId('+scrid+'); return false;">Открыть</a></li>';
    html += '<li><a href="#" onclick="query.appendId('+scrid+'); return false;">Добавить в список</a></li>';
    html += '<li><a href="#" onclick="openQueryPage('+scrid+'); return false;">Открыть в новом списке</a></li>';
    html += '</ul>';
    // Если есть что показать - показываем
    if (html) {
        var pos = defPosition(evt);
        //alert(pos.x + " " + pos.y + " " + window.innerWidth + " " + html);
        menu.innerHTML = html;
        menu.style.top = pos.y;
        var x = pos.x;
        if(x > window.innerWidth - 300)
            x = window.innerWidth - 300;
        if(x < 0)
            x = 0;
        menu.style.left = x;
        menu.style.display = "";
    }
    // Блокируем всплывание стандартного браузерного меню
    return false;
} 
