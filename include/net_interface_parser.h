#ifndef NET_INTERFACE_PARSER_H_
#define NET_INTERFACE_PARSER_H_

//data types
#define RX_BYTES 0
#define TX_BYTES 1

//errors
#define UNKNOWN_TYPE -1
#define OPENDIR_ERR -2
#define OPENFILE_ERR -3
#define LINE_RET_NULL -4

#include <sys/types.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long get_rxtx(int rxtx, struct ifaddrs *ifa);

#endif