function debugInfo(line)
{
	debug.textContent = line;
}

function onNoteChanged(index,title,text)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  titleNode.textContent = title;
  textNode.textContent = text;
}


function r() 
{
        var list = document.getElementsByClassName('note');
	//debug.innerText = 'Length '+ list.length;
	for(var i = 0; i < list.length; i++)
        {
             var node = list[i];
            // var index = 0;
            if(typeof(node) !== "undefined" && typeof(node.attributes) !== "undefined")
            {
                 var index = node.attributes['index'].value;
                 var title = document.getElementById('title'+index).textContent;
	//debug.innerText += 'Title: ""'+ title +'""\n';

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
                 node.className = 'note ' + cla;
    	//debug.innerText += 'Cla: ""'+ node.className +'""\n';

                 var textNode = document.getElementById('note'+index);
                 if(typeof(textNode.attributes['contentEditable']) !== "undefined" 
                    && textNode.attributes['contentEditable'].value == 'true')
                    textNode.className = 'editable';
                 var titleNode = document.getElementById('title'+index);
                 if(typeof(titleNode.attributes['contentEditable']) !== "undefined" 
                    && titleNode.attributes['contentEditable'].value == 'true')
                    titleNode.className = 'noteTitle editable';
                 else
                    titleNode.className = 'noteTitle';
            }
         }
}


function init()
{
  if(record.isInsertMode())
    initNoteWidget(-1);
  editor.noteChanged.connect(onNoteChanged);
}

