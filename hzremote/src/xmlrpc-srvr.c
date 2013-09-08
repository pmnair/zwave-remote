#include <stdio.h>
#include <xmlrpc-c/base.h>
#include <xmlrpc-c/abyss.h>
#include <xmlrpc-c/server.h>
#include <xmlrpc-c/server_abyss.h>

#include <xmlrpc-c/config.h>
#include <parted/parted.h>

static void dieOnFault (char *ident, xmlrpc_env * const envP) 
{
	if (envP->fault_occurred) {
		fprintf(stderr, "ERROR:%s: %s (%d)\n",
		ident, envP->fault_string, envP->fault_code);
		exit(1);
	}
}

static xmlrpc_value * get_disk_partitions(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo) 
{
	const char *disk_name;
    xmlrpc_value *disk_partn_array;
    xmlrpc_value *disk_partn;
    xmlrpc_value *result;
    PedDisk *disk;
    PedPartition *partn;
    PedDevice *device = NULL;

	printf("get_disk_partitions: Function Called\n");
	
	xmlrpc_decompose_value(envP, paramArrayP, "(s)", &disk_name);
	dieOnFault("get_disk_partitions-decompose-args", envP);

	printf("get_disk_partitions: Find partitions for %s\n", disk_name);

	device = ped_device_get(disk_name);
    if((disk = ped_disk_new(device)) != NULL) 
    {
		disk_partn_array = xmlrpc_array_new(envP);
		dieOnFault("get_disk_partitions-init-array", envP);
        for(partn = ped_disk_next_partition(disk, NULL); partn; partn = ped_disk_next_partition(disk, partn))
        {
            if(partn->num < 0)
                continue;
			
            printf("Num=%d, start=%lld, size=%lld, fs=%s\n", partn->num, partn->geom.start, partn->geom.length,
                                                               (partn->fs_type) ? partn->fs_type->name : "NA");
			disk_partn = xmlrpc_build_value(envP, "{s:i,s:I,s:I,s:s}", "partn_num", partn->num, 
																	   "partn_start", partn->geom.start, 
																	   "partn_length", partn->geom.length,
                                                               	       "partn_fs_type", (partn->fs_type) ? partn->fs_type->name : "NA");
			dieOnFault("get_disk_partitions-build-partn", envP);
			xmlrpc_array_append_item(envP, disk_partn_array, disk_partn);		
			dieOnFault("get_disk_partitions-add-partn", envP);
			xmlrpc_DECREF(disk_partn);
        }         
		
		result = xmlrpc_build_value(envP, "A", disk_partn_array);
		dieOnFault("get_disk_partitions-build-result", envP);
		
        ped_disk_destroy(disk);
		xmlrpc_DECREF(disk_partn_array);
    }    
	
	return result;
}

static xmlrpc_value * get_disk_name(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo) 
{
    xmlrpc_value *disk_name_array;
    xmlrpc_value *disk_name;
	xmlrpc_value *result;
    PedDevice *device = NULL;
	int ii = 0;
	
    ped_device_probe_all();

	printf("get_disk_name: Function Called\n");

	disk_name_array = xmlrpc_array_new(envP);
	dieOnFault("get_disk_name-init-array", envP);
    while((device = ped_device_get_next(device)))
    {
        printf("Device:%s\n",device->path);

		disk_name = xmlrpc_string_new(envP, device->path);
		dieOnFault("get_disk_name-build-diskname", envP);
		xmlrpc_array_append_item(envP, disk_name_array, disk_name);		
		dieOnFault("get_disk_name-add-diskname", envP);
		xmlrpc_DECREF(disk_name);
    }
	
	result = xmlrpc_build_value(envP, "A", disk_name_array);
	dieOnFault("get_disk_name-build-result", envP);

	xmlrpc_DECREF(disk_name_array);

	printf("get_disk_name: Returning\n");
	
	return result;
}

static xmlrpc_value * get_disk_info(
		xmlrpc_env * const envP, 
		xmlrpc_value * const paramArrayP, 
		void * const serverInfo, 
		void * const channelInfo) 
{
    xmlrpc_value *disk_name_array;
    xmlrpc_value *disk_name;
	xmlrpc_value *result;
    PedDevice *device = NULL;
	int ii = 0;
	
    ped_device_probe_all();

	printf("get_disk_name: Function Called\n");

	disk_name_array = xmlrpc_array_new(envP);
	dieOnFault("get_disk_name-init-array", envP);
    while((device = ped_device_get_next(device)))
    {
        printf("Device:%s\n",device->path);

		disk_name = xmlrpc_string_new(envP, device->path);
		dieOnFault("get_disk_name-build-diskname", envP);
		xmlrpc_array_append_item(envP, disk_name_array, disk_name);		
		dieOnFault("get_disk_name-add-diskname", envP);
		xmlrpc_DECREF(disk_name);
    }
	
	result = xmlrpc_build_value(envP, "A", disk_name_array);
	dieOnFault("get_disk_name-build-result", envP);

	xmlrpc_DECREF(disk_name_array);

	printf("get_disk_name: Returning\n");
	
	return result;
}

int main(int argc, char **argv)
{
    xmlrpc_server_abyss_parms serverparm;
    xmlrpc_registry * registryP;
    xmlrpc_env env;
	int ii;
	struct xmlrpc_method_info3 const methodInfo[] = {
		{
		.methodName = "xrp_poc.get_disk_name",
		.methodFunction = &get_disk_name,
		},
		{
		.methodName = "xrp_poc.get_disk_info",
		.methodFunction = &get_disk_info,
		},
		{
		.methodName = "xrp_poc.get_disk_partitions",
		.methodFunction = &get_disk_partitions,
		}		
	};
		
    xmlrpc_env_init(&env);
	dieOnFault("init", &env);

    registryP = xmlrpc_registry_new(&env);
	dieOnFault("init_registry", &env);

	for ( ii = 0; ii < ( sizeof( methodInfo ) / sizeof( struct xmlrpc_method_info3 ) ); ii++ )
	{
		xmlrpc_registry_add_method3(&env, registryP, &methodInfo[ii]);
		dieOnFault("add_method", &env);
	}

    serverparm.config_file_name = NULL;
    serverparm.registryP = registryP;
    serverparm.port_number = 8080;
    serverparm.log_file_name = "/tmp/xmlrpc_log";

    printf("Starting XML-RPC server...\n");

    xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(registryP));

    return 0;    
}

/*
gcc -I ../NG-OXCI_2011-07-20/local/include -o xrpcs xmlrpc-srvr.c -L../NG-OXCI_2011-07-20/local/lib -Wl,--rpath -Wl,../NG-OXCI_2011-07-20/local/lib -lxmlrpc_server_abyss -lxmlrpc_server -lxmlrpc_abyss -lxmlrpc -lxmlrpc_util -lpthread -lparted
*/

