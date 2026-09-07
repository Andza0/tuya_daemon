#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include "tuya_cacert.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
#include "tuyalink_core.h"
#include "tuya_handle.h"
#include "arg_parse.h"
#include "log_wrapper.h"
#include <signal.h>
#include "cpu_monitoring.h"
#include "daemon.h"

void sig_handler(int signum);
void prepare_sig_handler(void);

sig_atomic_t sig_flag = 0;

int main(int argc, char *argv[]){
    
    set_mask(7);
    struct arguments *args;
    struct argp *argp;
    int arg_ret = init_arg_structs(&args, &argp);
    if(arg_ret == -1){
        return -1;
    }

    arg_init(argc, argv, args, argp);

    if(argc < 3){
        print_argp_help(argp, argv);
        return 1;
    }
    arg_parse(argp, argc, argv, args);

    prepare_sig_handler();  

    int ret;
    if(args->daemon == 1){
        ret = start_daemon();
        if(ret != 0){
            goto free_args;
        }
    }

    tuya_mqtt_context_t client;
    ret = prepare_tuya(&client, &(tuya_mqtt_config_t) {
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
    if(ret != OPRT_OK){
        system_log(0, "Unable to init or connect to the cloud.");
        goto free;
    }


    int inforet = 0;
    struct sysinfo *info = malloc(sizeof(struct sysinfo));
    if(info == NULL){
        system_log(1, "Unable to allocate memory for system info");
        inforet = -1;
    }


    struct ifaddrs *addrs = NULL;

    int ifaddrsret = 0;
    ifaddrsret = getifaddrs(&addrs);
    if(ifaddrsret == -1){
        system_log(1, "Unable to get information for interfaces");
    }


    struct cpu_stats *old;
    struct cpu_stats *new;
    struct cpu_usage *usage;
    int cpu_status = init_cpu_structs(&old, &new, &usage);
    if(cpu_status == -1){
        system_log(1, "Unable to allocate memory for cpu monitoring.");
    }
    else{
        ret = read_cpu_stats(old);
        if(ret != 10) {system_log(1, "Unable to read all cpu data");}
    }

    time_t last_report = 0;
    time_t now;

    while (sig_flag != 1)
    {
        now = time(NULL);

        if((now - last_report) < 10){
            continue;
        }

        if(cpu_status != -1){
            ret = read_cpu_stats(new); //CPU usage report
            if(ret != 10) {system_log(1, "Unable to read all cpu data");}
            else{
                calc_cpu_usage(old, new, usage);
                send_cpu_usage_reports(&client, usage, now);
                *old = *new;
            }
        }



        if(inforet == 0){ //sysinfo report
            ret = sysinfo(info);
            if(ret < 0){
                system_log(1, "Cannot read data from sys info");
            }
            else{
                ret = send_reports(&client, info, now);
                if(ret < 0){
                    system_log(1, "Failed to send sysinfo report");
                }
            }
        }

        if(ifaddrsret == 0){ //network interface report
            ret = send_interface_reports(&client, addrs, now);
            if(ret < 0){
                system_log(1, "Failed to send interface report");
            }
            last_report = now;
        }
        tuya_mqtt_loop(&client);
    }

    free:
    system_log(6, "Freeing system info memory.. ");
    if(info != NULL){free(info);}
    if(old != NULL) {free(old);}
    if(new != NULL) {free(new);}
    if(usage != NULL) {free(usage);}
    freeifaddrs(addrs);
    tuya_mqtt_deinit(&client);
    free_args:
    if(args != NULL){ free(args); }
    if(argp != NULL){ free(argp); }
    return 0;
}

void prepare_sig_handler(void)
{
    struct sigaction sig = {0};
    sig.sa_handler = &sig_handler;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    sigaction(SIGINT, &sig, NULL);
    sigaction(SIGTERM, &sig, NULL);
    sigaction(SIGQUIT, &sig, NULL);
}

void sig_handler(int signum)
{
    sig_flag = 1;
}