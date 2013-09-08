
#ifndef _DB_UTILS_H_
#define _DB_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "log.h"

#define HZ_REMOTE_DB	"/opt/zwave/bin/hzr.db"

typedef struct _db_row { 
        char **row;
        int nrows;
} db_row_S;

int 
db_exec( const char *query, db_row_S *result );

void
db_free_result( db_row_S *result );

#endif /* _DB_UTILS_H_ */

