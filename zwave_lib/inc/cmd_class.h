//
//  cmd_class.h
//
//  Created by Praveen Murali Nair on 6/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef CMD_CLASS_H
#define CMD_CLASS_H

#include "defs.h"
#include "zw_api.h"
#include "genlist.h"

struct cmd_class {
	list_node list;
	const char* name;
	unsigned char type;
  
	int (*process_msg)( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid );
	int (*version)( zw_api_ctx_S *ctx, u8 nodeid, void *resp );
	int (*get)( zw_api_ctx_S *ctx, u8 nodeid, void *resp );
	int (*set)( zw_api_ctx_S *ctx, u8 nodeid, void *resp );
	int (*report)( zw_api_ctx_S *ctx, u8 nodeid, void *resp );
};

int 
register_cmd_class( struct cmd_class *cops );

int 
unregister_cmd_class( struct cmd_class *cops);

int 
cc_process_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid );

int 
cc_version( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp );

int 
cc_get( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp );

int 
cc_set( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *val );

int 
cc_report( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp );

void print_cmd_classes();

#endif /* CMD_CLASS_H */

