#ifndef CPU_MONITORING_H_
#define CPU_MONITORING_H_

#include <stdio.h>
#include <stdlib.h>

struct cpu_stats{ //cpu user  nice  system  idle  iowait  irq  softirq  steal  guest  guest_nice
    long user; //time spent running normal user-space programs.
    long nice; //time spent running user-space programs that were started with a modified niceness value.
    long sys; //time spent executing kernel code, such as system calls and kernel work.
    long idle; //time where the CPU had nothing to do.
    long iowait; //time the CPU was idle while waiting for I/O, typically disk or network-related work.
    long irq; //time spent handling hardware interrupts.
    long softirq; //time spent handling software interrupts, often networking and other deferred kernel work.
    long steal; //in a virtual machine, time when the hypervisor was running another VM instead of yours.
    long guest; //time spent running virtual CPUs for guest VMs under Linux virtualization.
    long guest_nice; //same idea as guest, but for niced guest workloads.
};

struct cpu_usage{
    long active;
    long idle;
};

int read_cpu_stats(struct cpu_stats *cpu);
int calc_cpu_usage(struct cpu_stats *old, struct cpu_stats *new, struct cpu_usage *result);

#endif