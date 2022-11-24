/*
ContextualTracker
http://ContextualTracker.wiadomosc.info/

Copyright (C) 2006 Tobiasz 'gosciu' Cudnik
gosc@wiadomosc.info

You can use ContextualTracker free of charge.
Distribution and code reuse NOT permitted.
*/

var ct_location = self.location;
var ct_statusWorking = false;
var _ct_trackMouse = false;
var _ct_trackedFrames = Array();
var _ct_URLClickHandlers = Array();
var _ct_funcClickHandlers = Array();

// test window.status avability
window.status = 'test';
if ( window.status == 'test' ) {
	ct_statusWorking = true;
	window.status = '';
}

// registers URL handler (a GET request)
// using varNames you can change arg names, eg varNames['channel'] == 'ch' changes arg &channel= to &ch= in URI
// null values in varNames turns off specifed arg
function ct_registerURLClickHandler( url, varNames )
{
	var idx = _ct_URLClickHandlers.length;
	_ct_URLClickHandlers[idx] = new Array();

	_ct_URLClickHandlers[idx]['url'] = url;
	_ct_URLClickHandlers[idx]['vars'] = varNames || new Object();

	return idx+1;
}

function ct_registerFuncClickHandler( handler )
{
	_ct_funcClickHandlers.push( handler );

	return _ct_funcClickHandlers.length;
}

function ct_runClickHandlers( data )
{
	// run URL handlers first...
	for ( var i=0; i < _ct_URLClickHandlers.length; i++ ) {
		var url = '';
		var name = '';

		for ( key in data ) {
			// skip unwanted args
			if (! _ct_URLClickHandlers[i]['vars'][ key ] && typeof _ct_URLClickHandlers[i]['vars'][ key ] != 'undefined' )	// skip arg
				continue;

			if ( _ct_URLClickHandlers[i]['vars'][ key ] )
				url += '&'+_ct_URLClickHandlers[i]['vars'][ key ]+'='+escape( data[ key ] );
			else {
				if ( typeof encodeURIComponent == 'function' )
					url += '&'+key+'='+encodeURIComponent( data[ key ] );
				else
					url += '&'+key+'='+escape( data[ key ] );
			}
		}

		ct_sendData( _ct_URLClickHandlers[i]['url']+url );
	}

	// and now function handlers...
	for ( var i=0; i < _ct_funcClickHandlers.length; i++ ) {
		var url = '';
		eval ( _ct_funcClickHandlers+"( data )" );
	}
}

function ct_cancelClick( frameObj )
{
	var idx = ct_findFrame( frameObj );

	if ( idx < 0 )
		return false;

	_ct_trackedFrames[idx]['clicked'] = false;
	return true;
}

function ct_trackFrame( obj, callbackFunc, vars )
{
	var idx = _ct_trackedFrames.length;
	_ct_trackedFrames[idx] = new Array();

	_ct_trackedFrames[idx]['frame'] = obj;
	_ct_trackedFrames[idx]['callback'] = callbackFunc;
	_ct_trackedFrames[idx]['mouseover'] = false;
	_ct_trackedFrames[idx]['clicked'] = false;

	if ( vars != null )
		_ct_trackedFrames[idx]['vars'] = vars;

	if ( ct_isIE() ) {
		obj.attachEvent('onfocus', _ct_trackFrameFocus);
	}
	else if ( ct_isOpera() ) {
		if ( idx == 0 ) {	// register only once
			window.addEventListener('blur', _ct_trackCheck, false);
			window.addEventListener('mouseover', _ct_trackFrameMouseReverse, false);
			window.addEventListener('mouseout', _ct_trackFrameMouseReverse, false);
		}
	}
	else {
		if ( idx == 0 )		// register only once
			window.addEventListener('beforeunload', _ct_trackCheck, false);
		obj.addEventListener('mouseover', _ct_trackFrameMouse, false);
		obj.addEventListener('mouseout', _ct_trackFrameMouse, false);
	}
}

function ct_findFrame( frameObj )
{
	for ( var i=0; i < _ct_trackedFrames.length; i++ ) {
		if ( frameObj == _ct_trackedFrames[i]['frame'] )
			return i;
	}

	return -1;
}

function ct_sendData( url )
{
	// avoid caching
	url += url.indexOf('?') == -1 ? '?time='+(new Date()).getTime(): '&time='+(new Date()).getTime();
	url += '&charset='+document.charset;
//	var bug = new Image();
//	bug.src = url;
	var head = document.getElementsByTagName("head")[0];
	var script = document.createElement('script');
	script.type = 'text/javascript';
	script.src = url;
	head.appendChild(script);

	ct_wait(500);
}

// Code from http://www.faqts.com/knowledge_base/view.phtml/aid/1602
function ct_wait(numberMillis) {
	var now = new Date();
	var exitTime = now.getTime() + numberMillis;
	while (true) {
		now = new Date();
		if (now.getTime() > exitTime)
			return;
	}
}

// TRACKING STUFF (priv)

function _ct_trackCheck( event, callbackFunc )
{
	for ( var i=0; i < _ct_trackedFrames.length; i++ ) {
		if ( _ct_trackedFrames[i]['mouseover'] && ! _ct_trackedFrames[i]['clicked'] ) {
			_ct_trackedFrames[i]['clicked'] = true;
			_ct_trackRunCallback( i );
			break;
		}
	}
}

function _ct_mouseOver( obj, mouse )
{
	var frameLeft = ct_findX( obj );
	var frameTop = ct_findY( obj );

	var inX = (mouse.x >= frameLeft && mouse.x <= ( frameLeft + parseInt(obj.width) ) );
	var inY = (mouse.y >= frameTop && mouse.y <= ( frameTop + parseInt(obj.height) ) );

	return inX && inY;
}

function _ct_trackRunCallback( idx )
{
	if ( typeof _ct_trackedFrames[idx]['vars'] != 'undefined' )
		eval( _ct_trackedFrames[idx]['callback']+"( _ct_trackedFrames["+idx+"]['frame'], _ct_trackedFrames["+idx+"]['vars'] )" );
	else
		eval( _ct_trackedFrames[idx]['callback']+"( _ct_trackedFrames["+idx+"]['frame'] )" );
}

function _ct_trackFrameFocus( e )
{
	var idx = ct_findFrame( e.target || e.srcElement );

	if ( idx == -1 )
		return;

	if ( _ct_mouseOver( _ct_trackedFrames[idx]['frame'], ct_getMouse( e ) ) && ! _ct_trackedFrames[idx]['clicked'] ) {
		window.focus();
		_ct_trackedFrames[idx]['clicked'] = true;
		_ct_trackRunCallback( idx );
	}
}

function _ct_trackFrameMouse( e )
{
	var idx = ct_findFrame( e.target || e.srcElement );

	if ( idx == -1 )
		return;

	if ( e.type == 'mouseover' )
		_ct_trackedFrames[idx]['mouseover'] = true;
	else
		_ct_trackedFrames[idx]['mouseover'] = false;
}

function _ct_trackFrameMouseReverse( e )
{
	if ( e.type == 'mouseover' ) {		// mouse over the window means no mouse over any frame
		for ( var i=0; i < _ct_trackedFrames.length; i++ ) {
			_ct_trackedFrames[i]['mouseover'] = false;
		}
	}
	else {
		for ( var i=0; i < _ct_trackedFrames.length; i++ ) {
			var frameLeft = ct_findX( _ct_trackedFrames[i]['frame'] );
			var frameTop = ct_findY( _ct_trackedFrames[i]['frame'] );
			var mouse = ct_getMouse( e );

			var inFrameX = (mouse.x >= frameLeft && mouse.x <= ( frameLeft + parseInt(_ct_trackedFrames[i]['frame'].width) ) );
			var inFrameY = (mouse.y >= frameTop && mouse.y <= ( frameTop + parseInt(_ct_trackedFrames[i]['frame'].height) ) );

			if ( inFrameX && inFrameY ) {
				_ct_trackedFrames[i]['mouseover'] = true;
				break;
			}
		}
	}
}

// USEFULL STUFF

function ct_getMouse(e)
{
	var pos = {x:0, y:0};

	if (e.pageX || e.pageY)
	{
		pos['x'] = e.pageX;
		pos['y'] = e.pageY;
	}
	else if (e.clientX || e.clientY)
	{
		pos['x'] = e.clientX + document.body.scrollLeft;
		pos['y'] = e.clientY + document.body.scrollTop;
	}

	return pos;
}

function ct_findX( obj )
{
	var x = 0;
	while (obj)
	{
		x += obj.offsetLeft;
		obj = obj.offsetParent;
	}
	return(x);
}

function ct_findY( obj )
{
	var y = 0;
	while (obj)
	{
		y += obj.offsetTop;
		obj = obj.offsetParent;
	}
	return(y);
}

function ct_getUrlVar( url, varName )
{
	var match = url.match( new RegExp("[&?]"+escape(varName)+"=(.+?)(&|$)" ) );

	if ( match != null )
		return match[1];
	else
		return '';
}

function ct_isIE()
{
	return window.attachEvent && ! ct_isOpera();
}

function ct_isW3C()
{
	return window.addEventListener;
}

function ct_isOpera()
{
	return window.opera;
}

function ct_browser()
{
	if ( ct_isIE() )
		return 'ie';
	
	if ( ct_isOpera() )
		return 'opera';

	return '';
}