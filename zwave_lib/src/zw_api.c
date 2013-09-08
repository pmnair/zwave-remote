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

#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/file.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "zw_api.h"
#include "zw_node.h"
#include "cmd_class.h"
#include "log.h"

LIST_HEAD( msg_list );
LIST_HEAD( ack_wait_list );
LIST_HEAD( resp_wait_list );
pthread_mutex_t list_lock;

static int 
zw_open_port( const char *portname, int *port )
{
        struct termios tios;
	int rc = -1;

	*port = open( portname, O_RDWR | O_NOCTTY );
        if ( *port < 0) {
		perror( portname );
		goto out;
	}
 
        if ( flock( *port, LOCK_EX | LOCK_NB ) < 0 ) {
		perror( portname );
                close( *port );
                goto out;
        }
        tios.c_cflag = CS8 | CREAD | CLOCAL;

        tios.c_cc[VMIN] = 1; 
        tios.c_cc[VTIME] = 0;

        cfsetispeed( &tios, BAUDRATE );
        cfsetospeed( &tios, BAUDRATE );

        tios.c_lflag = 0;

        tios.c_iflag = IGNBRK;
        tios.c_oflag = 0;

	tcflush( *port, TCIOFLUSH );
        if ( 0 > tcsetattr( *port, TCSANOW, &tios ) ) {
		perror( portname );
                close( *port );
		goto out;
        }

        usleep ( 1000 );  
	tcflush( *port, TCIOFLUSH );
	rc = 0;
out:
        return rc;
}


static int 
zw_read_port( int port, u8 *buff, int len, long timeout )
{
        int bytes = 0;
        struct timeval tv;
        fd_set fds;
	int rc;

        while ( bytes < len ) {
                FD_ZERO( &fds );
                FD_SET( port, &fds );
                tv.tv_sec = 0;
                tv.tv_usec = 100000;

		rc = select( FD_SETSIZE, &fds, NULL, NULL, &tv );
		if ( 0 > rc ) perror("zw_read_port");
                if( 0 >= rc ) {
                        return bytes;
                }

                bytes += read( port, buff + bytes, len - bytes );
        }

        return bytes;
}

static int 
zw_write_port( const int port, u8 *buff, const int len )
{
        int rc = -1;

        rc = write( port, buff, len );
        if ( len != rc ) {
		perror( "zw_write_port" );
                goto out;
        }

        tcflush( port, TCIOFLUSH );

out:
        return rc;
}

static void 
zw_print_line( unsigned char *buff, int len )
{
	int idx = 0;
	while( idx < len ) SYSLOG_DEBUG( "%02X ", buff[ idx++ ] );
}

static u8 
zw_checksum( const u8 *buff, int len )
{
        int csum = 0xff;
	int i;

        for( i = 0; i < len; i++ )
		csum ^= buff[ i ];

        return csum;
}

static int 
zw_send_first_message( )
{
	zwave_msg_S *req = NULL;

	if ( list_empty( &msg_list ) )return 0;

	req = (zwave_msg_S *)list_pop_front( &msg_list );
	if ( !req ) return 1;	

	zw_write_port( req->port, req->cmd, req->len );

	list_add((list_node *)&ack_wait_list, (list_node *)req);

	return 0;
}

int 
zw_send_request( zw_api_ctx_S *ctx, u8 *buff, int len, int nodeid, int resp_req, int resp_id )
{
	zwave_msg_S *req;
	int index = 0;
	int i;

	pthread_mutex_lock (&list_lock);
	req = calloc( 1, sizeof( zwave_msg_S ) );
	if ( !req ) {
		SYSLOG_FAULT("calloc failed");
		return 1;
	}
	req->cmd[ index++ ] = SOF;
	req->cmd[ index++ ] = len + 2 ;
	req->cmd[ index++ ] = REQUEST ;

	for (i=0; i<len;i++ ) req->cmd[index++] = buff[i];

	req->cmd[ index ] = zw_checksum( req->cmd + 1, len + 2 );
	req->port = ctx->port;
	req->len = len + 4;
	req->node_id = nodeid;
	req->resp_req = resp_req;
	req->resp_id = resp_id;
	req->ts = time( &req->ts );	
	req->retry = 0;

	list_add((list_node *)&msg_list, (list_node *)req);
	pthread_mutex_unlock (&list_lock);

	return 0;
}

static void 
zw_purge_first_resp_wait_list( u8 resp_id )
{
	zwave_msg_S *req = NULL;
	if ( list_empty( &resp_wait_list ) ) {
		SYSLOG_FAULT("FATAL: No msgs in resp wait Q");
		goto out;
	}

	req = (zwave_msg_S *)list_front( &resp_wait_list );
	if ( !req ) { 
		SYSLOG_FAULT("FATAL: Failed to pop msg from the resp wait Q");
		goto out;
	}

	if ( req->resp_id == resp_id ) {
		list_remove( &resp_wait_list, (list_node *)req );
		free( req );
	}
	else
		SYSLOG_DEBUG("Not purging wait Q: Response ID did not match");
out:
	return;
}

static void
zw_process_resp_FUNC_ID_ZW_GET_SUC_NODE_ID( zw_api_ctx_S *ctx, u8 *frame )
{
	u8 buff[512];

	if ( 0 == frame[ 2 ] ) {
		SYSLOG_DEBUG( "No SUC, we become SUC");
		buff[0]=FUNC_ID_ZW_ENABLE_SUC;
		buff[1]=1; // 0=SUC,1=SIS
		buff[2]=ZW_SUC_FUNC_NODEID_SERVER;
		zw_send_request( ctx, buff, 3, 0, RESP_NOT_REQ, 0 );

		buff[0]=FUNC_ID_ZW_SET_SUC_NODE_ID;
		buff[1]=ctx->node_id;
		buff[2]=1; // TRUE, we want to be SUC/SIS
		buff[3]=0; // no low power
		buff[4]=ZW_SUC_FUNC_NODEID_SERVER;
		zw_send_request( ctx, buff, 5, 0, RESP_NOT_REQ, 0 );
	} else if ((unsigned char)frame[2] != ctx->node_id ) {
		SYSLOG_INFO( "requesting network update from SUC");
		buff[0]=FUNC_ID_ZW_REQUEST_NETWORK_UPDATE;
		zw_send_request( ctx, buff, 1, 0, RESP_NOT_REQ, 0 );
	}

}

static void
zw_process_resp_FUNC_ID_SERIAL_API_GET_INIT_DATA( zw_api_ctx_S *ctx, u8 *frame )
{
	u8 buff[512];
	if (frame[4] == MAGIC_LEN) {
		int i, j;
		for ( i = 5; i < 5+MAGIC_LEN; i++ ) {
			for ( j = 0; j < 8; j++ ) {
				u8 nodeid = (i-5)*8+j+1;
				if ((unsigned char)frame[i] & (0x01 << j)) {
					buff[0]=FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO;
					buff[1]=nodeid;
					SYSLOG_INFO("Requesting protocol info for: %d", nodeid);
					create_zw_node( nodeid );
					zw_send_request( ctx, buff , 2, nodeid, RESP_REQ, FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO );
				}
			}
		}
	}
}

static void
zw_process_resp_FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO( zw_api_ctx_S *ctx, u8 *frame )
{
	// test if node is valid
	if (frame[6] != 0) {

		zwave_msg_S *req = NULL;
		if ( list_empty( &resp_wait_list ) )
		{
			SYSLOG_FAULT("FATAL: No msgs in resp wait Q");
		}
		else {
			req = (zwave_msg_S *)list_front( &resp_wait_list );
			if ( !req ) {
				SYSLOG_FAULT("FATAL: Failed to get msg from the resp wait Q");
			}
			else
				register_zw_node( ctx, frame, req->node_id );
		}

		if (((unsigned char)frame[2]) & (0x01 << 7)) {
			SYSLOG_DEBUG( "listening node");
		} else {
			SYSLOG_DEBUG( "sleeping node");
		}
		if (((unsigned char)frame[3]) & (0x01 << 7)) {
			SYSLOG_DEBUG( "optional functionality");
		}
		if (((unsigned char)frame[3]) & (0x01 << 6)) {
			SYSLOG_DEBUG( "1000ms frequent listening slave");
		}
		if (((unsigned char)frame[3]) & (0x01 << 5)) {
			SYSLOG_DEBUG( "250ms frequent listening slave");
		}
		switch (frame[5]) {
			case BASIC_TYPE_CONTROLLER:
				printf( "BASIC TYPE: Controller");
				break;
			;;
			case BASIC_TYPE_STATIC_CONTROLLER:
				printf( "BASIC TYPE: Static Controller");
				break;
			;;
			case BASIC_TYPE_SLAVE:
				printf( "BASIC TYPE: Slave");
				break;
			;;
			case BASIC_TYPE_ROUTING_SLAVE:
				printf( "BASIC TYPE: Routing Slave");
				break;
			;;
			default:
				printf( "BASIC TYPE: %x",frame[5]);
				break;
			;;
		}
		switch ((unsigned char)frame[6]) {
			case GENERIC_TYPE_GENERIC_CONTROLLER:
				printf( "GENERIC TYPE: Generic Controller");
				break;
			;;
			case GENERIC_TYPE_STATIC_CONTROLLER:
				printf( "GENERIC TYPE: Static Controller");
				break;
			;;
			case GENERIC_TYPE_THERMOSTAT:
				printf( "GENERIC TYPE: Thermostat");
				break;
			;;
			case GENERIC_TYPE_SWITCH_MULTILEVEL:
				printf( "GENERIC TYPE: Multilevel Switch");
				break;
			;;
			case GENERIC_TYPE_SWITCH_REMOTE:
				printf( "GENERIC TYPE: Remote Switch");
				break;
			;;
			case GENERIC_TYPE_SWITCH_BINARY:
				printf( "GENERIC TYPE: Binary Switch");
				break;
			;;
			case GENERIC_TYPE_SENSOR_BINARY:
				printf( "GENERIC TYPE: Sensor Binary");
				break;
			case GENERIC_TYPE_WINDOW_COVERING:
				printf( "GENERIC TYPE: Window Covering");
				break;
			;;
			case GENERIC_TYPE_SENSOR_MULTILEVEL:
				printf( "GENERIC TYPE: Sensor Multilevel");
				break;
			;;
			case GENERIC_TYPE_SENSOR_ALARM:
				printf( "GENERIC TYPE: Sensor Alarm");
				break;
			;;
			default:
				printf( "GENERIC TYPE: 0x%x",frame[6]);
				break;
			;;

		}
		printf( "SPECIFIC TYPE: 0x%x",frame[7]);

	} else {
		printf("Invalid generic class (0x%x), ignoring device",(unsigned char)frame[6]);
	}

}

void 
zw_process_frame( zw_api_ctx_S *ctx, u8 *frame, int length )
{
	u8 tempbuf[512];

	if (frame[0] == RESPONSE) {
		switch ((unsigned char)frame[1]) {
			case FUNC_ID_ZW_GET_SUC_NODE_ID:
				printf("\nGot reply to GET_SUC_NODE_ID, node: %d",frame[2]);
				zw_process_resp_FUNC_ID_ZW_GET_SUC_NODE_ID( ctx, frame );
				break;
			;;
			case ZW_GET_ROUTING_INFO:
				printf("\nGot reply to ZW_GET_ROUTING_INFO:");
				break;
			;;
			case ZW_MEMORY_GET_ID:
				printf("\nGot reply to ZW_MEMORY_GET_ID:");
				printf("\nHome id: 0x%02x%02x%02x%02x, our node id: %d",(unsigned char) frame[2],(unsigned char) frame[3],(unsigned char)frame[4],(unsigned char)frame[5],(unsigned char)frame[6]);
				ctx->node_id = frame[ 6 ];
				break;
			;;
			case ZW_MEM_GET_BUFFER:
				printf("\nGot reply to ZW_MEM_GET_BUFFER");
				break;
			;;
			case ZW_MEM_PUT_BUFFER:
				printf("\nGot reply to ZW_MEM_PUT_BUFFER");
				break;
			;;
			case FUNC_ID_SERIAL_API_GET_INIT_DATA:
				printf("\nGot reply to FUNC_ID_SERIAL_API_GET_INIT_DATA:");
				zw_process_resp_FUNC_ID_SERIAL_API_GET_INIT_DATA( ctx, frame );
				break;
			;;
			case FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:
				printf("\nGot reply to FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO:");
				zw_process_resp_FUNC_ID_ZW_GET_NODE_PROTOCOL_INFO( ctx, frame );
				break;
			;;
			case FUNC_ID_ZW_REQUEST_NODE_INFO:
				printf( "\nGot reply to FUNC_ID_ZW_REQUEST_NODE_INFO:");
				break;
			;;
			case FUNC_ID_ZW_SEND_DATA:
				switch(frame[2]) {
					case 1:
						printf("\nZW_SEND delivered to Z-Wave stack");
						break;
					case 0:
						printf( "\nERROR: ZW_SEND could not be delivered to Z-Wave stack");
						break;
					default:
						printf( "\nERROR: ZW_SEND Response is invalid!");
				}

				break;
			case FUNC_ID_SERIAL_API_GET_CAPABILITIES:
				printf( "\nGot reply to FUNC_ID_SERIAL_API_GET_CAPABILITIES:");
				printf("\nSerAppV:%i,r%i,Manf %i,Typ %i,Prod %i",(unsigned char)frame[2],(unsigned char)frame[3], ((unsigned char)frame[4]<<8) + (unsigned char)frame[5],((unsigned char)frame[6]<<8) + (unsigned char)frame[7],((unsigned char)frame[8]<<8) + (unsigned char)frame[9]);	
				break;
			case ZW_GET_VERSION:
				printf( "\nGot reply to ZW_VERSION:");
				printf("\nZWave Version: %c.%c%c",(unsigned char)frame[9],(unsigned char)frame[11],(unsigned char)frame[12]);	
				break;
			default:
				printf( "\nTODO: handle response for 0x%x ",(unsigned char)frame[1]);
				break;
			;;
		}

		zw_purge_first_resp_wait_list( frame[1] );
	} else if (frame[0] == REQUEST) {

		switch (frame[1]) {
			case FUNC_ID_ZW_SEND_DATA:
			{
				printf("\nZW_SEND Response with callback %i received",(unsigned char)frame[2]);
			}
			break;
			case FUNC_ID_ZW_ADD_NODE_TO_NETWORK:
				printf( "\nFUNC_ID_ZW_ADD_NODE_TO_NETWORK:");
				break;
			case FUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK:
				printf( "\nFUNC_ID_ZW_REMOVE_NODE_FROM_NETWORK:");
				break;

			case FUNC_ID_APPLICATION_COMMAND_HANDLER:
				printf( "\nFUNC_ID_APPLICATION_COMMAND_HANDLER:");
				if ( COMMAND_CLASS_WAKE_UP == frame[5] )
					zw_node_wakeup_handler( ctx, frame[3] );

				cc_process_msg( ctx, frame, frame[3] );
				break;
			;;
			case FUNC_ID_ZW_APPLICATION_UPDATE:
				switch((unsigned char)frame[2]) {
					case UPDATE_STATE_NODE_INFO_RECEIVED:
						printf( "FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_RECEIVED received from node %d - ",(unsigned int)frame[3]);
						switch((unsigned char)frame[5]) {
							case BASIC_TYPE_ROUTING_SLAVE:
							case BASIC_TYPE_SLAVE:
								//printf( "BASIC_TYPE_SLAVE:");
								switch(frame[6]) {
									case GENERIC_TYPE_SWITCH_MULTILEVEL:
										printf( "GENERIC_TYPE_SWITCH_MULTILEVE");
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_SWITCH_MULTILEVEL;
										tempbuf[4] = SWITCH_MULTILEVEL_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										zw_send_request ( ctx, tempbuf, 6, frame[3], RESP_REQ, FUNC_ID_ZW_SEND_DATA );
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										zw_send_request( ctx, tempbuf,6, frame[3], RESP_REQ, FUNC_ID_ZW_SEND_DATA );
										break;
									case GENERIC_TYPE_SWITCH_BINARY:
										printf( "GENERIC_TYPE_SWITCH_BINARY");
										tempbuf[0] = FUNC_ID_ZW_SEND_DATA;
										tempbuf[1] = frame[3];
										tempbuf[2] = 0x02;
										tempbuf[3] = COMMAND_CLASS_BASIC;
										tempbuf[4] = BASIC_GET;
										tempbuf[5] = TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE;
										zw_send_request( ctx, tempbuf, 6, frame[3], RESP_REQ, FUNC_ID_ZW_SEND_DATA );
										break;
									case GENERIC_TYPE_SWITCH_REMOTE:
										printf( "GENERIC_TYPE_SWITCH_REMOTE");
										break;
									case GENERIC_TYPE_SENSOR_MULTILEVEL:
										printf( "GENERIC_TYPE_SENSOR_MULTILEVEL");
										break;
									;;
									default:
										printf( "unhandled class");
										break;
									;;
								}
							default:
								break;
							;;
						}
						break;
					case UPDATE_STATE_NODE_INFO_REQ_FAILED:
						printf( "FUNC_ID_ZW_APPLICATION_UPDATE:UPDATE_STATE_NODE_INFO_REQ_FAILED received");
						break;
				        case UPDATE_STATE_NEW_ID_ASSIGNED:
					{
						printf( "** Network change **: ID %d was assigned to a new Z-Wave node",(unsigned char)frame[3]);
					}
						break;
					case UPDATE_STATE_DELETE_DONE:
						printf( "** Network change **: Z-Wave node %d was removed",(unsigned char)frame[3]);
						break;
					;;
					default:
						break;
					;;
				}	
				break;
			;;
			default:
			;;


		}

	} else {
		// should not happen
	}
	fflush(stdout);
	return;
}

static int 
zw_wait_list_empty()
{
	return ( list_empty( &ack_wait_list ) && list_empty( &resp_wait_list ) );
}

static void *
zw_reader_thread( void *arg )
{
	zw_api_ctx_S *ctx = (zw_api_ctx_S *)arg;
        int port = ctx->port;
        int rc = 0;
        u8 buffer [256];
	int flen = 0;
	zwave_msg_S *req = NULL;

	while( 1 ) {
		memset( buffer, 0, 256 );
		rc = zw_read_port( port, buffer, 1, 100 );
		if ( 1 != rc ) { 
			if ( zw_wait_list_empty() ) {
				rc = zw_send_first_message( );
				if ( rc ) {
					SYSLOG_FAULT( "sending message failed" );
				}
			}
			else  {
				time_t now;
				int ack_wait = 0;
				if ( !list_empty( &ack_wait_list ) ) {
					SYSLOG_WARN("Ack Wait list not empty");
					req = (zwave_msg_S *)list_pop_front( &ack_wait_list );
					ack_wait = 1;
				}
				else if ( !list_empty( &resp_wait_list ) ) {
					SYSLOG_WARN("Resp Wait list not empty");
					req = (zwave_msg_S *)list_pop_front( &resp_wait_list );
				}
				now = time( &now );
				if ( req && ( 5 <= ( now - req->ts ) ) ) {
					SYSLOG_WARN( "Msg for node %d; wait more than 5 seconds", req->node_id );
					if ( ack_wait && !req->retry ) {
						req->retry = 1;
						SYSLOG_WARN( "Requeuing message");
						pthread_mutex_lock (&list_lock);
						list_add((list_node *)&msg_list, (list_node *)req);
						pthread_mutex_unlock (&list_lock);
					}
					else {	
						SYSLOG_FAULT( "Trashing message; retry(%d)", req->retry);
						free( req );
					}
				}	
			}
		
			continue;
		}

		switch( buffer[ 0 ] ) {
		case ACK:
			SYSLOG_DEBUG( "ACK received" );
			if ( list_empty( &ack_wait_list ) ) {
				SYSLOG_FAULT("FATAL: No msgs in ack wait Q");
				break;
			}

			req = (zwave_msg_S *)list_pop_front( &ack_wait_list );
			if ( !req ) { 
				SYSLOG_FAULT("FATAL: Failed to pop msg from the ack wait Q");
				break;
			}
			if ( req->resp_req ) {
				list_add((list_node *)&resp_wait_list, (list_node *)req);
				break;	
			}

			free( req );
			break;
		case NAK:
			SYSLOG_DEBUG( "NAK received" );
			break;
		case CAN:
			SYSLOG_DEBUG( "CAN received" );
			break;
		case SOF:
			SYSLOG_DEBUG( "SOF received" );
			rc = zw_read_port( port, buffer + 1, 1, 100 );
			if ( 1 != rc ) {
				SYSLOG_FAULT( "read framelen failed/timedout %d", rc );
				break;
			}
			flen = buffer[ 1 ];
			SYSLOG_DEBUG(" Framelen: %d", flen);
			rc = zw_read_port( port, buffer + 2, flen, 100 );
			if ( flen != rc ) {
				SYSLOG_FAULT( "read frame failed/timedout %d", rc );
				break;
			}
			zw_print_line( buffer, flen );
			buffer[0] = ACK; 
			zw_write_port( port, buffer, 1 );
			zw_process_frame( ctx, buffer + 2, flen - 2 );
			fflush(stdout);
			break;
		default:
			SYSLOG_WARN( "Unknown Frame Type received: %02x", buffer[ 0 ] );
		}
	}

	return NULL;
}

int 
zw_api_init( const char *portname, zw_api_ctx_S *ctx )
{
	int rc = -1;
	unsigned char buffer[256];
	pthread_t readThread;

	ctx->node_id = -1;
	rc = zw_open_port ( portname, &ctx->port );	
	if ( rc ) {
		SYSLOG_FAULT("Failed to open port");
		goto out;
	}

	pthread_mutex_init(&list_lock, NULL);

        buffer[0] = 0x15; //NAK
        write( ctx->port, buffer, 1 );
        pthread_create( &readThread, NULL, zw_reader_thread, (void*)ctx );

        buffer[0] = ZW_GET_VERSION;
        zw_send_request( ctx, buffer , 1, 0, RESP_REQ, ZW_GET_VERSION );

        buffer[0] = ZW_MEMORY_GET_ID;
        zw_send_request( ctx, buffer , 1, 0, RESP_REQ, ZW_MEMORY_GET_ID );

        buffer[0] = FUNC_ID_SERIAL_API_GET_CAPABILITIES;
        zw_send_request( ctx, buffer , 1, 0, RESP_REQ, FUNC_ID_SERIAL_API_GET_CAPABILITIES );

        buffer[0] = FUNC_ID_ZW_GET_SUC_NODE_ID;
        zw_send_request( ctx, buffer , 1, 0, RESP_REQ, FUNC_ID_ZW_GET_SUC_NODE_ID );

        buffer[0] = FUNC_ID_SERIAL_API_GET_INIT_DATA;
        zw_send_request( ctx, buffer , 1, 0, RESP_REQ, FUNC_ID_SERIAL_API_GET_INIT_DATA );

	rc = 0;

out:
        return rc;
}
	

