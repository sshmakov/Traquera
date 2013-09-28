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
"}
</style>
<script  type="text/javascript">
{"
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


"}
</script>
</head>
<body>
Запрос {doc($scrdoc)/*/fields/field[@name = "Id"]/node()} .  <b>{doc($scrdoc)/*/fields/field[@name = "Title"]/node()}</b>
<p id="debug" style="display: none"/>
{                                           
for $d in doc($scrdoc)/*/Description
return (
	<div id="desc">
		<h4>{$d/string(@name)}</h4>
		<blockquote><pre style="white-space: pre-wrap; font-family: serif" contentEditable="true" id="descText">
			{$d/node()}
		</pre></blockquote>
                <script  type="text/javascript">descText.addEventListener("input", changedDesc, false);</script>
	</div>
)
}
{
for $i in doc($scrdoc)/*/notes/note
order by xs:dateTime($i/@cdatetime)
return (
<div class="note" index="{$i/@index}">
<p>
<span id="title{$i/@index}" class="noteTitle" contentEditable="{"false" (: $i/@editable :)}" index="{$i/@index}">{$i/string(@title)}</span> 
<span class="noteInfo">{$i/string(@author)}</span>
</p>
<p>
<div class="nd">
<div class="noteDate">
  {$i/string(@createdate)}
</div>
</div>
</p>
<blockquote><pre style="white-space: pre-wrap; font-family: serif" contentEditable="{$i/@editable}" id="note{$i/@index}" index="{$i/@index}">
	{string($i/node())}
</pre></blockquote>
<script  type="text/javascript">{fn:concat("title",$i/@index, ".addEventListener(""input"", changedTitle, false);")}</script>
<script  type="text/javascript">{fn:concat("note",$i/@index, ".addEventListener(""input"", changedText, false);")}</script>
</div>
)
}
</body>
<script  type="text/javascript">{"r();"}</script>
</html> 