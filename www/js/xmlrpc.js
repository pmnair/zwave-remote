/*
 * Copyright (c) 2008 David Crawshaw <david@zentus.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* Utilities for IE */
// Make sure we haven't already been loaded
/*var HTTP1;
if (HTTP1 && (typeof HTTP1 != "object" || HTTP1.NAME))
	throw new Error("Namespace 'HTTP1' already exists");

// Create our namespace, and specify some meta-information
HTTP1 = {};
HTTP1.NAME = "HTTP1";	// The name of this namespace
HTTP1.VERSION = 1.1;	// The version of this namespace

// This is a list of XMLHttpRequest creation factory functions to try
HTTP1._factories = [
	//function() { return new XMLHttpRequest(); },
	function() { return new ActiveXObject("MSXML2.XMLHTTP"); },
	function() { return new ActiveXObject("Microsoft.XMLHTTP"); }
];

// When we find a factory that works, store it here
HTTP1._factory = null;
*/
/**
 * Create and return a new XMLHttpRequest object.
 * 
 * The first time we're called, try the list of factory functions until
 * we find one that returns a nonnull value and does not throw an
 * exception.  Once we find a working factory, remember it for later use.
 */
/*HTTP1.newRequest = function() 
{
	if (HTTP1._factory != null) 
			return HTTP1._factory();

	for(var i = 0; i < HTTP1._factories.length; i++) 
	{
		try {
			var factory = HTTP1._factories[i];
			var request = factory();
			if (request != null) 
			{
			  HTTP1._factory = factory;
			  return request;
			}
		}
		catch(e) {
			continue;
		}
	}

	// If we get here, none of the factory candidates succeeded,
	// so throw an exception now and for all future calls.
	HTTP1._factory = function() {
		throw new Error("XMLHttpRequest not supported");
	}
	HTTP1._factory(); // Throw an error
}*/

/*
 * An XML-RPC library for JavaScript.
 *
 * The xmlrpc() function is the public entry point.
 */

/*
 * API:
	var xmlrpc = function
	(
			server,	 // URL of the server. 
			method,	 // Name of the RPC method. 
			params,	 // Array of parameters as JS Objects. 
			callback,   // Callback function, passed return variable as a JS Obj. 
			callErr,	// Callback error function, passed error as string. 
			callFinal   // Callback finalise function, always called. 
	)
*/

/*
 * Execute an XML-RPC method and return the response to 'callback'.
 * Parameters are passed as JS Objects, and the callback function is
 * given a single JS Object representing the server's response.
 */
var xmlrpc = function(server, method, params, callback, callErr, callFinal) {
/*    
	if (callErr == null)
		callErr = alert;

	var request = new XMLHttpRequest();
	try {
		request.open("POST", server, false);	// false = Synchronous
	}
	catch(err) {
		return;
	}


	//request.setRequestHeader("User-Agent", "XMLHttpRequest");
	//request.setRequestHeader("Accept-Language", "en");
	request.setRequestHeader("Content-Type", "text/xml");
	var sending = xmlrpc.writeCall(method, params);
	//alert(sending);
	request.send(sending);
	//alert(request.status);

	if (request.status == 200) { //the request has been returned
		var ret = null;
		try {
			if (request.responseXML) {
				// responseXML is a XML document object that can be parsed using the DOM.
				ret = xmlrpc.parseResponse(request.responseXML); 
			}
			else {
				throw "Bad XML: '" + request.responseText + "'";
			}
		} 
		catch(err) {
			err.message = "XML-RPC: " + err.message;
			callErr(err);
			throw err;
		}
	
		try {
			callback(ret);
		} 
		catch(err) {
			err.message = "Callback: " + err.message;
			callErr(err);
			throw err;
		}
		finally {
			if (callFinal)
				callFinal();
		}
	}
	else {
		callErr("Connection Error " + request.status);
		return;
	}
*/

    var request = new XMLHttpRequest();
	var sending = xmlrpc.writeCall(method, params);
	request.open("POST", server, true);	// false = Synchronous

	request.onreadystatechange = function() {
		//alert("status = " + request.status);
		if (request.status == 200) { //the request has been returned
			var ret = null;
			try {
				if (request.responseXML) {
					// responseXML is a XML document object that can be parsed using the DOM.
					ret = xmlrpc.parseResponse(request.responseXML); 
				}
				else {
					throw "Bad XML: '" + request.responseText + "'";
				}
			} 
			catch(err) {
				err.message = "XML-RPC: " + err.message;
				callErr(err);
				throw err;
			}
		
			try {
				callback(ret);
			} 
			catch(err) {
				err.message = "Callback: " + err.message;
				callErr(err);
				throw err;
			}
			finally {
				if (callFinal)
					callFinal();
			}
		}
		else {
			callErr("Connection Error " + request.status);
			return;
		}
	};

	request.setRequestHeader("Content-Type", "text/xml");
	request.send(sending);
};

xmlrpc.writeCall = function(method, params) {
	out = "<?xml version=\"1.0\"?>\n";
	out += "<methodCall>\n";
	out += "<methodName>"+ method + "</methodName>\n";
	//alert("params.length = " + params.length);

	out += "<params>\n";
	if (params && params.length > 0) {
		for (var i=0; i < params.length; i++)  {
			out += "<param><value>";
			out += xmlrpc.writeParam(params[i]);
			out += "</value></param>";
		}
	}
	out += "</params>\n";
	
	out += "</methodCall>\n";

	return out;
};

xmlrpc.writeParam = function(param) 
{
	var val;
	if (param == null)
		return "<nil />";
	switch (typeof(param)) 
	{
		case "boolean":
			if (param == true) param = "1"; //LF: change boolean values to 0 or 1 for Logicube API
			else param = "0";
			return "<boolean>" + param + "</boolean>";
		case "string":
			param = param.replace(/</g, "&lt;");
			param = param.replace(/&/g, "&amp;");
			return "<string>" + param + "</string>";
		case "undefined":   return "<nil/>";
		case "number":
			return /\./.test(param) ?
				"<double"> + param + "</double>" :
				"<int>" + param + "</int>";
		case "object":
			if (param.constructor == Array) 
			{
				out = "<array><data>\n";
				for (var i in param) 
				{
					if(param[i] !== "")
					{
											out += "  <value>";
						val = xmlrpc.writeParam(param[i]);
											out += val;
											out += "</value>\n";
										}
				}
				out += "</data></array>";
				return out;
			} 
			else if (param.constructor == Date) 
			{
				out = "<dateTime.iso8601>";
				out += param.getUTCFullYear();
				if (param.getUTCMonth() < 10)
					out += "0";
				out += param.getUTCMonth();
				if (param.getUTCDate() < 10)
					out += "0";
				out += param.getUTCDate() + "T";
				if (param.getUTCHours() < 10)
					out += "0";
				out += param.getUTCHours() + ":";
				if (param.getUTCMinutes() < 10)
					out += "0";
				out += param.getUTCMinutes() + ":";
				if (param.getUTCSeconds() < 10)
					out += "0";
				out += param.getUTCSeconds();
				out += "</dateTime.iso8601>";
				return out;
			} 
			else 
			{ /* struct */
				out = "<struct>\n";
				for (var i in param) 
				{
					out += "<member>";
					out += "<name>" + i + "</name>";
					out += "<value>" + xmlrpc.writeParam(param[i]) + "</value>";
					out += "</member>\n";
				}
				out += "</struct>\n";
				return out;
			}
	}
};


//helper function for XML-RPC parsing in Firefox
function getNextSibling(startBrother) {
	endBrother = startBrother.nextSibling;
	while (endBrother.nodeType != 1) {
 		endBrother = endBrother.nextSibling;
	}
	return endBrother;
}

xmlrpc.parseResponse = function(dom) { // dom is the XML response from the server
	var method = new Array(1);
	method[0] = "";
	var methResp = dom.childNodes[dom.childNodes.length - 1];
	
	if (methResp.nodeName != "methodResponse")
		throw "malformed &lt;methodResponse&gt;, got " + methResp.nodeName;

	//var params = methResp.childNodes[0];
	var params = getNextSibling(methResp.childNodes[0]);
	
	//alert(params.nodeName);
	if (params.nodeName == "fault") {
		var fault = xmlrpc.parse(getNextSibling(params.childNodes[0]), method);
		throw fault["faultString"];
	}
	if (params.nodeName != "params")
		throw "malformed &lt;params&gt;, got &lt;" + params.nodeName + "&gt;";

	//var param = params.childNodes[0];
	var param = getNextSibling(params.childNodes[0]);
	if (param.nodeName != "param")
		throw "malformed &lt;param&gt;, got &lt;" + param.nodeName + "&gt;";

	var value = param.childNodes[0];
	//var value = getNextSibling(param.childNodes[0]);
	if (value.nodeName != "value")
		throw "malformed &lt;value&gt;, got &lt;" + value.nodeName + "&gt;";

	var res =  xmlrpc.parse(value, method);
	return res;
};

// Note that this is a recursive function
xmlrpc.parse = function(value, method) {
	var Index = new Array(2);
	
	if (value.nodeName != "value") {
		//alert("value.nodeName: " + value.nodeName + " method: " + method);
		throw "parser: expected &lt;value&gt;";
	}

	var type = value.childNodes[0];
	//alert_callback('parse ' + type.nodeName);
	if (type == null)
		throw "parser: expected &lt;value&gt; to have a child";
	switch (type.nodeName) {
		case "boolean":
			var res = type.childNodes[0].data == "1" ? true : false;
			return res;
		case "i4":
		case "i8":
		case "int":
			var res = parseInt(type.childNodes[0].data);
			return res;
		case "double":
			var res = parseFloat(type.childNodes[0].data);
			return res;
		case "#text": //Apache XML-RPC 2 doesn't wrap strings with <string>
			var res = type.data;
			return res;
		case "string":
			if (typeof(type.childNodes[0]) == "undefined") var res = "";	//LF: handle empty string
			else var res = type.childNodes[0].data;
			return res;
		case "array":
			var data = type.childNodes[0];
			var res = new Array();
			for (var i=0; i < data.childNodes.length; i++) {
				if (data.childNodes[i].nodeType != 1) continue;	//LF: only get Element nodes
				res[res.length] = xmlrpc.parse(data.childNodes[i], method);
			}
			return res;
		case "struct":
			var members = type.childNodes;
			//alert(dump(members, 3));
			var res = {};
			for (var i=0; i < members.length; i++) {
				if (members[i].nodeType != 1) continue;	//LF: only get Element nodes
				
				var name = members[i].childNodes[0].childNodes[0].data;
				var value = xmlrpc.parse(getNextSibling(members[i].childNodes[0]), method);
								
				res[name] = value;
				//document.write(method + " res[" + name + "]: " + res[name] + "<br>\n");
			}
			return res;
		case "dateTime.iso8601":
			var s = type.childNodes[0].data;
			var d = new Date();
			d.setUTCFullYear(s.substr(0, 4));
			d.setUTCMonth(parseInt(s.substr(4, 2)) - 1);
			d.setUTCDate(s.substr(6, 2));
			d.setUTCHours(s.substr(9, 2));
			d.setUTCMinutes(s.substr(12, 2));
			d.setUTCSeconds(s.substr(15, 2));
			return d;
		case "base64":
			//alert("TODO base64"); // XXX
		default:
			throw "parser: expected type, got5 <"+type.nodeName+">";
	}
}
