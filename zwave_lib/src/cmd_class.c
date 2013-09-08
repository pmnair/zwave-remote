//
//  cmd_class.c
//
//  Created by Praveen Murali Nair on 6/18/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "cmd_class.h"
#include "log.h"

LIST_HEAD( cmd_classes );

void print_cmd_classes()
{
	list_node *node = NULL;
	list_foreach(node, (&cmd_classes)) {
		SYSLOG_DEBUG( "CmdClass: %s", ((struct cmd_class *)node)->name );
	}
}

int
cc_process_generic_msg( zw_api_ctx_S *ctx, const u8* frame )
{
	u8 tempbuf[512];
	int rc = -1;
	switch ((unsigned char)frame[5]) {
		case COMMAND_CLASS_CONTROLLER_REPLICATION:
			SYSLOG_DEBUG( "COMMAND_CLASS_CONTROLLER_REPLICATION");
			// 0x1 0xb 0x0 0x4 0x2 0xef 0x5 0x21 0x31 0x7 0x1 0x1 0xf (#######!1####)
			if (frame[6] == CTRL_REPLICATION_TRANSFER_GROUP) {
				// we simply ack the group information for now
				tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
				zw_send_request( ctx, tempbuf , 1, 0, RESP_NOT_REQ, 0 );
			} else {
				// ack everything else, too
				tempbuf[0]=FUNC_ID_ZW_REPLICATION_COMMAND_COMPLETE;
				zw_send_request( ctx, tempbuf , 1, 0, RESP_NOT_REQ, 0 );
			}
			rc = 0;
			break;
		case COMMAND_CLASS_MULTI_INSTANCE:
			SYSLOG_DEBUG( "COMMAND_CLASS_MULTI_INSTANCE");
			if (frame[6] == MULTI_INSTANCE_REPORT) {
				int instanceCount = (unsigned char)frame[8];
				SYSLOG_DEBUG( "Got MULTI_INSTANCE_REPORT from node %i: Command Class 0x%x, instance count: %i",(unsigned char)frame[3],(unsigned char)frame[7], instanceCount);
				// instance count == 1 -> assume instance 1 is "main" device and don't add new device
			} else if (frame[6] == MULTI_INSTANCE_CMD_ENCAP) {
				SYSLOG_DEBUG( "Got MULTI_INSTANCE_CMD_ENCAP from node %i: instance %i Command Class 0x%x type 0x%x",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8],(unsigned char)frame[9]);
				if (frame[8] == COMMAND_CLASS_SENSOR_MULTILEVEL) {
					SYSLOG_DEBUG( "CommandSensorMultilevelReport: node=%i, inst=%i, type=%i, valuem=%i, value=%i",
							frame[3], frame[7], frame[10], frame[11], frame[12] );
				} else if ((frame[8] == COMMAND_CLASS_BASIC) && (frame[9] == BASIC_REPORT)) {
					// 41	07/22/10 12:05:17.485		0x1 0xc 0x0 0x4 0x0 0x7 0x6 0x60 0x6 0x1 0x20 0x3 0x0 0xb2 (#######`## ###) <0xb795fb90>
					// 36	07/22/10 12:05:17.485		FUNC_ID_APPLICATION_COMMAND_HANDLER: <0xb795fb90>
					// 36	07/22/10 12:05:17.485		COMMAND_CLASS_MULTI_INSTANCE <0xb795fb90>
					// 36	07/22/10 12:05:17.485		Got MULTI_INSTANCE_CMD_ENCAP from node 7: instance 1 Command Class 0x20 type 0x3 <0xb795fb90>

					SYSLOG_DEBUG( "Got basic report from node %i, instance %i, value: %i",(unsigned char)frame[3],(unsigned char)frame[7], (unsigned char) frame[10]);

				} else if ((frame[8] == COMMAND_CLASS_BASIC) && (frame[9] == BASIC_SET)) {
					SYSLOG_DEBUG( "Got basic set from node %i, instance %i, value: %i",(unsigned char)frame[3],(unsigned char)frame[7], (unsigned char) frame[10]);
				}
			}
			rc = 0;
			break;
		case COMMAND_CLASS_VERSION:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_VERSION");
			if (frame[6] == VERSION_REPORT) {
				SYSLOG_DEBUG( "REPORT: Lib.typ: 0x%x, Prot.Ver: 0x%x, Sub: 0x%x App.Ver: 0x%x, Sub: 0x%x",
					(unsigned char)frame[7], (unsigned char)frame[8], (unsigned char)frame[9], (unsigned char)frame[10], (unsigned char)frame[11]);
			}
			rc = 0;
			break;
		default:
			SYSLOG_DEBUG( "Function not implemented - unhandled command class: %x",(unsigned char)frame[5]);
			break;
		;;
	}

	return rc;
}

int
cc_process_unimplemented_msg( const u8 *frame )
{
	switch ((unsigned char)frame[5]) {
		case COMMAND_CLASS_METER:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_METER");
			break;
		case COMMAND_CLASS_MANUFACTURER_SPECIFIC:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_MANUFACTURER_SPECIFIC");
			break;
		case COMMAND_CLASS_WAKE_UP:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_WAKE_UP - ");
			break;
		case COMMAND_CLASS_SENSOR_MULTILEVEL:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_SENSOR_MULTILEVEL - ");
			break;
		case COMMAND_CLASS_SENSOR_ALARM:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_SENSOR_ALARM - ");
			break;
		case COMMAND_CLASS_SWITCH_MULTILEVEL:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_SWITCH_MULTILEVEL - ");
			if (frame[6] == SWITCH_MULTILEVEL_REPORT) {
				SYSLOG_DEBUG( "Got switch multilevel report from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
				SYSLOG_DEBUG( "Send light changed event");
			} else if ((unsigned char)frame[6] == SWITCH_MULTILEVEL_SET) {
				SYSLOG_DEBUG( "Got switch multilevel set from node %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7]);
				SYSLOG_DEBUG( "Send light changed event");
			}
			break;
		case COMMAND_CLASS_SWITCH_ALL:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_SWITCH_ALL - ");
			if (frame[6] == SWITCH_ALL_ON) {
				SYSLOG_DEBUG( "Got switch all ON from node %i",(unsigned char)frame[3]);
				SYSLOG_DEBUG( "Send light changed event");
			}
			if (frame[6] == SWITCH_ALL_OFF) {
				SYSLOG_DEBUG( "Got switch all OFF from node %i",(unsigned char)frame[3]);
				SYSLOG_DEBUG( "Send light changed event");
			}
			break;
		case COMMAND_CLASS_ALARM:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_ALARM - ");
			if (frame[6] == ALARM_REPORT) {
				SYSLOG_DEBUG( "Got ALARM from node %i, type: %i, level: %i",(unsigned char)frame[3],(unsigned char)frame[7],(unsigned char)frame[8]);
				SYSLOG_DEBUG( "Send sensor tripped changed event");
			}
			break;
		case COMMAND_CLASS_BASIC:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_BASIC - ");
			break;
		case COMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_CLIMATE_CONTROL_SCHEDULE: - ");
			if (frame[6] == SCHEDULE_GET) {
				SYSLOG_DEBUG( "Got SCHEDULE_GET from node %i for day: %i",frame[3],frame[7]);
			}
			break;
		case COMMAND_CLASS_ASSOCIATION:
			SYSLOG_DEBUG( "\nCOMMAND_CLASS_ASSOCIATION - ");
			break;
		case COMMAND_CLASS_THERMOSTAT_SETPOINT:
			SYSLOG_DEBUG( "COMMAND_CLASS_THERMOSTAT_SETPOINT - ");
			break;
		case COMMAND_CLASS_CLOCK:
			SYSLOG_DEBUG( "COMMAND_CLASS_CLOCK - ");
			break;
			;;
		case COMMAND_CLASS_THERMOSTAT_MODE:
			SYSLOG_DEBUG( "COMMAND_CLASS_THERMOSTAT_MODE - ");
			break;
			;;
		case COMMAND_CLASS_MULTI_CMD:
			SYSLOG_DEBUG( "COMMAND_CLASS_MULTI_CMD - ");
			break;
		default:
			SYSLOG_WARN( "Function not implemented - unhandled command class: %x",(unsigned char)frame[5]);
			break;
		;;
	}
	return 0;
}

int
cc_process_msg( zw_api_ctx_S *ctx, const u8* frame, u8 nodeid )
{
	list_node *node = NULL;
	struct cmd_class *cmd_cls = NULL;
	int rc = -1;
	int cls_found = 0;

	switch ( frame[ 5 ] ) {
		case COMMAND_CLASS_CONTROLLER_REPLICATION:
		case COMMAND_CLASS_MULTI_INSTANCE:
		case COMMAND_CLASS_VERSION:
			rc = cc_process_generic_msg( ctx, frame );
			goto out;
		default:
			break;
	}

	list_foreach( node, (&cmd_classes) ) {
		cmd_cls = (struct cmd_class *)node;
		if ( cmd_cls->type == frame[ 5 ] ) {
			if ( cmd_cls->process_msg )
				rc = cmd_cls->process_msg( ctx, frame, nodeid );
			else
				SYSLOG_WARN( "CmdCLass: %s does not register process msg", cmd_cls->name );
			cls_found = 1;
			break;
		}
	}

	if( !cls_found ) cc_process_unimplemented_msg( frame );
out:
	return rc;
}

int
cc_version( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp )
{
	list_node *node = NULL;
	struct cmd_class *cmd_cls = NULL;
	int rc = -1;

	list_foreach( node, (&cmd_classes) ) {
		cmd_cls = (struct cmd_class *)node;
		if ( cmd_cls->type == cls_type ) {
			if ( cmd_cls->version )
				rc = cmd_cls->version( ctx, nodeid, resp );
			else
				SYSLOG_WARN( "CmdCLass: %s does not register version", cmd_cls->name );
			break;
		}
	}
	return rc;
}

int
cc_get( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp )
{
	list_node *node = NULL;
	struct cmd_class *cmd_cls = NULL;
	int rc = -1;

	list_foreach( node, (&cmd_classes) ) {
		cmd_cls = (struct cmd_class *)node;
		if ( cmd_cls->type == cls_type ) {
			if ( cmd_cls->get )
				rc = cmd_cls->get( ctx, nodeid, resp );
			else
				SYSLOG_WARN( "CmdCLass: %s does not register get", cmd_cls->name );
			break;
		}
	}
	return rc;
}

int
cc_set( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *val )
{
	list_node *node = NULL;
	struct cmd_class *cmd_cls = NULL;
	int rc = -1;

	list_foreach( node, (&cmd_classes) ) {
		cmd_cls = (struct cmd_class *)node;
		if ( cmd_cls->type == cls_type ) {
			if ( cmd_cls->set )
				rc = cmd_cls->set( ctx, nodeid, val );
			else
				SYSLOG_WARN( "CmdCLass: %s does not register set", cmd_cls->name );
			break;
		}
	}
	return rc;
}

int
cc_report( zw_api_ctx_S *ctx, u8 nodeid, const u8 cls_type, void *resp )
{
	list_node *node = NULL;
	struct cmd_class *cmd_cls = NULL;
	int rc = -1;

	list_foreach( node, (&cmd_classes) ) {
		cmd_cls = (struct cmd_class *)node;
		if ( cmd_cls->type == cls_type ) {
			if ( cmd_cls->report )
				rc = cmd_cls->report( ctx, nodeid, resp );
			else
				SYSLOG_WARN( "CmdCLass: %s does not register report", cmd_cls->name );
			break;
		}
	}
	return rc;
}

int 
register_cmd_class( struct cmd_class *ccls )
{
	int rc = 1;
	list_add((list_node *)&cmd_classes, (list_node *)ccls);
	print_cmd_classes();
	return rc;
}

int 
unregister_cmd_class( struct cmd_class *ccls)
{
	int rc = 1;
	list_remove((list_node *)&cmd_classes, (list_node *)ccls);
	print_cmd_classes();
	return rc;
}


