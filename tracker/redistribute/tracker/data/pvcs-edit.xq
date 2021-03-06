xquery version "1.0" encoding "utf-8";
<html>
<head>
  <link rel="stylesheet" href="pvcs-style.css"/>
  <link rel="stylesheet" href="../../../data/editor.css"/>
  <script src="qrc:/js/jquery.js"><!-- --></script>
  <script src="../../../data/editor-common.js"><!-- --></script>
  <script src="../../../data/e-widget.js"><!-- --></script>
  <script src="../../../data/w-controller.js"><!-- --></script>
  <script src="../../../data/t-decorator.js"><!-- --></script>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>
Запрос {$scr/*/fields/field[@name = "Id"]/node()} .  <b>{$scr/*/fields/field[@name = "Title"]/node()}</b>
<p id="debug" style="display: visible"></p>
	<div>
                <div>
			<img src="qrc:/images/edit.png" onclick="textEditor.editDescription()" class="btn" width="12" height="12" title="edit"/>
			<img src="qrc:/images/filesave.png" onclick="textEditor.saveDescription()" class="btn" width="12" height="12" title="save"/>
			<img src="qrc:/images/cancel.png" onclick="textEditor.cancelDescription()" class="btn" width="12" height="12" title="cancel"/>
			<!-- <img src="qrc:/images/edit.png" onclick="javascript:descEdit(); return false;" class="btn" width="12" height="12"/> -->
			<h4>Description:{$scr/*/Description/string(@name)}</h4>
		</div>
       		<blockquote><pre id="descText" class="descText">
			{string($scr/*/Description)}
		</pre></blockquote>
                <object type="application/scrnote" width="100%" height="400" id="widgetDesc" data="dddd" style="display:none">
                     <param name="noteIndex" value="-1" />
                </object>
	</div>
{for $i in $scr/*/notes/note order by xs:dateTime($i/@cdatetime), xs:integer($i/@index)
return (
  <div class="note" index="{$i/@index}">
    <div id="block{$i/@index}">
    	<!-- <img src="qrc:/images/edit.png" onclick="javascript:editNote({$i/@index}); return false;" class="btn" width="12" height="12"/> -->
    	<img src="qrc:/images/edit.png" onclick="textEditor.editNote({$i/@index})" class="btn" width="12" height="12"/>
    	<img src="qrc:/images/filesave.png" onclick="textEditor.saveNote({$i/@index})" class="btn" width="12" height="12"/>
    	<img src="qrc:/images/cancel.png" onclick="textEditor.cancelNote({$i/@index})" class="btn" width="12" height="12"/>
    	<img src="qrc:/images/editdelete.png" onclick="textEditor.removeNote({$i/@index})" class="btn" width="12" height="12"/>
    <div><span id="title{$i/@index}" class="noteTitle" index="{$i/@index}">{$i/string(@title)}</span>
    [ {$i/string(@author)} ({$i/string(@createdate)})] <span id="noteState{$i/@index}"></span>
    </div><span style="display: none" id="savednote{$i/@index}" >{string($i/node())}</span>

    <blockquote><pre style="white-space: pre-wrap; font-family: serif" id="note{$i/@index}" index="{$i/@index}" class="notetext">
    	{string($i/node())}
    </pre></blockquote>
    </div>
  </div>
)}
</body>}
<script  type="text/javascript">
<!--
//textEditor = new TextAreaEditor();
textEditor = new WidgetEditor();
textDecorator.decorateAll();

r();
init();
-->
</script>
</html> 
