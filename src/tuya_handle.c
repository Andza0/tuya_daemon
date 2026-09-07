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

    int linescount = 5;
    char data[64 * linescount];
    for(struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next){
        if(strcmp(i->ifa_name, "lo") == 0 || i->ifa_addr->sa_family != AF_INET){
            continue;
        }

        char ip_string[INET_ADDRSTRLEN];
        parse_inet(i, IP_ADDR, ip_string);

        char netmask[INET_ADDRSTRLEN]; 
        parse_inet(i, MASK, netmask);

        rx_bytes = get_rxtx(RX_BYTES, i);
        tx_bytes = get_rxtx(TX_BYTES, i);

        snprintf(data, sizeof(data),
            "{"
            "\"in\":{\"value\":\"%s\",\"time\":%ld},"
            "\"rx\":{\"value\":%ld,\"time\":%ld},"
            "\"tx\":{\"value\":%ld,\"time\":%ld},"
            "\"ip\":{\"value\":\"%s\",\"time\":%ld},"
            "\"ms\":{\"value\":\"%s\",\"time\":%ld},"
            "}",
            i->ifa_name, now,
            (rx_bytes / (1024 * 1024)), now,
            (tx_bytes / (1024 * 1024)), now,
            ip_string, now,
            netmask, now);
        
        return tuyalink_thing_property_report(client, NULL, data);
    }
    return 1;
}

int send_reports(tuya_mqtt_context_t *client, struct sysinfo *info, time_t now)
{
    int linescount = 3;
    char data[64 * linescount];
    snprintf(data, sizeof(data),
        "{"
        "\"mt\":{\"value\":%ld,\"time\":%ld},"
        "\"mf\":{\"value\":%ld,\"time\":%ld},"
        "\"up\":{\"value\":\"%d:%d:%d\",\"time\":%ld}"
        "}",
        ((info->totalram)/ (1024 * 1024)), now,
        ((info->freeram)/(1024 * 1024)), now,
        (int)(info->uptime / 3600), (int)((info->uptime % 3600) / 60), (int)((info->uptime % 3600) % 60), now);

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