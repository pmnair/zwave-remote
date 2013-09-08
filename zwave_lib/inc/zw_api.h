
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
