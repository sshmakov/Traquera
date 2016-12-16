xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" href="../../../data/style.css"/>
<script src="qrc:/js/jquery.js"><!-- --></script>
<script src="../../../data/w-controller.js"><!-- --></script>
<script src="../../../data/t-decorator.js"><!-- --></script>
<script src="pvcs-decorator.js"><!-- --></script>
<script src="../../../data/funcs.js"><!-- --></script>
</head>
{let $scr := doc(fn:iri-to-uri(string($scrdoc)))
return
<body>
<a name="top"/>
<!-- Содержание -->
<a href="#top" class="totop hidden contLink contMenu2" id="ttt">Наверх</a>
<p id="contextMenuId" class="contMenu"/>
<div class="contents contMenu2" id="toc">Содержание:<span class="tocswitch">[<a href="javascript:toctog()" id="tocswitch"  >убрать</a>]</span>
<div id="contentsBody">
<ul>
{for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<li><a href="#notePoint{$i/@index}" class="contLink">{$i/string(@title)} [{$i/string(@createdate)} ({$i/string(@author)})]</a></li>
)
}
</ul>
<a href="#changes" class="contLink" id="tocLast">Changes</a>
</div>
</div>


<!-- Верхняя панель -->
<div class="topWr">
  <div class="topPanel">
    <div class="topRel">
    Запрос {$scr/*/fields/field[@name = "Id"]/node()}. <b>{$scr/*/fields/field[@name = "Title"]/node()}</b>
    </div>
   
		{if($scr/*/fields/field[@name = "Initial Request"]/text()) then (
                <div class="topIR replNums"> (по {$scr/*/fields/field[@name = "Initial Request"]/text()})</div>
                ) else (<div class="topDummy"><span class="hidden">.</span></div>)}
  </div>
</div>



<!-- Остальной текст -->
<p id="debug"><!-- --></p>
<p id="debug2"/>
	<div>
                <p class="noteHeader"><span class="noteTitle"><!-- {$scr/*/Description/string(@name)} -->Description</span> ({$scr/*/fields/field[@name = "Submitter"]/node()})</p>

		<pre style="white-space: pre-wrap; font-family: serif" class="noteText replNums">
			{string($scr/*/Description)}
		</pre>
	</div>
{
for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<div index="{$i/@index}" class="note"><a name="notePoint{$i/@index}" class="ref2"><!-- --></a>
<p  class="noteHeader"><span id="title{$i/@index}" class="noteTitle">{$i/string(@title)}</span> [{$i/string(@createdate)} ({$i/string(@author)})]</p>
	<pre class="noteText replNums">{string($i)}</pre>
</div>
)
}
<h4><a name="changes" class="ref2"><!-- --></a>Changes:</h4>
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
window.textDecorator = new TrkDecorator();
textDecorator.decorateAll();


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
