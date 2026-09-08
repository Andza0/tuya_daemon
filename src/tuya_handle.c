#include "tuya_handle.h"

void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
    TY_LOGI("on connected");

    //tuyalink_thing_property_report(context, NULL, "{\"power\":{\"value\":1234,\"time\":1631708204231}}");
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

int send_interface_reports(tuya_mqtt_context_t *client, struct ifaddrs *ifa, time_t now)
{
    long rx_bytes;
    long tx_bytes;

    int linescount = 3;
    int sz = linescount * 64;
    char data[sz];
    get_all_interface_json(ifa, data, sz);
    return tuyalink_thing_property_report(client, NULL, data);
    return 1;
}

int send_reports(tuya_mqtt_context_t *client, struct sysinfo *info, time_t now)
{
    int linescount = 3;
    char data[64 * linescount];
    convert_sysinfo_json(info, data, 64*linescount);
    return tuyalink_thing_property_report(client, NULL, data);
}

int send_cpu_usage_reports(tuya_mqtt_context_t *client, struct cpu_usage *cpu, time_t now)
{
    int linescount = 2;
    char data[64 * linescount];
    snprintf(data, sizeof(data),
        "{"
        "\"at\":{\"value\":%ld,\"time\":%ld},"
        "\"it\":{\"value\":%ld,\"time\":%ld}"
        "}",
        cpu->active, now,
        cpu->idle, now);
    return tuyalink_thing_property_report(client, NULL, data);
}