//
//  zw_node.c
//
//  Created by Praveen Murali Nair on 6/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "zw_node.h"
#include "cmd_class.h"
#include "log.h"

LIST_HEAD( zw_nodes );
LIST_HEAD( zw_nodes_pending );

static u8
get_cmd_class( const u8 gtype )
{
	u8 cmd_cls = COMMAND_CLASS_BASIC;

	switch ( gtype )
	{
	case GENERIC_TYPE_SWITCH_BINARY:
		cmd_cls = COMMAND_CLASS_SWITCH_BINARY;
		break;
	case GENERIC_TYPE_SWITCH_TOGGLE:
		cmd_cls = COMMAND_CLASS_SWITCH_TOGGLE_BINARY;
		break;
	case GENERIC_TYPE_SENSOR_BINARY:
		cmd_cls = COMMAND_CLASS_SENSOR_BINARY;
		break;
	default:
		break;
	}

	return cmd_cls;
}

int
zw_node_set_batt_level( u8 id, u8 level )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			pthread_mutex_lock( &zwnode->lock );
			zwnode->batt_level = level;
			pthread_mutex_unlock( &zwnode->lock );
			rc = 0;
			break;
		}
	}

	return rc;
}

int
zw_node_set_state( u8 id, u8 state )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			pthread_mutex_lock( &zwnode->lock );
			if ( zwnode->state != state )
				SYSLOG_INFO( "State Change on node(%d): %d", id, state );
			zwnode->state = state;
			pthread_mutex_unlock( &zwnode->lock );
			rc = 0;
			break;
		}
	}

	return rc;
}

int
zw_node_set_label( u8 id, char *label )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			pthread_mutex_lock( &zwnode->lock );
			snprintf( zwnode->name, MAX_ZW_NODE_NAME, "%s", label );
			pthread_mutex_unlock( &zwnode->lock );
			rc = 0;
			break;
		}
	}

	return rc;

}

void
zw_node_wakeup_handler( zw_api_ctx_S *ctx, u8 id )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int batt = -1;
	int state = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			if ( 0 != cc_get( ctx, id, COMMAND_CLASS_BATTERY, (void *)&batt ) )
				SYSLOG_FAULT( "Get Battery state command failed for node %d", id );
			if ( 0 != cc_get( ctx, id, zwnode->cclass, (void *)&state ) )
				SYSLOG_FAULT( "Get state command failed for node %d", id );
			break;
		}
	}
}

int
zw_node_get_version( zw_api_ctx_S *ctx, u8 id, void *resp )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			rc = cc_version( ctx, id, zwnode->cclass, resp );
			break;
		}
	}

	return rc;
}

int
zw_node_get_value( zw_api_ctx_S *ctx, u8 id, void *resp )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			rc = cc_get( ctx, id, zwnode->cclass, resp );
			break;
		}
	}

	return rc;
}

int
zw_node_set_value( zw_api_ctx_S *ctx, u8 id, void *value )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			rc = cc_set( ctx, id, zwnode->cclass, value );
			break;
		}
	}

	return rc;
}

int
zw_node_get_report( zw_api_ctx_S *ctx, u8 id, void *resp )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			rc = cc_report( ctx, id, zwnode->cclass, resp );
			break;
		}
	}

	return rc;
}

int
zw_node_get_battery_status( zw_api_ctx_S *ctx, u8 id, void *resp )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;
		if ( zwnode->id == id ) {
			rc = cc_get( ctx, id, COMMAND_CLASS_BATTERY, resp );
			break;
		}
	}

	return rc;
}

int
zw_node_set_wakeup_interval( zw_api_ctx_S *ctx, u8 id, int intvl )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;
	int val = intvl;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;
		if ( zwnode->id == id ) {
			rc = cc_set( ctx, id, COMMAND_CLASS_WAKE_UP, (void *)&val );
			break;
		}
	}

	return rc;
}

int
zw_node_get_wakeup_interval( zw_api_ctx_S *ctx, u8 id, void *resp )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;
		if ( zwnode->id == id ) {
			rc = cc_get( ctx, id, COMMAND_CLASS_WAKE_UP, resp );
			break;
		}
	}

	return rc;
}

void 
zw_list_nodes()
{
	list_node *node = NULL;
	struct zw_node *zwnode;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		SYSLOG_INFO( "ZW_NODE: %s - %d\n", zwnode->name, zwnode->id );
/*
		printf( "	btype: %s\n", zw_get_basic_type( zwnode->btype ) );
		printf( "	gtype: %s\n", zw_get_gen_type( zwnode->gtype ) );
		printf( "	stype: %s\n", zw_get_spec_type( zwnode->stype ) );
		printf( "	cclass: %s\n", zw_get_cmd_class( zwnode->cclass ) );
*/
	}
}

list_head *
zw_get_node_list()
{
	return (&zw_nodes);
}

struct zw_node *
create_zw_node( int id )
{
	struct zw_node *zwnode = calloc( 1, sizeof( struct zw_node ) );
	if ( !zwnode ) {
		perror( "zw_node" );
		goto out;
	}

	zwnode->id     = id;
	pthread_mutex_init( &zwnode->lock, NULL );

	list_add((list_node *)&zw_nodes, (list_node *)zwnode);
out:
	return zwnode;
}

int 
register_zw_node( zw_api_ctx_S *ctx, const u8 *frame, int id )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	SYSLOG_INFO( "register_zw_node: %d\n", id );
	fflush(stdout);
	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {

			zwnode->mode  = frame[ 2 ];	
			zwnode->func  = frame[ 3 ];	
			zwnode->btype  = frame[ 5 ];	
			zwnode->gtype  = frame[ 6 ];	
			zwnode->stype  = frame[ 7 ];	
			zwnode->cclass = get_cmd_class( zwnode->gtype );
			cc_get( ctx, id, zwnode->cclass, (void *)&zwnode->state );
			rc = 0;
			break;
		}
	}

	return rc;
}

int 
unregister_zw_node( int id )
{
	list_node *node = NULL;
	struct zw_node *zwnode;
	int rc = -1;

	list_foreach(node, (&zw_nodes)) {
		zwnode = (struct zw_node *)node;	
		if ( zwnode->id == id ) {
			rc = 0;
			break;
		}
	}

	if ( 0 == rc ) 
		list_remove((list_node *)&zw_nodes, (list_node *)zwnode);

	return rc;
}


