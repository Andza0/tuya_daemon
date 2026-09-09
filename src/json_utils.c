#include "json_helper.h"

int convert_interface_json(struct ifaddrs *i, char dest[], size_t sz)
{
    if(i == NULL || dest == NULL || sz == 0){
        return -1;
    }
    int ret;
    char ip_string[INET_ADDRSTRLEN] = "";
    ret = parse_inet(i, IP_ADDR, ip_string);
    if(ret == -1){
        system_log(3, "Unable to read ip_addres");
    }

    char netmask[INET_ADDRSTRLEN] = "";
    ret = parse_inet(i, MASK, netmask);
    if(ret == -1){
        system_log(3, "Unable to read netmask");
    }

    long rx_bytes = get_rxtx(RX_BYTES, i);
    if(rx_bytes == OPENDIR_ERR || rx_bytes == OPENFILE_ERR || rx_bytes == UNKNOWN_TYPE || rx_bytes == LINE_RET_NULL || rx_bytes == BAD_FORMAT){
        char error[64];
        snprintf(error, 64, "Unable to read received bytes, returned: %ld", rx_bytes);
        system_log(1, error);
    }
    long rx = rx_bytes / (1024 * 1024);

    long tx_bytes = get_rxtx(TX_BYTES, i);
    if(tx_bytes == OPENDIR_ERR || tx_bytes == OPENFILE_ERR || tx_bytes == UNKNOWN_TYPE || tx_bytes == LINE_RET_NULL || tx_bytes == BAD_FORMAT){
        char error[64];
        snprintf(error, 64, "Unable to read transceived bytes, returned: %ld", tx_bytes);
        system_log(1, error);
    }
    long tx = tx_bytes / (1024 * 1024);

    snprintf(dest, sz,
    "{"
    "\"name\": \"%s\","
    "\"ip_addr\": \"%s\","
    "\"netmask\": \"%s\","
    "\"rx\": %ld,"
    "\"tx\": %ld"
    "},", i->ifa_name, ip_string, netmask, rx, tx);
    return 0;
}

int get_all_interface_json(struct ifaddrs *ifa, char result[], size_t sz)
{
    if(result == NULL || sz == 0){
        return -1;
    }
    char interface[128];
    result[0] = '\0';
    for(struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next){
        if(i->ifa_name == NULL || 
        i->ifa_addr == NULL || 
        strcmp(i->ifa_name, "lo") == 0 || 
        i->ifa_addr->sa_family != AF_INET){
            continue;
        }
        convert_interface_json(i, interface, sizeof(interface));
        size_t used = strlen(result);
        size_t interface_len = strlen(interface);
        size_t needed = used + interface_len + 1;
        if(needed > sz){
            return -1;
        }
        strcat(result, interface);
    }
    return 0;
}

int convert_sysinfo_json(struct sysinfo *info, char dest[], size_t sz)
{
    if(info == NULL || dest == NULL || sz == 0){
        return -1;
    }
    snprintf(dest, sz,
    "{"
    "\"total_ram\":%ld,"
    "\"free_ram\":%ld,"
    "\"uptime\":\"%d:%d:%d\""
    "}",
    ((info->totalram)/ (1024 * 1024)),
    ((info->freeram)/(1024 * 1024)),
    (int)(info->uptime / 3600), (int)((info->uptime % 3600) / 60), (int)((info->uptime % 3600) % 60));

    return 0;
}

int convert_cpu_json(struct cpu_usage *cpu, char dest[], size_t sz)
{    
    if(cpu == NULL || dest == NULL || sz == 0){
        return -1;
    }
    snprintf(dest, sz,
        "{"
        "\"active\" : %ld, "
        "\"idle\": %ld"
        "}",
        cpu->active,
        cpu->idle);

    return 0;
}

int check_action_code(char *msg, char *action_code)
{
    cJSON *root = cJSON_Parse(msg);
    if(root == NULL){
        system_log(1, "Unable to read the received json from the cloud.");
        return -1;
    }
    cJSON *action = cJSON_GetObjectItemCaseSensitive(root, "actionCode");
    if(!cJSON_IsString(action) || action_code == NULL){
        cJSON_Delete(root);
        return -1;
    }

    int res = strcmp(action->valuestring, action_code);
    cJSON_Delete(root);
    return res;
}