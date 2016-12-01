/*

    Редактор текстов на основе textarea

*/

function TextAreaEditor()
{
    record.changedState.connect(this.recordChangedState);
}

TextAreaEditor.prototype = {
    isDescEditing : false,
    isNoteEditing : false,
    noteIndex : 0,

    doNote : function(index, func)
    {
        var textId, titleId, editTextId, editTitleId;
        if(index === -1)
        {
            textId = "#descText";
            titleId = "#descTitle";
            editTextId = "#editDesc";
            editTitleId = "#editTitleDesc";
        }
        else
        {
            textId = "#note"+index;
            titleId = "#title"+index;
            editTextId = "#editNote"+index;
            editTitleId = "#editTitle"+index;
        }
        return func(textId, titleId, editTextId, editTitleId, index);
    },


    doAllNote : function(func)
    {
        var index;
        for(index=-1; index<record.noteCount; index++)
            this.doNote(index, func);
    },


    showEditor : function (index, visible)
    {
        if(visible)
        {
            if(this.isDescEditing && index === -1)
                return false;
            if(this.isNoteEditing && index === this.noteIndex)
                return false;
            this.closeAnother(index);
        }
        this.doNote(index, function(textId, titleId, editTextId, editTitleId)
        {
            if(visible)
            {
                $(textId).hide();
                $(titleId).hide();
                if(!$(editTextId).size())
                {
                    $(textId).after("<textarea id='"+editTextId.replace(/#/,"")+"' class='noteEditArea'/>");
                    $(editTextId).css("margin-top", $(textId).css("margin-top"));
                    $(editTextId).css("margin-left", $(textId).css("margin-left"));
                    $(editTextId).css("margin-bottom", $(textId).css("margin-bottom"));
                    $(editTextId).css("margin-right", $(textId).css("margin-right"));
                }
//                $(editTextId).html($(textId).html());
                if(index === -1)
                    $(editTextId).text(record.description);
                else
                    $(editTextId).text(record.noteText(index));
                $(editTextId).show();
		//$('html,body').animate({scrollTop: $('#editNote0').offset().top});
                $(editTextId).focus();
                $(editTextId).parent(".blockText").children(".btnsBeginEdit").hide();
                $(editTextId).parent(".blockText").children(".btnsEndEdit").show();
            }
            else
            {
                if(index === -1)
                    $(textId).text(editor.description);
                else
                    $(textId).text(editor.noteText(index));
                $(textId).show();
                $(titleId).show();
                $(editTextId).hide();
                $(editTextId).parent(".blockText").children(".btnsBeginEdit").show();
                $(editTextId).parent(".blockText").children(".btnsEndEdit").hide();
            }
        });
        this.isDescEditing = index === -1 && visible;
        this.isNoteEditing = index !== -1 && visible;
        this.noteIndex = index;
        return true;
    },

    /*
  connectWidget : function (widget)
  {
    widget.submitTriggered.connect(submitNoteWidget);
    widget.cancelTriggered.connect(cancelNoteWidget);
  }
  */

    cancelAll: function()
    {
        if(this.isDescEditing)
            this.cancelDescription();
        if(this.isNoteEditing)
            this.cancelNote(this.noteIndex);
    },

    closeAnother : function(index)
    {
        if(this.isDescEditing && (index !== -1))
            this.saveDescription();
        if(this.isNoteEditing && (index !== this.noteIndex))
            this.saveNote(this.noteIndex);
    },

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
        if(this.doNote(index, function(textId, titleId, editTextId, editTitleId)
        {
            if(index === -1)
            {
                var newDesc = $(editTextId).val();
                if(editor.setDescription(newDesc))
                {
                    descText.textContent = newDesc;
                    return true;
                }
            }
            else
            {
                var newTitle = $(editTitleId).val();
                var newText = $(editTextId).val();
                if(editor.setNote(index, newTitle, newText))
                {
                    $(textId).text(newText);
                    $(titleId).text(newTitle);
                    return true;
                }
            }
            return false;
        }))
            this.showEditor(index,false);
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
    },

    recordChangedState : function(newMode)
    {
        if(newMode === 0)
            textEditor.cancelAll();
    },

    insertButtons : function()
    {
        this.doAllNote(function(textId, titleId, editTextId, editTitleId, index)
        {
            var noteSuf = index === -1 ?  "Description()" : "Note("+index+")";
            $(textId).before(
                        '<div class="btnsBeginEdit" style="margin-top: 15px;">'
                        + (index === -1 ? '' : '<img src="qrc:/images/editdelete.png" onclick="textEditor.remove'+noteSuf+'" class="btn" width="12" height="12" title="cancel"/>')
                        +'<img src="qrc:/images/edit.png" onclick="textEditor.edit'+noteSuf+'" class="btn" width="12" height="12" title="edit"/>'
                        +'</div>'
                        +'<div class="btnsEndEdit" style="margin-top: 15px;">'
                        +'<img src="qrc:/images/filesave.png" onclick="textEditor.save'+noteSuf+'" class="btn" width="12" height="12" title="save"/>'
                        +'<img src="qrc:/images/cancel.png" onclick="textEditor.cancel'+noteSuf+'" class="btn" width="12" height="12" title="cancel"/>'
                        +'</div>'
                        );
        });
    },

    init : function()
    {
        this.insertButtons();
    }
}

textEditor = new TextAreaEditor();
