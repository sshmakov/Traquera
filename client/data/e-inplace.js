
// windget

function InplaceEditor()
{
}

InplaceEditor.prototype = {
  //=================================
  // designMode

  removeBr :  function (text)
  {
  	return text
  		.replace(/\<br\/\>/gi,"\n")
  		.replace(/\<br\>\<\/br\>/gi,"\n")
  		.replace(/\<br\>/gi,"\n")
  	;
  },

  setEditorHandler : function (node)
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
  },

  clearEditorHandler : function (node)
  {
  	$(node).unbind("keypress");
  },

  editNote : function (index)
  {
    if(!editor.enableModify())
      return;
    debugInfo('edit');
    var textNode = document.getElementById('note'+index);
    $('#note'+index).removeClass("textstatic").addClass("texteditor");
    textNode.contentEditable = true;
    setEditorHandler('#note'+index);
    return false;
  },

  saveNote : function (index)
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
  },

  cancelNote : function (index)
  {
  	debugInfo('cancel');
          var textNode = document.getElementById('note'+index);
  	textNode.contentEditable = false;
  	textNode.textContent = editor.noteText(index);
  	$('#note'+index).addClass("textstatic");
  	$('#note'+index).removeClass("texteditor");
  	clearEditorHandler('#note'+index);
  	return false;
  },

  editDescription : function ()
  {
  	if(!editor.enableModify())
  		return;
  	debugInfo('edit');
          var textNode = document.getElementById('descText');
  	$('#descText').removeClass("textstatic").addClass("texteditor");
  	textNode.contentEditable = true;
  	setEditorHandler('#descText');
  	return false;
  },

  saveDescription : function ()
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
  },

  cancelDescription : function()
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

}



