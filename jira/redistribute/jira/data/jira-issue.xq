xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" href="jira-style.css"/>
<link rel="stylesheet" href="menu.css"/>
<style>
<!--
.noteT {
font-weight: bold;
}


-->
</style>
<script src="qrc:/js/jquery.js"><!-- --></script>
<script src="jira-funcs.js"><!-- --></script>
<script src="w-controller.js"><!-- --></script>
<script src="t-decorator.js"><!-- --></script>
<script src="jira-decorator.js"><!-- --></script>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>

<a href="#top" class="totop hidden contLink" id="ttt">Наверх</a>
<p id="contextMenuId" class="contMenu"/>
<div class="contents" id="toc">Содержание:<span class="tocswitch">[<a href="javascript:toctog()" id="tocswitch"  >убрать</a>]</span>
<div id="contentsBody">
<ul>
{for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<li><a href="#notePoint{$i/@index}" class="contLink">{$i/string(@author)} ({$i/string(@createdate)})</a></li>
)
}
</ul>
<a href="#changes" class="contLink" id="tocLast">Changes</a>
</div>
</div>

<a name="top">{$scr/*/fields/field[@id = "issuekey"]/node()}. <b>{$scr/*/fields/field[@id = "summary"]/node()}</b></a>
<p id="debug" style="display: none"></p>
<p id="debug2" style="display: none"/>
	<div>
                <div><span class="noteTitle">{$scr/*/Description/string(@name)}</span> ({$scr/*/fields/field[@id = "creator"]/node()})</div>

		<blockquote><pre style="white-space: pre-wrap; font-family: serif" class="noteText">
			{string($scr/*/Description)}
		</pre></blockquote>
	</div>
{
for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<div index="{$i/@index}" class="note"><a name="notePoint{$i/@index}"/>
<span id="title{$i/@index}" class="noteTitle">{$i/string(@author)}</span> ({$i/string(@createdate)})
		<blockquote><pre style="white-space: pre-wrap; font-family: serif" class="noteText">
	{string($i)}
		</pre></blockquote>
</div>
)
}
<h4><a name="changes">Changes:</a></h4>
<table cols="3" width="100%">
<col width="150"/>
<col width="150"/>
{
for $i in $scr/*/history/change
order by (:xs:dateTime($i/string(@datetime)) descending,:) xs:integer($i/@index) descending
return (
<tr><td valign="top">{string($i/@createdate)}</td><td valign="top">{fn:concat(string($i/@author),":")}</td><td valign="top">{string($i/@action)}</td></tr>
)
}
</table>
<script type="text/javascript">
<!--
initJiraFunc();
//replaceKeys();
textDecorator.decorateAll();
//hideNeed();



function onScroll(e) {
  var off = toc.offsetTop + toc.offsetHeight;
  if(window.scrollY >= off)
    ttt.classList.remove('hidden');
  else
    ttt.classList.add('hidden');
}

document.addEventListener('scroll', onScroll);
-->
</script>
</body>}
</html> 
