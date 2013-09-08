
#include <stdio.h>
#include <pthread.h>

#include "module.h"
#include "cmd_class.h"
#include "zw_api.h"
#include "zw_node.h"

#include "log.h"

static int
batt_proc_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid )
{
	int val = -1;

	SYSLOG_DEBUG( "COMMAND_CLASS_BATTERY - processing message");
	if ((unsigned char)frame[6] == BATTERY_REPORT) {
		SYSLOG_DEBUG( "BATTERY_REPORT: Battery level: %d",(unsigned char)frame[7]);
		val = frame[ 7 ];
		if ((unsigned char)frame[7] == 0xff) {
			SYSLOG_DEBUG( "Battery low warning from node %d", nodeid );
		}
		if ( 0 != zw_node_set_batt_level( nodeid, val ) )
			SYSLOG_DEBUG( "Setting node battery level failed" );
	}

	return 0;
}

static int
batt_get( zw_api_ctx_S *ctx, u8 nodeid, void *resp )
{
	u8 buff[1024];
	int rc;

	buff[0] = FUNC_ID_ZW_SEND_DATA;
	buff[1] = nodeid;
	buff[2] = 2;
	buff[3] = COMMAND_CLASS_BATTERY;
	buff[4] = BATTERY_GET;
	buff[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	buff[6] = 3;

	rc = zw_send_request( ctx, buff, 7, nodeid, RESP_REQ, FUNC_ID_ZW_SEND_DATA );

	return rc;
}

struct cmd_class batt = {
	.name		= "Battery",
	.type		= COMMAND_CLASS_BATTERY,
	.process_msg	= batt_proc_msg,
	.get		= batt_get,
};

static void __init_mod batt_init( void )
{
	register_cmd_class( &batt );
}

static void __exit_mod batt_exit( void )
{
	unregister_cmd_class( &batt );
}


