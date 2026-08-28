#include <stdio.h>
#include <sys/sysinfo.h>
#include <assert.h>
#include <stdlib.h>
//#include "cJSON.h"
#include "tuya_cacert.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
//#include "system_interface.h"
//#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include "tuya_handle.h"
#include <time.h>

int main(int argc, char argv[]){
    const char productId[] = "b5lviadl7bzxzm3y";
    const char deviceId[] = "2622d2dd1a1bdae547xlwb";
    const char deviceSecret[] = "SSdyXu64UJ0COu3g";
    tuya_mqtt_context_t client;

    
    struct sysinfo *info = malloc(1 * sizeof(struct sysinfo));

    int ret;

    ret = tuya_mqtt_init(&client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = deviceId,
        .device_secret = deviceSecret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });
    assert(ret == OPRT_OK);

    ret = tuya_mqtt_connect(&client);
    assert(ret == OPRT_OK);


    time_t last_report = 0;
    time_t now = time(NULL);
    char data[64];
    for (;;)
    {
        if((now - last_report) > 10){
            ret = sysinfo(info);
            if(ret != 0){
                perror("Klaida skaitant duomenis:");
                if(info != NULL){
                    free(info);
                }
                return ret;
            }
            snprintf(data, sizeof(data), "{\"memtotal\":{\"value\":%ld,\"time\":1631708204231}}", (info->totalram)/1000000);
            tuyalink_thing_property_report(&client, NULL, data);
            last_report = now;
        }
        tuya_mqtt_loop(&client);
    }

    if(info != NULL){
        free(info);
    }
    return ret;
}