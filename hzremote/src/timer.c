//
//  timer.c
//  zwave-remote
//
//  Created by Praveen Nair on 12/12/13.
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
#include "timer.h"
#include "log.h"

typedef struct _timer
{
	int	nodeid;
	int	hour;
        int	min;
        int	sec;
        int	duration;
} timer_S ;

typedef struct _timers
{
	timer_S **timer;
        int	no;
        int	nalloc;
} timers_S ;

timers_S g_timers;

static int
decompose_time( const char *time, int *hrs, int *min, int *sec )
{
        int rc = -1;
        char *time_cp = strdup( time );
	char *token = NULL, *ptr = NULL;
        
        /* tokenize the message */
        token = strtok_r( time_cp, ":", &ptr );
        if ( !token ) goto out;
        *hrs = atoi( token );
        
        token = strtok_r( NULL, ":", &ptr );
        if ( !token ) goto out;
        *min = atoi( token );
        
        token = strtok_r( NULL, ":", &ptr );
        if ( !token ) goto out;
        *sec = atoi( token );
        
        rc = 0;
out:
        if ( time_cp ) free( time_cp );
        
        return rc;
}

void
timer_add( int nodeid, const char *time, int duration )
{
        int ii = 0;
        int hrs = -1, min = -1, sec = -1;
        timers_S *timers = &g_timers;
        
        if ( timers->no >= timers->nalloc ) {
                timers->nalloc += 5;
                
                if ( 0 == timers->no )
                        timers->timer = calloc( timers->nalloc, sizeof( timer_S * ) );
                else
                        timers->timer = realloc( timers->timer, timers->nalloc * sizeof( timer_S * ) );
                
                for( ii = timers->no; ii < timers->nalloc; ii++ )
                        timers->timer[ ii ] = calloc( 1, sizeof( timer_S ) );
        }
        
        if ( -1 == decompose_time( time, &hrs, &min, &sec ) ) {
                SYSLOG_DEBUG( "Error Decomposing time %s", time );
                return;
        }
	SYSLOG_DEBUG( "Timer for node=%d, hrs=%d, min=%d, sec=%d, dur=%d",
               			nodeid, hrs, min, sec, duration );
	ii = timers->no++;
        timers->timer[ ii ]->nodeid = nodeid;
        timers->timer[ ii ]->hour = hrs;
        timers->timer[ ii ]->min = min;
        timers->timer[ ii ]->sec = sec;
        timers->timer[ ii ]->duration = duration;
}
