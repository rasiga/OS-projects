#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
struct mem
{
	int pid;
	void* memory_region;
};

void sample(void*);
int thread_create(void (*start_routine)(void*), void *arg) 
{
  //create stack using malloc
  
  // allocate twice the page size 
  void *stack = malloc(PGSIZE*2);
  if(stack==NULL)
  {
	printf(1,"Malloc for stack returned NULL");
	return -1;
  }
  // page align
  if((uint)stack % PGSIZE != 0)
  {
	stack = stack + (PGSIZE - (uint)stack % PGSIZE);
  }

  //invoke clone 
  int x=clone(start_routine,arg,stack);
  
  printf(1,"Current pid %d thread %d \n",getpid(),x);
  
  
  //table(2d array) - maintain pid and memory region allocated (to enable freeing the stack)
  return 0;
}

int thread_join(int pid)
{
  int x=join(pid);
  printf(1,"join pid %d\n",x);
  // need to free stack  
  return 0;
}
void lock_init(lock_t *lock)
{
  
}
void 
lock_acquire(lock_t *lock)
{
	//int myturn = FetchAndAdd(&lock->ticket);
	//while(lock->turn != myturn);
}
void
lock_release(lock_t *lock)
{
	//FetchAndAdd(&lock->turn);
}
void cv_wait(cond_t *cond, lock_t *lock)
{

}
void cv_signal(cond_t *cond)
{

}

int
main(int argc, char *argv[])
{
  int s=5;
  void *p;
  p=(void *)&s;
  int x = thread_create(sample,p);
  printf(1,"Inside main %d\n",x);
  //printf(1,"clone %d join %d",clone(NULL,NULL,NULL),join(0));
  exit();
}

void sample(void* a)
{
  printf(1,"sample in it %d %d\n",*(int *)a,getpid());
  //sleep(60);
  exit();
}
