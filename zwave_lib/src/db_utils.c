
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


