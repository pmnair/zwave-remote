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
#include "log.h"

static pthread_mutex_t val_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tsw_value = PTHREAD_COND_INITIALIZER;
int tsw_val = -1;

static int
toggle_sw_proc_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid )
{
	int val = -1;
	SYSLOG_DEBUG( "COMMAND_CLASS_BINARY_TOGGLE_SWITCH - processing message" );
	if ((unsigned char)frame[6] == SWITCH_TOGGLE_BINARY_SET) {
		SYSLOG_DEBUG( "SWITCH_TOGGLE_BINARY_SET received from node %d value %d", nodeid,(unsigned char)frame[7]);
		val = frame[7];
	}
	else if ((unsigned char)frame[6] == SWITCH_TOGGLE_BINARY_GET) {
		SYSLOG_DEBUG( "SWITCH_TOGGLE_BINARY_GET received from node %d value %d", nodeid,(unsigned char)frame[7]);
		val = frame[7];

	}
	else if ((unsigned char)frame[6] == SWITCH_TOGGLE_BINARY_REPORT) {
		SYSLOG_DEBUG( "SWITCH_TOGGLE_BINARY_REPORT received from node %d value %d", nodeid,(unsigned char)frame[7]);
		val = frame[7];
	}
	else {
		SYSLOG_INFO( "%i received from node %d", frame[6], nodeid);
		goto out;
	}

	pthread_mutex_lock( &val_lock );
	tsw_val = val;
	pthread_cond_broadcast( &tsw_value );
	pthread_mutex_unlock( &val_lock );

out:
	return 0;
}

static int
toggle_sw_get( zw_api_ctx_S *ctx, u8 nodeid, void *resp )
{
	u8 buff[1024];
	int rc;
	int *value = (int *)resp;
	struct timespec ts;

	buff[0] = FUNC_ID_ZW_SEND_DATA;
	buff[1] = nodeid;
	buff[2] = 2;
	buff[3] = COMMAND_CLASS_SWITCH_TOGGLE_BINARY;
	buff[4] = SWITCH_TOGGLE_BINARY_GET;
	buff[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
	buff[6] = 3;

	rc = zw_send_request( ctx, buff, 7, 3, RESP_REQ, FUNC_ID_ZW_SEND_DATA );

	pthread_mutex_lock( &val_lock );

	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 5;
	rc = pthread_cond_timedwait(&tsw_value, &val_lock, &ts);
	*value = tsw_val;
	pthread_mutex_unlock( &val_lock );

	return rc;
}

static int
toggle_sw_set( zw_api_ctx_S *ctx, u8 nodeid, void *value )
{
	u8 buff[1024];
	int level = *(int *)value;

	buff[0] = FUNC_ID_ZW_SEND_DATA;
	buff[1] = nodeid;
	buff[2] = 3;
	buff[3] = COMMAND_CLASS_SWITCH_TOGGLE_BINARY;
	buff[4] = SWITCH_TOGGLE_BINARY_SET;
	buff[5] = level;
	buff[6] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;

	return zw_send_request( ctx, buff, 7, 3, RESP_REQ, FUNC_ID_ZW_SEND_DATA );
}

static int
toggle_sw_report( zw_api_ctx_S *ctx, u8 nodeid, void *resp )
{
	return toggle_sw_get( ctx, nodeid, resp );
}

struct cmd_class toggle_sw = {
	.name		= "ToggleSwitch",
	.type		= COMMAND_CLASS_SWITCH_TOGGLE_BINARY,
	.process_msg	= toggle_sw_proc_msg,
	.get		= toggle_sw_get,
	.set		= toggle_sw_set,
	.report		= toggle_sw_report
};

static void __init_mod toggle_sw_init( void )
{
	register_cmd_class( &toggle_sw );
}

static void __exit_mod toggle_sw_exit( void )
{
	unregister_cmd_class( &toggle_sw );
	pthread_cond_destroy( &tsw_value );
	pthread_mutex_destroy( &val_lock );
}


