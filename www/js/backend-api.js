
var intervalId = 0;

var nodeList = new Array();
var xmlserver = "http://" + location.host + "/RPC2";
//var garage = "http://10.0.0.16:8181/snapshot.cgi?user=visitor&pwd=";
var garage = "http://" + location.hostname + ":8181" + "/snapshot.cgi?user=visitor&pwd=";

function setupRefresh( interval )
{
	if ( intervalId > 0 ) {
		alert( "Clearing intervalId: " + intervalId );
		clearInterval( intervalId );
	}
	intervalId = setInterval("GetNodeList()", interval );
//	setInterval("RefreshVideo()", 2000 );
}

function RefreshVideo()
{
	//alert( garage );
	document.getElementById("cam1").src = garage;
}

function RefreshAllNodes()
{
	for( var i in nodeList ) {
		RefreshNodeState( nodeList[i]["NodeId"] );
	}
	GetNodeList();
}

//XML-RPC error function
function err(e) {
        xmlrpc_error_msg = "XML-RPC " + e;
        alert(xmlrpc_error_msg);
}

//XML-RPC final function
function final() {
        //alert('final');
}

function callback(ret) {
        var method = ret['Method'];
//	alert( ret['Method'] + ": " + ret['Result'] );
	switch( method ) {
		case "hzremote.getNodeList":
			var nodes = ret['NodeList'];
			var nodeStr = "<table width='100%' class='nodeList' cellpadding=0 cellspacing=0 border=0 >";
			var nodeSetupStr = "<table width='100%' class='nodeList' cellpadding=0 cellspacing=0 border=0 >";
			for( var i in nodes ) {
				nodeList[i] = nodes[i];
				if ( nodeList[i]["NodeType"] != "BASIC" ) {
					nodeSetupStr += "<tr><td class='nodeName'>";
					nodeSetupStr += "<input type=\"text\"/ id=\"" + nodeList[i]["NodeId"] + "\" name=\"Name\" value=\"" + nodeList[i]["NodeName"] + "\" id=\"Name\">";
					nodeSetupStr += "</td><td class='nodeId'>";	
					nodeSetupStr += nodeList[i]["NodeId"] + "</td>";
					nodeSetupStr += "<td class='nodeType'>" + nodeList[i]["NodeType"] + "</td>";
					nodeSetupStr += "<td class='nodeState'>" + nodeList[i]["NodeState"] + "</td><td class='nodeControl'>";
                                	nodeSetupStr += "<button type=\"button\" onclick=\"SetNodeName(" + nodeList[i]["NodeId"] + ")\">Update</button>";
					nodeSetupStr += "</td></tr>";
					
					nodeStr += "<tr><td class='nodeName'>";
					if ( nodeList[i]["NodeName"] != "" ) {
						nodeStr += nodeList[i]["NodeName"] + "</td><td class='nodeType'>" + nodeList[i]["NodeType"] + "</td>";
						nodeStr += "<td class='nodeState'>" + nodeList[i]["NodeState"] + "</td>";
					}
					else {
						nodeStr += " Node: " + nodeList[i]["NodeId"] + "</td><td class='nodeType'>" + nodeList[i]["NodeType"] + "</td>";
						nodeStr += "<td class='nodeState'>" + nodeList[i]["NodeState"] + "</td>";
					}

					nodeStr += "<td class='nodeControl'>";
					if ( nodeList[i]["NodeState"] == "OFF" ) 
						nodeStr += "<button type=\"button\" onclick=\"TurnSwitchOn(" + nodeList[i]["NodeId"] + ")\">ON</button></td></tr>";
					else if ( nodeList[i]["NodeState"] == "ON" ) 
						nodeStr += "<button type=\"button\" onclick=\"TurnSwitchOff(" + nodeList[i]["NodeId"] + ")\">OFF</button></td></tr>";
					
				}
			}	
			nodeStr += "</table>";
			nodeSetupStr += "</table>";
			//document.getElementById("nodelist").innerHTML=nodeStr;
			//alert( $('#tabs').tabs() );
			//$('#tabs').tabs().value = nodeStr;
			document.getElementById("tabs-1").innerHTML=nodeStr;
			document.getElementById("tabs-2").innerHTML=nodeSetupStr;
		break;
		case "hzremote.turnSwitchOff":
			if ( ret['Result'] == 0 )
				alert( "Switch Off Successful!" );
			else	
				alert( "Switch Off failed!!!" );
			GetNodeList();
		break;
		case "hzremote.turnSwitchOn":
			if ( ret['Result'] == 0 )
				alert( "Switch On Successful!" );
			else	
				alert( "Switch On failed!!!" );
			GetNodeList();
		break;
		case "hzremote.refreshState":
		break;
		case "hzremote.setNodeLabel":
			GetNodeList();
		break;
	}
}

//Get the list of known nodes
function GetNodeList() {
        var params = new Array();
        xmlrpc( xmlserver, "hzremote.getNodeList", params, callback, err, final );
}

//SetNodeName
function SetNodeName( node ) {
	var param_node  = {'NodeId': node, 'NodeLabel' : document.getElementById(node).value };
        var params = new Array();
        params[0] = param_node;
        xmlrpc( xmlserver, "hzremote.setNodeLabel", params, callback, err, final );
}

//Turn SWitch OFF
function TurnSwitchOff( node ) {
	var param_nodeid  = {'NodeId': node};
        var params = new Array();
        params[0] = param_nodeid;
        xmlrpc( xmlserver, "hzremote.turnSwitchOff", params, callback, err, final );
}

//Turn SWitch ON
function TurnSwitchOn( node ) {
        var param_nodeid  = {'NodeId': node};
        var params = new Array();
        params[0] = param_nodeid;
        xmlrpc( xmlserver, "hzremote.turnSwitchOn", params, callback, err, final );
}

//Refresh Node State
function RefreshNodeState( node ) {
        var param_nodeid  = {'NodeId': node};
        var params = new Array();
        params[0] = param_nodeid;
        xmlrpc( xmlserver, "hzremote.refreshState", params, callback, err, final );
}

