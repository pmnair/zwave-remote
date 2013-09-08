//
//  cmd_class.h
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

