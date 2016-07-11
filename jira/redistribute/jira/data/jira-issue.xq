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

.topWr {
    position: relative;
    min-height: 40px;
    z-index: 10;
    background: none repeat scroll 0% 0% #FFF;
    margin: -10px 0px 0px 0px;
    -padding: 10px 0px;
}

.topPanel {
position: fixed;
width: 100%;
    height: 60px;
    margin: -10px -10px;
    border-bottom: 1px solid #888;
    box-shadow: 2px 2px 5px rgba(0,0,0,0.5); 
    -background: none repeat scroll 0% 0% #EEF;
background: -webkit-linear-gradient(top, rgb(248, 248, 248) 0%, rgb(226, 226, 226) 100%);

 -display: inline-block;
    display: table;
    padding: 10px 0px 0px 0px;
}

.topRel {
    -height:100%;
    position: relative;
    z-index: 6;
    display: table;
    -min-height: 60px;
    -max-width: 90%;
    -background: none repeat scroll 0% 0% #EEE;
    -box-shadow: 0px 0px 0px 1px rgba(0, 0, 0, 0.15);
    padding: 0px 0px 0px 10px;
    -padding: 10px 10px 10px 10px;
    margin: 0px 10px;
    -min-width: 1000px;
-display:inline-block;
display: table-cell;
vertical-align: middle;
}

.topDummy {
    position: relative;
    z-index: 6;
    padding: 0px 0px 0px 0px;
    margin: 0px 10px;
    display: table-cell;
    -display:inline-block;
    height: 50px;
    -height:100%;
    width:10px;
    vertical-align:middle;
}

.topIR {
    float: right;
    z-index: 6;
    margin: 0px 10px;
    line-height: 50px;
    white-space: pre;
}

.contMenu2 {
margin-left: 5px; 
margin-right: 5px; 
margin-bottom: 5px; 
max-width: 50%;
}

.underTitleBox {
margin-top: 60px; 
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
<div class="contents contMenu2 underTitleBox" id="toc">Содержание:<span class="tocswitch">[<a href="javascript:toctog()" id="tocswitch"  >убрать</a>]</span>
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

 <div class="topWr">
            <div class="topPanel">
                <div class="topRel">
<a name="top">Запрос {$scr/*/fields/field[@id = "issuekey"]/node()}. <b>{$scr/*/fields/field[@id = "summary"]/node()}</b></a>
                </div>
                <div class="topDummy">
                    <span class="hidden">.</span>
                </div>
            </div>
        </div>
	<p/>
	<div class="">
                <p class="noteHeader"><span class="noteTitle"><!-- {$scr/*/Description/string(@name)} -->Description</span> ({$scr/*/fields/field[@systemName = "creator"]/node()})</p>

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
