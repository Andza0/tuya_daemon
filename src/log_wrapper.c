
#include <syslog.h>

void set_mask(int LOG)
{
    setlogmask(LOG_UPTO (LOG));
}

void system_log(int LOG,char *msg)
{
    switch (LOG)
    {
    case LOG_EMERG: //0
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_ALERT: //1
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_CRIT: //2
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_ERR: //3
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_WARNING: //4
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_NOTICE: //5
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_INFO: //6
        syslog(LOG, "%s\n", msg);
        break;
    case LOG_DEBUG: //7
        syslog(LOG, "%s\n", msg);
        break;
    default:
        break;
    }
}