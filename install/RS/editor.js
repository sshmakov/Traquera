var editor = new Object;
var titles = [];
var notes = [];
var desc = "";
var noteChangedSignal = function () {};

editor.setNote = function(index,title,text)
{
	titles[index] = title;
	notes[index] = text;
	noteChangedSignal();
	return true;
};

editor.setDescription = function(text) 
{
	editor.description = text;
	return true;
};

editor.description = "";

editor.enableModify = function()
{
	return true;
}

editor.noteChanged = new Object;
editor.noteChanged.connect = function(onNoteChanged)
{
	noteChangedSignal = onNoteChanged;
};
  
editor.startEditNote = function(index)
{
	return true;
};

editor.noteTitle = function(index)
{
	return titles[index];
};

editor.noteText = function(index)
{
	return notes[index];
};

editor.startEditDescription = function()
{
	return true;
};

