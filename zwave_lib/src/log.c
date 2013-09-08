
#include <stdio.h>
#include "module.h"
#include "log.h"

static void __init_mod log_init( void )
{
	openlog( "homezremote", LOG_PID, LOG_DAEMON );
	setlogmask( LOG_UPTO(LOG_INFO) );
}

static void __exit_mod log_exit( void )
{
	closelog();
}


