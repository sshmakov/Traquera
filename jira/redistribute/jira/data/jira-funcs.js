function debugInfo(line)
{
	debug.textContent = line;
}


function initJiraFunc()
{
  if(typeof(window.record) === "undefined")
    window.record = view.controller.currentRecord;
}

/*

function replaceKeys()
{
  //var key = view.controller.currentRecord.project.jiraProjectKey();
  var key = record.project.jiraProjectKey();
  debugInfo(key);
  var re1 = new RegExp("(\\b" + key + "-[0-9]+\\b)", "g");
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
*/

function menuSCR(key, evt) { 
    var scrid = key.replace(/\w+-([0-9]+)/,'$1');
    // Блокируем всплывание события contextmenu
    evt = evt || window.event;
    evt.cancelBubble = true;
    // Показываем собственное контекстное меню
    var menu = contextMenuId;
    var rec = {};
    var summary = "(-)", fix = "", state = "";
    
    var project = record.project;
    var rec = project.serverGet('rest/api/2/issue/'+key);
    
    //var rec = global.getRecord(scrid);
    //debugInfo(JSON.stringify(rec));
    if(rec)
    {
        summary = rec.fields.summary;
        state = rec.fields.status.name;
        fix = rec.fields.resolution;
    }
    else
        title = "<i>(не найден)</i>";
    if(fix && fix !== "null")
    {
	state = state + " " + fix;
    }
    var html = "";
    html = '<ul class="SimpleContextMenu">';
    html += "<li><b>" + key + '</b> <span class="shortInfo">' + state +"</span><br/>"
            + summary + "</li>";
    html += '<li><a href="#" onclick="copyToClip(\''+key+'\'); return false;">Копировать номер</a></li>';
    html += '<li><a href="#" onclick="winContr.openRecord('+scrid+'); return false;">Открыть</a></li>';
    html += '<li><a href="#" onclick="query.appendId('+scrid+'); return false;">Добавить в список</a></li>';
    html += '<li><a href="#" onclick="winContr.openRecordList('+scrid+'); return false;">Открыть в новом списке</a></li>';
    html += '</ul>';
    // Если есть что показать - показываем
    //if (html) {
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
	debugInfo(JSON.stringify(pos));
        menu.style.display = "";
    //}
    // Блокируем всплывание стандартного браузерного меню
    return false;
} 

// Закрываем контекстное при клике левой или правой кнопкой по документу 
// Функция для добавления обработчиков событий 
function addHandler(object, event, handler, useCapture) { 
      if (object.addEventListener) { 
            object.addEventListener(event, handler, useCapture ? useCapture : false); 
      } else if (object.attachEvent) { 
            object.attachEvent('on' + event, handler); 
      } //else alert("Add handler is not supported"); 
} 

addHandler(document, "contextmenu", function() { 
      contextMenuId.style.display = "none"; 
}); 

addHandler(document, "click", function() { 
      contextMenuId.style.display = "none"; 
});

// Функция для определения координат указателя мыши 
function defPosition(event) { 
      var x = y = 0; 
      if (document.attachEvent != null) { // Internet Explorer & Opera 
            x = window.event.clientX + (document.documentElement.scrollLeft ? document.documentElement.scrollLeft : document.body.scrollLeft); 
            y = window.event.clientY + (document.documentElement.scrollTop ? document.documentElement.scrollTop : document.body.scrollTop); 
      } else if (!document.attachEvent && document.addEventListener) { // Gecko 
            x = event.clientX + window.scrollX; 
            y = event.clientY + window.scrollY; 
      } else { 
            // Do nothing 
      } 
      return {x:x, y:y}; 
} 

function copyToClip(text)
{
  global.setClipboardText(text);
}


function saveSelection() {
    var sel = window.getSelection();
    return sel.rangeCount ? sel.getRangeAt(0) : null;
}

 

function restoreSelection(range) {
    var sel = window.getSelection();
    sel.removeAllRanges();
    sel.addRange(range);

}

window.textDecorator = new JiraDecorator();

