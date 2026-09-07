#include "daemon.h"

int start_daemon()
{
    pid_t pid = fork();
    if(pid > 0){
        system_log(6, "Parent exiting..");
        return pid;
    }

    if(setsid() < 0){
        system_log(2, "Unable to create a new session");
        return -1;
    }
    if(chdir("/") < 0){
        system_log(2, "Unable to change directory");
        return -1;
    }

}