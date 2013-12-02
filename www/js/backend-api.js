
var intervalId = 0;

var nodeList = new Array();
var xmlserver = "http://" + location.host + "/RPC2";

function clearRefresh(interval) {
    if (intervalId > 0) {
        clearInterval(intervalId);
    }
}
function setupRefresh(interval) {
    clearRefresh();
    intervalId = setInterval("GetNodeList()", interval);
}

function RefreshAllNodes() {
    clearRefresh();
    GetNodeList();
    setupRefresh( 10000 );
}

//XML-RPC error function
function err(e) {
    xmlrpc_error_msg = "XML-RPC " + e;
    clearRefresh();
    //alert(xmlrpc_error_msg);
}

//XML-RPC final function
function final() {
    //alert('final');
}

function callback(ret) {
    var method = ret['Method'];
    switch( method ) {
        case "hzremote.getNodeList":
            var nodes = ret['NodeList'];
            var nodeStr = "<table width='100%' class='nodeList' cellpadding=0 cellspacing=0 border=0 >";
            var nodeSetupStr = "<table width='100%' class='nodeList' cellpadding=0 cellspacing=0 border=0 >";
            for( var i in nodes ) {
                nodeList[i] = nodes[i];
                //alert(nodes[i]["NodeType"]);
                if ( nodeList[i]["NodeType"] != "BASIC" ) {
                    nodeSetupStr += "<tr><td class='nodeName'>";
                    nodeStr += "<tr><td class='nodeName'>";
                    if ( nodeList[i]["NodeName"] != "" ) {
                        nodeSetupStr += "<input type='text' class='form-control' id='" + 
                        nodeList[i]["NodeId"] + "' name='Name' value='" + 
                        nodeList[i]["NodeName"] + "' id='Name'>";
                        nodeStr += "<h4>" + nodeList[i]["NodeName"] + "</h4></td>";
                    }
                    else {
                        nodeSetupStr += "<input type='text' class='form-control' id='" + 
                        nodeList[i]["NodeId"] +
                        "' name='Name' value='Node: " + nodeList[i]["NodeId"] + ", " +
                        nodeList[i]["NodeType"] + "' id='Name'>";
                        nodeStr += "<h4>Node: " + nodeList[i]["NodeId"] + ", " + nodeList[i]["NodeType"] + "</h4></td>";
                    }
                
                    nodeSetupStr += "<td class='nodeControl'><button type='button' class='btn btn-success' onclick='SetNodeName(" + 
                                    nodeList[i]["NodeId"] + ")'>Update</button></td></tr>";
                    
                    nodeStr += "<td class='nodeControl'>";
                    if (nodeList[i]["NodeType"] == "Switch") {
                        if (nodeList[i]["NodeState"] == "OFF") {
                            var switchName = "switchNode" + nodeList[i]["NodeId"];
                            nodeStr += "<div class='onoffswitch'><input type='checkbox' name=" + switchName +
                            " class='onoffswitch-checkbox' id=" + switchName + 
                            " onclick='TurnSwitchOn(" + nodeList[i]["NodeId"] + ")'>" +
                            " <label class='onoffswitch-label' for=" + switchName + "> \
                            <div class='onoffswitch-inner'></div> \
                            <div class='onoffswitch-switch'></div> \
                            </label></div>";
                        }
                        else if ( nodeList[i]["NodeState"] == "ON" ) {
                            var switchName = "switchNode" + nodeList[i]["NodeId"];
                            nodeStr += "<div class='onoffswitch'><input type='checkbox' name=" + switchName +
                            " class='onoffswitch-checkbox' id=" + switchName + 
                            " onclick='TurnSwitchOff(" + nodeList[i]["NodeId"] + ")' checked>" +
                            " <label class='onoffswitch-label' for=" + switchName + "> \
                            <div class='onoffswitch-inner'></div> \
                            <div class='onoffswitch-switch'></div> \
                            </label></div>";
                        }
                    }
                    else if (nodeList[i]["NodeType"] == "DoorSensor") {
                        if (nodeList[i]["NodeState"] == "CLOSE")
                            nodeStr += "<h4>Closed</h4>";//"<img align='center' src='images/door.png'>";
                        else if (nodeList[i]["NodeState"] == "OPEN")
                            nodeStr += "<h4>Open</h4>";//"<img src='images/door_open.png'>";
                    }
                    nodeStr += "</td>";
                }
            }	
            nodeStr += "</table>";
            nodeSetupStr += "</table>";
            $('#tab-remote').html(nodeStr);
            $('#tab-setup').html(nodeSetupStr);
            break;
        case "hzremote.turnSwitchOff":
            if ( ret['Result'] != 0 )
                alert( "Switch Off failed!!!" );
            GetNodeList();
            break;
        case "hzremote.turnSwitchOn":
            if ( ret['Result'] != 0 )
                alert( "Switch On failed!!!" );
            GetNodeList();
            break;
        case "hzremote.refreshState":
            break;
        case "hzremote.setNodeLabel":
            //GetNodeList();
            break;
    }
}

//Get the list of known nodes
function GetNodeList() {
    var params = new Array();
    xmlrpc( xmlserver, "hzremote.getNodeList", params, callback, err, final );
}

//SetNodeName
function SetNodeName(node) {
    var param_node  = {'NodeId': node, 'NodeLabel' : document.getElementById(node).value };
    var params = new Array();
    params[0] = param_node;
    xmlrpc( xmlserver, "hzremote.setNodeLabel", params, callback, err, final );
}

//Turn SWitch OFF
function TurnSwitchOff(node) {
    var param_nodeid  = {'NodeId': node};
    var params = new Array();
    params[0] = param_nodeid;
    xmlrpc( xmlserver, "hzremote.turnSwitchOff", params, callback, err, final );
}

//Turn SWitch ON
function TurnSwitchOn(node) {
    var param_nodeid  = {'NodeId': node};
    var params = new Array();
    params[0] = param_nodeid;
    xmlrpc( xmlserver, "hzremote.turnSwitchOn", params, callback, err, final );
}

//Refresh Node State
function RefreshNodeState(node) {
    var param_nodeid  = {'NodeId': node};
    var params = new Array();
    params[0] = param_nodeid;
    xmlrpc( xmlserver, "hzremote.refreshState", params, callback, err, final );
}
