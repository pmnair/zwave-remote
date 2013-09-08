
#include "xmlrpc-utils.h"

void 
dieOnFault (char *ident, xmlrpc_env * const envP) 
{
	if (envP->fault_occurred) {
		SYSLOG_FAULT("ERROR:%s: %s (%d)\n",
		ident, envP->fault_string, envP->fault_code);
		exit(1);
	}
}

void 
assertValue ( xmlrpc_value *val ) 
{
	if ( !val ) {
		SYSLOG_FAULT( "Failed to allocate xmlrpc struct" );
		exit(1);
	}
}

void
xmlrpc_set_struct_int( xmlrpc_env * const envP,
			xmlrpc_value *pstruct, 
			const char *key,
			int val )
{
	xmlrpc_value *int_val = xmlrpc_int_new( envP, val );
	assertValue( int_val );

	xmlrpc_struct_set_value( envP, pstruct, key, int_val );
	xmlrpc_DECREF( int_val );
}

void
xmlrpc_set_struct_string( xmlrpc_env * const envP,
			xmlrpc_value *pstruct, 
			const char *key,
			const char *val )
{
	xmlrpc_value *str_val = xmlrpc_string_new( envP, val );
	assertValue( str_val );

	xmlrpc_struct_set_value( envP, pstruct, key, str_val );
	xmlrpc_DECREF( str_val );
}
