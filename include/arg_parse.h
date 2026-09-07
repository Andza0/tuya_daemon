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

void arg_init(int argc, char **argv, struct arguments *args, struct argp *argp);
error_t arg_parse(struct argp *argp, int argc, char **argv, struct arguments *args);
void print_argp_help(struct argp *argp, char **argv);

#endif