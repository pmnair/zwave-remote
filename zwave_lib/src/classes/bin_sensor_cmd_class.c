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


