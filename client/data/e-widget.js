
// widget

function WidgetEditor()
{
}

WidgetEditor.prototype = {
  showEditor : function (index, visible)
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
  },

  /*
  connectWidget : function (widget)
  {
    widget.submitTriggered.connect(submitNoteWidget);
    widget.cancelTriggered.connect(cancelNoteWidget);
  }
  */

  editNote : function(index)
  {
    if(editor.enableModify())
      this.showEditor(index,true);
  },

  cancelNote : function(index)
  {
    this.showEditor(index,false);
  },

  saveNote : function (index)
  {
    //var blockNode = document.getElementById('block'+index);
    if(index === -1)
    {
       var newDesc = widgetDesc.noteText();
       if(editor.setDescription(newDesc))
       {
         descText.textContent = newDesc;
         this.showEditor(-1,false);
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
         this.showEditor(index,false);
       }
    }
  },

  removeNote : function (index)
  {
    if(editor.removeNote(index))
    {
      $(".note[index='"+index+"']").fadeOut(2000);
    }
  },

  editDescription : function()
  {
    this.editNote(-1);
  },

  saveDescription : function()
  {
    this.saveNote(-1);
  },

  cancelDescription : function()
  {
    this.cancelNote(-1);
  }
}

/*
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

*/