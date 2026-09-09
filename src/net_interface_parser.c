#include "net_interface_parser.h"


int parse_inet(struct ifaddrs *ifa, int type, char result[])
{   
    if(result == NULL){
        return -1;
    }

    if(ifa == NULL){
        result[0] = '\0';
        return -1;
    }

    struct sockaddr_in *data = NULL;
    switch(type){
        case IP_ADDR:
            data = (struct sockaddr_in *)ifa->ifa_addr;
            break;
        case MASK:
            data = (struct sockaddr_in *)ifa->ifa_netmask;
            break;
        default:
            return -1;
    }
    
    if(data == NULL){
        result[0] = '\0';
        return -1;
    }

    if(data->sin_family != AF_INET){
        result[0] = '\0';
        return -1;
    }

    if((inet_ntop(AF_INET, &data->sin_addr, result, INET_ADDRSTRLEN)) == NULL){
        result[0] = '\0';
        return -1;
    }

    return 0;
}

long get_rxtx(int rxtx, struct ifaddrs *ifa)
{
    if(ifa == NULL || ifa->ifa_name == NULL){
        return NULL_PTR;
    }

    DIR *dirp = opendir("/sys/class/net");
    if(dirp == NULL){
        return OPENDIR_ERR;
    }

    struct dirent *entry;
    while((entry = readdir(dirp)) != NULL)
    {   
        if(strcmp(entry->d_name, ifa->ifa_name) != 0){
            continue;
        }
        size_t pathsz = 64 + strlen(entry->d_name);
        char path[pathsz];
        snprintf(path, pathsz, "/sys/class/net/%s/statistics", entry->d_name);
        DIR *data_dir = opendir(path);
        if(data_dir == NULL){
            closedir(dirp);
            return OPENDIR_ERR;
        }

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
            closedir(dirp);
            closedir(data_dir);
            return UNKNOWN_TYPE;
        }

        size_t filepathsz = strlen(path) + strlen(filename) + 2;
        char filepath[filepathsz];
        snprintf(filepath, filepathsz, "%s/%s", path, filename);
        FILE *data_file = fopen(filepath, "r");
        if(data_file == NULL){
            system_log(1, filepath);
            closedir(data_dir);
            closedir(dirp);
            return OPENFILE_ERR;
        }

        size_t linesz = 32;
        char line[linesz];
        if(fgets(line, linesz, data_file) == NULL){
            closedir(data_dir);
            closedir(dirp);
            fclose(data_file);
            return LINE_RET_NULL;
        }
        char *end;
        long ret = strtol(line, &end, 10);
        if(line == end){
            closedir(data_dir);
            closedir(dirp);
            fclose(data_file);
            return BAD_FORMAT;
        }
        closedir(data_dir);
        closedir(dirp);
        fclose(data_file);
        return ret;
    }
    closedir(dirp);
    return OPENFILE_ERR;
}