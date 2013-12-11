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
			const xmlChar *tname = xmlTextReaderGetAttribute( reader, "name" );
			const xmlChar *on = xmlTextReaderGetAttribute( reader, "on" );
			const xmlChar *off = xmlTextReaderGetAttribute( reader, "off" );
                        
			printf( "xmlconfig_load_timer: Name=$s Id=%s ON=%s, OFF=%s\n",
                                     tname, nodeid, on, off );
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
        
out:
        return 0;
}

