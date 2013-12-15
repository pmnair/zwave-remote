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
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <xmlrpc-c/config.h>

#include "xmlrpc-utils.h"
#include "xmlrpc-methods.h"
#include "xmlconfig.h"
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
	.methodName = "hzremote.toggleSwitchOnOff",
	.methodFunction = &xmlrpc_toggle_switch_on_off,
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

static char short_opts[] = "dc:";
static const struct option long_opts[] = {
        { "daemon",	0,	0,	'd' },
        { "config",	1,	0,	'c' },
        { NULL, 0, NULL, 0 }
};

static char *usage_txt =
"Call: hzremote -d|--daemon [-c|--config <config file>]\n\n";

int main(int argc, char **argv)
{
	xmlrpc_server_abyss_parms serverparm;
	xmlrpc_registry * registryP;
	xmlrpc_env env;
	int ii;
	int c;
	int dmn = 0;
        char *config_file = NULL;
        
	while ( ( c = getopt_long( argc, argv, short_opts, long_opts, NULL ) ) != -1 )
        {
		switch( c ) {
                        case 'd':
                                dmn = 1;
                                break;
                        case 'c':
                                config_file = strdup( optarg );
                                break;
                        case '?':
                        default:
                                fprintf(stderr, "unknown option\n");
                                fprintf(stderr, "%s", usage_txt);
                                exit(1);
                }
                
        }
        if ( dmn && (0 != daemon( 0, 0 ) )) {
                SYSLOG_FAULT("Failed to daemonize");
                return 1;
	}

	if ( zw_api_init( "/dev/ttyUSB0", &hzr_ctx.zw_ctx ) ) {
		SYSLOG_FAULT("zWave API Init failed");
		return 1;
	}

        sleep(3);
        zw_list_nodes();

        if ( config_file )
        	xmlconfig_load( config_file );
        
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

	if ( config_file ) free( config_file );
	return 0;
}

/*
gcc -I ../NG-OXCI_2011-07-20/local/include -o xrpcs xmlrpc-srvr.c -L../NG-OXCI_2011-07-20/local/lib -Wl,--rpath -Wl,../NG-OXCI_2011-07-20/local/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc -lxmlrpc_util -lpthread -lparted
*/

