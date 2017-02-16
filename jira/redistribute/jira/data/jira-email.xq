xquery version "1.0" encoding "utf-8";
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
<style>
<!--
.noteT {
font-weight: bold;
}

.noteText {
padding: 3px;
margin: 3px;
white-space: pre-wrap; 
font-family: serif;
font-size: small;
}

.descText {
padding: 3px;
margin: 3px;
white-space: pre-wrap; 
-font-family: serif;
}

.noteTitle {
font-weight: bold;
}

.noteDate {
font-size:small; 
font-style: italic; 
color: gray;
width: 300px;
}

.note {
padding: 2px 10px;
margin-top: 10px;
border: 1px solid gray;
}


-->
</style>
</head>
{for $scr in doc($scrs)/*/scr
(: let $scr := doc(fn:iri-to-uri(string($scrdoc))) :)
return
<body>
<hr/>
<a name="top">{$scr/fields/field[@id = "issuekey"]/node()}. <b>{$scr/fields/field[@id = "summary"]/node()}</b></a>
	<div>
                <div><span class="noteTitle">{$scr/Description/string(@name)}</span> ({$scr/fields/field[@id = "creator"]/node()})</div>

		<blockquote><pre style="white-space: pre-wrap; " class="noteText">
			{string($scr/Description)}
		</pre></blockquote>
	</div>
{
for $i in $scr/notes/note
order by xs:dateTime($i/string(@cdatetime)), xs:integer($i/@index)
return (
<div index="{$i/@index}" class="note"><a name="notePoint{$i/@index}"/>
<span id="title{$i/@index}" class="noteTitle">{$i/string(@author)}</span> ({$i/string(@createdate)})
		<blockquote><pre style="white-space: pre-wrap;" class="noteText">
	{string($i)}
		</pre></blockquote>
</div>
)
}
</body>}
</html> 
