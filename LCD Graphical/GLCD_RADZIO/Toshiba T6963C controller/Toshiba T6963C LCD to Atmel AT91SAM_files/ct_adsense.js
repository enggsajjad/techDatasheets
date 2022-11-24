/*
ContextualTracker
http://ContextualTracker.wiadomosc.info/

Copyright (C) 2006 Tobiasz 'gosciu' Cudnik
gosc@wiadomosc.info

You can use ContextualTracker free of charge.
Distribution and code reuse NOT permitted.
*/

function ct_adsenseRun()
{
	var ad = document.getElementsByTagName("iframe");

	for ( var i = 0; i < ad.length; i++ ) {
		if ( ad[i].src.indexOf('googlesyndication.com') > -1 ) {
			ct_trackFrame( ad[i], 'ct_adsenseClick' );
		}
	}

	if ( i == 0 )
		setTimeout('ct_adsenseRun()', 500);
}

function ct_adsenseClick( frameObj )
{
	var data = new Object();

	if ( ct_location != self.location ) {	// posiblity of page redirection with mouse over adsense frame (no click)
		ct_cancelClick( frameObj );
		return;
	}

	// link unit ?
	if ( window.status && ct_getUrlVar( frameObj.src, 'format').match(/_al$/) )
		data['ad_url'] = window.status;
	else if ( window.status ) {
		if ( window.status.substr(0,36) == 'http://services.google.com/feedback/' ) {
			ct_cancelClick( frameObj );
			return;
		}

		var match = window.status.match(/(\S+)$/);

		if ( match != null )
			data['ad_url'] = 'http://'+match[1];
	}
	else if ( ct_statusWorking ) {	// posiblity of clicking on blank space of iframe
		ct_cancelClick( frameObj );
		return;
	}

	data['sys'] = "adsense";
	data['src_title'] = document.title;
	data['src_url'] = ct_location;
	data['channel'] = ct_getUrlVar( frameObj.src, 'channel');
	data['ad_size'] = frameObj.width+"x"+frameObj.height;
	data['browser'] = ct_browser();

	ct_runClickHandlers( data );
}