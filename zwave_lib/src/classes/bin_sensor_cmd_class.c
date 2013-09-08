
#include <stdio.h>
#include <pthread.h>

#include "module.h"
#include "cmd_class.h"
#include "zw_api.h"
#include "zw_node.h"

#include "log.h"

static int
bin_sensor_proc_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid )
{
	int val = -1;

	SYSLOG_DEBUG( "COMMAND_CLASS_SENSOR_BINARY - processing message");
	if (frame[6] == SENSOR_BINARY_REPORT) {
		SYSLOG_DEBUG( "Got sensor report from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
		val = frame[ 7 ];
		if ( 0 != zw_node_set_state( nodeid, val ) )
			SYSLOG_DEBUG( "Setting node bin sensor state failed" );

	}

	return 0;
}

static int
bin_sensor_get( zw_api_ctx_S *ctx, u8 nodeid, void *resp )
{
	u8 buff[1024];
	int rc;

	buff[0] = FUNC_ID_ZW_SEND_DATA;
	buff[1] = nodeid;
	buff[2] = 2;
	buff[3] = COMMAND_CLASS_SENSOR_BINARY;
	buff[4] = SENSOR_BINARY_GET;
	buff[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	buff[6] = 3;

	rc = zw_send_request( ctx, buff, 7, nodeid, RESP_REQ, FUNC_ID_ZW_SEND_DATA );

	return rc;
}


struct cmd_class bin_sensor = {
	.name		= "BinarySensor",
	.type		= COMMAND_CLASS_SENSOR_BINARY,
	.process_msg	= bin_sensor_proc_msg,
	.get		= bin_sensor_get,
};

static void __init_mod bin_sensor_init( void )
{
	register_cmd_class( &bin_sensor );
}

static void __exit_mod bin_sensor_exit( void )
{
	unregister_cmd_class( &bin_sensor );
}


