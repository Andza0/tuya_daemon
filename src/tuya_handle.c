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
        case THING_TYPE_MODEL_RSP:
            TY_LOGI("Model data:%s", msg->data_string);
            break;

        case THING_TYPE_PROPERTY_SET:
            TY_LOGI("property set:%s", msg->data_string);
            break;

        case THING_TYPE_PROPERTY_REPORT_RSP:
            break;
        case THING_TYPE_ACTION_EXECUTE:
            TY_LOGI("action execute:%s", msg->data_string);
            FILE *logfp = fopen("/tmp/tuya_action.log", "a");
            if (logfp == NULL) {TY_LOGI("Unable to open log file"); break;}
            fprintf(logfp, "%s\n", msg->data_string);
            fclose(logfp);
            break;
        default:
            break;
    }
    printf("\r\n");
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

        struct sockaddr_in *sa = (struct sockaddr_in *)i->ifa_addr;
        char ip_string[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &sa->sin_addr, ip_string, INET_ADDRSTRLEN);

        struct sockaddr_in *mask = (struct sockaddr_in *)i->ifa_netmask;
        char mask_string[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &mask->sin_addr, mask_string, INET_ADDRSTRLEN);

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
            mask_string, now);
        
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