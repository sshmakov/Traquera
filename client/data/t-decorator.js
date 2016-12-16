/*
   Базовый декоратор
*/

function TextDecorator()
{
    this.record = winContr.record;
    winContr.record.changedState.connect(this.onRecordStateChanged);
    this.textReplace = [
      [/(ftp:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
      [/(mailto:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
      [/(https?:\/\/[^ \s"']+)/gi, '<a href="$1" class="url">$1</a>']
    ];
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
    //this.allNoteText().each(this.decorateNote);
    // onclick="javascript:openUrl(\'$1\')"
    $(".url").click(function()
    {
      var url = this.href;
      textDecorator.openUrl(url);
      return false;
    });
  },

  onRecordStateChanged : function()
  {
  },

  applyReplaceArray : function(text, arr)
  {
    if(typeof arr === "undefined")
      arr = this.replaceArray;
    if(typeof arr !== "object")
      return text;
    var i;
    for(i=0; i<arr.length ; i++)
    {
      var re = arr[i][0];
      var to = arr[i][1];
      text = text.replace(re, to);
    }
    return text;
  },

  openUrl : function(url)
  {
    //debug2.innerText = url;
    global.shell('cmd /c start iexplore "'+url+'"');
  }
}

window.textDecorator = new TextDecorator();