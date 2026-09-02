#include "net_interface_parser.h"

long get_rxtx(int rxtx, struct ifaddrs *ifa)
{
    DIR *dirp = opendir("/sys/class/net");
    if(dirp == NULL){
        return OPENDIR_ERR;
    }
    struct dirent *entry;
    while((entry = readdir(dirp)) != NULL)
    {   
        if(strcmp(entry->d_name, ifa->ifa_name) == 0){
            size_t pathsz = 64 + strlen(entry->d_name);
            char path[pathsz];
            snprintf(path, pathsz, "/sys/class/net/%s/statistics", entry->d_name);
            DIR *data_dir = opendir(path);

            char *filename;

            switch (rxtx)
            {
            case RX_BYTES:
                filename = "rx_bytes";
                break;
            case TX_BYTES:
                filename = "tx_bytes";
                break;
            default:
                return UNKNOWN_TYPE;
            }

            size_t filepathsz = strlen(path) + strlen(filename) + 1;
            char filepath[filepathsz];
            snprintf(filepath, filepathsz, "%s/%s", path, filename);
            FILE *data_file = fopen(filepath, "r");
            if(data_file == NULL){
                return OPENFILE_ERR;
            }

            char *line;
            size_t linesz = 32;
            line = fgets(line, linesz, data_file);
            char *end;
            long ret = strtol(line, &end, 10);
            if(line != end){
                closedir(dirp);
                fclose(data_file);
                return ret;
            }
        }
    }
}