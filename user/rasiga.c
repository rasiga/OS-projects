#include "types.h"
#include "stat.h"
#include "user.h"
#include "syscall.h"
#include "pstat.h"

void sample()
{
	int rc,i,temp,sum; 
	//rc = fork();	
	for(i=0;i<4;i++)
	{
		
		rc = fork();
		if(rc==0)
		{
			sum=0;
			if((i%2)==0)
			{
				if(settickets(100)<0)
				{
					printf(1,"\nError in set");
					exit();
				}
			}
			else
			{
				if(settickets(50)<0)
				{
					printf(1,"\nError in set");				
					exit();
				}
			}
			for(temp=0;;temp++)
			{	
				sum+=temp; 
			}
			exit();
		}
	}		
}
int main()
{
	int i=0;//,j=0,temp=0,sum=0,rc;
	struct pstat *p = NULL;
	p = (struct pstat*)malloc(sizeof(struct pstat)*64);
	if(p==NULL)
		exit();
	printf(1,"parent pid%d\n",getpid());
	sample();
		
	while(1)
	{
		if(getpinfo(p)<0)
			exit();
	        /*j++;
		if(j==15)
		{
			rc = fork();
			if(rc==0)
			{
				for(temp=0;;temp++)
				{
					if(temp==200)
						sleep(2000);
					sum+=temp; 
				}
				exit();
			}
		
		}*/
		for(i=0;i<64;i++) //to get the process info of all 64 processes in the system
		{
			if(p[i].inuse == 1)
			{
				printf(1,"pid %d:",p[i].pid);
				printf(1,"name %s:",p[i].name);
				printf(1,"# of sched %d:",p[i].n_schedule);
				printf(1,"tickets %d:",p[i].tickets);
				printf(1,"stride %d:",p[i].stride);
				printf(1,"pass %d:",p[i].pass);
				printf(1,"\n");
			}
		}
		sleep(100);
	}
	exit();
}
