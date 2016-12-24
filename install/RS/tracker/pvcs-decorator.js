/*
  Декоратор запросов PVCS Tracker
*/

function TrkDecorator()
{
    this.record = winContr.record;
    winContr.record.changedState.connect(this.onRecordStateChanged);
    if(winContr.record.mode !== 0)
        $("issueTitle").html("Новый запрос");
    var arr = [
                [/^#([0-9]+)/g, '<a href="#" class="scrLink">#$1</a>'],
                [/([\s\b,.^])#([0-9]+)/g, '$1<a href="#" class="scrLink">#$2</a>'],

                // RS-specific
                [/(\/\/RS[^<>"'\n]+)(#[0-9]+)/g, '<a href="#" class="perforce">$1$2</a>'],
                [/(I-)(0*)([1-9][0-9]*)/ig, '<a href="http://support.softlab.ru/Portal/InterSupport/Redirector/topic.asp?Id=$3" class="url">$1$2$3</a>'],
                [/(I-Support[ -]+)(0*)([1-9][0-9]*)/ig, '<a href="http://support.softlab.ru/Portal/InterSupport/Redirector/topic.asp?Id=$3" class="url">$1$2$3</a>'],
                [/(HotFix )([1-9][0-9][0-9][0-9]).([0-9][0-9]).([0-9]*)_([0-9]*)/ig, '<a href="file://corvus/Dst_build/RSBankV6/Work.60/RsBank/HFX_$2/$2.$3.$4/$5" class="url">$1$2.$3.$4_$5</a>'],
                [/(хотфикс )(5).([1-9])([0-9]*).([0-9][0-9]*).([0-9]*)_([0-9]*)/ig, '<a href="file://corvus/Dst_build/RSBank55/HFX_$2$3.$5/$6/$7" class="url">$1$2.$3$4.$5.$6_$7</a>']
            ];
    if(typeof this.replaceArray !== "object")
        this.replaceArray = [];
    this.replaceArray = this.replaceArray.concat(arr);
}

TrkDecorator.prototype = new TextDecorator();
TrkDecorator.prototype.txtDecorateAll = TextDecorator.prototype.decorateAll;

TrkDecorator.prototype.init = function ()
{
}

TrkDecorator.prototype.decorateNote = function (index)
{
}

TrkDecorator.prototype.decorateAll = function()
{
    if(typeof this.txtDecorateAll === "function")
        this.txtDecorateAll();

    // oncontextmenu="javascript:return menuSCR($1, event);"
    $(".scrLink").contextmenu(function(event) {
        var id = this.textContent.replace(/#/,"");
        return textDecorator.menuSCR(id,event);
    });

    // onclick="javascript:query.openRecordId($1);"
    $(".scrLink").click(function(event) {
        var id = this.textContent.replace(/#/,"");
        return query.openRecordId(id,event);
    });

    $(".perforce").click(function(event) {
        var doc = this.textContent.replace(/(#[0-9]+)/g,"");
        textDecorator.showPerforce(doc);
        return false;
    });

}

TrkDecorator.prototype.showPerforce = function(filename)
{
    var cmd = 'cmd /c start p4v -t history -s "'+filename+'"';
    global.shellLocale(cmd,'UTF-8');
    return false;
}

TrkDecorator.prototype.menuSCR = function(scrid, evt) 
{ 
    // Блокируем всплывание события contextmenu
    evt = evt || window.event;
    evt.cancelBubble = true;
    // Показываем собственное контекстное меню
    var menu = contextMenuId;
    var rec = global.getRecord(scrid);
    var title = "", fix = "", state = "";
    if(rec)
    {
        title = rec.title;
        state = rec.value("State") + " " + rec.value("Current State");
        fix = rec.value("Fixed In Build Eng.");
    }
    else
        title = "<i>(не найден)</i>";
    if(fix && fix != "0")
    {
        state = state + " " + fix;
    }
    //alert(menu);
    //alert(type);
    var html = "";
    html = '<ul class="SimpleContextMenu">';
    html += "<li><b>Запрос " + scrid + '</b> <span class="shortInfo">' + state +"</span><br/>"
            + title + "</li>";
    html += '<li><a href="#" onclick="copyToClip('+scrid+'); return false;">Копировать номер</a></li>';
    html += '<li><a href="#" onclick="query.openRecordId('+scrid+'); return false;">Открыть</a></li>';
    html += '<li><a href="#" onclick="query.appendId('+scrid+'); return false;">Добавить в список</a></li>';
    html += '<li><a href="#" onclick="openQueryPage('+scrid+'); return false;">Открыть в новом списке</a></li>';
    html += '</ul>';
    // Если есть что показать - показываем
    if (html) {
        var pos = defPosition(evt);
        //alert(pos.x + " " + pos.y + " " + window.innerWidth + " " + html);
        menu.innerHTML = html;
        menu.style.top = pos.y;
        var x = pos.x;
        if(x > window.innerWidth - 300)
            x = window.innerWidth - 300;
        if(x < 0)
            x = 0;
        menu.style.left = x;
        menu.style.display = "";
    }
    // Блокируем всплывание стандартного браузерного меню
    return false;
} 
