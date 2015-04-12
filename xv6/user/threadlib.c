#include "types.h"
#include "stat.h"
#include "user.h"

int thread_create(void (*start_routine)(void*), void *arg) 
{
  return 0;
}

int thread_join(int pid)
{
  return 0;
}

int
main(int argc, char *argv[])
{
  printf(1,"clone %d join %d",clone(NULL,NULL,NULL),join(0));
  exit();
}
