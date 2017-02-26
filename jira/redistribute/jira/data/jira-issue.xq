xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" href="jira-style.css"/>
<link rel="stylesheet" href="../../../data/menu.css"/>
<style>
<!--
.noteT {
font-weight: bold;
}

.debug {
display: visible;
position: fixed;
}

.topIR {
    float: right;
    -z-index: 6;
    margin: 0px 10px;
    line-height: 50px;
    white-space: pre;
}


-->
</style>
<script src="qrc:/js/jquery.js"><!-- --></script>
<script src="../../../data/w-controller.js"><!-- --></script>
<script src="../../../data/t-decorator.js"><!-- --></script>
<script src="../../../data/toc.js"><!-- --></script>
<script src="jira-decorator.js"><!-- --></script>
<script src="jira-funcs.js"><!-- --></script>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>

<a href="#top" class="totop hidden contLink contMenu2" id="ttt">Наверх</a>
<p id="contextMenuId" class="contMenu"/>
<div class="contents contMenu2" id="toc">Содержание:<span class="tocswitch">[<a href="javascript:toctog()" id="tocswitch"  >убрать</a>]</span>
<div id="contentsBody">
<ul>
{for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<li><a href="#notePoint{$i/@index}" class="contLink">{$i/string(@author)} ({$i/string(@createdate)})</a></li>
)
}
<li><a href="#links" class="contLink">{$scr/*/fields/field[@id = "issuelinks"]/string(@name)}</a></li>
<li><a href="#subtask" class="contLink">{$scr/*/fields/field[@id = "subtasks"]/string(@name)}</a></li>
<li><a href="#changes" class="contLink" id="tocLast">Changes</a></li>
</ul>
</div>
</div>
<div>
<div class="topWr">
  <div class="topPanel">
    <div class="topRel">
      <span id="issueTitle">
          <a href="#" class="jiraExtLink" target="_blank">{$scr/*/fields/field[@role = "id"]/node()}</a>.  
          <span class="title">{$scr/*/fields/field[@role = "title"]/node()}</span>
      </span>
    </div>
    <div class="topIR" id="parentLink">-</div>
  </div>
</div>
<p id="debug" style="display: none"></p>
<p id="debug2" style="display: none"><!-- --></p>
<a name="top" class="issueAnchor"><!-- --></a>
<div class="topMargin">
        <p class="noteTitle"><span class="noteAuthor">{$scr/*/Description/string(@name)}</span> ({$scr/*/fields/field[@id = "creator"]/node()})</p>
        <blockquote><pre class="noteText">
                {string($scr/*/Description)}
        </pre></blockquote>
</div>
{
for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<div index="{$i/@index}" class="note"><a name="notePoint{$i/@index}" class="issueAnchor"><!-- --></a>
<p class="noteTitle"><span id="title{$i/@index}" class="noteAuthor">{$i/string(@author)}</span> ({$i/string(@createdate)})</p>
		<blockquote><pre class="noteText">
	{string($i)}
		</pre></blockquote>
</div>
)
}
<h4><a name="links" class="issueAnchor"><!-- --></a>{$scr/*/fields/field[@id = "issuelinks"]/string(@name)}</h4>
<div id="linksBody"><!-- --></div>
<h4><a name="subtask" class="issueAnchor"><!-- --></a>{$scr/*/fields/field[@id = "subtasks"]/string(@name)}</h4>
<div id="tasksBody"><!-- --></div>
<h4><a name="changes" class="issueAnchor"><!-- --></a>Changes:</h4>
<div id="changelog"><!-- --></div>

<script type="text/javascript">
<!--
initJiraFunc();
window.textDecorator = new JiraDecorator();
textDecorator.decorateAll();


function onScroll(e) {
  var off = toc.offsetTop + toc.offsetHeight;
  if(window.scrollY >= off)
    ttt.classList.remove('hidden');
  else
    ttt.classList.add('hidden');
}

document.addEventListener('scroll', onScroll);

if(global.tochidden)
	tocshow(0);

-->
</script>
</div>
</body>}
</html> 
