function debugInfo(line)
{
	debug.textContent = line;
}

function setNote(index,title,text)
{
	editor.setNote(index,title,text);
	return false;
}

function changedDesc(evt)
{
	editor.setDescription(evt.srcElement.textContent);
}


function changedText(evt)
{
        var index = evt.srcElement.attributes['index'].value;
//	var note = document.getElementById('note' + index).innerText;
	var title = document.getElementById('title' + index).textContent;
	editor.setNote(index, title, evt.srcElement.textContent);
}

function changedTitle(evt)
{
        var index = evt.srcElement.attributes['index'].value;
	var note = document.getElementById('note' + index).textContent;
	editor.setNote(index, evt.srcElement.textContent, note);
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

function showEditor(index, visible)
{
  var sIndex;
  if(index === -1)
	sIndex = "Desc";
  else
	sIndex = index;

  var blockNode = document.getElementById('block'+sIndex);
  var widgetNode = document.getElementById('widget'+sIndex);
  if(visible)
  {
    blockNode.style.display = "none";
    widgetNode.style.display = "";
  }
  else
  {
    blockNode.style.display = "";
    widgetNode.style.display = "none";
  }
}


function initNoteWidget(index)
{
  if(editor.enableModify())
    showEditor(index,true);
}

function connectWidget(widget)
{
  widget.submitTriggered.connect(submitNoteWidget);
  widget.cancelTriggered.connect(cancelNoteWidget);
}

function closeNoteWidget(index)
{
  showEditor(index,false);
}

function submitNoteWidget(index)
{
  //var blockNode = document.getElementById('block'+index);
  if(index === -1)
  {
     var newDesc = widgetDesc.noteText();
     if(editor.setDescription(newDesc))
     {
       descText.textContent = newDesc;
       showEditor(-1,false);
     }
  }
  else
  {
     var widgetNode = document.getElementById('widget'+index);
     var newTitle = widgetNode.noteTitle();
     var newText = widgetNode.noteText();
     if(editor.setNote(index, newTitle, newText))
     {
       var titleNode = document.getElementById('title'+index);
       var textNode = document.getElementById('note'+index);
       titleNode.textContent = newTitle;
       textNode.textContent = newText;
       showEditor(index,false);
     }
  }
}

function cancelNoteWidget(index)
{
  var blockNode = document.getElementById('block'+index);
  var widgetNode = document.getElementById('widget'+index);
  blockNode.style.display = "";
  widgetNode.style.display = "none";
}

function onNoteChanged(index,title,text)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  titleNode.textContent = title;
  textNode.textContent = text;
}

function editNote(index)
{
  editor.noteChanged.connect(onNoteChanged);
  editor.startEditNote(index);
  var state = document.getElementById('noteState'+index);
  state.textContent = 'editing';
  //var titleNode = document.getElementById('title'+index);
  //var textNode = document.getElementById('note'+index);
  //titleNode.disabled = "";
  //textNode.contentEditable = true;
  //debug.innerText = 'Debug';
}

function saveNote(index)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  editor.setNote(index,titleNode.value,textNode.textContent);
  titleNode.disabled = true;
  textNode.contentEditable = false;
  titleNode.value = editor.noteTitle(index);
  textNode.textContent = editor.noteText(index);
	return false;
}

function resetNote(index)
{
  var titleNode = document.getElementById('title'+index);
  var textNode = document.getElementById('note'+index);
  titleNode.disabled = true;
  textNode.contentEditable = false;
  titleNode.value = editor.noteTitle(index);
  textNode.textContent = editor.noteText(index);
  return false;
}

function init()
{
  if(record.isInsertMode())
    initNoteWidget(-1);
}

function descEdit()
{
    editor.startEditDescription();
}

//=================================
// designMode

function removeBr(text)
{
	return text
		.replace(/\<br\/\>/gi,"\n")
		.replace(/\<br\>\<\/br\>/gi,"\n")
		.replace(/\<br\>/gi,"\n")
	;
}

function setEditorHandler(node)
{
// node = 'pre[contenteditable="true"]'
        $(node).bind("keypress", function(event) {

if (event.which == 8) {
       // Get the DOM node containing the start of the selection
        if (window.getSelection && window.getSelection().getRangeAt) {
            range = window.getSelection().getRangeAt(0);
        } else if (document.selection && document.selection.createRange) {
            range = document.selection.createRange();
        }

        if (range) {
            node = this.lastChild;
            while (node) {
                if ( isRangeAfterNode(range, node) ) {
                    nodeToDelete = node;
                    break;
                } else {
                    node = node.previousSibling;
                }
            }

            if (nodeToDelete) {
                this.removeChild(nodeToDelete);
            }
        }
        return false;
}

/*
    if (window.getSelection && event.which == 8) { // backspace
        // fix backspace bug in FF
        // https://bugzilla.mozilla.org/show_bug.cgi?id=685445
        var selection = window.getSelection();
        if (!selection.isCollapsed || !selection.rangeCount) {
            return;
        }

        var curRange = selection.getRangeAt(selection.rangeCount - 1);
        if (curRange.commonAncestorContainer.nodeType == 3 && curRange.startOffset > 0) {
            // we are in child selection. The characters of the text node is being deleted
            return;
        }

        var range = document.createRange();
        if (selection.anchorNode != this) {
            // selection is in character mode. expand it to the whole editable field
            range.selectNodeContents(this);
            range.setEndBefore(selection.anchorNode);
        } else if (selection.anchorOffset > 0) {
            range.setEnd(this, selection.anchorOffset);
        } else {
            // reached the beginning of editable field
            return;
        }
        range.setStart(this, range.endOffset - 1);


        var previousNode = range.cloneContents().lastChild;
        if (previousNode && previousNode.contentEditable == 'false') {
            // this is some rich content, e.g. smile. We should help the user to delete it
            range.deleteContents();
            event.preventDefault();
        }
    }
*/
        	if (event.which != 13)
        		return true;

        	var docFragment = document.createDocumentFragment();

        	var newEle;
        	//add the br, or p, or something else
        	//newEle = document.createElement('br');
        	//docFragment.appendChild(newEle);

        	//add a new line
        	newEle = document.createTextNode('\n');
        	docFragment.appendChild(newEle);

        	//make the br replace selection
        	var range = window.getSelection().getRangeAt(0);
        	range.deleteContents();
        	range.insertNode(docFragment);

        	//create a new range
        	range = document.createRange();
        	range.setStartAfter(newEle);
        	range.collapse(true);

        	//make the cursor there
        	var sel = window.getSelection();
        	sel.removeAllRanges();
        	sel.addRange(range);

        	return false;
        });
}

function clearEditorHandler(node)
{
	$(node).unbind("keypress");
}

function editNoteInPlace(index)
{
	if(!editor.enableModify())
		return;
	debugInfo('edit');
        var textNode = document.getElementById('note'+index);
	$('#note'+index).removeClass("textstatic").addClass("texteditor");
	textNode.contentEditable = true;
	setEditorHandler('#note'+index);
	return false;
}

function saveNoteInPlace(index)
{
	debugInfo('save');
        var textNode = document.getElementById('note'+index);
	textNode.contentEditable = false;
	var t = textNode.innerHTML;
	t = removeBr(t);
	textNode.innerHTML = t;
	t = textNode.textContent;
	editor.setNote(index, editor.noteTitle(index), t);
	textNode.textContent = editor.noteText(index);
	$('#note'+index).addClass("textstatic");
	$('#note'+index).removeClass("texteditor");
	clearEditorHandler('#note'+index);
	return false;
}
function cancelNoteInPlace(index)
{
	debugInfo('cancel');
        var textNode = document.getElementById('note'+index);
	textNode.contentEditable = false;
	textNode.textContent = editor.noteText(index);
	$('#note'+index).addClass("textstatic");
	$('#note'+index).removeClass("texteditor");
	clearEditorHandler('#note'+index);
	return false;
}

function editDescInPlace()
{
	if(!editor.enableModify())
		return;
	debugInfo('edit');
        var textNode = document.getElementById('descText');
	$('#descText').removeClass("textstatic").addClass("texteditor");
	textNode.contentEditable = true;
	setEditorHandler('#descText');
	return false;
}

function saveDescInPlace(index)
{
	debugInfo('save');
        var textNode = document.getElementById('descText');
	textNode.contentEditable = false;
	var t = textNode.innerHTML;
	t = removeBr(t);
	textNode.innerHTML = t;
	t = textNode.textContent;
	editor.setDescription(t);
	textNode.textContent = editor.description;
	$('#descText').addClass("textstatic");
	$('#descText').removeClass("texteditor");
	clearEditorHandler('#descText');
	return false;
}
function cancelDescInPlace(index)
{
	debugInfo('cancel');
        var textNode = document.getElementById('descText');
	textNode.contentEditable = false;
	textNode.textContent = editor.description;
	$('#descText').addClass("textstatic");
	$('#descText').removeClass("texteditor");
	clearEditorHandler('#descText');
	return false;
}

