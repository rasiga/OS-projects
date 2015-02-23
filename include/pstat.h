#ifndef _PSTAT_H_
#define _PSTAT_H_

struct pstat
{
	int inuse; //whether this process is in use (except unused all processes are used)
	int pid; //PID of process
	char name[16];//name of process
	int tickets;//number of tickets given to process
	int pass;//current pass value for the process
	int stride;//stride value
	int n_schedule;//number of times chosen for scheduling
};
#endif 
