/* race condition in CV? (must atomically release lock and sleep) */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

#define N 1
int ppid;
int pid1, pid2;
volatile int bufsize;
int result;
lock_t lock;
cond_t nonfull, nonempty;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void produce(void *arg);
void consume(void *arg);

int
main(int argc, char *argv[])
{
  int i;
  ppid = getpid();
  printf(1,"\n Lock initialized\n");
  lock_init(&lock);
  bufsize = 0;
  
  assert((pid1 = thread_create(produce, NULL)) > 0);
  printf(1,"\n Producer created\n");
  assert((pid2 = thread_create(consume, NULL)) > 0);
  printf(1,"\n Consumer created\n");

  for(i = 0; i < 500; i++) {
    result <<= 1;
    sleep(1);
  }

  printf(1, "%p\n", result);
  if(result & 0x3ff)
    printf(1, "TEST PASSED\n");
  exit();
}

void
produce(void *arg) {
  while(1) {
 
    lock_acquire(&lock);
    printf(1,"\n Producer acquired lock\n"); 
    while(bufsize == N)
    {
	  printf(1,"\n Waiting for consumer to consume\n");
	  cv_wait(&nonfull, &lock);
    }
    result <<= 1;
    result |= 1;

    bufsize++;
    printf(1,"\nSignalling consumer\n");
    cv_signal(&nonempty);
    printf(1,"\n Producer released lock");
    lock_release(&lock);
  }
}

void
consume(void *arg) {
  while(1) {
    
    lock_acquire(&lock);
    printf(1,"\nConsumer acquired lock\n");
    while(bufsize == 0)
    {
	  printf(1,"\nWaiting for producer to produce\n");
	  cv_wait(&nonempty, &lock);
    }
    result <<= 1;
    result |= 1;

    bufsize--;
    printf(1,"\nSignalling producer\n");
    cv_signal(&nonfull);
    lock_release(&lock);
  }
}
