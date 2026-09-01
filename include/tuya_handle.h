#ifndef TUYA_HANDLE_H_
#define TUYA_HANDLE_H_

#include <stdio.h>
#include <sys/sysinfo.h>
#include <assert.h>
#include <stdlib.h>
#include "cJSON.h"
#include "tuya_log.h"
#include "tuya_error_code.h"
#include "system_interface.h"
#include "mqtt_client_interface.h"
#include "tuyalink_core.h"
#include <time.h>
#include <sys/types.h>
#include <ifaddrs.h>


void on_connected(tuya_mqtt_context_t* context, void* user_data);
void on_disconnect(tuya_mqtt_context_t* context, void* user_data);
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);
int send_reports(tuya_mqtt_context_t *client, struct sysinfo *info, time_t now);

#endif