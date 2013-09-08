
#include <stdio.h>
#include <unistd.h>
#include "cmd_class.h"
#include "zw_api.h"
#include "zw_node.h"

int main( )
{
	zw_api_ctx_S ctx;
	int rc;
	int val;
	int opt;
	
	printf( "In main\n" );
	fflush(stdout);
	print_cmd_classes();

#ifdef __MACH__
	rc = zw_api_init( "/dev/tty.SLAB_USBtoUART", &ctx );
#else
	rc = zw_api_init( "/dev/ttyUSB0", &ctx );
#endif
	if ( rc ) {
		printf("zWave API Init failed\n");
		return 1;
	}

	sleep(3);
	zw_list_nodes();
	
        do {

                printf("\n=== MENU ===\n");
                printf("1. List Nodes\n");
                printf("2. Switch state ON\n");
                printf("3. Switch state OFF\n");
                printf("4. SWitch State Get\n");
                printf("5. Get Sensor State\n");
                printf("6. Get Sensor Battery Level\n");
		printf("7. Set sensor interval\n");
		printf("8. Get sensor interval\n");
                printf("9. Exit\n");

                scanf( "%d", &opt );
                switch( opt ) {
                case 1:
                        zw_list_nodes();
                        break;
                case 2:
			val = 255;
			zw_node_set_value( &ctx, 3, (void *)&val );
                        break;
                case 3:
			val = 0;
			zw_node_set_value( &ctx, 3, (void *)&val );
                        break;
                case 4:
        		zw_node_get_value( &ctx, 3, (void *)&val );
			printf("\n$$$$$$ Switch Value: %d\n", val);
                        break;
                case 5:
        		zw_node_get_value( &ctx, 4, (void *)&val );
			printf("\n$$$$$$ Sensor State Value: %d\n", val);
                        break;
                case 6:
			zw_node_get_battery_status( &ctx, 4, (void *)&val );
			printf("\n$$$$$$ Sensor battery Value: %d\n", val);
                        break;
		case 7:
			//zw_node_set_wakeup_interval( &ctx, 4, 7200 );
			zw_node_set_wakeup_interval( &ctx, 4, 61 );
			break;
		case 8:
			zw_node_get_wakeup_interval( &ctx, 4, (void *)&val );
			printf( "Wake up interval: %d\n", val );
			break;
                case 9:
                        break;
                default:
                        printf("Not Implemented\n");
                        break;
                }

        } while(opt != 9);
/*
	sleep(3);
	zw_node_get_value( zw_port, 3, (void *)&val );

	printf("\n$$$$$$ Switch Value: %d\n", val);
	sleep(3);
	val = 0;
	zw_node_set_value( zw_port, 3, (void *)&val );
	sleep(3);
	zw_node_get_value( zw_port, 3, (void *)&val );

	printf("\n$$$$$$ Switch Value: %d\n", val);
	sleep(3);

	sleep(3);
	val = 255;
	zw_node_set_value( zw_port, 3, (void *)&val );
	sleep(3);
	zw_node_get_value( zw_port, 3, (void *)&val );

	printf("\n$$$$$$ Switch Value: %d\n", val);
	sleep(3);
*/
//	while( getchar() != 'q' ) sleep(1);
/*
	frame[0] = 0x25;
	cc_process_msg( frame  );
	cc_version( 0x25, (void*)frame  );
	cc_get( 0x25, (void*)frame  );
	cc_set( 0x25, (void*)frame  );
	cc_report( 0x25, (void*)frame  );

	frame[0] = 0x00;
	cc_process_msg( frame  );
	cc_version( 0x00, (void*)frame  );
	cc_get( 0x00, (void*)frame  );
	cc_set( 0x00, (void*)frame  );
	cc_report( 0x00, (void*)frame  );

	gen_foo( "module_a" );
	gen_foo( "module_d" );

	gen_bar( "module_a" );
	gen_bar( "module_c" );
*/
	return 0;
}
