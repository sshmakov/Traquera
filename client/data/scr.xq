xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<link rel="stylesheet" href="style.css"/>
<script src="qrc:/js/jquery.js"><!-- --></script>
<style>
<!--
.noteT {
font-weight: bold;
}

.shortInfo {
color: rgb(0,128,0);

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
margin: 60px 5px 5px 5px; 
max-width: 50%;
}

.ref {
    position: float;
    top: 0px;
}

.ref2 {
    position: relative;
    top: -55px;
}

-->
</style>
<script src="funcs.js" LANGUAGE="JScript"><!-- --></script>
<script src="funcs2.js"><!-- --></script>
<script src="funcs3.js"><!-- --></script>
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
function replaceISup()
{
  var url = 'http://support.softlab.ru/Portal/InterSupport/Redirector/topic.asp?Id=';
  var re1 = /(I-)(0*)([1-9][0-9]*)/ig;
  var pro1 = '<a href="' + url+ '$3" '
	+ 'onclick="javascript:openUrl(\''+ url + '$3\')">$1$2$3</a>';
  var re2 = /(I-Support[ -]+)(0*)([1-9][0-9]*)/ig;
  var pro2 = '<a href="' + url+ '$3" '
	+ 'onclick="javascript:openUrl(\''+ url + '$3\')">$1$2$3</a>';

  var notes = document.getElementsByClassName('replNums'); //noteText
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(re1, pro1);
    ns = ns.replace(re2, pro2);
    notes[i].innerHTML = ns;
  }
}

function replaceHF()
{
  //var re = /([\s\b,.])#([0-9]+)/g;
  var urlV6 = "file://corvus/Dst_build/RSBankV6/Work.60/RsBank/HFX";
  var url55 = "file://corvus/Dst_build/RSBank55/HFX";
  var reV6 = /(HotFix )([1-9][0-9][0-9][0-9]).([0-9][0-9]).([0-9]*)_([0-9]*)/ig;
  var proV6 = '<a href="' + urlV6+ '_$2/$2.$3.$4/$5" onclick="javascript:openUrl(\'' + urlV6 + '_$2/$2.$3.$4/$5\')" target="_blank">$1$2.$3.$4_$5</a>';
  var re55 = /(хотфикс )(5).([1-9])([0-9]*).([0-9][0-9]*).([0-9]*)_([0-9]*)/ig;
  var pro55 = '<a href="' + url55+ '_$2$3.$5/$6/$7" onclick="javascript:openUrl(\'' + urlV6 + '_$2$3.$5/$6/$7\')" target="_blank">$1$2.$3$4.$5.$6_$7</a>';

  var notes = document.getElementsByClassName('replNums'); //noteText
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(reV6, proV6);
    ns = ns.replace(re55, pro55);
    notes[i].innerHTML = ns;
  }
}



$(document).ready( function(){
    r();
    replaceNumbers();
    replacePerforce();
    replaceHTTP();
    replaceISup();
    replaceHF();
    hideNeed();
});


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
