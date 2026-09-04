#ifndef ARG_PARSE_H_
#define ARG_PARSE_H_

#include <argp.h>
#include <stdio.h>

struct arguments{
    char *productId;
    char *deviceId;
    char *deviceSecret;
    int daemon;
};

void arg_parse(int argc, char **argv, struct arguments *args);

#endif