function ddd2()
{
  debug2.innerText = "Debug2";
}

function tocshow(show)
{
	if(show)
	{
		contentsBody.style.display = '';
		global.tochidden = 0;
		tocswitch.innerText = "убрать";
	}
	else
	{
		contentsBody.style.display = 'none';
		global.tochidden = 1;
		tocswitch.innerText = "показать";
	}
}

function toctog()
{
	if(global.tochidden)
		tocshow(1);
	else
		tocshow(0);
}

function hideNeed()
{
	if(global.tochidden)
		tocshow(0);
	else
		tocshow(1);
}


function testShell(cmd)
{
  global.shell(cmd);
}


function showPerforce(file)
{
  //debug2.innerText = file;
  var cmd = 'cmd /c start p4v -t history -s "'+file+'"';
  global.shellLocale(cmd,'UTF-8');
}

function openUrl(url)
{
  //debug2.innerText = url;
  global.shell('cmd /c start iexplore "'+url+'"');
}


// 187985, 165379, 182772

function replacePerforce()
{
  var re1 = /(\/\/RS[^<>"'\n]+)(#[0-9]+)/g;
  var pro1 = '<a href="';
  var pro2 = '" onclick="javascript:showPerforce('+"'";
  var pro3 = "'"+');">';
  var ep = '</a>';
  var notes = document.getElementsByClassName('noteText');
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(re1,pro1+'$1'+pro2+'$1'+pro3+'$1$2'+ep);
    notes[i].innerHTML = ns;
  }
}


//  132513, 132042, 169475 

function replaceHTTP()
{
  var re1 = /(https*:\/\/[^ \s"']+)/g;
  //var pro1 = '<a href="javascript:openUrl('+"'";
  //var pro2 = "'"+');">';
  var pro1 = '<a href="';
  var pro2 = '" onclick="javascript:openUrl('+"'";
  var pro3 = "'"+');">';
  var ep = '</a>';
  var notes = document.getElementsByClassName('noteText');
  for(var i = 0; i < notes.length; i++)
  {
    var s = notes[i].innerHTML;
    var ns;
    ns = s.replace(re1,pro1+'$1'+pro2 +'$1'+pro3+'$1'+ep);
    notes[i].innerHTML = ns;
  }
}
