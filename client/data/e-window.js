// window
function WindowEditor()
{
  this.noteIndex = -2;
}

WindowEditor.prototype = {
  editNote : function (index)
  {
    this.noteIndex = index;
    editor.startEditNote(index);
    var state = document.getElementById('noteState'+index);
    state.textContent = 'editing';
  },

  saveNote : function(index)
  {
    if(this.noteIndex == index)
    {
      editor.endEditNote(true);
      this.noteIndex = -2;
      var titleNode = document.getElementById('title'+index);
      var textNode = document.getElementById('note'+index);
      titleNode.value = editor.noteTitle(index);
      textNode.textContent = editor.noteText(index);
    }
    return false;
  },

  cancelNote : function(index)
  {
    if(this.noteIndex == index)
    {
      editor.endEditNote(false);
      this.noteIndex = -2;
      var titleNode = document.getElementById('title'+index);
      var textNode = document.getElementById('note'+index);
      titleNode.value = editor.noteTitle(index);
      textNode.textContent = editor.noteText(index);
    }
    return false;
  },

  editDescription : function()
  {
    editor.startEditDescription();
  },

  saveDescription : function()
  {
    editor.saveDescription();
  },

  cancelDescription : function()
  {
    editor.cancelEditDescription();
  }
}

