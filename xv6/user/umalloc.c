#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"
#define PGSIZE 4096

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

int mem_num=0;
typedef struct
{
	int pids;
	void* stacks;
	int alloc;
}mem_table;

mem_table table[400];
//void *stack=NULL;
typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}

int thread_create(void (*start_routine)(void*), void *arg) 
{
  //create stack using malloc
  
  // allocate twice the page size 
  void *stack= malloc( (uint)(PGSIZE*2) );
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
  int x=0;
  x=clone(start_routine,arg,stack);
  table[mem_num].pids=x;
  table[mem_num].stacks = stack;
  table[mem_num].alloc=1;
  mem_num++;  
  //printf(1,"Current pid %d thread %d \n",getpid(),x);
  
  
  //table(2d array) - maintain pid and memory region allocated (to enable freeing the stack)
  return x;
}

inline int fetch_and_add( int * variable, int value ) {
      asm volatile("lock; xaddl %%eax, %2;"
                   :"=a" (value)                  //Output
                   :"a" (value), "m" (*variable)  //Input
                   :"memory");
      return value;
  }

int thread_join(int pid)
{
  int i;
  int x=join(pid);
  //printf(1,"join pid %d\n",x);
  //free(stack);
  // need to free stack 
  for(i=0;i<mem_num;i++)
  {
    if(table[i].pids == x && table[i].alloc == 1)
    {
        table[i].alloc=0;
	free(table[i].stacks);
	break;
    }
  } 
  return x;
}
void lock_init(lock_t *lock)
{
  lock->ticket = 0;
  lock->turn = 0;
}
void 
lock_acquire(lock_t *lock)
{
  int myturn = fetch_and_add(&lock->ticket,1);
  while (lock->turn != myturn)
   ; // spin
}
void
lock_release(lock_t *lock)
{
  fetch_and_add(&lock->turn,1);
}
void cv_wait(cond_t *cond, lock_t *lock)
{

}
void cv_signal(cond_t *cond)
{

}



void sample(void* a)
{
  printf(1,"sample in it %d %d\n",*(int *)a,getpid());
  //sleep(60);
  exit();
}
