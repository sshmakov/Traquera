function r()
{
        var list = document.getElementsByClassName('note');
	//debug2.innerHTML = 'D0';
	//debug2.innerText = 'Length '+ list.length;
        var l = list.length;
        var dd = 'aa ';
	for(var i = 0; i < l; i++)
        {
             var node = list[i];
            // var index = 0;
             var index = node.attributes['index'].value;
             //var title = 'eeeee'; 
             var title = document.getElementById('title'+index).innerText;
             dd = dd + index + ', ';
             var cla = 'an';
             if (title == 'Analitical Note') cla = 'an';
             else if (title == 'Engineering Note') cla = 'en';
             else if (title == 'Manager note') cla = 'mn';
             else if (title == 'Test Note') cla = 'tn';
             else if (title == 'Document Note') cla = 'dn';
             else if ((title == 'Release Note') || (title == 'HotFixEng')) cla = 'rn';
	     else if (title == 'Tech Support Note') cla = 'sn';
             else 
               cla = 'on';
             node.classList.add(cla)
             //node.className = 'note ' + cla;
         }
	//debug2.innerHTML = 'D2';
	//debug.innerHTML = dd;
	//debug2.innerHTML = 'D3';
}


function replaceNumbers()
{
  //var re = /([\s\b,.])#([0-9]+)/g;
  var re1 = /^#([0-9]+)/g;
  var re2 = /([\s\b,.^])#([0-9]+)/g;
  var pro1 = '<a href="javascript:query.openRecordId(';
  var pro2 = ');">';
  var pro3 = '<a href="javascript:query.appendId(';
  var pro4 = ');">';
  var ep = '</a>';
  var notes = document.getElementsByClassName('noteText');
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(re1,pro1+'$1'+pro2+'#$1'+ep+'<img src="plus-m.png" onClick="javascript:query.appendId($1);"/>');
    ns = ns.replace(re2,'$1'+pro1+'$2'+pro2+'#$2'+ep+'<img src="plus-m.png" onClick="javascript:query.appendId($2);"/>');
    notes[i].innerHTML = ns;
  }
}


function showPerforceFile(filename)
{
	//debug.innerHTML = 'starting';
	var WshShell = filename;
	//debug.innerHTML = 'creating';
	WshShell = new window.ActiveXObject("WScript.Shell");
	//debug.innerHTML = 'result';
	//debug2.innerHTML = "("+ WshShell + ")";
	WshShell.Run("%comspec% ");
	//debug.innerHTML = 'started';
}