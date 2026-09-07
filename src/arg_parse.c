#include "arg_parse.h"


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'p':
            arguments->productId = arg;
            break;

        case 'd':
            arguments->deviceId = arg;
            break;
        case 's':
            arguments->deviceSecret = arg;
            break;
        case 'f':
            arguments->daemon = 1;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

void arg_init(int argc, char **argv, struct arguments *args, struct argp *argp)
{
    static char doc[] = "tuya daemon";

    static char args_doc[] = "-p productID -d deviceID -s device secret -f daemon";

    static struct argp_option options[] = {
        {"productId",       'p', "PROD_ID",     0, "Product ID"},
        {"deviceId",        'd', "DEV_ID",      0, "Device ID"},
        {"deviceSecret",    's', "DEV_SECRET",  0, "Device Secret"},
        {"daemon",          'f', NULL,          0, "Daemon"},
        {0}
    };

    *argp = (struct argp)
    {
        options,
        parse_opt,
        args_doc,
        doc
    };

    argp_parse(argp, argc, argv, 0, NULL, args);
}
error_t arg_parse(struct argp *argp, int argc, char **argv, struct arguments *args)
{
    return argp_parse(argp, argc, argv, 0, NULL, args);
}

void print_argp_help(struct argp *argp, char **argv){
    argp_help(argp, stdout, ARGP_HELP_USAGE, argv[0]);
}