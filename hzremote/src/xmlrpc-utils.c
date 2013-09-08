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
