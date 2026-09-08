#include "json_helper.h"

void convert_interface_json(struct ifaddrs *i, char dest[], size_t sz)
{
    char ip_string[INET_ADDRSTRLEN];
    parse_inet(i, IP_ADDR, ip_string);

    char netmask[INET_ADDRSTRLEN]; 
    parse_inet(i, MASK, netmask);

    long rx_bytes = get_rxtx(RX_BYTES, i);
    if(rx_bytes == OPENDIR_ERR || rx_bytes == OPENFILE_ERR || rx_bytes == UNKNOWN_TYPE || rx_bytes == LINE_RET_NULL){
        char error[64];
        snprintf(error, 64, "Unable to read received bytes, returned: %ld", rx_bytes);
        system_log(1, error);
    }
    long rx = rx_bytes / (1024 * 1024);

    long tx_bytes = get_rxtx(TX_BYTES, i);
    if(tx_bytes == OPENDIR_ERR || tx_bytes == OPENFILE_ERR || tx_bytes == UNKNOWN_TYPE || tx_bytes == LINE_RET_NULL){
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
    "\"rx\": \"%ld\","
    "\"rx\": \"%ld\","
    "}", i->ifa_name, ip_string, netmask, rx, tx);

}

void get_all_interface_json(struct ifaddrs *ifa, char result[], size_t sz)
{
    char interface[128];
    for(struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next){
        if(strcmp(i->ifa_name, "lo") == 0 || i->ifa_addr->sa_family != AF_INET){
            continue;
        }
        convert_interface_json(i, interface, sizeof(interface));
        strcat(result, interface);
    }
}

void convert_sysinfo_json(struct sysinfo *info, char dest[], size_t sz)
{
    snprintf(dest, sz,
    "{"
    "\"total_ram\":%ld,"
    "\"free_ram\":\"%ld\","
    "\"uptime\":\"%d:%d:%d\","
    "}",
    ((info->totalram)/ (1024 * 1024)),
    ((info->freeram)/(1024 * 1024)),
    (int)(info->uptime / 3600), (int)((info->uptime % 3600) / 60), (int)((info->uptime % 3600) % 60));
}

void convert_cpu_json(struct cpu_usage *cpu, char dest[], size_t sz)
{
    snprintf(dest, sz,
        "{"
        "\"active\" : %ld,"
        "\"idle\": %ld,}"
        "}",
        cpu->active,
        cpu->idle);
}