#include "cpu_monitoring.h"

int read_cpu_stats(struct cpu_stats *cpu)
{
    FILE *fp = fopen("/proc/stat", "r");
    if(fp == NULL) {return -1;}

    int fields = fscanf(fp,
        "cpu %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
        &cpu->user,
        &cpu->nice,
        &cpu->sys,
        &cpu->idle,
        &cpu->iowait,
        &cpu->irq,
        &cpu->softirq,
        &cpu->steal,
        &cpu->guest,
        &cpu->guest_nice
    );

    fclose(fp);
    return fields;
}

int calc_cpu_usage(struct cpu_stats *old, struct cpu_stats *new, struct cpu_usage *result)
{
    long active_old = old->user + old->nice + old->sys + old->irq + old->softirq + old->steal + old->guest + old->guest_nice;
    long idle_old = old->idle + old->iowait;

    long active_new = new->user + new->nice + new->sys + new->irq + new->softirq + new->steal + new->guest + new->guest_nice;
    long idle_new = new->idle + new->iowait;

    long active_delta = active_new - active_old;
    long idle_delta = idle_new - idle_old;

    result->active = active_delta;
    result->idle = idle_delta;
}