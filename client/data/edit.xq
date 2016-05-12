xquery version "1.0" encoding "utf-8";
<html>
<head>
<style>
{"
.noteTitle {
font-weight: bold;
}
.noteInfo {
}

.nd {
!display: block;
!position: absolute;
!right: 20px;
}
.noteDate {
!display: inline-block;
!position: relative;
!top: -40px;
!az-index: 2;
!text-align: right;
font-size:small; 
font-style: italic; 
color: gray;
width: 300px;
}

.noteText {
white-space: pre-wrap; 
font-family: serif;
}

.note {
padding: 2px 10px;
margin-top: 10px;
border: 1px solid gray;
-webkit-border-radius: 20px;
}

.an {
background: -webkit-linear-gradient(top, rgb(250, 243, 210) 0%, #fefefe 100%);
dbackground-color: rgb(250, 243, 210);
}
.en {
background-color: rgb(206, 226, 211);
}
.mn {
background-color: rgb(249, 228, 151);
}
.tn {
background-color: rgb(180, 200, 255);
}
.dn {
background-color: rgb(220, 228, 151);
}
.rn {
background-color: rgb(192,192,192);
}
.sn {
background-color: rgb(255,255,120);
}
.editable {
abackground-color: white;
border: 1px inset lightgray;
padding: 3px
}

.btn {
float: right;
top: 5 px;
right: 5 px;
margin: 5 px;
padding: 2px 10px;
}
"}
</style>
<script  type="text/javascript">
<!--
function setNote(index,title,text)
{
	editor.setNote(index,title,text);
	return false;
}
function changedDesc(evt)
{
	editor.setDescription(evt.srcElement.innerText);
}


function changedText(evt)
{
        var index = evt.srcElement.attributes['index'].value;
//	var note = document.getElementById('note' + index).innerText;
	var title = document.getElementById('title' + index).innerText;
	editor.setNote(index, title, evt.srcElement.innerText);
}

function changedTitle(evt)
{
        var index = evt.srcElement.attributes['index'].value;
	var note = document.getElementById('note' + index).innerText;
	editor.setNote(index, evt.srcElement.innerText, note);
}

function r() 
{
        var list = document.getElementsByClassName('note');
	//debug.innerText = 'Length '+ list.length;
	for(var i = 0; i != list.lenght; i++)
        {
             var node = list[i];
            // var index = 0;
             var index = node.attributes['index'].value;
             var title = document.getElementById('title'+index).innerText;
	//debug.innerText = 'Title: ""'+ title +'""';

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
             node.className = 'note ' + cla;
	//debug.innerText = 'Cla: ""'+ node.className +'""';

	     var textNode = document.getElementById('note'+index);
	     if(textNode.attributes['contentEditable'].value == 'true')
		textNode.className = 'editable';
	     var titleNode = document.getElementById('title'+index);
	     if(titleNode.attributes['contentEditable'].value == 'true')
		titleNode.className = 'noteTitle editable';
	     else
		titleNode.className = 'noteTitle';
         }
}

function showEditor(index, visible)
{
  var sIndex;
  if(index === -1)
	sIndex = "Desc";
  else
	sIndex = index;

  var blockNode = document.getElementById('block'+sIndex);
  var widgetNode = document.getElementById('widget'+sIndex);
  if(visible)
  {
    blockNode.style.display = "none";
    widgetNode.style.display = "";
  }
  else
  {
    blockNode.style.display = "";
    widgetNode.style.display = "none";
  }
}


function initNoteWidget(index)
{
  if(editor.enableModify())
    showEditor(index,true);
}

function connectWidget(widget)
{
  widget.submitTriggered.connect(submitNoteWidget);
  widget.cancelTriggered.connect(cancelNoteWidget);
}

function closeNoteWidget(index)
{
  showEditor(index,false);
}

function submitNoteWidget(index)
{
  //var blockNode = document.getElementById('block'+index);
  if(index === -1)
  {
     var newDesc = widgetDesc.noteText();
     if(editor.setDescription(newDesc))
     {
       descText.innerText = newDesc;
       showEditor(-1,false);
     }
  }
  else
  {
     var widgetNode = document.getElementById('widget'+index);
     var newTitle = widgetNode.noteTitle();
     var newText = widgetNode.noteText();
     if(editor.setNote(index, newTitle, newText))
     {
       var titleNode = document.getElementById('title'+index);
       var textNode = document.getElementById('note'+index);
       titleNode.innerText = newTitle;
       textNode.innerText = newText;
       showEditor(index,false);
     }
  }
}

function cancelNoteWidget(index)
{
  var blockNode = document.getElementById('block'+index);
  var widgetNode = document.getElementById('widget'+index);
  blockNode.style.display = "";
  widgetNode.style.display = "none";
}

function onNoteChanged(index,title,text)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  titleNode.innerText = title;
  textNode.innerText = text;
}

function editNote(index)
{
  editor.noteChanged.connect(onNoteChanged);
  editor.startEditNote(index);
  var state = document.getElementById('noteState'+index);
  state.innerText = 'editing';
  //var titleNode = document.getElementById('title'+index);
  //var textNode = document.getElementById('note'+index);
  //titleNode.disabled = "";
  //textNode.contentEditable = true;
  //debug.innerText = 'Debug';
}

function saveNote(index)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  editor.setNote(index,titleNode.value,textNode.innerText);
  titleNode.disabled = true;
  textNode.contentEditable = false;
  titleNode.value = editor.noteTitle(index);
  textNode.innerText = editor.noteText(index);
	return false;
}

function resetNote(index)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  titleNode.disabled = true;
  textNode.contentEditable = false;
  titleNode.value = editor.noteTitle(index);
  textNode.innerText = editor.noteText(index);
  return false;
}

function init()
{
  if(record.isInsertMode())
  {
    initNoteWidget(-1);
  }
  descEdit();
}

function descEdit()
{
    editor.startEditDescription();
}
-->
</script>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>
Запрос {$scr/*/fields/field[@name = "Id"]/node()} .  <b>{$scr/*/fields/field[@name = "Title"]/node()}</b>
<p id="debug" style="display: none"/>
	<div>
                <div><img src="qrc:/images/edit.png" onclick="javascript:descEdit(); return false;" class="btn" width="12" height="12"/><h4>{$scr/*/Description/string(@name)}</h4></div>
       		<blockquote><pre style="white-space: pre-wrap; font-family: serif" id="descText">
			{string($scr/*/Description)}
		</pre></blockquote>
                <object type="application/scrnote" width="100%" height="400" id="widgetDesc" data="dddd" style="display:none">
                     <param name="noteIndex" value="-1" />
                </object>
	</div>
{for $i in $scr/*/notes/note
order by xs:dateTime($i/@cdatetime)
return (
<div class="note" index="{$i/@index}">
<div id="block{$i/@index}">
<img src="qrc:/images/edit.png" onclick="javascript:editNote({$i/@index}); return false;" class="btn" width="12" height="12"/>
<p><span id="title{$i/@index}" class="noteTitle" index="{$i/@index}">{$i/string(@title)}</span>
[ {$i/string(@author)} ({$i/string(@createdate)})] <span id="noteState{$i/@index}"></span>
</p>

<blockquote><pre style="white-space: pre-wrap; font-family: serif" id="note{$i/@index}" index="{$i/@index}">
	{string($i/node())}
</pre></blockquote>
</div>
<!-- 
<script  type="text/javascript">{fn:concat("title",$i/@index, ".addEventListener(""input"", changedTitle, false);")}</script>
<script  type="text/javascript">{fn:concat("note",$i/@index, ".addEventListener(""input"", changedText, false);")}</script>
-->
<object type="application/scrnote" width="100%" height="400" id="widget{$i/@index}" data="dddd" style="display:none">
     <param name="noteIndex" value="{$i/@index}" />
</object>
</div>)}
</body>}
<script  type="text/javascript">
<!--
//r();
init();
-->
</script>
</html> 
