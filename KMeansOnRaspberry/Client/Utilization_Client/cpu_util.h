#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define STAT "/proc/stat"
struct stats_cpu {
	unsigned long long cpu_user;
	unsigned long long cpu_nice;
	unsigned long long cpu_sys;
	unsigned long long cpu_idle;
	unsigned long long cpu_iowait;
	unsigned long long cpu_steal;
	unsigned long long cpu_hardirq;
	unsigned long long cpu_softirq;
	unsigned long long cpu_guest;
	unsigned long long cpu_guest_nice;
}; 

double get_cpu_util_perc(int cpu_id);
int read_stat_cpu(struct stats_cpu *st_cpu, int cpu_id);
void copy_struct(struct stats_cpu src, struct stats_cpu *dest);
double calc_util_perc(struct stats_cpu prev, struct stats_cpu cur);
