#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "tuya_cacert.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
#include "tuyalink_core.h"
#include "tuya_handle.h"
#include "arg_parse.h"
#include "log_wrapper.h"



int main(int argc, char *argv[]){

    set_mask(7);
    if(argc <= 1){
        system_log(0, "Correct program launch format: -p {product id} -d {device id} -s {device secret}\n");
        return 1;
    }
    struct arguments *args = malloc(sizeof(struct arguments));
    if(args == NULL){
        return 1;
    }

    arg_parse(argc, argv, args);

    tuya_mqtt_context_t client;

    int ret;

    ret = tuya_mqtt_init(&client, &(const tuya_mqtt_config_t) {
        .host = "m1.tuyacn.com",
        .port = 8883,
        .cacert = tuya_cacert_pem,
        .cacert_len = sizeof(tuya_cacert_pem),
        .device_id = args->deviceId,
        .device_secret = args->deviceSecret,
        .keepalive = 100,
        .timeout_ms = 2000,
        .on_connected = on_connected,
        .on_disconnect = on_disconnect,
        .on_messages = on_messages
    });
    assert(ret == OPRT_OK);

    ret = tuya_mqtt_connect(&client);
    assert(ret == OPRT_OK);

    struct sysinfo *info = malloc(1 * sizeof(struct sysinfo));
    time_t last_report = 0;
    time_t now;
    char data[64];
    struct ifaddrs *addrs = malloc(sizeof(struct ifaddrs));

    int ifaddrsret = 0;
    if(addrs == NULL){
        system_log(1, "Unable to allocate memory for interfaces");
        ifaddrsret = -1;
    }
    else{
        ifaddrsret = getifaddrs(&addrs);
        if(ret != 0){
            system_log(1, "Unable to get information for interfaces");
        }
    }
    for (;;)
    {
        now = time(NULL);
        if((now - last_report) > 10){
            ret = sysinfo(info);
            if(ret != 0){
                system_log(1, "Cannot read data from sys info");
            }
            else{
                ret = send_reports(&client, info, now);
                if(ret != 1){
                    system_log(1, "Failed to send sysinfo report");
                }
                if(ifaddrsret == 0){
                    ret = send_interface_reports(&client, addrs, now);
                    if(ret != 1){
                        system_log(1, "Failed to send interface report");
                    }
                }
            }
            last_report = now;
        }
        tuya_mqtt_loop(&client);
    }

    free:
    if(info != NULL){
        system_log(6, "Freeing system info memory.. ");
        free(info);
    }
    if(addrs != NULL){
        system_log(6, "Freeing network interface memory.. ");
        free(addrs);
    }
    tuya_mqtt_deinit(&client);
    return 0;
}