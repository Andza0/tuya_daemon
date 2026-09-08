#include "daemon.h"

void start_daemon()
{
    pid_t pid = fork();
    if(pid > 0){
        system_log(6, "Parent exiting..");
        exit(0);
    }

    if(setsid() < 0){
        system_log(2, "Unable to create a new session");
        exit(0);
    }

    pid = fork();
    if(pid > 0){
        system_log(6, "First child exiting..");
        exit(0);
    }
    if(chdir("/") < 0){
        system_log(2, "Unable to change directory");
        exit(0);
    }

}