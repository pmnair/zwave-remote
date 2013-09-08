//
//  zw_node.h
//
//  Created by Praveen Murali Nair on 6/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ZW_NODE_H
#define ZW_NODE_H

#include <pthread.h>
#include "defs.h"
#include "zw_api.h"
#include "genlist.h"

#define MAX_ZW_NODE_NAME	32

#define ZW_NODE_STATE_ON	255
#define ZW_NODE_STATE_OFF	0

struct zw_node {
	list_node list;
	char name[ MAX_ZW_NODE_NAME ];
	int id;
	u8 mode;
	u8 func;
	u8 btype;		
	u8 gtype;		
	u8 stype;		
	u8 cclass;
	u8 batt_level;
	u8 state;
	pthread_mutex_t lock;
};

int
zw_node_set_batt_level( u8 id, u8 level );

int
zw_node_set_state( u8 id, u8 state );

int
zw_node_set_label( u8 id, char *label );

void
zw_node_wakeup_handler( zw_api_ctx_S *ctx, u8 nodeid );

int
zw_node_get_version( zw_api_ctx_S *ctx, u8 id, void *resp );

int
zw_node_get_value( zw_api_ctx_S *ctx, u8 id, void *resp );

int
zw_node_set_value( zw_api_ctx_S *ctx, u8 id, void *value );

int
zw_node_get_report( zw_api_ctx_S *ctx, u8 id, void *resp );

int
zw_node_get_battery_status( zw_api_ctx_S *ctx, u8 id, void *resp );

int
zw_node_set_wakeup_interval( zw_api_ctx_S *ctx, u8 id, int intvl );

int
zw_node_get_wakeup_interval( zw_api_ctx_S *ctx, u8 id, void *resp );

void 
zw_list_nodes();

list_head *
zw_get_node_list();

struct zw_node *
create_zw_node( int id );

int
register_zw_node( zw_api_ctx_S *ctx, const u8 *frame, int id );

int
unregister_zw_node( int id );


#endif /* ZW_NODE_H */

