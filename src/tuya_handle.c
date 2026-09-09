#include "tuya_handle.h"

void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
    TY_LOGI("on connected");
}

void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{
    TY_LOGI("on disconnect");
}

void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
    TY_LOGI("on message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);
    switch (msg->type) {
        case THING_TYPE_ACTION_EXECUTE:
            if(check_action_code(msg->data_string, "write_log") == 0){
                log_file(msg->data_string, "/tmp/tuya_action.log");
            }
            break;
        default:
            break;
    }
    printf("\r\n");
}

int prepare_tuya(tuya_mqtt_context_t *client, tuya_mqtt_config_t *config)
{
    int ret = tuya_mqtt_init(client, config);
    if(ret != OPRT_OK){
        return ret;
    }

    ret = tuya_mqtt_connect(client);
    return ret;
}

int send_interface_reports(tuya_mqtt_context_t *client, struct ifaddrs *ifa)
{
    int linescount = 0;
    for(struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next) { linescount++; }
    int sz = linescount * 128;
    char interfaces[sz];
    get_all_interface_json(ifa, interfaces, sz);
    char data[32 + sizeof(interfaces)];
    snprintf(data, 32 + sizeof(interfaces), "{ \"interfaces\" : [%s]}", interfaces);

    return tuyalink_thing_property_report(client, NULL, data);
}

int send_sysinfo_reports(tuya_mqtt_context_t *client, struct sysinfo *info)
{
    int linescount = 3;
    size_t sz = linescount * 72;
    char data[sz];
    convert_sysinfo_json(info, data, sz);
    return tuyalink_thing_property_report(client, NULL, data);
}

int send_cpu_usage_reports(tuya_mqtt_context_t *client, struct cpu_usage *cpu)
{
    int linescount = 2;
    size_t sz = linescount * 64;
    char data[sz];
    convert_cpu_json(cpu, data, sz);
    return tuyalink_thing_property_report(client, NULL, data);
}