/*
   Объект для работы с функциями окна приложения, в котором открыта страница запроса
*/

function WindowController()
{
    /* начальная инициализация */
    if(typeof record === "object")
        this.record = record;
    else if(typeof view.currentRecord === "object")
        this.record = view.currentRecord;
    if(typeof editor === "object")
        this.editor = editor;

    $(document).ready(function() {
        if(this.record)
            this.record.changedState.connect(this.onRecordStateChanged);

        if(this.editor)
            this.editor.noteChanged.connect(this.onNoteChanged);
        });
}

WindowController.prototype = {
    record: 0,
    editor: 0,

    removeNote : function (index)
    {
        return this.editor && this.editor.removeNote(index);
/*
        {
            var obj = $(".note[index='"+index+"']");
            obj.slideUp(500, function() { obj.remove(); });
        }
*/
    },

    recordType: function()
    {
        if(this.record)
            return this.record.recordType;
        return 0;
    },

    /* открыть запрос с указанным id в новом окне */
    openRecord: function(id)
    {
        //view.openRecordId(id)
        global.mainWindowObj.openRecordId(id);
    },

    /* открыть новый список запросов с указанным списком id */
    openRecordList: function(idList)
    {
        var recType = this.recordType();
        global.mainWindowObj.openQueryById(idList, recType, false);
    },

    /* добавить указанные запросы в текущий список */
    addRecordToList: function(idList)
    {
        var recType = this.recordType();
        global.mainWindowObj.openQueryById(idList, recType, false)
    },


    onRecordStateChanged : function(newMode)
    {
        if(newMode === 0) // view
        {
            if(this.record)
                this.record.refresh();
            if(this.editor)
                this.editor.fullReload();
        }
        else if(newMode === 2) //insert
            $(".topWr").hide();
        else
            $(".topWr").show();
    },

    onNoteChanged: function(index,title,text)
    {
        $('#title'+index).text(title);
        $('#note'+index).text(text);
        textDecorator.decorateNote(index);
    }
}


window.winContr = new WindowController();
