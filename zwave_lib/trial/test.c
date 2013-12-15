//
//  test.c
//  zwave-remote
//
//  Created by Praveen Nair on 12/2/13.
//
//

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>

typedef enum {
        TIMER_TYPE_ON_OFF = 1,
        TIMER_TYPE_TOGGLE = 2,
        TIMER_TYPE_MAX
} timer_type_E ;

typedef struct _timer
{
	int		nodeid;
	int		hour;
        int		min;
        int		sec;
        int		duration;
        timer_type_E	type;
} timer_S ;

typedef struct _timers
{
	timer_S **timer;
        int	no;
        int	nalloc;
        pthread_t tid;
} timers_S ;

int
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
timer_add( timers_S *timers, int nodeid, const char *time, const char *type, int duration )
{
        int ii = 0;
        int hrs = -1, min = -1, sec = -1;
        timer_type_E timer_type;
        
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
                printf( "Error Decomposing time %s\n", time );
                return;
        }
        if ( 0 == strncmp( type, "OnOff", 5 ) )
                timer_type = TIMER_TYPE_ON_OFF;
        else if  ( 0 == strncmp( type, "Toggle", 6 ) )
                timer_type = TIMER_TYPE_TOGGLE;
        else {
                printf( "Unknown timer type %s\n", type );
                return;
        }
 
	printf( "Timer for node=%d, hrs=%d, min=%d, sec=%d, dur=%d type=%s(%d)\n",
               			nodeid, hrs, min, sec, duration, type, timer_type );
	ii = timers->no++;
        timers->timer[ ii ]->nodeid = nodeid;
        timers->timer[ ii ]->hour = hrs;
        timers->timer[ ii ]->min = min;
        timers->timer[ ii ]->sec = sec;
        timers->timer[ ii ]->duration = duration;
        timers->timer[ ii ]->type = timer_type;
}

void process_on_off_timer( timer_S *timer )
{
       
}

void *timer_server( void *arg )
{
	timers_S *timers = (timers_S *)arg;
        struct timespec now;
        int err;
        int ii;
        time_t tnow;
        struct tm now;
        
        while( 1 ) {
                time( &tnow );
                localtime_r( &tnow, &now );
                
        	for ( ii = 0; ii < timers->no; ii++ ) {
                        switch( timers->timer[ ii ]->type )
                        {
                                case TIMER_TYPE_ON_OFF:
                                        if ( timers->timer[ ii ]->active ||
                                             ((timers->timer[ ii ]->hrs == now.tm_hour) &&
                                              (timers->timer[ ii ]->min == now.tm_min) &&
                                              (timers->timer[ ii ]->sec <= now.tm_sec)) )
                                        {
                                                process_on_off_timer( timers->timer[ ii ] );
                                        }
                                        break;
                                case TIMER_TYPE_TOGGLE:
                                        break;
                        }
                }
        }
        
        return NULL;
}

timers_S timers;

int timer_server_init()
{
        if ( timers.no )
        	return pthread_create( &timers.tid, NULL, timer_server, (void *)&timers );
        
        return 0;
}

static int
xmlconfig_load_timer( xmlTextReaderPtr reader )
{
	int ret;
	const xmlChar *name;
	int cntr = 0;
        
	printf("xmlconfig_load_timer: ...\n");
	ret = xmlTextReaderRead( reader );
	while ( ret == 1 ) {
		name = xmlTextReaderConstName( reader );
		if ( name &&
                    (XML_READER_TYPE_END_ELEMENT != xmlTextReaderNodeType( reader )) &&
                    ( 0 == strncmp( name, "Timer", 5 ) ) )
		{
			const xmlChar *nodeid = xmlTextReaderGetAttribute( reader, "node" );
			const xmlChar *time = xmlTextReaderGetAttribute( reader, "time" );
			const xmlChar *duration = xmlTextReaderGetAttribute( reader, "duration" );
                        
                        timer_add( &timers, atoi((const char *)nodeid), (const char *)time, atoi((const char *)duration) );
			printf( "xmlconfig_load_timer: Node Id=%s time=%s, duration=%s\n",
                                     nodeid, time, duration );
                }
		if ( (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType( reader )) &&
			(0 == strncmp(name, "TimerConfig", 11)))
			break;
                
		ret = xmlTextReaderRead( reader );
	}
	return ret;
}

int load_general_configs( xmlTextReaderPtr reader )
{
	int ret;
	const xmlChar *name;
	int cntr = 0;
        
	ret = xmlTextReaderRead( reader );
	while ( ret == 1 ) {
		name = xmlTextReaderConstName( reader );
		if ( name && XML_READER_TYPE_END_ELEMENT != xmlTextReaderNodeType( reader ))
                        printf( "%d - Name=%s\n", ++cntr, name );
		if ( (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType( reader )) &&
			(0 == strncmp(name, "General", 7)))
			break;
		ret = xmlTextReaderRead( reader );
	}
	return ret;
}

int enumerate_node_configs( xmlTextReaderPtr reader )
{
	int ret;
	const xmlChar *name;
	int cntr = 0;
        
	ret = xmlTextReaderRead( reader );
	while ( ret == 1 ) {
		name = xmlTextReaderConstName( reader );
		if ( name &&
                    (XML_READER_TYPE_END_ELEMENT != xmlTextReaderNodeType( reader )) &&
                    ( 0 == strncmp( name, "Node", 7 ) ) )
		{
			const xmlChar *id = xmlTextReaderGetAttribute( reader, "id" );
			const xmlChar *nname = xmlTextReaderGetAttribute( reader, "name" );
			const xmlChar *type = xmlTextReaderGetAttribute( reader, "type" );
                        
			printf( "%d - Name=%s, Id=%s, type=%s\n", ++cntr, nname, id, type );
		}
		if ( (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType( reader )) &&
			(0 == strncmp(name, "NodeConfig", 10)))
			break;

		ret = xmlTextReaderRead( reader );
	}
	return ret;
}

int read_config_file( const char *filename )
{
	int rc = -1;
	int ret;
	xmlTextReaderPtr reader;
	const xmlChar *name;
        
	reader = xmlReaderForFile( filename, NULL, 0 );
	if ( reader != NULL ) {
		ret = xmlTextReaderRead( reader );
		while (ret == 1) {
			name = xmlTextReaderConstName( reader );
			printf("---%s\n", name);
			if ( name &&
                            (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType( reader )) &&
                            ( 0 == strncmp( name, "NodeConfig", 10 ) ) )
			{
				enumerate_node_configs( reader );
			}
                        else if ( name &&
                                 (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType( reader )) &&
                                 ( 0 == strncmp( name, "General", 7 ) ) )
                        {
                                load_general_configs( reader );
                        }
                        else if ( name &&
                                 (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType( reader )) &&
                                 ( 0 == strncmp( name, "TimerConfig", 11 ) ) )
                        {
                                xmlconfig_load_timer( reader );
                        }
			ret = xmlTextReaderRead( reader );
			printf("ret=%d\n", ret);
		}
		xmlFreeTextReader( reader );
		if (ret != 0) {
			fprintf( stderr, "%s : failed to parse\n", filename );
			goto out;
		}
		rc = 0;
	} else {
		fprintf( stderr, "Unable to open %s\n", filename );
	}
out:
	return rc;
}

int main( int argc, char **argv )
{
        if ( argc < 2 ) {
                printf( " usage: %s <config_File>\n", argv[0] );
                goto out;
        }
	read_config_file( argv[1] );
        
        timer_server_init();
        
        while( getchar() != 'q' )
                sleep( 5 );
out:
        return 0;
}

