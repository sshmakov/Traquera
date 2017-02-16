function tocshow(show)
{
	if(show)
	{
		contentsBody.style.display = '';
		global.tochidden = 0;
		tocswitch.innerText = "убрать";
	}
	else
	{
		contentsBody.style.display = 'none';
		global.tochidden = 1;
		tocswitch.innerText = "показать";
	}
}

function toctog()
{
	if(global.tochidden)
		tocshow(1);
	else
		tocshow(0);
}

function hideNeed()
{
	if(global.tochidden)
		tocshow(0);
	else
		tocshow(1);
}


