xquery version "1.0" encoding "utf-8";
declare namespace functx = "http://www.functx.com";
declare function functx:if-empty
  ( $arg as item()? ,
    $value as item()* )  as item()* {

  if (string($arg) != '')
  then data($arg)
  else $value
 } ;
<html>
<head>
<link rel="stylesheet" href="../../../data/top.css"/>
<link rel="stylesheet" href="../../../data/editor.css"/>
<link rel="stylesheet" href="jira-style.css"/>
<link rel="stylesheet" href="../../../data/menu.css"/>
<script src="qrc:/js/jquery.js">
<!--
-->
</script>
<script src="../../../data/e-textarea.js"><!-- --></script>
<script src="../../../data/w-controller.js"><!-- --></script>
<script src="../../../data/t-decorator.js"><!-- --></script>
<script src="jira-decorator.js"><!-- --></script>
<script src="jira-funcs.js"><!-- --></script>
<style>
<!--
.test-note-block {
}
.test-note-text {
white-space: pre-wrap; 
font-family: serif;
-margin-left:30pt;
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

.debug {
display: visible;
position: fixed;
}


.btnsBeginEdit {
display: visible;
}

.btnsEndEdit {
display: none;
}

.title {
font-weight: bold;
}

-->
</style>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>
<p id="contextMenuId" class="contMenu" ></p>
<div></div>
<div class="topWr">
  <div class="topPanel">
    <div class="topRel">
      <span id="issueTitle">{$scr/*/fields/field[@role = "id"]/node()}.  <span class="title">{$scr/*/fields/field[@role = "title"]/node()}</span></span>
    </div>
  </div>
</div>
<p id="debug" class="debug" style="display:none"></p>
<div>
	<div id="blockDesc" class="blockText">
                <pre id="descText" class="noteText">{string($scr/*/Description)}</pre>
	</div>
</div>

{for $i in $scr/*/notes/note
order by xs:dateTime($i/@cdatetime), xs:integer($i/@index)
return (
<div class="note" index="{$i/@index}">
<div id="block{$i/@index}"  class="blockText">
  <div>
    <span id="jiraTitle{$i/@index}" class="noteTitle" index="{$i/@index}">{$i/string(@author)}</span>
    ({$i/string(@createdate)}) <span id="noteState{$i/@index}"></span>
  </div>
  <span style="display: none" id="savednote{$i/@index}" >{string($i/node())}</span>
  <pre class="noteText" id="note{$i/@index}" index="{$i/@index}" style="white-space: pre-wrap; font-family: serif" >{string($i/node())}</pre>
</div>
</div>)}
</body>}
<script  type="text/javascript">
<!--
textEditor = new TextAreaEditor();

if(typeof textEditor.init === "function")
  textEditor.init();
//replaceKeys();
initJiraFunc();
textDecorator.decorateAll();



-->
</script>
</html> 
