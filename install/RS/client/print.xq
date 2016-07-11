<html>
<head>
<style type="text/css" media="print">
{"
.page {
  page-break-after: always;
}
"}
</style>
<style type="text/css">
{"
body {
font-size: 12pt;
font-family: sans-serif;
}
.emailText {
}
.record {
}
.dummy {
}
.fields {
border-spacing: 0px;
border-collapse: collapse;
}
.fieldGroup {
color: gray; 
font-size: 8pt;
}
.fieldName {
font-weight: bold;
}
.fieldValue {
}
.description {
white-space: pre-wrap; 
}
.noteTitle {
font-weight: bold; 
}
.note {
white-space: pre-wrap; 
afont-size: 12pt;
afont-family: serif;
}
"}
</style>
</head>
<body class="abody">
{for $rec in doc($scrs)/*/scr
return 
 (<div class="page"> 
    <div class="record">
    <p><b>Запрос #{$rec/fields/field[@name = "Id"]/node()}</b>. <i>{$rec/fields/field[@name = "Title"]/node()}</i></p>
    <table cols="4" class="fields" width="100%">
    <col width="20%"/>
    <col width="30%"/>
    <col width="20%"/>
    <col width="30%"/>
     {for $panel in doc($def)/*/panels/panel
         return (
	<span class="dummy">
               <tr class="fieldGroup"> <td colspan="4" align="left">{ $panel/string(@title) }</td> </tr>
               <span class="dummy">
                 {for $row in (0 to fn:count($panel/field) idiv 2) 
                 return 
                    <tr class="fields">
                    {for $field in $panel/field[fn:position() = $row*2+1 or fn:position() = $row*2+2]
                      (: let $pos := ($panel/field/fn:position()) :)
                      return (
                         <span class="dummy">
                           {for $d in $rec/*/field[@name = $field/string($field/@name) (: and @name != "Id" and @name != "Title" :)] (:   :)
                             return (
                               <span class="dummy">
                                 <td align="left" class="fieldName">{$field/string(@name)}:</td>
                                   	<td class="fieldValue">{string($d/node())}</td>
                               </span>
                             )
                           }
                         </span>
                     )
                    }
                 </tr>  
              }
              </span>
         </span>)
     }
    </table>
    <p class="noteTitle">Description:</p>
    <blockquote><PRE class="description">
    	{$rec/Description/node()}
    </PRE></blockquote>
    {for $note in $rec/notes/note
      order by xs:dateTime(fn:concat(
          fn:substring($note/string(@createdate), 7,4), "-",
          fn:substring($note/string(@createdate), 4,2), "-",
          fn:substring($note/string(@createdate), 1,2), "T",
    	  fn:string-join((for $kkk in 0 to 7 - fn:string-length(fn:substring($note/string(@createdate), 12)) return xs:string($kkk)), ""),
          fn:substring($note/string(@createdate), 12))), 
          xs:integer($note/@index)
      return (<div>
         <p class="noteTitle">{$note/string(@title)} [{$note/string(@author)}, {$note/string(@createdate)}]</p> <!-- {fn:string-join(($note/string(@author),$note/string(@createdate)),' ')} -->
         <blockquote><PRE  class="note">
             {$note/node()}
         </PRE></blockquote>
      </div>)
    }
  </div>
  <p>Changes:</p>
  <table cols="3" width="100%">
  <col width="150"/>
  <col width="150"/>
  {
  for $i in $rec/history/change
  order by (:xs:dateTime($i/string(@datetime)) descending,:) xs:integer($i/@index) descending
  return (
  <tr><td valign="top">{string($i/@createdate)}</td><td valign="top">{fn:concat(string($i/@author),":")}</td><td valign="top">{string($i/@action)}</td></tr>
  )
  }
  </table>
  <hr/>
  </div>)
}
</body>
</html> 