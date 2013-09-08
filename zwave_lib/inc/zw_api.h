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
#ifndef _ZW_API_H_
#define _ZW_API_H_

#include "defs.h"
#include "genlist.h"

#define MAX_CMD_SZ      128
#define MAX_ZWAVE_NODES 256

typedef struct zw_api_ctx {
	int port;
	int node_id;
} zw_api_ctx_S;

typedef struct zwave_msg {
        list_node list;
        u8	cmd[ MAX_CMD_SZ ];
        int     len;
        int     port;
        int     resp_req;
	int	resp_id;
	int	node_id;
	time_t	ts;
	int	retry;
}zwave_msg_S;   

int     
zw_api_init( const char *portname, zw_api_ctx_S *ctx );

void 
zw_process_frame( zw_api_ctx_S *ctx, u8 *frame, int length );

int
zw_send_request( zw_api_ctx_S *ctx, u8 *buff, int len, int nodeid, int resp_req, int resp_id );

#endif /* _ZW_API_H_ */
