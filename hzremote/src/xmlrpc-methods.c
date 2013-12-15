//
//  Created by Praveen Murali Nair on 09/07/2013.
//  Copyright (c) 2013 Praveen M Nair. All rights reserved.
//
// 	Redistribution and use in source and binary forms, with or without
//      modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//      * Neither the name of the <organization> nor the
//      names of its contributors may be used to endorse or promote products
//      derived from this software without specific prior written permission.
//
//      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//      ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//      WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//      DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//      DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//      (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//       LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//      ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//      SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
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
                        if ( zwnode->stype == 3 )
				type = "PushSwitch";
                        else
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

static int
xmlrpc_change_node_state( hzremote_ctx_S *ctx,
                         int nodeid,
                         int state )
{
        int res;
        int val = state;
        int retries = 5;
        
        do {
		res = zw_node_set_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
                if ( res ) usleep( 500 );
        } while ( res && retries-- );
        
        if ( res ) {
                SYSLOG_INFO( "xmlrpc_change_node_state: failed to set state (%d) for node(%d)", state, nodeid );
                goto out;
        }
        
        retries = 5;
        do {
        	res = zw_node_get_value( &ctx->zw_ctx, (u8)nodeid, (void *)&val );
                if ( res ) {
                        SYSLOG_INFO( "xmlrpc_change_node_state: failed to get state for node(%d)", nodeid );
                        usleep( 500 );
                }
                if ( val == state ) break;
        } while ( res && retries-- );

out:
        return res;
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
	xmlrpc_value *result = xmlrpc_struct_new( envP );

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_turn_switch_off: id - %d", nodeid );

        res = xmlrpc_change_node_state( ctx, nodeid, ZW_NODE_STATE_OFF );

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
	int res;
	xmlrpc_value *result = xmlrpc_struct_new( envP );

	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);

	SYSLOG_INFO( "xmlrpc_turn_switch_on: id - %d", nodeid );

        res = xmlrpc_change_node_state( ctx, nodeid, ZW_NODE_STATE_ON );

	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.turnSwitchOn" );
	xmlrpc_set_struct_int( envP, result, "Result", res );

	return result;
}

xmlrpc_value * xmlrpc_toggle_switch_on_off(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo)
{
	hzremote_ctx_S *ctx = (hzremote_ctx_S *)serverInfo;
	int nodeid;
	int res;
	xmlrpc_value *result = xmlrpc_struct_new( envP );
        
	xmlrpc_decompose_value( envP, paramArrayP, "({s:i,*})", "NodeId", &nodeid );
	dieOnFault("decompose_result", envP);
        
	SYSLOG_INFO( "xmlrpc_toggle_switch_on_off: id - %d", nodeid );
        
	/* Turn the node ON */
        res = xmlrpc_change_node_state( ctx, nodeid, ZW_NODE_STATE_ON );
        if ( res ) goto out;

        usleep(500);
        
        /* Turn the node OFF */
        res = xmlrpc_change_node_state( ctx, nodeid, ZW_NODE_STATE_OFF );

out:
	xmlrpc_set_struct_string( envP, result, "Method", "hzremote.toggleSwitchOnOff" );
	xmlrpc_set_struct_int( envP, result, "Result", res );
        
	return result;
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



