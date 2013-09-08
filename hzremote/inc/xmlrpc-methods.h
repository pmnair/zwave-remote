
#ifndef _XMLRPC_METHODS_H_
#define _XMLRPC_METHODS_H_

#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <xmlrpc-c/config.h>

#include "zw_api.h"
#include "log.h"

typedef struct _hzremote_ctx {
	zw_api_ctx_S zw_ctx;
} hzremote_ctx_S;

xmlrpc_value * xmlrpc_get_node_list(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);

xmlrpc_value * xmlrpc_turn_switch_off(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);

xmlrpc_value * xmlrpc_turn_switch_on(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);

xmlrpc_value * xmlrpc_toggle_switch(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);

xmlrpc_value * xmlrpc_refresh_state(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);


xmlrpc_value * xmlrpc_set_node_label(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo);

#endif /* _XMLRPC_METHODS_H_ */
