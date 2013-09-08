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
