
// widget

function WidgetEditor()
{
}

WidgetEditor.prototype = {
  isDescEditing : false,
  isNoteEditing : false,
  noteIndex : 0,

  /* (внутренний) открыть окно редактирования текста */
  showEditor : function (index, visible)
  {
    if(visible)
       this.closeAnother(index);
    var sIndex;
    if(index === -1)
  	sIndex = "Desc";
    else
  	sIndex = index;

    var blockId = 'block'+sIndex;
    var widgetId = 'widget'+sIndex;
    var blockNode = $('#'+blockId);
    var widgetNode = $('#'+widgetId);
    if(visible)
    {
      if(!$('#'+widgetId).size())
      {
        var objHtml = '<object type="application/scrnote" width="100%" height="400" id="'+widgetId+'"><param name="noteIndex" value="'+index+'"/></object>';
	blockNode.after(objHtml);
        widgetNode = $('#'+widgetId);
      }
      blockNode.hide();
      widgetNode.show();
    }
    else
    {
      blockNode.show();
      widgetNode.hide();
      widgetNode.remove();
    }
    this.isDescEditing = index === -1 && visible;
    this.isNoteEditing = index !== -1 && visible;
    this.noteIndex = index;
  },

  /*
  connectWidget : function (widget)
  {
    widget.submitTriggered.connect(submitNoteWidget);
    widget.cancelTriggered.connect(cancelNoteWidget);
  }
  */

  /* закрыть все окна редактирования текста без сохранения */
  cancelAll: function()
  {
    if(this.isDescEditing)
      this.cancelDescription();
    if(this.isNoteEditing)
      this.cancelNote(this.noteIndex);
  },

  /* закрыть все другие окна редактирования текста с сохранением */
  closeAnother : function(index) 
  {
    if(this.isDescEditing && (index !== -1))
      this.saveDescription();
    if(this.isNoteEditing && (index !== this.noteIndex))
      this.saveNote(this.noteIndex);
  },

  /* проверить возможность редактирования и открыть окно редактирования текста */
  editNote : function(index)
  {
    if(editor.enableModify())
      this.showEditor(index,true);
  },

  /* закрыть окно редактирования текста без сохранения */
  cancelNote : function(index)
  {
    this.showEditor(index,false);
  },

  /* закрыть все окна редактирования текста с сохранением */
  saveNote : function (index)
  {
    //var blockNode = document.getElementById('block'+index);
    if(index === -1)
    {
       if(widgetDesc)
       {	
         var newDesc = widgetDesc.noteText();
         if(editor.setDescription(newDesc))
         {
           descText.textContent = newDesc;
           this.showEditor(-1,false);
         }
       }
    }
    else
    {
       var widgetId = 'widget'+index;
       var widgetNode = document.getElementById(widgetId);
       var newTitle = widgetNode.noteTitle();
       var newText = widgetNode.noteText();
       if(editor.setNote(index, newTitle, newText))
       {
         var titleNode = document.getElementById('title'+index);
         var textNode = document.getElementById('note'+index);
         if(titleNode)
           titleNode.textContent = newTitle;
         textNode.textContent = newText;
         this.showEditor(index,false);
       }
    }
    textDecorator.decorateNote(index);
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

function submitNoteWidget()
{
  if(textEditor.isDescEditing)
     textEditor.saveDescription();
  else if(textEditor.isNoteEditing)
     textEditor.saveNote(textEditor.noteIndex);
}

function closeNoteWidget()
{
  if(textEditor.isDescEditing)
     textEditor.cancelDescription();
  else if(textEditor.isNoteEditing)
     textEditor.cancelNote(textEditor.noteIndex);
}

