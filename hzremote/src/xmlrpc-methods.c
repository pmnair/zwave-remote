
#include <stdio.h>
#include <unistd.h>

#include "xmlrpc-methods.h"
#include "xmlrpc-utils.h"
#include "zw_api.h"
#include "zw_node.h"
#include "log.h"

xmlrpc_value * xmlrpc_get_node_list(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo) 
{
	list_head *zw_nodes = NULL;
        list_node *node = NULL;
        struct zw_node *zwnode;
	xmlrpc_value *result = xmlrpc_struct_new( envP );
	xmlrpc_value *node_arr = xmlrpc_array_new( envP );

	assertValue( result );
	assertValue( node_arr );

	SYSLOG_DEBUG( "xmlrpc_get_node_list" );
	zw_nodes = zw_get_node_list();

        list_foreach(node, (zw_nodes)) { 
		xmlrpc_value *node_item = NULL;
		const char *type = "BASIC";
		const char *state = "OFF";

                zwnode = (struct zw_node *)node;
		switch( zwnode->cclass ) {
		case COMMAND_CLASS_SWITCH_BINARY:
			type = "Switch";
			state = ( zwnode->state == 0 )?"OFF":"ON";
			break;
		case COMMAND_CLASS_SENSOR_BINARY:
			type = "DoorSensor";
			state = ( zwnode->state == 0 )?"CLOSE":"OPEN";
			break;
		case COMMAND_CLASS_SWITCH_TOGGLE_BINARY:
			type = "ToggleSwitch";
			break;
		}
		node_item = xmlrpc_build_value( envP, "{s:i,s:s,s:s,s:s,s:i}", "NodeId", zwnode->id, 
							"NodeName", zwnode->name,
							"NodeType", type,
							"NodeState", state,
							"NodeBattLevel", zwnode->batt_level );
		assertValue( node_item );

		xmlrpc_array_append_item( envP, node_arr, node_item );
		xmlrpc_DECREF( node_item );
        }

	xmlrpc_struct_set_value( envP, result, "NodeList", node_arr );
	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.getNodeList" );
	xmlrpc_set_struct_int( envP, result, "Result", 0 );

	xmlrpc_DECREF( node_arr );

	return result;
}

xmlrpc_value * xmlrpc_turn_switch_off(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
	hzremote_ctx_S *ctx = (hzremote_ctx_S *)serverInfo;
	int nodeid;
	int res;
	int val = ZW_NODE_STATE_OFF;
	xmlrpc_value *result = xmlrpc_struct_new( envP );

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_turn_switch_off: id - %d", nodeid );

	res = zw_node_set_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
        if ( !res ) {
                usleep(500);
                res = zw_node_get_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
        }

	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.turnSwitchOff" );
	xmlrpc_set_struct_int( envP, result, "Result", res );

	return result;
}

xmlrpc_value * xmlrpc_turn_switch_on(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
	hzremote_ctx_S *ctx = (hzremote_ctx_S *)serverInfo;
	int nodeid;
	int val = ZW_NODE_STATE_ON;
	int res;
	xmlrpc_value *result = xmlrpc_struct_new( envP );

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_turn_switch_on: id - %d", nodeid );


	res = zw_node_set_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
	if ( !res ) {
		usleep(500);
		res = zw_node_get_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
	}
	
	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.turnSwitchOn" );
	xmlrpc_set_struct_int( envP, result, "Result", res );

	return result;
}

xmlrpc_value * xmlrpc_toggle_switch(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
/*
	xmlrpc_value *disk_name_array;
	xmlrpc_value *disk_name;
	xmlrpc_value *result;

	return result;
*/
	return NULL;
}

xmlrpc_value * xmlrpc_refresh_state(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
	hzremote_ctx_S *ctx = (hzremote_ctx_S *)serverInfo;
	int nodeid;
	int val = ZW_NODE_STATE_ON;
	int res;
	xmlrpc_value *result = xmlrpc_struct_new( envP );

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_refresh_state: id - %d", nodeid );

	res = zw_node_get_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
	
	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.refreshState" );
	xmlrpc_set_struct_int( envP, result, "Result", res );
	return result;
}


xmlrpc_value * xmlrpc_set_node_label(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
	int nodeid;
	char *nodename;
	int res;
	xmlrpc_value *result = xmlrpc_struct_new( envP );
	xmlrpc_value *rc;

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,s:s,*})", "NodeId", &nodeid, "NodeLabel", &nodename );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_set_node_label: id - %d, name - %s", nodeid, nodename );
	res = zw_node_set_label( nodeid, nodename );

	rc = xmlrpc_int_new( envP, res );
	xmlrpc_struct_set_value( envP, result, "Result", rc );
	
	xmlrpc_DECREF( rc );
	return result;
}



