#include "tuya_handle.h"
#include "net_interface_parser.h"

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

        default:
            break;
    }
    printf("\r\n");
}

int send_interface_reports(tuya_mqtt_context_t *client, struct ifaddrs *ifa, time_t now)
{
    long rx_bytes;
    long tx_bytes;

    char data[64];
    for(struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next){
        rx_bytes = get_rxtx(RX_BYTES, i);
        tx_bytes = get_rxtx(TX_BYTES, i);

        snprintf(data, sizeof(data),
        "{"
        "\"in\":{\"value\":%s,\"time\":%ld},"
        "}",
        i->ifa_name,
        now);
        
        int ret = tuyalink_thing_property_report(client, NULL, data);
        if(ret >= 0){
            return ret;
        }
    }
    return 1;
}

int send_reports(tuya_mqtt_context_t *client, struct sysinfo *info, time_t now)
{
    char data[64 * 3];
    snprintf(data, sizeof(data),
     "{"
     "\"mt\":{\"value\":%ld,\"time\":%ld},"
     "\"mf\":{\"value\":%ld,\"time\":%ld},"
     "\"up\":{\"value\":\"%d:%d:%d\",\"time\":%ld}"
     "}",
      ((info->totalram)/ (1024 * 1024)),
      now,
      ((info->freeram)/(1024 * 1024)),
       now,
        (int)(info->uptime / 3600), (int)((info->uptime % 3600) / 60), (int)((info->uptime % 3600) % 60),
        now);

    int ret = tuyalink_thing_property_report(client, NULL, data);

    return ret >= 0 ? 1 : 0;
}