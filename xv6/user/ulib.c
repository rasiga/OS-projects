#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}
/*
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
  int x=clone(start_routine,arg,stack);
  
  printf(1,"Current pid %d thread %d \n",getpid(),x);
  
  
  //table(2d array) - maintain pid and memory region allocated (to enable freeing the stack)
  return 0;
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
  int x=join(pid);
  printf(1,"join pid %d\n",x);
  // need to free stack  
  return 0;
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
}*/
