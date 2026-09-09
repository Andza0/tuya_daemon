#ifndef JSON_HELPER_H_
#define JSON_HELPER_H_

#include <stdio.h>
#include <ifaddrs.h>
#include "net_interface_parser.h"
#include "log_wrapper.h"
#include <sys/sysinfo.h>
#include "cpu_monitoring.h"

void convert_interface_json(struct ifaddrs *i, char dest[], size_t sz);
void convert_sysinfo_json(struct sysinfo *info, char dest[], size_t sz);
void get_all_interface_json(struct ifaddrs *ifa, char result[], size_t sz);
void convert_cpu_json(struct cpu_usage *cpu, char dest[], size_t sz);
int check_action_code(char *msg, char *action_code);

#endif