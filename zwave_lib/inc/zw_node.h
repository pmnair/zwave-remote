//
//  zw_node.h
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

