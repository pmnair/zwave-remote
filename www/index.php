<!DOCTYPE html>
<html>
<head>
<title>Home zWave Remote</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<link type="text/css" href="css/smoothness/jquery-ui-1.8.22.custom.css" rel="stylesheet" />
<script type="text/javascript" src="js/jquery-1.7.2.min.js"></script>
<script type="text/javascript" src="js/jquery-ui-1.8.22.custom.min.js"></script>
<script type="text/javascript" src="js/xmlrpc.js"></script>
<script type="text/javascript" src="js/backend-api.js"></script>
<script type="text/javascript">

function SetupMenu()
{
	alert( "Setup Menu..." );
}
$(function() {
	$( "#dialog" ).dialog( { buttons: [{ text: "Ok", click: function() { $(this).dialog("close"); } },
					   { text: "Notk", click: function() { $(this).dialog("close"); } }]
				},
				{ resizable: false },
				{ position: "center" },
				{ width: 460 } );
});

$(function() {
	// Tabs
	$('#tabs').tabs({ spinner: "Retrieving data..." });
	// Slider
	$('#slider').slider({
				range: true,
				values: [17, 67]
	});
});

$(function() {
	GetNodeList();
	setupRefresh( 10000 );
});

</script>
<style type="text/css">
	/*demo page css*/
	body{ font: 100% "Trebuchet MS", sans-serif; margin: 100px;}
	.demoHeaders { margin-top: 2em; }
	#dialog_link {padding: .4em 1em .4em 20px;text-decoration: none;position: relative;}
	#dialog_link span.ui-icon {margin: 0 5px 0 0;position: absolute;left: .2em;top: 50%;margin-top: -8px;}
	ul#icons {margin: 0; padding: 0;}
	ul#icons li {margin: 2px; position: relative; padding: 4px 0; cursor: pointer; float: left;  list-style: none;}
	ul#icons span.ui-icon {float: left; margin: 0 4px;}
	table.nodeList {border-bottom:solid 1px;}
	table.nodeList tr td {padding: 5px;}
	table.nodeList tr td.nodeName {width:40%;border:solid 1px;border-right:none;border-bottom:none;}
	table.nodeList tr td.nodeType {width:25%;border:solid 1px;border-right:none;border-bottom:none;}
	table.nodeList tr td.nodeId {width:5%;border:solid 1px;border-right:none;border-bottom:none;}
	table.nodeList tr td.nodeState {text-align: center; width:25%;border:solid 1px;border-right:none;border-bottom:none;} 
	table.nodeList tr td.nodeControl {width:10%;border:solid 1px;}
	div#tabs {width: 600px; margin: 0pt auto;}
	iframe#cam1 {width: 320px; height: 240px; margin: 20px 0 0 300px;}
</style>


</head>
<body>
<h1 align="center">Home zWave Remote</h1>

<script type="text/javascript">
</script>
<div id="tabs">
	<ul>
		<li><a href="#tabs-1">Remote</a></li>
		<li><a href="#tabs-2">Setup</a></li>
		<li><a href="#tabs-3">Logs</a></li>
		<li><a href="#tabs-4">About</a></li>
	</ul>
	<div id="tabs-1"> Refreshing Node List...
	<button align="center" type="button" onclick="RefreshAllNodes()">RefreshNodes</button>
	</div>
	<div id="tabs-2"> Refreshing Node List...
	</div>
	<div id="tabs-3"> Refreshing Node List...
	</div>
	<div id="tabs-4"> Home zWave Remote: A remote control for the Home applinces, based on zWave.
	</div>
</div>
<iframe id="cam1" src="" />
</body>
</html> 
