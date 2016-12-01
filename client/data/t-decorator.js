/*
   Базовый декоратор
*/

function TextDecorator()
{
    winContr.record.changedState.connect(this.onRecordStateChanged);
}

TextDecorator.prototype = {
  init: function ()
  {
  },

  allNoteText: function()
  {
    return $(".noteText");
  },

  decorateNote : function (noteItem)
  {
  },

  decorateAll : function ()
  {
    this.allNoteText().each(this.decorateNote);
  },

  onRecordStateChanged : function()
  {
  }
}

window.textDecorator = new TextDecorator();