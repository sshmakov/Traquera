xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" href="style.css"/>
<style>
<!--
.noteT {
font-weight: bold;
}


-->
</style>
<script src="funcs.js" LANGUAGE="JScript">
<!--
-->
</script>
<script src="funcs2.js">
<!--
-->
</script>
<script src="funcs3.js">
<!--
-->
</script>
<script src="qrc:/js/jquery.js">
<!--
-->
</script>
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
<li><a href="#notePoint{$i/@index}" class="contLink">{$i/string(@title)} [{$i/string(@createdate)} ({$i/string(@author)})]</a></li>
)
}
</ul>
<a href="#changes" class="contLink" id="tocLast">Changes</a>
</div>
</div>
<a name="top">Запрос {$scr/*/fields/field[@name = "Key"]/node()}. <b>{$scr/*/fields/field[@name = "Summary"]/node()}</b></a>
<p id="debug"></p>
<!--
<a href="#" onclick="javascript:global.shell('C:\\Windows\\system32\\cmd.exe /c c:\\1\\1.bat лорлрлрлор'); return false;">Run</a> 
<a href="#" onclick="javascript:global.shellLocale('C:\\Windows\\system32\\cmd.exe /c c:\\1\\1.bat лорлрлрлор'); return false;">RunLocale</a> 
<a href="#" onclick="javascript:global.shellLocale('C:\\Windows\\system32\\cmd.exe /c c:\\1\\1.bat лорлрлрлор','IBM 866'); return false;">Run866</a> 
<a href="#" onclick="javascript:global.shellLocale('C:\\Windows\\system32\\cmd.exe /c c:\\1\\1.bat лорлрлрлор','Windows-1251'); return false;">Run1251</a> 
<a href="#" onclick="javascript:global.shellLocale('C:\\Windows\\system32\\cmd.exe /c c:\\1\\1.bat лорлрлрлор','UTF-8'); return false;">RunUTF-8</a> 
-->
<p id="debug2"/>
	<div>
                <div><span class="noteTitle"><!-- {$scr/*/Description/string(@name)} -->Description</span> ({$scr/*/fields/field[@name = "Submitter"]/node()})</div>

		<blockquote><pre style="white-space: pre-wrap; font-family: serif" class="noteText">
			{string($scr/*/Description)}
		</pre></blockquote>
	</div>
{
for $i in $scr/*/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<div index="{$i/@index}" class="note"><a name="notePoint{$i/@index}"/>
<span id="title{$i/@index}" class="noteTitle">{$i/string(@title)}</span> [{$i/string(@createdate)} ({$i/string(@author)})]
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
r();
replaceNumbers();
replacePerforce();
replaceHTTP();
hideNeed();

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
