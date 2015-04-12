#include "types.h"
#include "stat.h"
#include "user.h"

#define PGSIZE 4096
struct mem
{
	int pid;
	void* memory_region;
};
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
    
  return 0;
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
int
main(int argc, char *argv[])
{
  thread_create(NULL,NULL);
  //printf(1,"clone %d join %d",clone(NULL,NULL,NULL),join(0));
  exit();
}
