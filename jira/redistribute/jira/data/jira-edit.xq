xquery version "1.0" encoding "utf-8";
<html>
<head>
<link rel="stylesheet" href="style.css"/>
<link rel="stylesheet" href="jira-style.css"/>
<script src="qrc:/js/jquery.js">
<!--
-->
</script>
<script src="jira-funcs.js"><!-- --></script>
<script src="e-inplace.js"><!-- --></script>
<script>
<!--
textEditor = new InplaceEditor();
-->
</script>
<style>
<!--
.test-note-block {
}
.test-note-text {
white-space: pre-wrap; 
font-family: serif;
margin-left:30pt
}

.jiraIssueLink {
font-weight: bold;
}


.contextMenu {
display: none;
background-color: rgb(249, 249, 249);
    position:absolute; 
    top:0; 
    left:0; 
    display:none; 
    float:left;
    display: block;
    margin: 0px;
    padding: 0px;
    box-shadow: 3px 3px 10px rgba(0,0,0,0.5); 
    z-index: 20;
}

-->
</style>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>
<p id="contextMenuId" class="contextMenu2"></p>
<div></div>
Запрос {$scr/*/fields/field[@role = "id"]/node()} .  <b>{$scr/*/fields/field[@role = "title"]/node()}</b> 
<input type="button" onclick="clickTest()" value="Test" id="testbtn"/>
<p id="debug" style="display: visible"></p>
	<div>
                <div id="blockDesc">
                <div>
			<div class="btnsEdit">
				<img src="qrc:/images/edit.png" onclick="textEditor.editDescription()" class="btn" width="12" height="12" title="edit"/>
				<img src="qrc:/images/filesave.png" onclick="textEditor.saveDescription()" class="btn" width="12" height="12" title="save"/>
				<img src="qrc:/images/cancel.png" onclick="textEditor.cancelDescription()" class="btn" width="12" height="12" title="cancel"/>
			</div>
			<!-- <img src="qrc:/images/edit.png" onclick="javascript:descEdit(); return false;" class="btn" width="12" height="12"/> -->
			<h4>{$scr/*/Description/string(@name)} ({$scr/*/fields/field[@id = "creator"]/node()}):</h4>
		</div>
       		<pre id="descText" class="test-note-text">{string($scr/*/Description)}
		</pre>
		</div>
	</div>
{for $i in $scr/*/notes/note
order by xs:dateTime($i/@cdatetime), xs:integer($i/@index)
return (
<div class="note" index="{$i/@index}">
<div id="block{$i/@index}">
	<!-- <img src="qrc:/images/edit.png" onclick="javascript:editNote({$i/@index}); return false;" class="btn" width="12" height="12"/> -->
	<div class="btnsEdit">
		<img src="qrc:/images/edit.png" onclick="textEditor.editNote({$i/@index})" class="btn" width="12" height="12"/>
		<img src="qrc:/images/filesave.png" onclick="textEditor.saveNote({$i/@index})" class="btn" width="12" height="12"/>
		<img src="qrc:/images/cancel.png" onclick="textEditor.cancelNote({$i/@index})" class="btn" width="12" height="12"/>
	</div>
	<div class="btnsView">
		<img src="qrc:/images/editdelete.png" onclick="textEditor.removeNote({$i/@index})" class="btn" width="12" height="12"/>
	</div>
<div>
<span id="title{$i/@index}" class="noteTitle" index="{$i/@index}">{$i/string(@author)}</span>
({$i/string(@createdate)}) <span id="noteState{$i/@index}"></span>
</div><span style="display: none" id="savednote{$i/@index}" >{string($i/node())}</span>
<pre class="test-note-text" id="note{$i/@index}" index="{$i/@index}" style="white-space: pre-wrap; font-family: serif" >{string($i/node())}
</pre>
</div>
<!-- 
<script  type="text/javascript">{fn:concat("title",$i/@index, ".addEventListener(""input"", changedTitle, false);")}</script>
<script  type="text/javascript">{fn:concat("note",$i/@index, ".addEventListener(""input"", changedText, false);")}</script>
-->
</div>)}
</body>}
<script  type="text/javascript">
<!--
var JiraRecord = editor.record;
document.getElementById('issuelink').setAttribute("href","https://jira.allrecall.com/browse/TTT-1");
r();
init();

function clickTest()
{
	//document.getElementById('testbtn').value = "Test 2";
	replaceKeys();
	return false;
}


function replaceKeys()
{
  var key = "TTT" //record.project.jiraProjectKey();
  debugInfo(key);
  var re1 = new RegExp("(\\b" + key + "-[0-9]+\\b)", "g");
  var pro1 = '<span class="jiraIssueLink">';
  var pro2 = '</span>';
  $(".test-note-text").each(function(index, item) {
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

function menuSCR(key, evt) { 
    var scrid = key.replace(/\w+-([0-9]+)/,'$1');
    // Блокируем всплывание события contextmenu
    evt = evt || window.event;
    evt.cancelBubble = true;
    // Показываем собственное контекстное меню
    var menu = contextMenuId;
    var rec = {};
    var title = "(-)", fix = "", state = "";
    
    var project = record.project;
    var db = project.db;
    var rec = db.get('rest/api/2/issue/'+key); // global.getRecord(scrid);
    
    //var rec = global.getRecord(scrid);
    alert(JSON.stringify(rec));
    if(rec)
    {
        title = rec.summary;
        state = rec.state;
        fix = rec.value("Fixed In Build Eng.");
    }
    else
        title = "<i>(не найден)</i>";
    if(fix && fix != "0")
    {
	state = state + " " + fix;
    }
    //alert(menu);
    //alert(type);
    var html = "";
    html = '<ul class="SimpleContextMenu">';
    html += "<li><b>Запрос " + scrid + '</b> <span class="shortInfo">' + state +"</span> 123 <br/>"
            + title + "</li>";
    html += '<li><a href="#" onclick="copyToClip('+scrid+'); return false;">Копировать номер</a></li>';
    html += '<li><a href="#" onclick="query.openRecordId('+scrid+'); return false;">Открыть</a></li>';
    html += '<li><a href="#" onclick="query.appendId('+scrid+'); return false;">Добавить в список</a></li>';
    html += '<li><a href="#" onclick="openQueryPage('+scrid+'); return false;">Открыть в новом списке</a></li>';
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
	alert(JSON.stringify(pos));
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

-->
</script>
</html> 
