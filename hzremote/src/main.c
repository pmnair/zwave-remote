#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <xmlrpc-c/config.h>

#include "xmlrpc-utils.h"
#include "xmlrpc-methods.h"
#include "zw_node.h"

hzremote_ctx_S hzr_ctx;

struct xmlrpc_method_info3 const methodInfo[] = {
	{
	.methodName = "hzremote.getNodeList",
	.methodFunction = &xmlrpc_get_node_list,
	.serverInfo = &hzr_ctx,
	},
	{
	.methodName = "hzremote.turnSwitchOff",
	.methodFunction = &xmlrpc_turn_switch_off,
	.serverInfo = &hzr_ctx,
	},
	{
	.methodName = "hzremote.turnSwitchOn",
	.methodFunction = &xmlrpc_turn_switch_on,
	.serverInfo = &hzr_ctx,
	},
	{
	.methodName = "hzremote.toggleSwitch",
	.methodFunction = &xmlrpc_toggle_switch,
	.serverInfo = &hzr_ctx,
	},
	{
	.methodName = "hzremote.refreshState",
	.methodFunction = &xmlrpc_refresh_state,
	.serverInfo = &hzr_ctx,
	},
	{
	.methodName = "hzremote.setNodeLabel",
	.methodFunction = &xmlrpc_set_node_label,
	.serverInfo = &hzr_ctx,
	}
};

int main(int argc, char **argv)
{
	xmlrpc_server_abyss_parms serverparm;
	xmlrpc_registry * registryP;
	xmlrpc_env env;
	int ii;
		
	if ( ( argc > 1 ) && ( 0 == strncmp( argv[1], "--daemon", 8 ) ) ) {
		if ( 0 != daemon( 0, 0 ) ) {
			SYSLOG_FAULT("Failed to daemonize");
			return 1;
		}
	}

	if ( zw_api_init( "/dev/ttyUSB0", &hzr_ctx.zw_ctx ) ) {
		SYSLOG_FAULT("zWave API Init failed");
		return 1;
	}

        sleep(3);
        zw_list_nodes();

	xmlrpc_env_init(&env);
	dieOnFault("init", &env);

	registryP = xmlrpc_registry_new(&env);
	dieOnFault("init_registry", &env);

	for ( ii = 0; ii < ( sizeof( methodInfo ) / sizeof( struct xmlrpc_method_info3 ) ); ii++ )
	{
		xmlrpc_registry_add_method3(&env, registryP, &methodInfo[ii]);
		dieOnFault("add_method", &env);
	}

	serverparm.config_file_name = NULL;
	serverparm.registryP = registryP;
	serverparm.port_number = 8080;
	serverparm.log_file_name = "/tmp/xmlrpc_log";

	printf("Starting XML-RPC server...\n");

	xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(registryP));

	return 0;
}

/*
gcc -I ../NG-OXCI_2011-07-20/local/include -o xrpcs xmlrpc-srvr.c -L../NG-OXCI_2011-07-20/local/lib -Wl,--rpath -Wl,../NG-OXCI_2011-07-20/local/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc -lxmlrpc_util -lpthread -lparted
*/

