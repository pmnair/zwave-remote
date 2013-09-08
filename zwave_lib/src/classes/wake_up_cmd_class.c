
#include <stdio.h>
#include <pthread.h>

#include "module.h"
#include "cmd_class.h"
#include "zw_api.h"

#include "log.h"


static int 
wake_up_proc_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid )
{
        u8 buff[1024];

	SYSLOG_DEBUG( "COMMAND_CLASS_WAKE_UP - processing message");
	// 0x1 0x8 0x0 0x4 0x4 0x2 0x2 0x84 0x7 0x74 (#########t)
	// if ((COMMAND_CLASS_WAKE_UP == frame[5]) && ( frame[6] == WAKE_UP_NOTIFICATION)) {

	if (frame[6] == WAKE_UP_NOTIFICATION) {

		// handle broadcasts from unconfigured devices
		if (frame[2] & RECEIVE_STATUS_TYPE_BROAD ) {
			SYSLOG_DEBUG( "Got broadcast wakeup from node %i, doing WAKE_UP_INTERVAL_SET",frame[3]);
		} else {
			SYSLOG_DEBUG( "Got unicast wakeup from node %i, doing WAKE_UP_NO_MORE_INFORMATION",frame[3]);
			// send to sleep
			buff[0]=FUNC_ID_ZW_SEND_DATA;
			buff[1]=frame[3]; // destination
			buff[2]=2; // length
			buff[3]=COMMAND_CLASS_WAKE_UP;
			buff[4]=WAKE_UP_NO_MORE_INFORMATION;
			buff[5]=TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
			zw_send_request( ctx, buff, 6, nodeid, RESP_REQ, FUNC_ID_ZW_SEND_DATA );
		}

	} else {
		SYSLOG_DEBUG( "%i received from node %d", frame[6], nodeid );
	}
	fflush(stdout);
	return 0;
}

static int 
wake_up_set( zw_api_ctx_S *ctx, u8 nodeid, void *value )
{
        u8 buff[1024];
	int interval = *(int *)value;


        buff[0] = FUNC_ID_ZW_SEND_DATA;
        buff[1] = nodeid;
        buff[2] = 6;
        buff[3] = COMMAND_CLASS_WAKE_UP;
        buff[4] = WAKE_UP_INTERVAL_SET;
        buff[5] = (u8)( (interval >> 16) & 0xff );
        buff[6] = (u8)( (interval >> 8) & 0xff );
        buff[7] = (u8)( interval & 0xff );
        buff[8] = ctx->node_id;
        buff[9] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	buff[10] = 3;

	SYSLOG_DEBUG( "wake_up_set: %d %x %x %x", interval, buff[5], buff[6], buff[7]  );
        return zw_send_request( ctx, buff, 11, nodeid, RESP_REQ, FUNC_ID_ZW_SEND_DATA );
}

static int 
wake_up_get( zw_api_ctx_S *ctx, u8 nodeid, void *value )
{
        u8 buff[1024];

        buff[0] = FUNC_ID_ZW_SEND_DATA;
        buff[1] = nodeid;
        buff[2] = 2;
        buff[3] = COMMAND_CLASS_WAKE_UP;
        buff[4] = WAKE_UP_INTERVAL_GET;
        buff[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
        buff[6] = 3;

        return zw_send_request( ctx, buff, 6, nodeid, RESP_REQ, FUNC_ID_ZW_SEND_DATA );
}


struct cmd_class wake_up = {
        .name		= "WakeUp",
	.type		= COMMAND_CLASS_WAKE_UP,
	.process_msg	= wake_up_proc_msg,
	.set		= wake_up_set,
	.get		= wake_up_get,
};

static void __init_mod wake_up_init( void )
{
	register_cmd_class( &wake_up );
}

static void __exit_mod wake_up_exit( void )
{
	unregister_cmd_class( &wake_up );
}

