//
//  xmlconfig.c
//  zwave-remote
//
//  Created by Praveen Murali Nair on 12/10/13.
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
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/xmlreader.h>
#include "zw_api.h"
#include "zw_node.h"
#include "log.h"

static int
xmlconfig_load_timer( xmlTextReaderPtr reader )
{
	int ret;
	const xmlChar *name;
        
	ret = xmlTextReaderRead( reader );
	while ( ret == 1 ) {
		name = xmlTextReaderConstName( reader );
		if ( name &&
                    (XML_READER_TYPE_END_ELEMENT != xmlTextReaderNodeType( reader )) &&
                    ( 0 == xmlStrncmp( name, (const xmlChar *)"Timer", 7 ) ) )
		{
			const xmlChar *nodeid = xmlTextReaderGetAttribute( reader, (const xmlChar*)"node" );
			const xmlChar *tname = xmlTextReaderGetAttribute( reader, (const xmlChar*)"name" );
			const xmlChar *on = xmlTextReaderGetAttribute( reader, (const xmlChar*)"on" );
			const xmlChar *off = xmlTextReaderGetAttribute( reader, (const xmlChar*)"off" );
                        
			SYSLOG_DEBUG( "xmlconfig_load_timer: Name=%s Id=%s ON=%s, OFF=%s",
                                     tname, nodeid, on, off );
                }
		if ( (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType( reader )) &&
			(0 == xmlStrncmp(name, (const xmlChar *)"TimerConfig", 11)))
			break;

		ret = xmlTextReaderRead( reader );
	}
	return ret;
}

static int
xmlconfig_load_node( xmlTextReaderPtr reader )
{
	int ret;
	const xmlChar *name;
        
	ret = xmlTextReaderRead( reader );
	while ( ret == 1 ) {
		name = xmlTextReaderConstName( reader );
		if ( name &&
                    (XML_READER_TYPE_END_ELEMENT != xmlTextReaderNodeType( reader )) &&
                    ( 0 == xmlStrncmp( name, (const xmlChar *)"Node", 7 ) ) )
		{
			const xmlChar *id = xmlTextReaderGetAttribute( reader, (const xmlChar*)"id" );
			const xmlChar *nname = xmlTextReaderGetAttribute( reader, (const xmlChar*)"name" );
			const xmlChar *type = xmlTextReaderGetAttribute( reader, (const xmlChar*)"type" );
                        
			SYSLOG_DEBUG( "xmlconfig_load_node: Name=%s Id=%s Type=%s",
                                     	nname, id, type );
                        
                        ret = zw_node_set_label( atoi((const char*)id), (char *)nname );
                        if ( ret )
                                SYSLOG_DEBUG( "xmlconfig_load_node: set label for node(%d) failed %d", atoi((const char*)id), ret );
		}
		if ( (XML_READER_TYPE_END_ELEMENT == xmlTextReaderNodeType( reader )) &&
			(0 == xmlStrncmp(name, (const xmlChar *)"NodeConfig", 10)))
			break;
                
		ret = xmlTextReaderRead( reader );
	}
	return ret;
}

int xmlconfig_load( const char *filename )
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
			if ( name &&
                            (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType( reader )) &&
                            ( 0 == xmlStrncmp( name, (const xmlChar *)"NodeConfig", 10 ) ) )
			{
				xmlconfig_load_node( reader );
			}
			else if ( name &&
                            (XML_READER_TYPE_ELEMENT == xmlTextReaderNodeType( reader )) &&
                            ( 0 == xmlStrncmp( name, (const xmlChar *)"TimerConfig", 10 ) ) )
			{
				xmlconfig_load_timer( reader );
			}
			ret = xmlTextReaderRead( reader );
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
