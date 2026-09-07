#ifndef LOG_WRAPPER_H_
#define LOG_WRAPPER_H_

#include <syslog.h>
#include <stdio.h>
#include "cJSON.h"
#include <string.h>

void set_mask(int LOG);
void system_log(int LOG,char *msg);
int check_action_code(char *msg, char *action_code);
void log_file(char *msg, char *file);

#endif