/*
   Базовый декоратор
*/

function TextDecorator()
{
    this.record = winContr.record;
    winContr.record.changedState.connect(this.onRecordStateChanged);
    var arr = [
                // [/(ftp:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
                // [/(mailto:\/\/[^ \s"']+)/gi, '<a href="$1" onclick="javascript:openUrl(\'$1\')">$1</a>'],
                [/((?:ftp:)|(?:mailto:)|(?:https?:)\/\/[^ \s"']+)/gi, function(str, sub)
                {
                    if(sub.search("<|>") !== -1)
                        return sub;
                    return '<a href="'+ sub + '" class="url">' + sub + '</a>';
                } ]
            ];
    if(typeof this.replaceArray !== "object")
        this.replaceArray = [];
    this.replaceArray = this.replaceArray.concat(arr);
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
        $(".noteText").html(function(index, old) {
            var s = textDecorator.applyReplaceArray(old);
            return s;
        });

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
