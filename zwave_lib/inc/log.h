
#ifndef _LOG_H_
#define _LOG_H_

#include <syslog.h>

#define SYSLOG_FAULT( fmt, ... )	syslog( LOG_CRIT, fmt, ## __VA_ARGS__ );
#define SYSLOG_INFO( fmt, ... )		syslog( LOG_NOTICE, fmt, ## __VA_ARGS__ );
#define SYSLOG_DEBUG( fmt, ... )	syslog( LOG_DEBUG, fmt, ## __VA_ARGS__ );
#define SYSLOG_WARN( fmt, ... )		syslog( LOG_WARNING, fmt, ## __VA_ARGS__ );


#endif /* _LOG_H_ */
