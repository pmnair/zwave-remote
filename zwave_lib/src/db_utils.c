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
#include "module.h"
#include "log.h"
#include "db_utils.h"

sqlite3 *pHzrDb = NULL;

static int callback( void *udata, int argc, char **argv, char **col )
{
        int i;
	db_row_S *rows = (db_row_S *)udata;
	int len = 0;
	int rowidx = rows->nrows;

	rows->row = realloc( rows->row, (++rows->nrows * sizeof(char *)) );
	if ( rows->row == NULL ) {
		perror( "rows" );
		return 1;
	}
	
        for(i=0; i<argc; i++){
		len += strlen( col[i] ) + (argv[i] ? strlen(argv[i]) : strlen("NULL")) + 2;
                printf("%s = %s\n", col[i], argv[i] ? argv[i] : "NULL");
        }
	len++;

	rows->row[ rowidx ] = calloc( len, sizeof( char ));
	if ( rows->row[ rowidx ] == NULL ) {
		perror( "row" );
		return 1;
	}

        for(i=0; i<argc; i++){
		int slen = strlen( rows->row[ rowidx ] );
		snprintf( rows->row[ rowidx ] + slen, len - slen, "%s=%s|", col[i], argv[i] ? argv[i] : "NULL");
                printf("%s = %s\n", col[i], argv[i] ? argv[i] : "NULL");
        }

        printf("\n");
        return 0;
}

int db_exec( const char *query, db_row_S *result )
{
	char *err = 0;
	int rc;

	rc = sqlite3_exec( pHzrDb, query, callback, result, &err );
	if( SQLITE_OK != rc ){
		SYSLOG_FAULT("SQL error: %sn", err );
		sqlite3_free( err );
	}

	return rc;
}

static void __init_mod db_init( void )
{
	if( sqlite3_open( HZ_REMOTE_DB, &pHzrDb ) ){
		SYSLOG_FAULT( "Can't open database: %s", sqlite3_errmsg( pHzrDb ) );
		sqlite3_close( pHzrDb );
	}
}

static void __exit_mod db_exit( void )
{
	sqlite3_close( pHzrDb );
}


