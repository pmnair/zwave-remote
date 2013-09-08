
#ifndef _XMLRPC_UTILS_H_
#define _XMLRPC_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <xmlrpc-c/config.h>

#include "log.h"

void 
dieOnFault (char *ident, xmlrpc_env * const envP);

void 
assertValue ( xmlrpc_value *val );

void
xmlrpc_set_struct_string( xmlrpc_env * const envP,
			xmlrpc_value *pstruct, 
			const char *key,
			const char *val );

void
xmlrpc_set_struct_int( xmlrpc_env * const envP,
			xmlrpc_value *pstruct, 
			const char *key,
			int val );


#endif /* _XMLRPC_UTILS_H_ */
