function r()
{
        var list = document.getElementsByClassName('note');
	//debug2.innerHTML = 'D0';
	//debug2.innerText = 'Length '+ list.length;
        var l = list.length;
        var dd = 'aa ';
	for(var i = 0; i < l; i++)
        {
             var node = list[i];
            // var index = 0;
             var index = node.attributes['index'].value;
             //var title = 'eeeee'; 
             var title = document.getElementById('title'+index).innerText;
             dd = dd + index + ', ';
             var cla = 'an';
             if (title == 'Analitical Note') cla = 'an';
             else if (title == 'Engineering Note') cla = 'en';
             else if (title == 'Manager note') cla = 'mn';
             else if (title == 'Test Note') cla = 'tn';
             else if (title == 'Document Note') cla = 'dn';
             else if ((title == 'Release Note') || (title == 'HotFixEng')) cla = 'rn';
	     else if (title == 'Tech Support Note') cla = 'sn';
             else 
               cla = 'on';
             node.classList.add(cla)
             //node.className = 'note ' + cla;
         }
	//debug2.innerHTML = 'D2';
	//debug.innerHTML = dd;
	//debug2.innerHTML = 'D3';
}

function copyToClip(text)
{
    global.setClipboardText(text);
}

function openQueryPage(numbers)
{
    var g = global;
    var m = g.mainWindowObj;
    m.openQueryById(numbers,false);
}

function replaceNumbers()
{
  //var re = /([\s\b,.])#([0-9]+)/g;
  var re1 = /^#([0-9]+)/g;
  var re2 = /([\s\b,.^])#([0-9]+)/g;
  var pro1 = '<a href="javascript:query.openRecordId(';
  var pro21 = ');" oncontextmenu="javascript:return menuSCR(';
  var pro22 = ', event);" >';
  var pro3 = '<a href="javascript:query.appendId(';
  var pro4 = ');">';
  var ep = '</a>';
  var notes = document.getElementsByClassName('noteText');
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(re1,pro1+'$1'+pro21+'$1'+pro22+'#$1'+ep
	//+'<img src="plus-m.png" onClick="javascript:query.appendId($1);"/>'
	);
    ns = ns.replace(re2,'$1'+pro1+'$2'+pro21+'$2'+pro22+'#$2'+ep
	//+'<img src="plus-m.png" onClick="javascript:query.appendId($2);"/>'
	);
    notes[i].innerHTML = ns;
  }
}


function showPerforceFile(filename)
{
	//debug.innerHTML = 'starting';
	var WshShell = filename;
	//debug.innerHTML = 'creating';
	WshShell = new window.ActiveXObject("WScript.Shell");
	//debug.innerHTML = 'result';
	//debug2.innerHTML = "("+ WshShell + ")";
	WshShell.Run("%comspec% ");
	//debug.innerHTML = 'started';
}

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

function menuSCR(scrid, evt) { 
    // Блокируем всплывание события contextmenu
    evt = evt || window.event;
    evt.cancelBubble = true;
    // Показываем собственное контекстное меню
    var menu = contextMenuId;
    var rec = global.getRecord(scrid);
    var title = "";
    if(rec)
        title = rec.title;
    else
        title = "<i>(не найден)</i>";
    //alert(menu);
    //alert(type);
    var html = "";
    html = '<ul class="SimpleContextMenu">';
    html += "<li>Запрос " + scrid + " "+ title +"</li>";
    html += '<li><a href="#" onclick="copyToClip('+scrid+'); return false;">Копировать номер</a></li>';
    html += '<li><a href="#" onclick="query.openRecordId('+scrid+'); return false;">Открыть</a></li>';
    html += '<li><a href="#" onclick="query.appendId('+scrid+'); return false;">Добавить в список</a></li>';
    html += '<li><a href="#" onclick="openQueryPage('+scrid+'); return false;">Открыть в новом списке</a></li>';
    html += '</ul>';
    // Если есть что показать - показываем
    if (html) {
        var pos = defPosition(evt);
        //alert(pos.x + " " + pos.y + " " + html);
        menu.innerHTML = html;
        menu.style.top = pos.y;
        menu.style.left = pos.x;
        menu.style.display = "";
    }
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
